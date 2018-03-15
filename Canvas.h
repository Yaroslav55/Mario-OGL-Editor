#pragma once
void SaveBackGround(char* index);
void makeCheckImage(GLubyte checkImage[64][64][4]);
int RoundDiplCord(int cord, int round);
void CreateOrDelCube1(float ***BGarray, float rgb[], int moveX, int moveY, int mouseX, int mouseY, int size, int mode);
int NextTitle();
