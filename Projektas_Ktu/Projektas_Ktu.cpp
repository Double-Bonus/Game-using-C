// Projektas_Ktu.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <stdbool.h>	//Kad veiktu bool C kalboje C++ nereikia tokio headerio
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "objects.h"

#define WIDTH 1600  //Taip daroma, kad negalima butu  toliau pakeisti situ verciu
#define HEIGHT 900  //Panasiai kaip su const int
#define sizeEnem 10	//Priesu masyvo max dydis			USE UPPER CASE!!!!!!!!!!!!!!

//Funkciju prototipai
void initPlayer(Player* player);
void moveUp(Player* player);
void moveDown(Player* player);
void moveLeft(Player* player);
void moveRight(Player* player);
void initBullet(Bullet bullet[], int size);
void initEnemyBullet(Bullet bullet[], int size, int radius);
void drawBullet(Bullet bullet[], int size);
void fireBullet(Bullet bullet[], int size, Player* player);
void updateBullet(Bullet bullet[], int size, Player* player);
void initEnemy(const char* file_name, Enemy enemy[]);
void drawEnemy(Enemy enemy[], int size, ALLEGRO_BITMAP* picture);
void updateEnemy(Enemy enemy[], int size, Bullet bullet[], int bulletSize, Player* player);
void fireEnemyBullet(Bullet bullet[], int size, Enemy enemy[], int sizeEnemy);
void writeHighScore(Player* player);
void delayReload(bool* reload);
void drawPowerUp(int time, bool notUsed, ALLEGRO_BITMAP* picture);
void updatePowerUp(Player* player, bool* notUsed);
void initHighscore(int A[]);
void moveEnemy(Enemy enemy[], int sizeEnemy);
void drawHighScore(int A[], ALLEGRO_FONT* font);
void drawButtons(ALLEGRO_FONT* font, ALLEGRO_BITMAP* buttn[]);
void drawInstructionsText(ALLEGRO_FONT* font);

//Globalus kintamieji

int bulletCount = 100;    // DABARTINIS KULKU SKAICIUS, NE MAX
int reloadTime = 60;    //Ciklu skaicius reikalingas uzsitaisyti
const int NUM_BULLETS = 100; //Galima ir defininti bet nera skirtumo (taip patogiau lygtais greiciau uzkrauna programa)
const int NUM_BULLETS_ENEMY1 = 150;
int NUM_ENEMY = 3;
const char* enemy_tut = "enemyTutorial.txt";
const char* enemy_lvl_1 = "enemy1_LVL.txt";
const char* enemy_lvl_2 = "enemy2_LVL.txt";
int timeCount = 9000;
int records[5];
ALLEGRO_SAMPLE* judesys = NULL;
ALLEGRO_SAMPLE* sample = NULL;
ALLEGRO_SAMPLE_ID vaflis;
ALLEGRO_SAMPLE* muzika = NULL;
ALLEGRO_SAMPLE_INSTANCE* instance1 = NULL;

