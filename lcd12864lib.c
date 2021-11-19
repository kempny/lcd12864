               
/********************************************************************************
* lcd12864lib.c
*       This file is part of lcd12864 - RaspberryPi library
*       for LCD display 12864,
*       serial interface 
*       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
*
*********************************************************************************
*    lcd12864 is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with lcd12864 library.  If not, see <http://www.gnu.org/licenses/>.
*
*********************************************************************************/



#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <pthread.h>
#include <lcd12864.h>
#include "wiringPi.h"
#include "font8x8.h"


#define DATA  1 // Mode - Sending data
#define CMD  0 // Mode - Sending command
#define MAXDISPL 16  // how many displays we handle

int konvline[4]={0x80, 0x90, 0x80, 0x90};

unsigned char byte;

struct dispdef parm[MAXDISPL];


int dispfree(int fdlcd);
void lcdLocI(int fdlcd, int col, int line);
void lcdCharI(int fdlcd, char val);

unsigned char cmd_datac;

unsigned char konv_bits[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

//*****************************************
// Display one line of text, truncated on the right if necessary 
// alpha mode

void lcdLine(int fdlcd, int x, int y, char* text)
{
int i, j; 

  if ( dispfree(fdlcd) ) return;

    lcdLocI (fdlcd, x, y) ; 

    j = ((strlen(text) + x) < parm[fdlcd].cols) ? strlen(text) : (parm[fdlcd].cols - x);
    for(i=0;i<j;i++)
    lcdCharI(fdlcd, text[i]);
    parm[fdlcd].free_disp=0;
}

//*****************************************
// Display one line of text, truncated on the right if necessary 
// graph mode

void lcdLineG(int fdlcd, int x, int y, char* text)
{
int i, j; 


    lcdLocG (fdlcd, x, y) ; 

    j = ((strlen(text) + x) < parm[fdlcd].cols) ? strlen(text) : (parm[fdlcd].cols - x);
    for(i=0;i<j;i++)
    lcdCharG(fdlcd, text[i]);
}

//*****************************************
// Display one line of text, truncated on the right if necessary 
// For internal use of library, display is already locked

void lcdLineI(int fdlcd, int x, int y, char* text)
{
int i, j; 

    lcdLocI (fdlcd, x, y) ; 

    j = ((strlen(text) + x) < parm[fdlcd].cols) ? strlen(text) : (parm[fdlcd].cols - x);
    for(i=0;i<j;i++)
    lcdCharI(fdlcd, text[i]);
}

//*****************************************
// Display block of text, wrap lines, truncated at the 
// end of display if necessary

void lcdBlock(int fdlcd, int x, int y, char* text)
{
int i, j;


  if ( dispfree(fdlcd) ) return;

    j = (strlen(text) + x + (parm[fdlcd].cols * y ) < parm[fdlcd].cols * parm[fdlcd].rows) ? strlen(text) : (parm[fdlcd].cols * parm[fdlcd].rows - (y * parm[fdlcd].cols) - x);


    for(i=0;i<j;i++)
     {
      lcdLocI(fdlcd, (x + i) % parm[fdlcd].cols, ((x + (y * parm[fdlcd].cols) + i) / parm[fdlcd].cols));
      lcdCharI(fdlcd, text[i]);
     }
    parm[fdlcd].free_disp=0;
}

/**********************************************************/
void *run(void *ptr)
{

int fdlcd;
int i;


  fdlcd =  (int)ptr;

  parm[fdlcd].pos = 0;
  parm[fdlcd].move = 1;
  while(parm[fdlcd].move)
   {
    if (! dispfree(fdlcd) )
     {
      if((parm[fdlcd].move_mode == 0)) // at end start from begining
       {
        lcdLineI(fdlcd, 0, parm[fdlcd].line, &parm[fdlcd].text[parm[fdlcd].pos]);
        if (parm[fdlcd].pos > strlen(parm[fdlcd].text) - parm[fdlcd].cols)
          lcdLineI(fdlcd, 
                   strlen(parm[fdlcd].text) - parm[fdlcd].pos, 
                   parm[fdlcd].line, 
                   "                              ");
          
         {
          parm[fdlcd].pos++;
          if (parm[fdlcd].pos == strlen(parm[fdlcd].text))
             parm[fdlcd].pos = 0;
         }
       }
      else
       if((parm[fdlcd].move_mode == 1)) // append begining of text to end of text
        {
          lcdLineI(fdlcd, 0, parm[fdlcd].line, &parm[fdlcd].text[parm[fdlcd].pos]);
          if(strlen(parm[fdlcd].text) > parm[fdlcd].cols)
           {
            parm[fdlcd].pos++;
            if (parm[fdlcd].pos > strlen(parm[fdlcd].text) - parm[fdlcd].cols -1)
             {
               lcdLineI(fdlcd, strlen(parm[fdlcd].text) - parm[fdlcd].pos +1, 
                              parm[fdlcd].line, parm[fdlcd].text);

               if (parm[fdlcd].pos > strlen(parm[fdlcd].text))
                 parm[fdlcd].pos = 1; //  not 0, the line is already displayed
             }
           }
        }
     }
    parm[fdlcd].free_disp = 0;
    for(i = 0; i < parm[fdlcd].speed; i++)
      Delay_mls(100);
   }
  return(0);
}

/**********************************************************/
// speed  - speed of animation, in 1/10 sec increments
// mode - mode of animation

void lcdRun(int fdlcd, int mode, int speed, int line, char* text)
{
    pthread_t thread_id;
    parm[fdlcd].speed = speed;
    parm[fdlcd].move_mode = mode;
    parm[fdlcd].line = line;
    strncpy(parm[fdlcd].text, text, ANIMLEN); 
    pthread_create (&thread_id, NULL, run, (void *)fdlcd);

}

/**********************************************************/
void lcdStop(int fdlcd)
{
  parm[fdlcd].move = 0;
}

//****************************************
// Wait for display free 
int dispfree(int fdlcd)
{
 static int j;
 for(j=0; j<100; j++)
  {
   if(parm[fdlcd].free_disp == 0)
    {parm[fdlcd].free_disp = 1;
     return 0;
    }
   Delay_mls(10);
  }

   printf("Display busy\n");
   return 1;
}


//*************************************
// Clear screen, cursor home

void lcdClr(int fdlcd)   {

  if ( dispfree(fdlcd) ) return;

  send_8(fdlcd, 0x01, CMD);
  Delay_mls(5);  // clear screen takes long time to complete
  send_8(fdlcd, 0x02, CMD);

  int i, j;
   for(i=0; i<16; i++)
    for(j=0; j<4; j++)
     parm[fdlcd].bufA[i][j] = 0x20;

  parm[fdlcd].curxA=0;
  parm[fdlcd].curyA=0;
  parm[fdlcd].free_disp=0;
}

//*************************************
// Define character


void lcdDefc(int fdlcd, int addr, unsigned char* matrix) {

int i;

send_8(fdlcd, 0x40 | addr * 8, CMD);
for (i=0; i<8; i++)
  send_8(fdlcd, matrix[i], DATA);

send_8(fdlcd, 0x80, CMD); //switch to DDRAM addressing
} 


//*************************************
// Set cursor to location row/column on LCD, alpha mode
// For internal use of library, display is already locked

void lcdLocI(int fdlcd, int col, int line)   {

  parm[fdlcd].curxA = col;
  parm[fdlcd].curyA = line;

  if (line > 1) col = col+16;

send_8(fdlcd, konvline[line] + col ,  CMD);
  send_8(fdlcd, konvline[line] | col / 2,  CMD);

  if (col % 2)
     send_8(fdlcd, parm[fdlcd].bufA[parm[fdlcd].curxA - 1][parm[fdlcd].curyA],  DATA);

}

//*************************************
// Set cursor to location row/column on LCD 

void lcdLoc(int fdlcd, int col, int line)   {

  parm[fdlcd].curxA = col;
  parm[fdlcd].curyA = line;

  if (line > 1) col = col+16;

  send_8(fdlcd, konvline[line] | col / 2,  CMD);
  if (col % 2)
     send_8(fdlcd, parm[fdlcd].bufA[parm[fdlcd].curxA - 1][parm[fdlcd].curyA],  DATA);

  parm[fdlcd].free_disp=0;
}

//*************************************
// Set cursor to location row/column on LCD graphic mode
// col  0 - 15   row  0 - 63

void lcdLocG(int fdlcd, int col, int row)   {

  if ( dispfree(fdlcd) ) return;


  if(row < 32)
    {
      send_8(fdlcd, 0x80 | row,  CMD);
      send_8(fdlcd, 0x80 | col / 2,  CMD);
    }
   else
    {
      send_8(fdlcd, 0x80 |(row-32),  CMD);
      send_8(fdlcd, 0x88 | col / 2,  CMD);
    }

  parm[fdlcd].curx = col;
  parm[fdlcd].cury = row;
  parm[fdlcd].free_disp=0;
}


//*************************************
// Display char at cursor position
// For internal use of library, display is already locked

void lcdCharI(int fdlcd, char val)   {

  send_8(fdlcd, val, DATA);

  parm[fdlcd].bufA[parm[fdlcd].curxA][parm[fdlcd].curyA] = val;
  parm[fdlcd].curxA++;
}

//*************************************
// Display char at curor position, alpha mode

void lcdChar(int fdlcd, char val)   {

  if ( dispfree(fdlcd) ) return;

  send_8(fdlcd, val, DATA);
 
  parm[fdlcd].bufA[parm[fdlcd].curxA][parm[fdlcd].curyA] = val;
  parm[fdlcd].curxA++;
  parm[fdlcd].free_disp=0;
}

//*************************************
// Display char at curor position, graphic mode

void lcdCharG(int fdlcd, unsigned int val)   {


int x, y;


   x = parm[fdlcd].curx;

   for(y=0;y<8;y++)
    {
      lcd_byte(fdlcd, x, parm[fdlcd].cury + y, font8x8[val][y]);
    }
   parm[fdlcd].curx++;

}

//*************************************
//*************************************
// Set display on/off, cursor on/off, blinking on/off
// 1 - ON, 0 - OFF, 99 - unchanged

void lcdSet(int fdlcd, int disp, int cur, int blin) // display, cursor, blink,
{

  if ( dispfree(fdlcd) ) return;

  if(disp != 99)
     parm[fdlcd].disp = disp;
  if(cur != 99)
     parm[fdlcd].cur = cur;
  if(cur != 99)
     parm[fdlcd].blin = blin;
  byte = (parm[fdlcd].disp << 2) | (parm[fdlcd].cur << 1) | parm[fdlcd].blin | 0x08;

  send_8(fdlcd, byte, CMD);
  parm[fdlcd].free_disp=0;
}


//*************************************
// Not to fast. 

void send_bit(int fdlcd, int bit)
{

//struct timespec tim3;

   digitalWrite(parm[fdlcd].serial, bit);

//   Delay_mcs(1);

   digitalWrite(parm[fdlcd].clock, HIGH);
   Delay_mcs(1);

   digitalWrite(parm[fdlcd].clock, LOW);
//   Delay_mcs(1);
}

//*************************************
void send_8(int fdlcd, int byte, int cmd_data)   {

int i;

//set RS bit
  digitalWrite(parm[fdlcd].rs,HIGH);  // enable interface
  for(i=0;i<5;i++) send_bit(fdlcd, 1); //start bits
  send_bit(fdlcd, 0); // write data 
  send_bit(fdlcd, cmd_data); // data/command
  send_bit(fdlcd, 0); //start data
  send_bit(fdlcd, (byte & 0x80) >> 7 ); // D7
  send_bit(fdlcd, (byte & 0x40) >> 6 ); // D6
  send_bit(fdlcd, (byte & 0x20) >> 5 ); // D5
  send_bit(fdlcd, (byte & 0x10) >> 4 ); // D4
  for(i=0;i<4;i++) send_bit(fdlcd, 0);  // data separator
  send_bit(fdlcd, (byte & 0x08) >> 3 ); // D3
  send_bit(fdlcd, (byte & 0x04) >> 2 ); // D2
  send_bit(fdlcd, (byte & 0x02) >> 1 ); // D1
  send_bit(fdlcd, (byte & 0x01)  );     // D0
  for(i=0;i<4;i++) send_bit(fdlcd, 0);  // stop bits
  digitalWrite(parm[fdlcd].rs,LOW);  // disable interface

}
//***************************************
void lcd_byte(int fdlcd, int col, int row, int value)
{

  if ( dispfree(fdlcd) ) return;



  int col1 = col;

       parm[fdlcd].bufG[col][row] = value ;

       if(col %2 == 1)
        {
         col1 = col - 1;
        }


      if(row < 32)
       {
         send_8(fdlcd, 0x80 | row,  CMD);
         send_8(fdlcd, 0x80 | col1 / 2,  CMD);
       }
      else
       {
         send_8(fdlcd, 0x80 |(row-32),  CMD);
         send_8(fdlcd, 0x88 | col1 / 2,  CMD);
       }
      if(col % 2 == 0)
       {
         send_8(fdlcd, value,  DATA);
       }
      else
       {
         send_8(fdlcd, parm[fdlcd].bufG[col - 1][row],  DATA);
         send_8(fdlcd, value,  DATA);
       }
  parm[fdlcd].free_disp=0;
}



//*************************************
// color = 1 set dot, 0 = clear dot

void lcd_dot(int fdlcd, int col, int row, int color)
{

  if ( dispfree(fdlcd) ) return;

  int col1 = col;

    if (color)
     {
       parm[fdlcd].bufG[col / 8][row] = parm[fdlcd].bufG[col /8][row] |  konv_bits[col % 8] ;
     }
    else
     {
       parm[fdlcd].bufG[col / 8][row] = parm[fdlcd].bufG[col /8][row] & ~ konv_bits[col % 8] ;
     }

    if((col /8) %2 == 1)
     {
      col1 = col - 8;
     }

      if(row < 32)
       {
         send_8(fdlcd, 0x80 | row,  CMD); 
         send_8(fdlcd, 0x80 | col1 / 16,  CMD); 
       }
      else
       {
         send_8(fdlcd, 0x80 |(row-32),  CMD); 
         send_8(fdlcd, 0x88 | col1 / 16,  CMD); 
       }
      if((col / 8) % 2 == 0)
       {
         send_8(fdlcd, parm[fdlcd].bufG[col /8][row],  DATA); 
       }
      else
       {
         send_8(fdlcd, parm[fdlcd].bufG[col /8 - 1][row],  DATA); 
         send_8(fdlcd, parm[fdlcd].bufG[col /8][row],  DATA); 
       }
  parm[fdlcd].free_disp=0;
}

//*************************************
void lcd_graph(int fdlcd)
{
  send_8(fdlcd, 0x20,  CMD); // set basic instruction
  Delay_mcs(100);
  send_8(fdlcd, 0x24,  CMD); // set extended instruction
  Delay_mcs(100);
  send_8(fdlcd, 0x26,  CMD); //  extended instruction, graphic display on
  Delay_mcs(100);
}

//*************************************
void lcd_alpha(int fdlcd)
{
  send_8(fdlcd, 0x20,  CMD); // set basic instruction
  Delay_mcs(100);
  send_8(fdlcd, 0x01,  CMD); //CLEAR display
  Delay_mls(15); // wait to complete clear display
  send_8(fdlcd, 0x02,  CMD); //HOME
  Delay_mcs(100);

}

//*************************************
void lcd_graph_cls(int fdlcd)
{
int x, y;

  for(y = 0; y < 64; y++)
    {
      if(y < 32)
       {
         send_8(fdlcd, 0x80 | y,  CMD); 
         send_8(fdlcd, 0x80,  CMD); 
       }
      else
       {
         send_8(fdlcd, 0x80 |(y-32),  CMD); 
         send_8(fdlcd, 0x88,  CMD); 
       }
      for(x = 0; x < 8; x++)
       {
         send_8(fdlcd, 0x00,  DATA); 
         send_8(fdlcd, 0x00,  DATA); 
         parm[fdlcd].bufG[x][y] = 0x00;
         parm[fdlcd].bufG[x + 1][y] = 0x00;
       }	
    }
  parm[fdlcd].curx=0;
  parm[fdlcd].cury =0;
}

//*************************************
int lcd_displ_file(int fdlcd, char* filename)
{
  int fd;
  int x, y;

  unsigned char pixels[80];

  fd=open(filename, O_RDONLY);

//read file signature
  read (fd, pixels, 3);
  if (pixels[0] != 'P' || pixels[1] != '4')
    {
     printf("Wrong file format, expected raw PBM, P4\n");
     exit(1);
    }
//skip comments
  for(;;)
   {
    read (fd, pixels,1);

    if(pixels[0] == '#')
      {
        for(;;)
         {
           read (fd, pixels, 1);
             if(pixels[0] == 0x0A) break; 
         }
      }
    else
      {
        break;
      }
   }
//skip  raster dimensions
  for(;;)
    {
      read (fd, pixels, 1);
        if(pixels[0] == 0x0A) break; 
    }

// display bitmap
  for(y = 0; y < 64; y++)
    {
      if(y < 32)
       {
         send_8(fdlcd , 0x80 | y,  CMD); 
         send_8(fdlcd, 0x80,  CMD); 
       }
      else
       {
         send_8(fdlcd, 0x80 |(y-32),  CMD);
         send_8(fdlcd, 0x88,  CMD);
       }
      for(x = 0; x < 8; x++)
       {
        read (fd, pixels, 1);
        send_8(fdlcd, pixels[0],  DATA);
        parm[fdlcd].bufG[x*2][y] = pixels[0];

        read (fd, pixels, 1);
        send_8(fdlcd, pixels[0],  DATA);
        parm[fdlcd].bufG[x*2 + 1][y] = pixels[0];
       }
    }
  return(0);
}

//*************************************
int lcdinit(int rs, int clock, int serial)
{

static int displcount =0;

// check wiringPi setup
  if (wiringPiSetup() == -1)
    {
          printf("wiringPi-Error\n");
      exit(1);
    }
  if (displcount == (MAXDISPL))
      {printf("To many displays, only %d are supported\n", MAXDISPL); exit(1);}

// Initialise display

  pinMode(rs, OUTPUT);
  digitalWrite(rs,LOW);  //disable interface

  pinMode(clock, OUTPUT);
  digitalWrite(clock,LOW);  
  pinMode(serial, OUTPUT);
  digitalWrite(serial,LOW); 

  parm[displcount].rs = rs;
  parm[displcount].clock = clock;
  parm[displcount].serial = serial;
  parm[displcount].cols = 16;
  parm[displcount].rows = 4;
  parm[displcount].disp = 1;
  parm[displcount].cur = 0;
  parm[displcount].blin = 0;


  parm[displcount].free_disp=0;
  send_8(displcount, 0x30,  CMD);
  Delay_mcs(150);
  send_8(displcount, 0x30,  CMD);
  Delay_mcs(50);
  send_8(displcount, 0x0C,  CMD); // display ON, cursor OFF, cursor blinking OFF
  Delay_mcs(150);
  send_8(displcount, 0x06,  CMD); //entry mode set
  Delay_mls(15); 
  send_8(displcount, 0x01,  CMD); //CLEAR display
  Delay_mls(15); // wait to complete clear display
  send_8(displcount, 0x02,  CMD); //HOME
  Delay_mls(15); 
// End of initialization


  return(displcount);
  displcount++;
}

