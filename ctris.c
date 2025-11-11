#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "graphx.h"
#include "keypadc.h"
#include "sys/timers.h"
#include "tice.h"
#include "sys/rtc.h"

struct Piece {
  uint8_t *mat;
  uint8_t size;
};

void initPiece(struct Piece *p, uint8_t size);
void set(struct Piece *p, uint8_t x, uint8_t y, uint8_t val);
void initPieces();
void printPiece(struct Piece *p);
void getPiece(struct Piece **ptr);
void drawPiece(struct Piece *p);
void down(struct Piece *p);
void left(struct Piece *p);
void right(struct Piece *p);
void rotate(struct Piece *p);
void drawGrid(uint8_t * grid);
void clearMat();
void leftRot(struct Piece *p);
void rightRot(struct Piece *p);
bool checkCollision(struct Piece* p, uint8_t * mat, uint8_t xOffset, uint8_t yOffset, uint8_t rotOffset);

clock_t timer();

bool inMenu = false;

struct Piece T;
struct Piece S;
struct Piece Z;
struct Piece I;
struct Piece L;
struct Piece J;
struct Piece O;
uint8_t x, y;
const uint8_t gridX = 10;
const uint8_t gridY = 14;
const int pixelSize = 240/gridY;
const int defaultFrameTime = 300;
clock_t frameTime = 300;
uint8_t * mat;
bool isPiece = false;

bool paused = false;

uint8_t * linesToClear;
uint8_t lineToClearSize;

clock_t moveStart;
clock_t frameStart;
int animatingFrames = -1;


int main(void) {

  timer_Disable(1);
  timer_Set(1, 0);
  timer_SetReload(1, 0);
  timer_Enable(1, TIMER_32K, TIMER_NOINT, TIMER_UP);

  frameStart = timer();
  linesToClear = (uint8_t *)malloc(sizeof(uint8_t ) * gridY);
  moveStart = timer();
  x = 5;
  y = 0;
  srand(time(NULL));
  const int screenWidth = 320;
  const int screenHeight = 240;
  const int wallSize = 100;
  clock_t start = timer();
  mat = (uint8_t *)malloc(sizeof(uint8_t ) * gridX * gridY);
  clearMat();
  struct Piece *currPiece;
  initPieces();
  kb_key_t key;
  gfx_Begin();

  bool upPressed = false;

  while (kb_Data[1] != kb_2nd) {
  
  {
    //gfx_PrintString(timer());
    gfx_SetDrawBuffer();
    frameStart = timer();
    kb_Scan();
    key = kb_Data[7];
    if(kb_Data[6] == kb_Enter) paused = !paused;
      if(paused)  
      {

      }
      else if(animatingFrames == -1)
      {
        if (!isPiece) {
          getPiece(&currPiece);
          isPiece = true;
        }
        if(key == kb_Left && timer() - moveStart > 150) left(currPiece);
        else if(key == kb_Right && timer() - moveStart > 150) right(currPiece);
        if(timer() - start > frameTime)
        {
          start = timer();
          down(currPiece);
        }
        if(key == kb_Up && !upPressed)
        {
          upPressed = true; 
          rightRot(currPiece);
        }
        else if(key != kb_Up) upPressed = false;
        if(key == kb_Down) frameTime = 50;
        else if(frameTime != defaultFrameTime)
        {
          frameTime = defaultFrameTime;
          start = timer();
        }
        for(int i = 0; i < gridY; i++)
        {
          uint8_t count = 0;
          for(int j = 0; j < gridX; j++)
          {
            if(mat[i * gridX+j] == 1) count++; 
          }
          if(count==10)
          {
            linesToClear[lineToClearSize] = i;
            lineToClearSize++;
            animatingFrames = 5;
          }
        }
      }
      else 
      {
        for(int i = 0; i < lineToClearSize; i++)
        {
          mat[linesToClear[i] * gridX + 9  - animatingFrames] = 0;
          mat[linesToClear[i] * gridX + animatingFrames] = 0;
        }
        start = timer();
        if(--animatingFrames == -1)
        {
          for(int i = 0; i < lineToClearSize; i++)
          {
            int y = linesToClear[i];
            for(int j = y-1; j >= 0; j--)
            {
              for(int px = 0; px < gridX; px++)
              {
                mat[(j+1)*gridX + px] = mat[(j)*gridX + px];
              }
            }
          }
          lineToClearSize = 0;
        }
      }
      gfx_FillScreen(0);
      gfx_SetColor(255);
      gfx_FillRectangle(0, 0, gridX * pixelSize, gridY * pixelSize);
      drawPiece(currPiece);
      drawGrid(mat);
      gfx_BlitBuffer();
      
    }
  }
  gfx_End();
  free(currPiece->mat);
  free(linesToClear);
  free(mat);
  free(T.mat);
  free(S.mat);
  free(Z.mat);
  free(O.mat);
  free(L.mat);
  free(J.mat);
  free(I.mat);
  return 0;
}

void initPiece(struct Piece *p, uint8_t size) {
  p->size = size;
  p->mat = (uint8_t *)malloc(sizeof(uint8_t ) * size * size);
  for (int i = 0; i < size * size; i++)
    p->mat[i] = 0;
}