int main()
{
	ALLEGRO_DISPLAY* display;


	if (!al_init()) //Patikrina ar pasileidzia allegro, jei ne isjungia
	{
		al_show_native_message_box(NULL, NULL, "Error", "Falied to initialize allegro", NULL, NULL);
		return -1;
	}

	display = al_create_display(WIDTH, HEIGHT); //Sukuriamas ekranas
	if (!display){
		al_show_native_message_box(NULL, NULL, "Error", "Falied to initialize the display", NULL, NULL);
		return -1;
	}

	Player player; //Sukuria Player struktura pavadinimu player is objects.h
	Bullet bullet[NUM_BULLETS]; //Tas pats kas su player tik cia Bullet strukturos bullet masyvas kurio dydis yra NUM_BULLETS
	Bullet enemBullet[NUM_BULLETS_ENEMY1];
	Enemy enemy[sizeEnem];


	bool done = false, draw = true;
	bool mainMeniu = true, reloadBool = false;
	bool redraw = true, optionsBool = false, highscoreBool = false, redrawOptions = false, redrawHigh = false, powerUpBool = true, playAgain = false;
	bool dudeBool = false, redrawDude = false;
	bool instructionBool = false, redrawInstruction = false;

	const float FPS = 60;

	//Instaliuojamos papildomos bibliotekos funkcijos
	al_init_image_addon();
	al_init_primitives_addon();
	al_install_keyboard();
	al_init_font_addon();
	al_init_ttf_addon();
	al_install_audio();
	al_init_acodec_addon();

	//Masyvams priskiriamos reiksmes
	initPlayer(&player);
	initBullet(bullet, NUM_BULLETS);
	initEnemy(enemy_tut, enemy);
	initEnemyBullet(enemBullet, NUM_BULLETS_ENEMY1, 4);

	//Rodykliu koordinaciu reiksmes
	int arowX = 350, arowY = 350;


	//Rezerjuojama vieta muzikos failams
	al_reserve_samples(5);


	sample = al_load_sample("Tema.ogg");
	judesys = al_load_sample("JUDESYS.ogg");
	muzika = al_load_sample("Piratai.ogg");

	instance1 = al_create_sample_instance(judesys);
	al_attach_sample_instance_to_mixer(instance1, al_get_default_mixer());

	ALLEGRO_KEYBOARD_STATE keystate;
	//Paleidziami failai
	ALLEGRO_BITMAP* character = al_load_bitmap("bicas60x60.png");   //reikia gale istrinti
	ALLEGRO_BITMAP* character_2 = al_load_bitmap("bicas_2_60x60.png");
	ALLEGRO_BITMAP* character_1 = al_load_bitmap("bicas60x60.png");
	ALLEGRO_BITMAP* target = al_load_bitmap("target_50x50.png");
	ALLEGRO_BITMAP* arrow = al_load_bitmap("arrow110x60.png");
	al_convert_mask_to_alpha(arrow, al_map_rgb(255, 255, 255));
	ALLEGRO_BITMAP* healthPotion = al_load_bitmap("PotionRedBottle30x30.png");
	ALLEGRO_FONT* font = al_load_font("Rastas.ttf", 25, 0);
	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	ALLEGRO_EVENT_QUEUE * event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_display_event_source(display));


	const int CBt = 9;
	ALLEGRO_BITMAP* buttons[CBt];	//THIS IS BS !!!!!!!!!!! USE STRUCT
	buttons[0] = al_load_bitmap("buttonUP_50x50.png");
	buttons[1] = al_load_bitmap("buttonDOWN_50x50.png");
	buttons[2] = al_load_bitmap("buttonRIGHT_50x50.png");
	buttons[3] = al_load_bitmap("buttonLEFT_50x50.png");
	buttons[4] = al_load_bitmap("buttonSPACEBAR_105x50.png");
	buttons[5] = al_load_bitmap("buttonR_50x50.png");
	buttons[6] = al_load_bitmap("buttonQ_50x50.png");
	buttons[7] = al_load_bitmap("buttonESC_50x50.png");
	buttons[8] = al_load_bitmap("buttonBACKSPACE_105x50.png");
	
	
	al_play_sample(sample, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, &vaflis);


	al_start_timer(timer);


	while (!done)
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);
		al_get_keyboard_state(&keystate);

		//Irasomi buvusieji rekordai
		initHighscore(records);

		//main meniu visa reiktu pataisyt
		while (mainMeniu)
		{
			ALLEGRO_EVENT event;
			al_wait_for_event(event_queue, &event);
			al_get_keyboard_state(&keystate);

			if (al_key_down(&keystate, ALLEGRO_KEY_ENTER))
			{
				switch (arowY)
				{
				case 350:
					al_play_sample(muzika, 1, 0, 1, ALLEGRO_PLAYMODE_LOOP, NULL);
					mainMeniu = false;
					break;
				case 450:
					optionsBool = true;
					redrawOptions = true;
					while (optionsBool)
					{
						ALLEGRO_EVENT event;
						al_wait_for_event(event_queue, &event);
						al_get_keyboard_state(&keystate);
						if (al_key_down(&keystate, ALLEGRO_KEY_Q)) {
							optionsBool = false;
							arowY = 350;
						}
						redrawOptions = true;
						if (redrawOptions && al_is_event_queue_empty(event_queue)) {
							redrawOptions = false;
							drawButtons(font, buttons);
							al_flip_display();
							al_clear_to_color(al_map_rgb(0, 0, 0));
						}
					}
					break;
				case 550:
					highscoreBool = true;
					redrawHigh = true;
					while (highscoreBool) {
						ALLEGRO_EVENT event;
						al_wait_for_event(event_queue, &event);
						al_get_keyboard_state(&keystate);
						if (al_key_down(&keystate, ALLEGRO_KEY_Q)) {
							highscoreBool = false;
							arowY = 350;
						}
						redrawHigh = true;
						if (redrawHigh && al_is_event_queue_empty(event_queue)) {  // kam juos piesti kiekviena karta!!!!!!!!!!!!!!!!!!!
							redrawHigh = false;
							drawHighScore(records, font);
							al_flip_display();
							al_clear_to_color(al_map_rgb(0, 0, 0));
						}
					}
					break;
				case 650:
					dudeBool = true;
					redrawDude = true;
					while (dudeBool)
					{
						ALLEGRO_EVENT event;
						al_wait_for_event(event_queue, &event);
						al_get_keyboard_state(&keystate);
						if (al_key_down(&keystate, ALLEGRO_KEY_Q)) {
							dudeBool = false;
							arowY = 350;
						}
						redrawDude = true;
						//Cia galima taisyti butu
						if (redrawDude && al_is_event_queue_empty(event_queue)) {
							redrawDude = false;
							al_draw_textf(font, al_map_rgb(0, 191, 255), 500, 200, 0, "First character ");
							al_draw_bitmap(character_1, 500, 250, 0);

							al_draw_textf(font, al_map_rgb(0, 191, 255), 500, 600, 0, "Second character ");
							al_draw_bitmap(character_2, 500, 650, 0);
							al_flip_display();
							al_clear_to_color(al_map_rgb(0, 0, 0));
							al_draw_bitmap(arrow, arowX, arowY, 0);
						}

						//Universale funkcija galima padarytu
						if (al_key_down(&keystate, ALLEGRO_KEY_DOWN)) {
							arowY += 400;
							al_rest(0.2);
						}
						if (al_key_down(&keystate, ALLEGRO_KEY_UP)) {
							arowY -= 400;
							al_rest(0.2);
						}
						if (al_key_down(&keystate, ALLEGRO_KEY_ENTER)) {
							switch (arowY)
							{
							case 250:
								character = al_load_bitmap("bicas60x60.png");
								muzika = al_load_sample("Piratai.ogg");
								break;
							case 650:
								character = al_load_bitmap("bicas_2_60x60.png");
								muzika = al_load_sample("Misija.ogg");
								break;
							}
						}
					}
					break;
				case 750:
					instructionBool = true;
					redrawInstruction = true;
					while (instructionBool)
					{
						ALLEGRO_EVENT event;
						al_wait_for_event(event_queue, &event);
						al_get_keyboard_state(&keystate);

						if (al_key_down(&keystate, ALLEGRO_KEY_Q)) {
							instructionBool = false;
							arowY = 350;
						}
						redrawInstruction = true;

						if (redrawInstruction && al_is_event_queue_empty(event_queue)) {
							redrawInstruction = false;
							drawInstructionsText(font);
							al_draw_bitmap(character_2, 900, 400, 0);
							al_draw_bitmap(target, 1300, 400, 0);
							int i;
							for (i = 970; i <= 1270; i += 30) {
								al_draw_filled_circle(i, 420, 2, al_map_rgb(255, 0, 0));
							}
							al_flip_display();
							al_clear_to_color(al_map_rgb(0, 0, 0));
						}
					}
					break;
				}
			}

			//Cia taip pat su universale rodykles funkcija galima
			if (al_key_down(&keystate, ALLEGRO_KEY_DOWN) && arowY < 700) {
				arowY += 100;
				al_rest(0.2);
			}

			if (al_key_down(&keystate, ALLEGRO_KEY_UP) && arowY > 350) {
				arowY -= 100;
				al_rest(0.2);
			}

			if (al_key_down(&keystate, ALLEGRO_KEY_ESCAPE))     
				done = true, mainMeniu = false;

			else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				done = true, mainMeniu = false;

			redraw = true;
			if (redraw && al_is_event_queue_empty(event_queue)) {
				redraw = false;
				al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 350, 0, "Pres enter to start a game");
				al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 450, 0, "Options");
				al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 550, 0, "Highscore");
				al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 650, 0, "Select character");
				al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 750, 0, "Instructions");

				al_draw_bitmap(arrow, arowX, arowY, 0);
				al_flip_display();
				al_clear_to_color(al_map_rgb(0, 0, 0));
			}
		} // MAIN MENIU


		al_stop_sample(&vaflis);
		//sample = al_load_sample("Tema.ogg");

		if (al_key_down(&keystate, ALLEGRO_KEY_ESCAPE))     
			done = true;

		else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			done = true;

		else if (event.type == ALLEGRO_EVENT_TIMER) {
			updateBullet(bullet, NUM_BULLETS, &player);
			updateBullet(enemBullet, NUM_BULLETS_ENEMY1, &player);
			delayReload(&reloadBool);

			if (player.score >= 150 && player.score <= 900 && NUM_ENEMY != 5) {
				NUM_ENEMY = 5;
				initEnemy(enemy_lvl_1, enemy);
				target = al_load_bitmap("mushroom70x70.png");
			}
			else if (player.score >= 900 && player.score <= 2300 && NUM_ENEMY != 7) {
				NUM_ENEMY = 7;
				initEnemy(enemy_lvl_2, enemy);
				target = al_load_bitmap("terrotis120x120.png");
				initEnemyBullet(enemBullet, NUM_BULLETS_ENEMY1, 3);
			}

			updateEnemy(enemy, NUM_ENEMY, bullet, NUM_BULLETS, &player); //Updatina kai nieko nepaspausta

			// move all 4 fubtions to updateMovement
			if (al_key_down(&keystate, ALLEGRO_KEY_UP))
				moveUp(&player);

			if (al_key_down(&keystate, ALLEGRO_KEY_DOWN))
				moveDown(&player);

			if (al_key_down(&keystate, ALLEGRO_KEY_RIGHT))
				moveRight(&player);

			if (al_key_down(&keystate, ALLEGRO_KEY_LEFT))
				moveLeft(&player);

			if (al_key_down(&keystate, ALLEGRO_KEY_SPACE) && bulletCount > 0)   //Tikrina ar space paspausta ir ar kulku yra
			{
				fireBullet(bullet, NUM_BULLETS, &player);
			}

			if (al_key_down(&keystate, ALLEGRO_KEY_R)) {
				reloadBool = true;
			}

			draw = true;
			updateBullet(bullet, NUM_BULLETS, &player);
			updateBullet(enemBullet, NUM_BULLETS_ENEMY1, &player);
			updatePowerUp(&player, &powerUpBool);
			updateEnemy(enemy, NUM_ENEMY, bullet, NUM_BULLETS, &player); //Updatina kai kazkas paspausta

			//Kad vaiksciotu ratu (pasiekus ekrano ribas)
			if (player.x > WIDTH + al_get_bitmap_width(character))
				player.x = -al_get_bitmap_width(character);

			if (player.x < -al_get_bitmap_width(character))
				player.x = WIDTH + al_get_bitmap_width(character);

			if (player.y > HEIGHT + al_get_bitmap_height(character))
				player.y = -al_get_bitmap_height(character);

			if (player.y < -al_get_bitmap_height(character))
				player.y = HEIGHT + al_get_bitmap_height(character);

			//Treciam lygyje pradeda judeti priesai
			if (NUM_ENEMY == 7)
				moveEnemy(enemy, NUM_ENEMY);

			//Nuo antro lygio saudo i zaideja priesai
			if (NUM_ENEMY > 4)
				fireEnemyBullet(enemBullet, NUM_BULLETS_ENEMY1, enemy, NUM_ENEMY);

		}


		if (draw && al_is_event_queue_empty(event_queue))
		{
			draw = false;
			al_draw_bitmap(character, player.x, player.y, 0);
			drawEnemy(enemy, NUM_ENEMY, target);

			if (NUM_ENEMY > 4) {
				drawBullet(enemBullet, NUM_BULLETS_ENEMY1);
			}


			drawBullet(bullet, NUM_BULLETS);
			drawPowerUp(timeCount, powerUpBool, healthPotion);

			al_draw_textf(font, al_map_rgb(44, 117, 255), 10, 800, 0, "Bullets: %d", bulletCount); //Kulku skaiciu
			al_draw_textf(font, al_map_rgb(255, 0, 0), 200, 800, 0, "Score: %d", player.score); //Rezultatas
			al_draw_textf(font, al_map_rgb(0, 100, 0), 350, 800, 0, "Lives %d", player.lives);  //Gyvybes  

			if (reloadBool)		//Kai uzsitaisoma
				al_draw_textf(font, al_map_rgb(0, 255, 0), 10, 850, 0, "RELOADING");

			timeCount--;

			al_draw_textf(font, al_map_rgb(255, 0, 255), 400, 50, 0, "Time left: %d", (timeCount / 60 + 1)); //bandom

			if (timeCount == 0 || player.lives < 1 || player.score == 2300) {
				writeHighScore(&player);
				NUM_ENEMY = 3;
				initBullet(bullet, NUM_BULLETS);
				initEnemy(enemy_tut, enemy);
				initEnemyBullet(enemBullet, NUM_BULLETS_ENEMY1, 4);

				int i;
				for (i = 3; i < sizeEnem; i++) {
					enemy[i].x = 8000;
					enemy[i].y = 8000;
					enemy[i].lives = 0;
					enemy[i].speed = 0;
					enemy[i].alive = false;
					enemy[i].sizeX = 600;
					enemy[i].sizeY = 600;
					enemy[i].dir = 0;
				}

				while (!playAgain) {
					ALLEGRO_EVENT event;
					al_wait_for_event(event_queue, &event);
					al_get_keyboard_state(&keystate);

					if (al_key_down(&keystate, ALLEGRO_KEY_SPACE)) {
						playAgain = true;
						mainMeniu = true;
						timeCount = 9000;
						target = al_load_bitmap("target_50x50.png");
						initPlayer(&player);
					}
					if (al_key_down(&keystate, ALLEGRO_KEY_ESCAPE)) {
						playAgain = true;
						done = true;
					}

					//Gal bool reikia redraw???
					if (al_is_event_queue_empty(event_queue)) {

						al_draw_textf(font, al_map_rgb(255, 0, 0), 600, 200, 0, "GAME OVER");
						al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 300, 0, "Your score is %d", player.score);
						al_draw_textf(font, al_map_rgb(0, 255, 0), 500, 400, 0, "Pres space to play again esc to quit");

						al_flip_display();
						al_clear_to_color(al_map_rgb(0, 0, 0));
					}
				}

			}
			playAgain = false;

			al_flip_display();
			al_clear_to_color(al_map_rgb(255, 255, 255));


		}
	} // while(!done)

	al_destroy_sample(sample);
	al_destroy_sample_instance(instance1);
	//al_destroy_sample(judesys);
	al_destroy_sample(muzika);
	al_destroy_display(display);
	al_uninstall_keyboard();
	al_destroy_bitmap(character);
	al_destroy_bitmap(character_2);
	al_destroy_bitmap(character_1);
	al_destroy_bitmap(target);
	al_destroy_bitmap(arrow);
	al_destroy_bitmap(healthPotion);
	al_destroy_font(font);
	al_destroy_timer(timer);
	al_destroy_event_queue(event_queue);

	for (int i = 0; i < CBt; i++)
		al_destroy_bitmap(buttons[i]);


	return 0;
}

