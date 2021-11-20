/********************************************************************************
* lcd12864.h:
*       This file is part of lcd12864 - RaspberryPi library
*       for LCD 12864 display 
*       serial interface 
*       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
*
*********************************************************************************
*    lcd12864 is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    lcd12864 is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
*
*********************************************************************************/

void Delay_sec(int seconds);
void Delay_mcs(int microsec);
void Delay_mls(int milisec);
void Delay_ns(int nanosec);
void send_8(int fdlcd, int byte, int cmd_data);
void lcdSet(int fdlcd, int disp, int cur, int blin); //display, cursor, blink on/off
void lcdLine(int fdlcd, int x, int y, char* text);
void lcdLineG(int fdlcd, int x, int y, char* text);
void lcdClr(int fdlcd);
int lcdinit(int rs, int clock, int serial);
void lcdLoc(int fdlcd, int col, int line);
void lcdLocG(int fdlcd, int col, int row);
void lcdChar(int fdlcd, char val); 
void lcdCharG(int fdlcd, unsigned int val);
void lcdBlock(int fdlcd, int x, int y, char* text);
void lcdRun(int fdlcd, int mode, int speed, int line, char* text);
void lcdStop(int fdlcd);
int lcd_displ_file(int fdlcd, char* filename);
void lcd_dot(int fdlcd, int x, int y, int color);
void lcd_byte(int fdlcd, int x, int y, int value);
void lcd_graph(int fdlcd);
void lcd_alpha(int fdlcd);
void lcd_graph_cls(int fdlcd);


#define ANIMLEN 200

struct  dispdef
{
  int rs;
  int clock;
  int serial;
  unsigned int cols;
  int free_disp;
  unsigned char bufG[16][64]; // graphic buffer
  unsigned char bufA[16][4]; // alphanumeric buffer
  int curx;
  int cury;
  int curxA;
  int curyA;
  int cur;
  int blin;
  int disp;
  unsigned int rows;
  int move;
  int speed;
  int line;
  unsigned int pos;
  int move_mode;
  char text[ANIMLEN];
};

