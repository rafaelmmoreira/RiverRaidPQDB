/**
 * ------------------------------------------------------------------------------*
 * File: riverRaid.ino                                                           *
 *       Port (por hora, incompleto) do River Raid para Arduino/PQDB             *
 * Author: Rafael de Moura Moreira <rafaelmmoreira@gmail.com>                    *
 * License: MIT License                                                          *
 * ------------------------------------------------------------------------------*
 * 																				 *
 * O lendário jogo River Raid, em versão para Arduino.		                     *
 * Desenvolvido e testado em um Arduino Uno, Rev. 3                              *
 *																				 *
 * Essa implementação supõe o uso do shield PQDB Mini. Porém, nem todos os compo-*
 * nentes da mesma são usados. Para que ele funcione, é necessário ter 3 botões e*
 * o display Nokia 5110. Para instruções das ligações do circuito, conferir o es-*
 * quemático da PQDB no repositório original.                                    *
 *                                                                               *
 * As bibliotecas utilizadas são os drivers da própria PQDB, também disponíveis  *
 * no repositório original do projeto.                                           *
 * https://github.com/projetopqdb/                                               *
 *                                                                               *
 * ------------------------------------------------------------------------------*
 */

#include "keypad.h"
#include "nokia5110.h"
#include "serial.h"

#define VAZIO -1
#define TAVIAO 10
#define THELIC 8

typedef struct{
  int x;
  int y;
} Objeto;

byte spriteAviao[TAVIAO][TAVIAO] = {0,0,0,0,1,1,0,0,0,0,
									0,0,0,0,1,1,0,0,0,0,
									0,0,0,1,1,1,1,0,0,0,
									0,0,1,1,1,1,1,1,0,0,
									0,1,1,1,1,1,1,1,1,0,
									1,1,1,1,1,1,1,1,1,1,
									1,0,0,0,1,1,0,0,0,1,
									0,0,0,1,1,1,1,0,0,0,
									0,0,1,0,1,1,0,1,0,0,
									0,1,0,0,1,1,0,0,1,0};
byte spriteHeli1[THELIC][THELIC] = {0,0,0,0,0,0,0,0,
									1,1,1,1,1,1,0,0,
									0,1,1,1,1,0,0,0,
									1,0,0,1,1,1,1,1,
									1,1,1,1,1,1,1,1,
									0,1,1,1,1,0,0,0,
									0,1,0,1,0,0,0,0,
									1,0,1,0,1,0,0,0};
byte spriteHeli2[THELIC][THELIC] = {0,1,0,0,1,0,0,0,
									0,0,1,1,0,0,0,0,
									0,1,1,1,1,0,0,0,
									1,0,0,1,1,1,1,1,
									1,1,1,1,1,1,1,1,
									0,1,1,1,1,0,0,0,
									0,1,0,1,0,0,0,0,
									1,0,1,0,1,0,0,0};
byte spriteTiro[4][2] = {1,1,
						1,1,
						1,1,
						1,1};
Objeto aviao;
Objeto helicopteros[3];
Objeto tiro;
unsigned int placar;
unsigned char tecla;
bool frame;

void gameInit()
{
  aviao.x = 48 - TAVIAO;
  aviao.y = 84/2 - TAVIAO/2;
  for (int i = 0; i < 3; i++)
  {
	  helicopteros[i].x = VAZIO;
	  helicopteros[i].y = VAZIO;
  }
  tiro.x = VAZIO;
  tiro.y = VAZIO;
  placar = 0;
  
}

void loop_Input()
{
  for (unsigned char j = 0; j < 20; j++)
  {
    tecla = kpReadKey();
    if (tecla != 'L' && tecla != 'R' && tecla != 'B') 
      tecla = 0;
  }
}