//Funkcija nustatyti zaidejo parametrus
void initPlayer(Player* player)
{
	player->x = 10;
	player->y = 10;
	player->ID = PLAYER;
	player->lives = 100;
	player->speed = 5;
	player->score = 0;
}

//Funkcija nustatyti priesu parametrus
//Naudojama keleta kartu keiciantis lygiams
void initEnemy(const char* file_name, Enemy enemy[])			//NENAUDOJAMAS INT SIZE!!!!!!!!!!!!!!!!!!!!!!!!!
{
	int i = 0;
	int count_enem;		//Priesu skaicius //NAME!!!!!!!!!!!11
	FILE* file = fopen(file_name, "r");

	fscanf(file, "%d", &count_enem);

	//Uzpildomos reiksmes
	for (i; i < count_enem; i++) {
		fscanf(file, "%d", &enemy[i].x);
		fscanf(file, "%d", &enemy[i].y);
		fscanf(file, "%d", &enemy[i].lives);
		fscanf(file, "%f", &enemy[i].speed);
		fscanf(file, "%d", &enemy[i].alive);
		fscanf(file, "%d", &enemy[i].sizeX);
		fscanf(file, "%d", &enemy[i].sizeY);
		fscanf(file, "%d", &enemy[i].dir);
	}
	fclose(file);
}

