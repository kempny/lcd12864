/********************************************************************************
* test.c 
*      This file is part of lcd12864 - RaspberryPi library
*       for LCD 128x64 dots display,
*       serial interface 
*       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
*
*********************************************************************************
*    lcdi2c is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    stepmotor is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
*
*********************************************************************************
**   demo program for single display                                           **    
**   compile with:                                                             **
**     make                                                                    **
**   run:                                                                      **
**     ./test                                                                  **
********************************************************************************/

#include <string.h> 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>
#include <pthread.h>
#include <lcd12864.h>


void *disptime(void *ptr)
{
int fdlcd;
int i;

fdlcd =  (int)ptr;

time_t czas1;
struct tm *loctime;
char buftime[25];

  for(;;)
   {
    time (&czas1);
    loctime = localtime (&czas1);
    strftime (buftime, 25, "%a %H:%M:%S", loctime);
    lcdLine(fdlcd,0,3,buftime);
    Delay_mls(500);
   }
}

/* S T A R T */
void main (argc, argv)
int argc;
char *argv[];
{

int i, j;

int lcd;
// Init LCD display
lcd=lcdinit(29, 28, 27);

// switch to graphical mode

lcd_graph(lcd);

lcd_graph_cls(lcd);

lcdLocG(lcd, 3, 4);

lcdCharG(lcd, 'R');
lcdCharG(lcd, 'A');
lcdCharG(lcd, 'S');
lcdCharG(lcd, 'P');
lcdCharG(lcd, 'B');
lcdCharG(lcd, 'E');
lcdCharG(lcd, 'R');
lcdCharG(lcd, 'R');
lcdCharG(lcd, 'Y');

lcdLocG(lcd, 6, 12);
 
lcdCharG(lcd, 'P');
lcdCharG(lcd, 'I');

lcdLocG(lcd, 3, 20);
lcdCharG(lcd, 'L');
lcdLocG(lcd, 4, 22);
lcdCharG(lcd, 'C');
lcdLocG(lcd, 5, 24);
lcdCharG(lcd, 'D');
lcdLocG(lcd, 6, 26);
lcdCharG(lcd, '1');
lcdLocG(lcd, 7, 28);
lcdCharG(lcd, '2');
lcdLocG(lcd, 8, 30);
lcdCharG(lcd, '8');
lcdLocG(lcd, 9, 32);
lcdCharG(lcd, '6');
lcdLocG(lcd, 10, 34);
lcdCharG(lcd, '4');

lcdLocG(lcd, 3, 54);

lcdCharG(lcd, 'C');
lcdCharG(lcd, ' ');
lcdCharG(lcd, 'L');
lcdCharG(lcd, 'i');
lcdCharG(lcd, 'b');
lcdCharG(lcd, 'r');
lcdCharG(lcd, 'a');
lcdCharG(lcd, 'r');
lcdCharG(lcd, 'y');

sleep(2);
lcd_displ_file(lcd, "./pilogo.pbm");

sleep(1);

lcdLineG(lcd, 10, 23, "Test");

sleep(2);

lcd_graph_cls(lcd);
 
for(i=0;i<64;i++)
  lcd_dot(lcd, i+20,i,1);
for(i=0;i<64;i++)
  lcd_dot(lcd, i+20,33,1);
for(i=0;i<64;i++)
  lcd_dot(lcd, 86-i,i,1);
for(i=0;i<64;i++)
  lcd_dot(lcd, 53,i,1);

for(j=1;j<24;j++)
 for(i=0;i<64;i++)
  lcd_dot(lcd, j * 2 +30,i,0);

sleep(1);



// Alphanumeric part

lcd_alpha(lcd);
lcdClr(lcd);
  lcdLine(lcd,2,1,"Alphanumeric");
  lcdLine(lcd,6,2,"mode");
sleep(2);



// Display some text


lcdClr(lcd);
  lcdLine(lcd,0,0,"Line 0");
sleep(1);
  lcdLine(lcd,1,1,"Line 1");
sleep(1);
  lcdLine(lcd,2,2,"Line 2");
sleep(1);
  lcdLine(lcd,3,3,"Line 3");
sleep(1);

 lcdClr(lcd);


// Display block of text
  lcdLine(lcd,0,0,"Display block");
  sleep(1);
  lcdBlock(lcd,3,1,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvw");
  sleep(1);
  lcdClr(lcd);
  lcdLoc(lcd, 0,0);
sleep(2);
// Create thread to display date and time on display 0

  lcdLine(lcd,0,0,"Start thread");
  lcdLine(lcd,0,1,"to display time");
sleep(1);
  pthread_t thread_id;

  pthread_create (&thread_id, NULL, disptime, (void *)lcd);
sleep(2);


//Display animated text

  lcdClr(lcd);
// Mode 1
  lcdLine(lcd,0,0,"Scroll mode 1");
  lcdRun(lcd, 1, 3, 1, "To be or not to be,  ");
//  lcdRun(lcd, 1, 3, 1, "++To be or not to be -- ");
  sleep(10);
  lcdStop(lcd);
  sleep(2);
  lcdClr(lcd);

//Mode 0
  lcdLine(lcd,0,0,"Scroll mode 0");
//  lcdRun(lcd, 0, 2, 1, "To be or not to be, that is the question");
  lcdRun(lcd, 0, 2, 1, " To be or not to be");
  sleep(10);
  lcdStop(lcd);
  sleep(2);
  lcdClr(lcd);
  lcdLine(lcd,4,1,"The End");
}                              