void printPiece(struct Piece *p) {
  for (int i = 0; i < p->size; i++) {
    for (int j = 0; j < p->size; j++)
      printf(p->mat[i * p->size + j] == 1 ? "1" : "0");
    printf("\n");
  }
  printf("\n");
}

void set(struct Piece *p, uint8_t x, uint8_t y, uint8_t val) {
  p->mat[x + y * p->size] = val;
}

void initPieces() {
  initPiece(&T, 3);
  initPiece(&S, 3);
  initPiece(&Z, 3);
  initPiece(&O, 2);
  initPiece(&I, 4);
  initPiece(&L, 3);
  initPiece(&J, 3);
  T.mat[1] = 1;
  T.mat[3] = 1;
  T.mat[4] = 1;
  T.mat[5] = 1;

  S.mat[4] = 1;
  S.mat[5] = 1;
  S.mat[6] = 1;
  S.mat[7] = 1;

  Z.mat[3] = 1;
  Z.mat[4] = 1;
  Z.mat[7] = 1;
  Z.mat[8] = 1;

  L.mat[1] = 1;
  L.mat[4] = 1;
  L.mat[7] = 1;
  L.mat[8] = 1;

  J.mat[1] = 1;
  J.mat[4] = 1;
  J.mat[7] = 1;
  J.mat[6] = 1;

  O.mat[0] = 1;
  O.mat[1] = 1;
  O.mat[2] = 1;
  O.mat[3] = 1;

  I.mat[1] = 1;
  I.mat[5] = 1;
  I.mat[9] = 1;
  I.mat[13] = 1;

}

void getPiece(struct Piece **ptr) {
  x = 5;
  y = 0;
  int index = rand() % 7;
  switch (index) {
  case (0):
    *ptr = &T;
    break;
  case (1):
    *ptr = &S;
    break;
  case (2):
    *ptr = &Z;
    break;
  case (3):
    *ptr = &I;
    break;
  case (4):
    *ptr = &J;
    break;
  case (5):
    *ptr = &L;
    break;
  case (6):
    *ptr = &O;
    break;
  }
}

void drawPiece(struct Piece *p)
{
  gfx_SetColor(24);
  for(int i = 0; i < p->size; i++){
    for(int j = 0; j < p->size; j++)
    {
      if(p->mat[i*p->size + j] == 1) 
        gfx_FillRectangle((x + j) * pixelSize, (y + i) * pixelSize, pixelSize, pixelSize); 
    }
  }
}

void down(struct Piece *p)
{
  if(!checkCollision(p, mat, 0, 1, 0))
    y++;
  else
  {
    isPiece = false;
    uint8_t px, py;
    for(int i = 0; i < p->size; i++){
      for(int j = 0; j < p->size; j++)
      {
        px = x+j;
        py = y+i;
        if(p->mat[i*p->size+j] == 1)
          mat[px + py * gridX] = 1;
      }
    }
  }
}
void left(struct Piece *p)
{
  if(!checkCollision(p, mat, -1, 0, 0))
  {
    x--;
    moveStart = timer();
  }
}

void right(struct Piece *p)
{
  if(!checkCollision(p, mat, 1, 0, 0))
  {
    x++;
    moveStart = timer();
  }
}

void rotate(struct Piece *p)
{

}

bool checkCollision(struct Piece* p, uint8_t * mat, uint8_t xOffset, uint8_t yOffset, uint8_t rotOffset)
{
  if(rotOffset != 0)
  {

  }
  uint8_t px, py;
  for(int i = 0; i < p->size; i++){
    for(int j = 0; j < p->size; j++)
    {
      px = x+j+xOffset;
      py = y+i+yOffset;
      if(p->mat[i*p->size + j] && (mat[px + py * gridX] || px < 0 || px >= gridX || py < 0 || py >= gridY) )
        return true;
    }
  }
  return false;
}

void drawGrid(uint8_t * grid)
{
  gfx_SetColor(24);
  for(int i = 0; i < gridY; i++)
  {
    for(int j = 0; j < gridX; j++)
    {
      if(grid[j + i * gridX] == 1) 
        gfx_FillRectangle(j * pixelSize, i * pixelSize, pixelSize, pixelSize); 
    }
  }
}

void clearMat()
{
  for(int i = 0; i < gridX * gridY; i++) mat[i]=0;
}

void leftRot(struct Piece *p)
{
}

void rightRot(struct Piece *p)
{
  uint8_t * buffer;
  buffer = (uint8_t *)malloc(sizeof(uint8_t )* p->size * p->size);
  for(int i = 0; i < p->size * p->size; i++) buffer[i] = p->mat[i];
  uint8_t x, y;
  for(int i = 0; i < p->size * p->size; i++)
  {
    x = i%p->size;
    y = i/p->size;
    p->mat[i] = buffer[(p->size - x-1) * p->size + y];
  }
  free(buffer);
}

clock_t timer()
{
  return (timer_Get(1) * 1000) / 32768;
}