//Judejimo funkcijos
void moveUp(Player* player)
{
	player->y -= player->speed;
}

void moveDown(Player* player)
{
	player->y += player->speed;
}

void moveLeft(Player* player)
{
	player->x -= player->speed;
}

void moveRight(Player* player)
{
	player->x += player->speed;
}

//Funkcija deklaruoti zaidejo kulku parametrus
void initBullet(Bullet bullet[], int size)
{
	int i = 0;
	for (i; i < size; i++)
	{
		bullet[i].ID = BULLET;
		bullet[i].speed = 3.5;
		bullet[i].live = false;
		bullet[i].radius = 2;
	}
}

//Funkcija deklaruoti prieso kulku parametrus
void initEnemyBullet(Bullet bullet[], int size, int radius)
{
	int i;
	for (i = 0; i < size; i++)
	{
		bullet[i].ID = ENEMY;	//Naudojamas veliau kad i kita puse skristu kulkos
		bullet[i].speed = 1.5;
		bullet[i].live = false;	//Ar reikia piesti ekrane kulka
		bullet[i].radius = radius;
	}
}

void drawBullet(Bullet bullet[], int size)
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (bullet[i].live)
		{
			//alegro funkcija piesianti pilna apskritima norimos spalvos
			al_draw_filled_circle(bullet[i].x, bullet[i].y, bullet[i].radius, al_map_rgb(255, 0, 0));
		}
	}
}

