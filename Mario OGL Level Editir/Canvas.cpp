#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <glut.h>
#include <SOIL.h>

#define DispSizeX 1150.0
#define DispSizeY 450.0

//Параметры текстуры шахматной доски
#define checkImageWidth 64
#define checkImageHeight 64

void SaveBackGround(char* index)
{
	/* save that image as another type */
	if (!SOIL_save_screenshot(index, SOIL_SAVE_TYPE_BMP, 0, 0, 600, 400))
		printf("Save background.. Error\n");
}
int RoundDiplCord(int cord, int round)
{

	return cord - fmod(cord, round);
}
void CreateOrDelCube1(float ***BGarray, float rgb[], int moveX, int moveY, int mouseX, int mouseY, int size, int mode)
{
	int i, j;
	if (size != 5)
	{
		for (i = 0; i < size;i++)
		{
			for (j = 0; j < size;j++)
			{
				BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10 + i][(RoundDiplCord(mouseX, 10) + moveX) / 10 + j][0] = mode;
				BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10 + i][(RoundDiplCord(mouseX, 10) + moveX) / 10 + j][1] = rgb[0];
				BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10 + i][(RoundDiplCord(mouseX, 10) + moveX) / 10 + j][2] = rgb[1];
				BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10 + i][(RoundDiplCord(mouseX, 10) + moveX) / 10 + j][3] = rgb[2];
			}
		}
	}
	else
	{
		BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10][(RoundDiplCord(mouseX, 10) + moveX) / 10][0] = mode;
		BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10][(RoundDiplCord(mouseX, 10) + moveX) / 10][1] = 0;
		BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10][(RoundDiplCord(mouseX, 10) + moveX) / 10][2] = 0;
		BGarray[(RoundDiplCord(DispSizeY - mouseY, 10) + moveY) / 10][(RoundDiplCord(mouseX, 10) + moveX) / 10][3] = 0;

	}
}
void makeCheckImage(GLubyte checkImage[checkImageHeight][checkImageWidth][4])
{
	int i, j, c;
	for (i = 0;i<checkImageHeight;i++)
	{
		for (j = 0;j<checkImageWidth;j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)250;
		}
	}
}
int NextTitle()
{
	char* buffer = new char[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	CharToOemA(buffer, buffer);
	//std::cout << buffer << std::endl;

	setlocale(LC_ALL, "");
	HANDLE search_file;
	WIN32_FIND_DATA aa;
	search_file = FindFirstFile("C:\\*", &aa);
	while (FindNextFile(search_file, &aa) != NULL)
	{
		//printf("%c\n", aa.cFileName);
		//cout << aa.cFileName << "\n";
	}
	return 0;
}
