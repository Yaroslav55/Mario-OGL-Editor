/**********************************************
		MARIO OGL EDITOR v0.3
						DEV
							YAROSLAV	25.07.17
************************************************/
#include "stdafx.h"
#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <windows.h>
#include <glut.h>
#include <soil.h>
#include <string>
#include <Canvas.h>

using namespace std;
int LoadLevel();
#define DispSizeX 1150.0
#define DispSizeY 450.0
//Lev conf
char* LevName = "LevGeom1.txt";
char* fileTextures1 = "LevDesign1.txt";//X
char* fileTextures2 = "LevDesign2.txt";//Y
GLfloat step = (GLfloat)1 / 16;
GLubyte canvas[64][64][4];
const unsigned char * bufferC;

int ArraySizeX = 60, ArraySizeY = 30, ArrayColors = 4;
char **ArrayMap;
char **ArrayTexturesID_X;
char **ArrayTexturesID_Y;
float ***BackGroundArray;
GLuint texture[12];
GLuint *textureBG;
float TexCorX, TexCorY;
char TextureID_X, TextureID_Y;

int CurWind = 0;
int CurFunc = 0;
string direct;
int speed = 40;
int MoveX = 0, MoveY = 0;//Transform
boolean X_Y = false;
int M_X = 0, M_Y = 0;
boolean editLab = false;
boolean firstRun = true;
char *ConsoleComand;
std::vector<char> buffer;
int EditorMode = 0;
int CurColor = 0;
int CurCubeSize = 1;
float rgbPalle[3] = {0.0f, 0.0f, 0.0f};
boolean makeBackGround = false;
int ButtonCord[2][8] = {
	{100, 50, 10, 140, 220, 300, 260, 180},
	{410, 410, 410, 410, 410, 410, 410, 410}
};
int ButtGUICord[4][2] = {
		{ 215, 400 },
		{ 223, 223 },
		{ 300, 215 },
		{ 212, 300 }
};