void drawEnemy(Enemy enemy[], int size, ALLEGRO_BITMAP* picture) 
{
	int i;
	for (i = 0; i < size; i++)
	{
		if (enemy[i].alive)
		{
			al_draw_bitmap(picture, enemy[i].x, enemy[i].y, 0);
		}
	}
}

//Atnaujina priesu informacija
void updateEnemy(Enemy enemy[], int size, Bullet bullet[], int bulletSize, Player* player) 
{
	int i, j;
	for (i = 0; i < size; i++)
	{
		for (j = 0; j < bulletSize; j++)
		{
			//Ziurima ar kulkos kordinate yra tarp prieso uzimamos zonos ar jis yra gyvas
			if (bullet[j].x > enemy[i].x && bullet[j].x < (enemy[i].x + 10) && bullet[j].y > enemy[i].y && bullet[j].y < (enemy[i].y + enemy[i].sizeY) && enemy[i].alive) {
				enemy[i].lives--;
				player->score++;	//Kai pataikoma padidinimas rezultatas
				bullet[j].live = false;
				bullet[j].x = 0;
				bullet[j].y = 0;
			}
		}

		//Kai riesas nebeturi gyvybiu bool alive pakeistas i false kad kita funkcija jo nebepiestu
		if (enemy[i].lives <= 0) {
			enemy[i].alive = false;
		}
	}
}

