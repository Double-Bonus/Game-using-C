//Structures in C cannot have constructor inside structure
#pragma once
#include <iostream>
using namespace std;

enum IDS { PLAYER, BULLET, ENEMY };  // by default enums are 32bit intigers     PLAYER lygus 0, BULLET 1, ENEMY 2

typedef struct 
{
	int x;
	int y;
	int ID; // Sito lyg nenaudoju
	int lives;
	int score;
	float speed;
}Player;

typedef struct 
{
	int ID;
	int x;
	int y;
	bool live;
	float speed;
	int radius;
}Bullet;

typedef struct 
{
	int x;
	int y;
	int lives;
	float speed;
	bool alive;
	int sizeX; //AR reikia???
	int sizeY;
	int dir;
}Enemy;