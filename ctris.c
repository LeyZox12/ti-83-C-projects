#include "raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define bint uint8_t

struct Piece {
  bint *mat;
  bint size;
};

void initPiece(struct Piece *p, bint size);
void set(struct Piece *p, bint x, bint y, bint val);
void initPieces();
void printPiece(struct Piece *p);
void getPiece(struct Piece **ptr);
void drawPiece(struct Piece *p);
void down(struct Piece *p);
void left(struct Piece *p);
void right(struct Piece *p);
void rotate(struct Piece *p);
void drawGrid(bint* grid);
bool checkCollision(struct Piece* p, bint* mat, bint xOffset, bint yOffset, bint rotOffset);
void clearMat();

struct Piece T;
struct Piece S;
struct Piece Z;
struct Piece I;
struct Piece L;
struct Piece J;
struct Piece O;
bint x, y;
const bint gridX = 10;
const bint gridY = 14;
const int pixelSize = 50;
bint* mat;
bool isPiece = false;

int main(void) {
  x = 5;
  y = 0;
  srand(time(NULL));
  const int screenWidth = 800;
  const int screenHeight = 800;
  const int wallSize = 100;
  clock_t start = clock();

  mat = (bint*)malloc(sizeof(bint) * gridX * gridY);
  clearMat();
  struct Piece *currPiece;
  initPieces();
  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
  SetTargetFPS(60);
  while (!WindowShouldClose()) {


    if (!isPiece) {
      getPiece(&currPiece);
      isPiece = true;
    }

    if(clock() - start > 300)
    {
      start = clock();
      down(currPiece);
    }
    if(IsKeyPressed(KEY_LEFT)) left(currPiece );
    if(IsKeyPressed(KEY_RIGHT)) right(currPiece );
    if(IsKeyPressed(KEY_UP)) rotate(currPiece);

    BeginDrawing();
    ClearBackground(BLACK);
    DrawRectangle(0, 0, gridX * pixelSize, gridY * pixelSize, WHITE);
    drawPiece(currPiece);
    drawGrid(mat);
    for (int i = 0; i < gridX; i++)
      DrawLine(pixelSize * i, 0, pixelSize * i, pixelSize * gridY, GRAY);
    for (int i = 0; i < gridY; i++)
      DrawLine(0, pixelSize * i, pixelSize * gridX, pixelSize * i, GRAY);
    EndDrawing();
  }
  CloseWindow();
  free(currPiece->mat);
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

void initPiece(struct Piece *p, bint size) {
  p->size = size;
  p->mat = (bint *)malloc(sizeof(bint) * size * size);
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

void set(struct Piece *p, bint x, bint y, bint val) {
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

  L.mat[0] = 1;
  L.mat[3] = 1;
  L.mat[6] = 1;
  L.mat[7] = 1;

  J.mat[2] = 1;
  J.mat[5] = 1;
  J.mat[8] = 1;
  J.mat[7] = 1;

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
  for(int i = 0; i < p->size; i++){
    for(int j = 0; j < p->size; j++)
    {
      if(p->mat[i*p->size + j] == 1) 
        DrawRectangle((x + j) * pixelSize, (y + i) * pixelSize, pixelSize, pixelSize,  BLUE); 
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
    bint px, py;
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
    x--;
}

void right(struct Piece *p)
{
  if(!checkCollision(p, mat, 1, 0, 0))
    x++;
}

void rotate(struct Piece *p)
{

}

bool checkCollision(struct Piece* p, bint* mat, bint xOffset, bint yOffset, bint rotOffset)
{
  if(rotOffset != 0)
  {

  }
  bint px, py;
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

void drawGrid(bint* grid)
{
  for(int i = 0; i < gridY; i++)
  {
    for(int j = 0; j < gridX; j++)
    {
      if(grid[j + i * gridX] == 1) 
        DrawRectangle(j * pixelSize, i * pixelSize, pixelSize, pixelSize,  BLUE); 
    }
  }
}

void clearMat()
{
  for(int i = 0; i < gridX * gridY; i++) mat[i]=0;
}

// U:\MinGW\bin\gcc.exe main.c -o ctris -Iinclude
// -IU:\MinGW\x86_64-w64-mingw32\include -Llib -lraylibdll
// gcc main.c -o ctris -Iinclude -Llib -lraylibdll