//Saudo zaidejo kulkas
void fireBullet(Bullet bullet[], int size, Player* player)
{
	int i = 0;
	for (i; i < size; i++)
	{
		if (!bullet[i].live)
		{
			--bulletCount;
			bullet[i].x = player->x + 35;  //Kad atrodytu lyg is sautuvo saudo
			bullet[i].y = player->y + 20;
			bullet[i].live = true;
			break;
		}
	}
}


void fireEnemyBullet(Bullet bullet[], int size, Enemy enemy[], int sizeEnemy) 
{
	int i, j;
	for (i = 0; i < size; i++)
	{
		if (!bullet[i].live) {
			for (j = 0; j < sizeEnemy; j++) {
				if (rand() % 5000 == 0 && enemy[j].alive)	//Atsitiktinai saudo
				{
					bullet[i].live = true;
					bullet[i].x = enemy[j].x;
					if (NUM_ENEMY == 7) {	//Atskiram lygiui kad atrodytu lyg is sautuvo saudo
						bullet[i].y = enemy[j].y + enemy[j].sizeY / 4;
					}
					else {
						bullet[i].y = enemy[j].y + 35; //Kitam lygiui kad atrodytu lyg is sautuvo saudo
					}
					break;
				}
			}
		}
	}
}


void updateBullet(Bullet bullet[], int size, Player* player)
{
	int i = 0;
	for (i; i < size; i++) {
		if (bullet[i].live) {
			//Kad tiktu viena funkcija tiek zaidejo tiek prieso kulkoms naudojamas ID jas atskirti
			if (bullet[i].ID == 1)
				bullet[i].x += bullet[i].speed;

			else if (bullet[i].ID == 2) {
				bullet[i].x -= bullet[i].speed;
				if ((bullet[i].x > player->x && bullet[i].x < player->x + 60) && (bullet[i].y > player->y && bullet[i].y < player->y + 60))         //Kai pataikoma i zaideja atimamos gyvybes ir  paleidziama muzika
				{
					player->lives--;
					al_play_sample_instance(instance1);
					bullet[i].live = false;
				}
			}
			if (bullet[i].x > WIDTH || bullet[i].x < 0)   //Kai kulka iseina uz ekrano ribu yra sunaikinama
			{
				bullet[i].live = false;
			}
		}
	}
}