void Timer(int value)
{
	switch (value)
	{
	case 0:
		firstRun = false;
		glutPostRedisplay();
		break;
	case 1:
		glutPostRedisplay();
		if (MoveX < 1200)
		{
			MoveX += 600;
			glutTimerFunc(10, Timer, 1);
		}
		else
			MoveX = 0;
		break;
	}
}
void DeleteArray()
{
	int i, j;
	delete[] ConsoleComand;
	delete[] BackGroundArray;
	delete[] ArrayMap;
	delete[] ArrayTexturesID_X;
	delete[] ArrayTexturesID_Y;

}
void CreateArray()
{
	int i, j;
	ConsoleComand = new char[10];
	ConsoleComand[0] = 0;
	//Create arrays
	BackGroundArray = new float**[ArraySizeX * 4];
	ArrayMap = new char*[ArraySizeX];
	ArrayTexturesID_X = new char*[ArraySizeX];
	ArrayTexturesID_Y = new char*[ArraySizeX];
	for (i = 0; i < ArraySizeX * 4; i++)
	{
		BackGroundArray[i] = new float*[ArraySizeY * 4];
		if (i < ArraySizeX)
		{
			ArrayMap[i] = new char[ArraySizeY];
			ArrayTexturesID_X[i] = new char[ArraySizeY];
			ArrayTexturesID_Y[i] = new char[ArraySizeY];
		}
	}
	for (i = 0; i < ArraySizeX * 4; i++)
	{
		for (j = 0; j < ArraySizeY * 4; j++)
			BackGroundArray[i][j] = new float[ArrayColors];
	}
}
void InitArray()
{
	for (int i = 0; i < ArraySizeX; i++)
	{
		for (int r = 0; r < ArraySizeY + 4; r++)
		{
			ArrayMap[i][r] = '-';
			ArrayTexturesID_X[i][r] = '0';
			ArrayTexturesID_Y[i][r] = '0';
			BackGroundArray[i][r][0] = 0;
			for (int j = 1; j < 4; j++)
				BackGroundArray[i][r][j] = 0;
		}
		ArrayMap[i][ArraySizeY + 3] = '$';
		ArrayTexturesID_X[i][ArraySizeY + 3] = '$';
		ArrayTexturesID_Y[i][ArraySizeY + 3] = '$';
	}
}
void RenderText(int x, int y, int z, char* text)
{
	glColor3f(0, 0, 0);
	glRasterPos3f(x, y, z);
	while (*text)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
}
void RenderButton(char* text, int posX, int posY, int height, int weight)
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1.0);
	glBegin(GL_QUADS);
		glVertex2f(posX, posY);
		glVertex2f(posX, posY + height);
		glVertex2f(posX + weight, posY + height);
		glVertex2f(posX + weight, posY);
	glEnd();

	RenderText(posX+1, posY+3, 0, text);
}
void RenderGUI(char* title, int posX, int posY, int weight, int height, float r, float g, float b)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glColor3f(r, g, b);
		glVertex2f(posX, posY);
		glVertex2f(posX, posY + height);
		glVertex2f(posX + weight, posY + height);
		glVertex2f(posX + weight, posY);
	glEnd();
	RenderText(240, 370, 0, title);
}
void RenderTexture(GLuint tex, int x, int y, int height, int weight)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBegin(GL_QUADS);
		glColor3f(1, 1, 1);
		glTexCoord2f(0, 0);
			glVertex2f(x, y);
		glTexCoord2f(0, 1);
			glVertex2f(x, y + height);
		glTexCoord2f(1, 1);
			glVertex2f(x + weight, y + height);
		glTexCoord2f(1, 0);
			glVertex2f(x + weight, y);
	glEnd();
}
/*
	MODE 0 = level geom
	MODE 1 = TexturesX array
	MODE 2 = TexturesY array
	MODE 3 = Open other files
*/
int LoadLevel(char* name, int mode)
{
	ifstream FileData("Level/" + (string)name, ios::in | ios::binary);
	if (FileData)
	{
		printf("Level open.. OK\n");
		for (int r = 0; r < ArraySizeY; r++)
		{
			if (mode == 0)
			{
				FileData.getline(ArrayMap[r], ArraySizeX - 1, '$');
			}
			else if (mode == 1)
				FileData.getline(ArrayTexturesID_X[r], ArraySizeX - 1, '$');
			else if (mode == 2)
				FileData.getline(ArrayTexturesID_Y[r], ArraySizeX - 1, '$');
			//cout << ArrayTexturesID_X[r] << endl;
		}
	}
	else
	{
		cout << "Level cannot open: " << LevName<< name << ".. Error"<< endl;
		return -1;
	}
	FileData.close();
	return 0;
}
void LoadBG(int sizeX)
{
	char a[22] = "Level/bg/sky2.png";
	textureBG = new GLuint[ArraySizeX / 15];
	for (int i = 0; i < sizeX * 40;i += 600)
	{
		a[12] = i / 600 + '0';
		if (!(textureBG[i / 600] = SOIL_load_OGL_texture(a,
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y)))
		{
			printf("Sky not found.. Error %s\n", a);
			textureBG[i / 600] = SOIL_load_OGL_texture("textures/sky2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		}
	}
}
void LoadTextures()
{

	texture[0] = SOIL_load_OGL_texture("textures/save.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[1] = SOIL_load_OGL_texture("textures/open.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[2] = SOIL_load_OGL_texture("textures/new.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[3] = SOIL_load_OGL_texture("textures/grid1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[4] = SOIL_load_OGL_texture("textures/terrain.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[5] = SOIL_load_OGL_texture("textures/info.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[6] = SOIL_load_OGL_texture("textures/Logo.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[7] = SOIL_load_OGL_texture("textures/switch1.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[8] = SOIL_load_OGL_texture("textures/RGB.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[9] = SOIL_load_OGL_texture("textures/ChangeBG.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[10] = SOIL_load_OGL_texture("textures/magic.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texture[11] = SOIL_load_OGL_texture("textures/money.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	LoadBG(ArraySizeY);
	for(int i = 0;i<sizeof(texture)/sizeof(GLuint);i++)
	{
		if (texture[i] == 0)
			printf("Textures not found %d!\n", i+1);
	}
	printf("Textures loaded!\n");
}

int SaveData(char* name, char **data)
{
	ofstream file("Level/"+(string)name, ios::in | ios::binary); // создаём объект класса ofstream для записи и связываем его с файлом cppstudio.txt
	if ( !file )
	{
		cout << name << " Cannot save.. Error" << endl;
		return -1;
	}
	for (int i = 0; i < ArraySizeY; i++)
	{
		file << data[i];
		//cout << data[i] <<endl;
	}

	file.close();

	cout << name<< " saved.. OK" << endl;
	return 0;
}
void Console()
{
	if (ConsoleComand[0] == 's')
	{
		if (*ConsoleComand == *"s-bg")
			makeBackGround = true;
		else if (*ConsoleComand == *"s-lvl")
		{
			SaveData(LevName, ArrayMap);
			SaveData(fileTextures1, ArrayTexturesID_X);
			SaveData(fileTextures2, ArrayTexturesID_Y);
			glutTimerFunc(10, Timer, 1);
		}
	}
	else if (ConsoleComand[0] == 'l')
	{
		if (ConsoleComand[2] == 'b')
		{
			LoadBG(ArraySizeY);
		}
		else if (ConsoleComand[2] == 'l')
		{
			LoadLevel(LevName, 0);
			LoadLevel(fileTextures1, 1);
			LoadLevel(fileTextures2, 2);
		}
	}
	else
		cout << ConsoleComand << " not found!" << endl;
}
void GenerateLevel()
{
	int i, r;//cicle
	for (i = 0; i < ArraySizeX; i++)
	{
		for (r = 0; r < ArraySizeY; r++)
		{
			if (ArrayMap[i][r] == '#')
			{
				TexCorX = step * (ArrayTexturesID_X[i][r] - '0');
				TexCorY = step * (ArrayTexturesID_Y[i][r] - '0');
				glBindTexture(GL_TEXTURE_2D, texture[4]);
				glBegin(GL_QUADS);
					glTexCoord2f(TexCorX, TexCorY);
						glVertex2f(i * 40 - MoveX, r * 40  - MoveY);
					glTexCoord2f(TexCorX, TexCorY + step);
						glVertex2f(i * 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(TexCorX + step, TexCorY + step);
						glVertex2f(i * 40 + 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(TexCorX + step, TexCorY);
						glVertex2f(i * 40 + 40 - MoveX, r * 40  - MoveY);
				glEnd();
			}
			else if (ArrayMap[i][r] == '*')
			{
				glBindTexture(GL_TEXTURE_2D, texture[11]);
				glBegin(GL_QUADS);
					glTexCoord2f(0, 0);
						glVertex2f(i * 40 - MoveX, r * 40 - MoveY);
					glTexCoord2f(0, 1);
						glVertex2f(i * 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(1, 1);
						glVertex2f(i * 40 + 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(1, 0);
						glVertex2f(i * 40 + 40 - MoveX, r * 40 - MoveY);
				glEnd();
			}
			else if (ArrayMap[i][r] == '0')
			{
				TexCorX = step * (49 - '0');
				TexCorY = step * (48 - '0');
				glBindTexture(GL_TEXTURE_2D, texture[4]);
				glBegin(GL_QUADS);
					glTexCoord2f(TexCorX, TexCorY);
						glVertex2f(i * 40 - MoveX, r * 40 - MoveY);
					glTexCoord2f(TexCorX, TexCorY + step);
						glVertex2f(i * 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(TexCorX + step, TexCorY + step);
						glVertex2f(i * 40 + 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(TexCorX + step, TexCorY);
						glVertex2f(i * 40 + 40 - MoveX, r * 40 - MoveY);
				glEnd();
			}
			if (ArrayMap[i][r] == '$')
			{
				glBindTexture(GL_TEXTURE_2D, texture[3]);
				glBegin(GL_QUADS);
					glTexCoord2f(TexCorX, TexCorY);
					glVertex2f(i * 40 - MoveX, r * 40 - MoveY);
					glTexCoord2f(TexCorX, TexCorY + step);
					glVertex2f(i * 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(TexCorX + step, TexCorY + step);
					glVertex2f(i * 40 + 40 - MoveX, r * 40 + 40 - MoveY);
					glTexCoord2f(TexCorX + step, TexCorY);
					glVertex2f(i * 40 + 40 - MoveX, r * 40 - MoveY);
				glEnd();
			}
		}
	}
	
}
int GenerateBackGround(int begin, int end)
{
	int i, r;//cicle
	glDisable(GL_TEXTURE_2D);
	for (i = 0; i < ArraySizeX*4; i++)
	{
		for (r = begin; r < ArraySizeY *4; r++)
		{
			if ( r == end)
			{
				break;
			}
			if (BackGroundArray[i][r][0] == 1)
			{
				glColor3f(BackGroundArray[i][r][1], BackGroundArray[i][r][2], BackGroundArray[i][r][3]);
				glBegin(GL_QUADS);
					glVertex2f(r * 10 - MoveX, i * 10 - MoveY);
					glVertex2f(r * 10 - MoveX, i * 10 + 10 - MoveY);
					glVertex2f(r * 10 + 10 - MoveX, i * 10 + 10 - MoveY);
					glVertex2f(r * 10 + 10 - MoveX, i * 10 - MoveY);
				glEnd();
			}
		}
	}
	return 1;
}
void RenderGrid(int indent)
{
	int i;
	glColor3f(1.0, 0.0, 0.0); //Выбираем белый цвет
	for(i = 0;i < 600/indent;i++)
	{
		glBegin(GL_LINES);
			glVertex2f(0, i*indent);
			glVertex2f(600, i *indent);

			glVertex2f(i*indent, 0);
			glVertex2f(i *indent, 400);
		glEnd();
	}
}
int ButtonCol(int mouse_x, int mouse_y, int x1, int y1, int Weight, int Height)
{
	if ((mouse_x > x1) && (mouse_x < x1 + Weight) &&
		(DispSizeY - mouse_y > y1) && (DispSizeY - mouse_y < y1 + Height))
		return 1;
	return 0;
}
void MouseButton(int button, int state, int x, int y) {
	if (!X_Y)
	{
		M_X = x;
		M_Y = y;
	}
	if( state == 0 )
	{
		switch (button)
		{
			case GLUT_LEFT_BUTTON:

				if (x > 600)
				{
					if (ButtonCol(x, y, 1000 + 135 * rgbPalle[0], 365, 10, 10))//Red
						CurColor = 1;
					else if (ButtonCol(x, y, 1000 + 135 * rgbPalle[1], 330, 10, 10))//Green
						CurColor = 2;
					else if (ButtonCol(x, y, 1000 + 135 * rgbPalle[2], 295, 10, 10))//Blue
						CurColor = 3;
					else
						CurColor = 0;
					for (int i = 0; i < 400; i+=25)
					{
						for (int r = 0; r < 400; r +=25)
						{
							if (ButtonCol(x, y, 600 + i, 50+ r, 25, 25))
							{
								TextureID_X = (float)i/25+'0';
								TextureID_Y = (float)r/25+'0';
							}
						}
					}
				}
				else if ((x < 600) && (DispSizeY - y > 400))
				{
					if (ButtonCol(x, y, ButtonCord[0][0], ButtonCord[1][0], 30, 30))//SAVE
					{
						CurWind = 2;
						ButtGUICord[0][0] = 215;
						ButtGUICord[0][1] = 401;
					}
					else if (ButtonCol(x, y, ButtonCord[0][1], ButtonCord[1][1], 30, 30))//OPEN
					{
						CurWind = 1;
						ButtGUICord[0][0] = 215;
						ButtGUICord[0][1] = 401;
					}
					else if (ButtonCol(x, y, ButtonCord[0][2], ButtonCord[1][2], 30, 30))//NEW
						CurWind = 4;
					else if (ButtonCol(x, y, ButtonCord[0][3], ButtonCord[1][3], 30, 30))//GRID
					{
						if (CurWind == 0)
						{
							texture[3] = SOIL_load_OGL_texture("textures/grid2.png", SOIL_LOAD_AUTO, 
								SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
							CurWind = 5;
						}
						else if (CurWind == 5)
							CurWind = 6;
						else
						{
							texture[3] = SOIL_load_OGL_texture("textures/grid1.png", 
								SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
							CurWind = 0;
						}
					}
					else if (ButtonCol(x, y, ButtonCord[0][4], ButtonCord[1][4], 30, 30))//INFO
					{
						CurWind = 3;
						ButtGUICord[0][0] = 215;
						ButtGUICord[0][1] = 401;
					}
					else if (ButtonCol(x, y, ButtonCord[0][5], ButtonCord[1][5], 30, 30))//Switch
					{
						if (EditorMode == 0)
						{
							texture[7] = SOIL_load_OGL_texture("textures/switch2.png", SOIL_LOAD_AUTO,
								SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
							EditorMode = 1;
						}
						else
						{
							texture[7] = SOIL_load_OGL_texture("textures/switch1.png", SOIL_LOAD_AUTO,
								SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
							EditorMode = 0;
						}
					}
					else if (ButtonCol(x, y, ButtonCord[0][6], ButtonCord[1][6], 30, 30))//Change BG
					{
						CurWind = 7;
					}
					else if (ButtonCol(x, y, ButtonCord[0][7], ButtonCord[1][7], 30, 30))//Magic
					{
						if (CurFunc != 1)
							CurFunc = 1;
						else
							CurFunc = 0;
						cout << "Magic wand is "<< CurFunc <<endl;
					}
				}
				else//Game disp
				{
					if (ButtonCol(x, y, ButtGUICord[0][0], ButtGUICord[1][0], 60, 25))//SAVE/OPEN
					{
						if (CurWind == 1)
						{
							LoadLevel(LevName, 0);
							LoadLevel(fileTextures1, 1);
							LoadLevel(fileTextures2, 2);
						
							LoadBG(ArraySizeX);
						
						}
						if (CurWind == 7)
						{
							std::string fileNameStr = "Level/bg/" + std::string(buffer.begin(), buffer.end()) + ".png";
							if (!(textureBG[0] = SOIL_load_OGL_texture(fileNameStr.c_str(),
								SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y)))
								printf("BG not found.. Error %s\n", fileNameStr);
						}
						if (CurWind == 4)//Create
						{
							if (buffer.size()) {
								std::string number(buffer.begin(), buffer.end());
								ArraySizeY = std::stoi(number);
								DeleteArray();
								CreateArray();
								InitArray();
								LoadBG(ArraySizeY);
							}

						}
						if( CurWind == 2 )//Save
						{
							SaveData(LevName, ArrayMap);
							SaveData(fileTextures1, ArrayTexturesID_X);
							SaveData(fileTextures2, ArrayTexturesID_Y);
							makeBackGround = true;
							glutPostRedisplay();
						}
						CurWind = 0;
						editLab = false;
					}
					if (ButtonCol(x, y, ButtGUICord[0][1], ButtGUICord[1][1], 60, 25))//Cancel
					{
						CurWind = 0;
					}
					else if (ButtonCol(x, y, ButtGUICord[0][2], ButtGUICord[1][2], 60, 25))//Edit label
					{
						if (!editLab)
							editLab = true;
						else
							editLab = false;
					}
					if( CurFunc == 1 )//Magic wand
					{
							rgbPalle[0] = BackGroundArray[(RoundDiplCord(DispSizeY - y, 10) + MoveY) / 10][(RoundDiplCord(x, 10) + MoveX) / 10][1];
							rgbPalle[1] = BackGroundArray[(RoundDiplCord(DispSizeY - y, 10) + MoveY) / 10][(RoundDiplCord(x, 10) + MoveX) / 10][2];
							rgbPalle[2] = BackGroundArray[(RoundDiplCord(DispSizeY - y, 10) + MoveY) / 10][(RoundDiplCord(x, 10) + MoveX) / 10][3];
					}
					else
					{
						if ((x + MoveX < ArraySizeX / 15 * 600) && (DispSizeY - y + MoveY < ArraySizeY / 15 * 400))
						{
							//cout << (DispSizeY - y + MoveY)<< "and"<< ArraySizeY / 15 * 400 <<endl;
							if (EditorMode == 0)
							{
								if ((TextureID_X == 55) && (TextureID_Y == 50))
									ArrayMap[(RoundDiplCord(M_X, 40) + MoveX) / 40][(RoundDiplCord(DispSizeY - y, 40) + MoveY) / 40] = '*';
								else if ((TextureID_X == 55) && (TextureID_Y == 49))
									ArrayMap[(RoundDiplCord(M_X, 40) + MoveX) / 40][(RoundDiplCord(DispSizeY - y, 40) + MoveY) / 40] = '0';
								else
									ArrayMap[(RoundDiplCord(M_X, 40) + MoveX) / 40][(RoundDiplCord(DispSizeY - y, 40) + MoveY) / 40] = '#';
								
								ArrayTexturesID_X[(RoundDiplCord(M_X, 40) + MoveX) / 40][(RoundDiplCord(DispSizeY - y, 40) + MoveY) / 40] = TextureID_X;
								ArrayTexturesID_Y[(RoundDiplCord(M_X, 40) + MoveX) / 40][(RoundDiplCord(DispSizeY - y, 40) + MoveY) / 40] = TextureID_Y;

							}
							else//Crate background
							{
								CreateOrDelCube1(BackGroundArray, rgbPalle, MoveX, MoveY, M_X, y, CurCubeSize, 1);
							}
						}
						else
							printf("Forbidden!\n");
					}
				}
			break;
			case GLUT_RIGHT_BUTTON:
				if ((x + MoveX < ArraySizeX / 15 * 600) && (DispSizeY - y + MoveY < ArraySizeY / 15 * 400))
				{
					if (EditorMode == 0)
					{
						ArrayMap[(RoundDiplCord(M_X, 40) + MoveX) / 40][(RoundDiplCord(DispSizeY - y, 40) + MoveY) / 40] = '-';
					}
					else//Delete BG
					{
						CreateOrDelCube1(BackGroundArray, rgbPalle, MoveX, MoveY, M_X, y, CurCubeSize, 0);
					}
				}
				else
					printf("Forbidden!\n");
				break;
			default:
				
			break;
		} 
		glutPostRedisplay();
	}
}
void mouseMove(int x, int y) 
{
	if ((x > 1000)  && ( x < 1135 ))
	{
		switch (CurColor)
		{
		case 1:
			rgbPalle[0] = (float)(x - 1000) / 135;
			glutPostRedisplay();
			break;
		case 2:
			rgbPalle[1] = (float)(x - 1000) / 135;
			glutPostRedisplay();
			break;
		case 3:
			rgbPalle[2] = (float)(x - 1000) / 135;
			glutPostRedisplay();
			break;
		}
	}
}
void RendernMenuButton()
{
	//BUTTON INFO
	glBindTexture(GL_TEXTURE_2D, texture[5]);

	glBegin(GL_QUADS);//TEST
		glVertex2f(-100, -100);
		glTexCoord2f(0, 1);
		glVertex2f(-100, -70);
		glTexCoord2f(1, 1);
		glVertex2f(70, -70);
		glTexCoord2f(1, 0);
		glVertex2f(70, -100);
		glTexCoord2f(0, 0);
	glEnd();
	glBegin(GL_QUADS);
		glVertex2f(ButtonCord[0][4], ButtonCord[1][4]);
			glTexCoord2f(0, 1);
		glVertex2f(ButtonCord[0][4], ButtonCord[1][4] + 30);
			glTexCoord2f(1, 1);
		glVertex2f(ButtonCord[0][4] + 30, ButtonCord[1][4] + 30);
			glTexCoord2f(1, 0);
		glVertex2f(ButtonCord[0][4] + 30, ButtonCord[1][4]);
			glTexCoord2f(0, 0);
	glEnd();
	//BUTTON Magic
	glBindTexture(GL_TEXTURE_2D, texture[10]);
	glBegin(GL_QUADS);
		glVertex2f(ButtonCord[0][7], ButtonCord[1][7]);
		glTexCoord2f(0, 1);
		glVertex2f(ButtonCord[0][7], ButtonCord[1][7] + 30);
		glTexCoord2f(1, 1);
		glVertex2f(ButtonCord[0][7] + 30, ButtonCord[1][7] + 30);
		glTexCoord2f(1, 0);
		glVertex2f(ButtonCord[0][7] + 30, ButtonCord[1][7]);
		glTexCoord2f(0, 0);
	glEnd();
	//BUTTON BG
	glBindTexture(GL_TEXTURE_2D, texture[9]);
	glBegin(GL_QUADS);
		glVertex2f(ButtonCord[0][6], ButtonCord[1][6]);
		glTexCoord2f(0, 1);
		glVertex2f(ButtonCord[0][6], ButtonCord[1][6] + 30);
		glTexCoord2f(1, 1);
		glVertex2f(ButtonCord[0][6] + 30, ButtonCord[1][6] + 30);
		glTexCoord2f(1, 0);
		glVertex2f(ButtonCord[0][6] + 30, ButtonCord[1][6]);
		glTexCoord2f(0, 0);
	glEnd();
	//BUTTON GRID
	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glBegin(GL_QUADS);
		glVertex2f(ButtonCord[0][3], ButtonCord[1][3]);
		glTexCoord2f(0, 1);
		glVertex2f(ButtonCord[0][3], ButtonCord[1][3] + 30);
		glTexCoord2f(1, 1);
		glVertex2f(ButtonCord[0][3] + 30, ButtonCord[1][3] + 30);
		glTexCoord2f(1, 0);
		glVertex2f(ButtonCord[0][3] + 30, ButtonCord[1][3]);
		glTexCoord2f(0, 0);
	glEnd();
	//BUTTON SWITCH
	glBindTexture(GL_TEXTURE_2D, texture[7]);
	glBegin(GL_QUADS);
		glVertex2f(ButtonCord[0][5], ButtonCord[1][5]);
		glTexCoord2f(0, 1);
		glVertex2f(ButtonCord[0][5], ButtonCord[1][5] + 30);
		glTexCoord2f(1, 1);
		glVertex2f(ButtonCord[0][5] + 30, ButtonCord[1][5] + 30);
		glTexCoord2f(1, 0);
		glVertex2f(ButtonCord[0][5] + 30, ButtonCord[1][5]);
		glTexCoord2f(0, 0);
	glEnd();
	//BUTTON SAVE
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glBegin(GL_QUADS);
	glVertex2f(ButtonCord[0][0], ButtonCord[1][0]);
	glTexCoord2f(0, 1);
	glVertex2f(ButtonCord[0][0], ButtonCord[1][0] + 30);
	glTexCoord2f(1, 1);
	glVertex2f(ButtonCord[0][0] + 30, ButtonCord[1][0] + 30);
	glTexCoord2f(1, 0);
	glVertex2f(ButtonCord[0][0] + 30, ButtonCord[1][0]);
	glTexCoord2f(0, 0);
	glEnd();
	//BUTTON OPEN
	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glBegin(GL_QUADS);
	glVertex2f(ButtonCord[0][1], ButtonCord[1][1]);
	glTexCoord2f(0, 1);
	glVertex2f(ButtonCord[0][1], ButtonCord[1][1] + 30);
	glTexCoord2f(1, 1);
	glVertex2f(ButtonCord[0][1] + 30, ButtonCord[1][1] + 30);
	glTexCoord2f(1, 0);
	glVertex2f(ButtonCord[0][1] + 30, ButtonCord[1][1]);
	glTexCoord2f(0, 0);
	glEnd();
	//BUTTON NEW
	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glBegin(GL_QUADS);
	glVertex2f(ButtonCord[0][2], ButtonCord[1][2]);
	glTexCoord2f(0, 1);
	glVertex2f(ButtonCord[0][2], ButtonCord[1][2] + 30);
	glTexCoord2f(1, 1);
	glVertex2f(ButtonCord[0][2] + 30, ButtonCord[1][2] + 30);
	glTexCoord2f(1, 0);
	glVertex2f(ButtonCord[0][2] + 30, ButtonCord[1][2]);
	glTexCoord2f(0, 0);
	glEnd();
}
void RGBPalette()
{
	glBindTexture(GL_TEXTURE_2D, texture[8]);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
			glVertex2f(950, 300);
		glTexCoord2f(0, 1);
			glVertex2f(950, 400);
		glTexCoord2f(1, 1);
			glVertex2f(1150, 400);
		glTexCoord2f(1, 0);
			glVertex2f(1150, 300);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glColor3f(1, 0, 0);
			glVertex2f(1000 + 135 * rgbPalle[0], 365);
			glVertex2f(1010 + 135 * rgbPalle[0], 365);
			glVertex2f(1010 + 135 * rgbPalle[0], 375);
			glVertex2f(1000 + 135 * rgbPalle[0], 375);
		glColor3f(0, 1, 0);
			glVertex2f(1000 + 135 * rgbPalle[1], 330);
			glVertex2f(1010 + 135 * rgbPalle[1], 330);
			glVertex2f(1010 + 135 * rgbPalle[1], 340);
			glVertex2f(1000 + 135 * rgbPalle[1], 340);
		glColor3f(0, 0, 1);
			glVertex2f(1000 + 135 * rgbPalle[2], 295);
			glVertex2f(1010 + 135 * rgbPalle[2], 295);
			glVertex2f(1010 + 135 * rgbPalle[2], 305);
			glVertex2f(1000 + 135 * rgbPalle[2], 305);
	glEnd();
	glColor3f(1, 1, 1);
	glEnable(GL_TEXTURE_2D);
}
void Display()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//BG
	glEnable(GL_TEXTURE_2D);
	glColor3f(1, 1, 1);
	for (int i = 0; i < ArraySizeY*40;i += 600)
	{
		glBindTexture(GL_TEXTURE_2D, textureBG[i/600]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(i - MoveX, 0 - MoveY);
			glTexCoord2f(1, 0);
			glVertex2f(i+600 - MoveX, 0 - MoveY);
			glTexCoord2f(1, 1);
			glVertex2f(i+600 - MoveX, 400 - MoveY);
			glTexCoord2f(0, 1);
			glVertex2f(i - MoveX, 400 - MoveY);
		glEnd();
	}
	glDisable(GL_TEXTURE_2D);

		if (makeBackGround)
		{
			char a[] = "Level/bg/sky0.png";
			a[12] = MoveX/600+ '0';
			GenerateBackGround(0, 5000);
			SaveBackGround(a);
		}
		else
			GenerateBackGround(0, 5000);
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
			glVertex2f(600, 0);
			glVertex2f(600, 450);

			glVertex2f(0, 400);
			glVertex2f(600, 400);
		glEnd();
		RenderText(0 - MoveX, 0 - MoveY, 0, "0,0");
		glBegin(GL_LINES);
			glVertex2f(0 -MoveX, 0 -MoveY);
			glVertex2f(ArraySizeY *40 -MoveX, 0 -MoveY);
			glVertex2f(0 - MoveX, 0 - MoveY);
			glVertex2f(0 - MoveX, 800 - MoveY);
		glEnd();
		glColor3f(0.7, 0.7, 0.8);
		glBegin(GL_QUADS);
			glVertex2f(0, 400);
			glVertex2f(0, 450);
			glVertex2f(600, 450);
			glVertex2f(600, 400);
		glEnd();
		glColor3f(rgbPalle[0], rgbPalle[1], rgbPalle[2]);
		glBegin(GL_QUADS);
			glVertex2f(1000, 0);
			glVertex2f(1000, 50);
			glVertex2f(1050, 50);
			glVertex2f(1050, 0);
		glEnd();

		glColor3f(1, 1, 1);
		glEnable(GL_TEXTURE_2D);
		GenerateLevel();
		RendernMenuButton();
		if (firstRun)
			RenderTexture(texture[6], 80, 100, 220, 480);
		RGBPalette();
		//Title
		glBindTexture(GL_TEXTURE_2D, texture[4]);
		glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
				glVertex2f(600, 50);
			glTexCoord2f(0, 1);
				glVertex2f(600, 450);
			glTexCoord2f(1, 1);
				glVertex2f(1000, 450);
			glTexCoord2f(1, 0);
				glVertex2f(1000, 50);
		glEnd();
		glBegin(GL_QUADS);
			glTexCoord2f(step * (TextureID_X - 48), step * (TextureID_Y - 48) );
				glVertex2f(950, 0);
			glTexCoord2f(step * (TextureID_X - 48), step * (TextureID_Y - 48) + 0.0625);
				glVertex2f(950, 50);
			glTexCoord2f(step * (TextureID_X - 48) + 0.0625, step * (TextureID_Y - 48) + 0.0625);
				glVertex2f(1000, 50);
			glTexCoord2f(step * (TextureID_X - 48) + 0.0625, step * (TextureID_Y - 48) );
				glVertex2f(1000, 0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		/*
			1 - OPEN
			2 - SAVE
			3 - INFO
			4 - NEW
		*/
		switch (CurWind)
		{
		case 1:
			RenderGUI("OPEN FILE", 100, 200, 400, 190, 0.8, 0.8, 1);
			RenderGUI("", 250, 300, 230, 30, 1, 1, 1);
			RenderButton("Cancel", ButtGUICord[0][1], ButtGUICord[1][1], 25, 60);
			RenderButton("Open", ButtGUICord[1][1], ButtGUICord[1][0], 25, 60);
			RenderText(105, 305, 0, "File name ..Level/");
			RenderText(252, 305, 0, LevName);
			break;
		case 2:
			RenderGUI("SAVE FILE", 100, 200, 400, 190, 0.8, 0.8, 1);
			RenderGUI("", 250, 300, 230, 30, 1, 1, 1);
			RenderButton("Cancel", ButtGUICord[0][1], ButtGUICord[1][1], 25, 60);
			RenderButton("Save", ButtGUICord[1][1], ButtGUICord[1][0], 25, 60);
			RenderText(105, 305, 0, "File name ..Level/");
			RenderText(252, 305, 0, LevName);
			break;
			case 3: {
				RenderGUI("INFO", 100, 200, 400, 190, 0.8, 0.8, 1);
				RenderButton("Done", ButtGUICord[0][1], ButtGUICord[1][1], 25, 60);
				RenderText(105, 210, 0, "Dev Yaroslav        25.07.17 beta ");
				break;
			}
			case 4: {	// Create new level
				RenderGUI("NEW", 100, 200, 400, 190, 0.8, 0.8, 1);
				RenderButton("Create", ButtGUICord[1][1], ButtGUICord[1][0], 25, 60);
				RenderButton("Cancel", ButtGUICord[0][1], ButtGUICord[1][1], 25, 60);
				RenderText(105, 305, 0, "Level weight: ");
				char *messageToPrint = (buffer.size()) ? &buffer[0] : " ";
				RenderButton(messageToPrint, ButtGUICord[2][1], ButtGUICord[1][2], 25, 40);
				RenderText(105, 280, 0, "Level hight : ");
				break;
			}
			case 5: {
				RenderGrid(40);
				break;

			}				
			case 6:
				RenderGrid(10);
				break;
			case 7: {
				RenderGUI("BackGround", 100, 200, 400, 190, 0.8, 0.8, 1);
				RenderButton("Select", ButtGUICord[1][1], ButtGUICord[1][0], 25, 60);
				RenderButton("Cancel", ButtGUICord[0][1], ButtGUICord[1][1], 25, 60);
				RenderText(105, 305, 0, " Name: ");
				char *messageToPrint = (buffer.size()) ? &buffer[0] : " ";
				RenderButton(messageToPrint, ButtGUICord[2][1], ButtGUICord[1][2], 25, 80);
				break;
			}
		}
	glDisable(GL_BLEND);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glFlush();
}
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:
		exit(0);
		break;
	case 8:
		buffer.clear();	
		break;
	case 49:
		CurCubeSize = 1;
		printf("Cur cube size is %d\n", CurCubeSize);
		break;
	case 50:
		printf("Cur cube size is %d\n", CurCubeSize);
		CurCubeSize = 2;
		break;
	case 51:
		printf("Cur cube size is %d\n", CurCubeSize);
		CurCubeSize = 3;
		break;
	case 52:
		printf("Cur cube size is %d\n", CurCubeSize);
		CurCubeSize = 4;
		break;
	case 53:
		printf("tex\n");
		CurCubeSize = 5;
		break;
	case 9://Console
		std::cin >> ConsoleComand;
		Console();
		glutPostRedisplay();
		break;
	case 'z':
		rgbPalle[CurColor-1] -= 0.01f;
		glutPostRedisplay();
		break;
	case 'x':
		rgbPalle[CurColor - 1] += 0.01f;
		glutPostRedisplay();
		break;
	case 'c':
		X_Y = true;
		M_X = x;
		break;
	case 'v':
		X_Y = false;
		break;
	case 'd':
		MoveX += speed;
		glutPostRedisplay();
		break;
	case 'a':
		MoveX -= speed;
		glutPostRedisplay();
		break;
	case 'w':
		MoveY += speed;
		glutPostRedisplay();
		break;
	case 's':
		MoveY -= speed;
		glutPostRedisplay();
		break;
	case 'r':
		InitArray();
		LoadLevel(LevName, 0);
		LoadLevel(fileTextures1, 1);
		LoadLevel(fileTextures2, 2);
		LoadBG( ArraySizeY );
		glutPostRedisplay();
		break;
	case 'q':
		SaveData(LevName, ArrayMap);
		SaveData(fileTextures1, ArrayTexturesID_X);
		SaveData(fileTextures2, ArrayTexturesID_Y);
		makeBackGround = true;
		glutTimerFunc(10, Timer, 1);
		break;
	}
	if( editLab == true )
	{
		if (buffer.size() < 10)
		{
			buffer.push_back(key);
			glutPostRedisplay();
		}
		else
			printf("Buffer full!\n");
	}
}
void Initialize() 
{
	glClearColor(0.8, 1.0, 0.6, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, DispSizeX, 0.0, DispSizeY, -1.0, 1.0);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(MouseButton);
	glutMotionFunc(mouseMove);
	TexCorX = (double)1 / 16 * 7;
	TexCorY = (double)1 / 16 * 15;
	CreateArray();
	InitArray();
	NextTitle();
	LoadTextures();
	glutTimerFunc(1500, Timer, 0);
	glClearDepth(1.0);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
}

int main(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(DispSizeX, DispSizeY);
	glutInitWindowPosition(400, 250);
	glutCreateWindow("Mario OGL Level Editor v0.3");
	glutDisplayFunc( Display );
	Initialize();	// Main cycle 
	glutMainLoop();
	return 0;
}