void loop_Update()
{
   switch(tecla)
   {
		case 'L':
			if (aviao.y > 0)
				aviao.y-=2;
		break;
		case 'R':
			if (aviao.y < 84 - TAVIAO)
				aviao.y+=2;
		break;
		case 'B':
			if (tiro.x == VAZIO && tiro.y == VAZIO)
			{
				tiro.y = aviao.y + TAVIAO/2;
				tiro.x = 38;
			}
   }
   
   if (tiro.x != VAZIO && tiro.y != VAZIO)
   {
	   tiro.x-=4;
     if (tiro.x < 0)
     {
        tiro.x = VAZIO;
        tiro.y = VAZIO;
     }
   }
   for (int i = 0; i < 3; i++)
   {
	   if (helicopteros[i].x != VAZIO && helicopteros[i].y != VAZIO)
	   {
		   helicopteros[i].x++;
		   helicopteros[i].y += random(-1,2);
      if (helicopteros[i].x >= 48 || helicopteros[i].y < 0 || helicopteros[i].y >= 84)
      {
        helicopteros[i].x = VAZIO;
        helicopteros[i].y = VAZIO; 
      }
	   }
   }
   
   //TODO: teste de colisão
   for (int i = 0; i < 3; i++)
   {
    if ((helicopteros[i].y >= aviao.y && helicopteros[i].y <= aviao.y + TAVIAO && helicopteros[i].x + THELIC >= aviao.x) || 
        (aviao.y >= helicopteros[i].y && aviao.y <= helicopteros[i].y + THELIC && helicopteros[i].x + THELIC >= aviao.x))
    {
          gameInit();
    }

    if((tiro.y >= helicopteros[i].y && tiro.y <= helicopteros[i].y + THELIC && helicopteros[i].x <= tiro.x && helicopteros[i].x + THELIC >= tiro.x))
        {
          helicopteros[i].x = VAZIO;
          helicopteros[i].y = VAZIO;
          tiro.x = VAZIO;
          tiro.y = VAZIO;
        }
   }
   
   int sorteio = random(0,20);
   if (sorteio == 0)
   {
	   for (int i = 0; i < 3; i++)
	   {
		   if (helicopteros[i].x == VAZIO && helicopteros[i].y == VAZIO)
		   {
			   helicopteros[i].x = 0;
			   helicopteros[i].y = random(10,60);
			   break;
		   }
	   }
   }
}

void loop_Draw()
{
	clearScreen();
  clearFrameBuffer();
	
	if (aviao.x != VAZIO && aviao.y != VAZIO)
	{
		for (int i = aviao.x; i < TAVIAO + aviao.x; i++)
		{
			for (int j = aviao.y; j < TAVIAO + aviao.y; j++)
			{
				setPixel(j, i, spriteAviao[i-aviao.x][j-aviao.y]);
			}
		}
	}
 
	for (int k = 0; k < 3; k++)
	{
		if (helicopteros[k].x != VAZIO && helicopteros[k].y != VAZIO)
		{
			if (frame == false)
			{
				for (int i = helicopteros[k].x; i < THELIC + helicopteros[k].x; i++)
				{
					for (int j = helicopteros[k].y; j < THELIC + helicopteros[k].y; j++)
					{
							setPixel(j, i, spriteHeli1[i-helicopteros[k].x][j-helicopteros[k].y]);
					}
				}
				frame = true;
			}
			else
			{
				for (int i = helicopteros[k].x; i < THELIC + helicopteros[k].x; i++)
				{
					for (int j = helicopteros[k].y; j < THELIC + helicopteros[k].y; j++)
					{
							setPixel(j, i, spriteHeli2[i-helicopteros[k].x][j-helicopteros[k].y]);
					}
				}
				frame = false;
			}
		}
	}
	if (tiro.x != VAZIO && tiro.y != VAZIO)
	{
		for (int i = tiro.x; i < 4 + tiro.x; i++)
		{
			for (int j = tiro.y; j < 2 + tiro.y; j++)
			{
				setPixel(j, i, spriteTiro[i-tiro.x][j-tiro.y]);
			}
		}
	}
    writeBuffer();
  
	delay(100);
  
}

// Nessa função é feita a inicialização do hardware, da seed dos sorteios e do jogo em si.
void setup() 
{
  initializeDisplay(0xC0,0x07,0x014);
  kpInit();
  randomSeed(analogRead(0));
  gameInit();
}

// O loop principal:
// -> Leia o novo input do usuário, se houver.
// -> Atualize as informações do jogo.
// -> Redesenhe a tela.
void loop() 
{
  loop_Input();
  loop_Update();
  loop_Draw();
}