void writeHighScore(Player* player) 
{
	//Atidarom faila
	FILE* failas = fopen("high.txt", "r");
	if (failas == NULL) {
		printf("Error opening file!\n");
		exit(1);  //KODEL ISVOSO EXITINIMA REIK SUKURTI NAUJA FAILAS TSG
	}

	// sukuriam  penkiu nariu rezultatu masyva
	int results[5], i;  //MAGIC NUMBER!!!!!!!!!!111
	for (i = 0; i < 5; i++) {
		fscanf(failas, "%d", &results[i]);
	}

	//Ziurima ar esamas rezultatas didesnis uz paskutini buvusi, jei taip tada naujas rezultatas rasomas i 5 masyvo vieta
	if (player->score > results[4]) {
		results[4] = player->score;
	}

	//Surikiavimas
	for (i = 4; i > 0; i--)
	{
		if (results[i] > results[i - 1]) {
			int a = results[i - 1];
			results[i - 1] = results[i];
			results[i] = a;
		}
	}
	fclose(failas);

	//Atidarome rasymui (Taip yra istrinami visi buve elementai)
	failas = fopen("high.txt", "w");
	if (failas == NULL) {
		printf("Error opening file!\n");
		exit(1);
	}
	for (i = 0; i < 5; i++) {
		fprintf(failas, "%d\n", results[i]);
	}
	fclose(failas);
}

void delayReload(bool* reload) 
{
	if (*reload) {
		reloadTime--;  //GLOBAL?????????????
		if (reloadTime == 0) {
			*reload = false;
			bulletCount = NUM_BULLETS;
			reloadTime = 60;
		}
	}
}

void drawPowerUp(int time, bool notUsed, ALLEGRO_BITMAP* picture) 
{
	if (time < 8000 && notUsed) {
		al_draw_bitmap(picture, 500, 500, 0);
	}
}

void updatePowerUp(Player* player, bool* notUsed) 
{
	//if (player->x==500 && player->y ==500)
	if ( (player->x > 500 && player->x < 525) && notUsed && (player->y > 450 && player->y < 550) ) {
		*notUsed = false;
		player->lives += 15;
	}
}

void initHighscore(int A[]) 
{
	int i; 
	FILE* failas = fopen("high.txt", "r");
	if (failas == NULL){
		printf("Error opening file!\n");
		exit(1);
	}

	for (i = 0; i < 5; i++) //MAGIC NUMBERS!!!!!!!!
	{
		fscanf(failas, "%d", &A[i]);
	}
	fclose(failas);
}


void drawHighScore(int A[], ALLEGRO_FONT* font) 
{
	int aukstis = 100;
	for (int i = 0; i < 5; i++) {
		al_draw_textf(font, al_map_rgb(0, 0, 255), 700, aukstis, 0, "%d.  %d", i + 1, A[i]);
		aukstis += 100;
	}
}

void moveEnemy(Enemy enemy[], int sizeEnemy) 
{
	int i;
	for (i = 0; i < sizeEnemy; i++)
	{
		if (enemy[i].y <= 0 || enemy[i].y >= HEIGHT - 60 || (rand() % 1000 == 0)) {
			enemy[i].dir = enemy[i].dir * (-1);
		}
		enemy[i].y += (enemy[i].dir * enemy[i].speed);
	}
}

//TODO Removve magic numbers
void drawButtons(ALLEGRO_FONT *font, ALLEGRO_BITMAP *buttn[]) 
{
	al_draw_textf(font, al_map_rgb(255, 255, 255), 500, 50, 0, "Controls");
	al_draw_textf(font, al_map_rgb(0, 191, 255), 500, 100, 0, "Move up ");
	al_draw_bitmap(buttn[0], 900, 100, 0);

	al_draw_textf(font, al_map_rgb(0, 191, 255), 500, 200, 0, "Move down");
	al_draw_bitmap(buttn[1], 900, 200, 0);

	al_draw_textf(font, al_map_rgb(0, 191, 255), 500, 300, 0, "Move right ");
	al_draw_bitmap(buttn[2], 900, 300, 0);

	al_draw_textf(font, al_map_rgb(0, 191, 255), 500, 400, 0, "Move left ");
	al_draw_bitmap(buttn[3], 900, 400, 0);

	al_draw_textf(font, al_map_rgb(139, 0, 0), 500, 800, 0, "Shoot ");
	al_draw_bitmap(buttn[4], 900, 800, 0);

	al_draw_textf(font, al_map_rgb(139, 0, 0), 500, 700, 0, "Reload weapon ");
	al_draw_bitmap(buttn[5], 900, 700, 0);

	al_draw_textf(font, al_map_rgb(0, 128, 0), 500, 500, 0, "Back to main menu ");
	al_draw_bitmap(buttn[6], 900, 500, 0);

	al_draw_textf(font, al_map_rgb(0, 128, 0), 500, 600, 0, "Quit the game ");
	al_draw_bitmap(buttn[7], 900, 600, 0);
}

//REDO THIS FUNCTION1!!!!!!!!!!!1
void drawInstructionsText(ALLEGRO_FONT* font)
{
	al_draw_textf(font, al_map_rgb(0, 191, 255), 300, 300, 0, "Shoot the target");
	al_draw_textf(font, al_map_rgb(0, 191, 255), 300, 350, 0, "Try to hit as many enemies as you can");
	al_draw_textf(font, al_map_rgb(0, 191, 255), 300, 400, 0, "Avoid enemy bullets");
	al_draw_textf(font, al_map_rgb(0, 191, 255), 300, 450, 0, "Complete all levels as possible faster");
	al_draw_textf(font, al_map_rgb(0, 0, 255), 900, 200, 0, " Bullet count is written in bottom left corner");
	al_draw_textf(font, al_map_rgb(0, 0, 255), 900, 250, 0, "Your score is written in bottom at middle");
	al_draw_textf(font, al_map_rgb(0, 0, 255), 900, 300, 0, "Timer is written in Top left corner");
}