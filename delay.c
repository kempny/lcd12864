/********************************************************************************
*   delay:
*       This file is part of stepmotor - RaspberryPi stepper motor 
*       control library using A4988 controller
*       Copyright (C) 2021  Boguslaw Kempny kempny@stanpol.com.pl
*
*********************************************************************************
*    stepmotor is free software: you can redistribute it and/or modify
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
********************************************************************************/


#include <time.h>
#include <unistd.h>
#include <stdio.h>


struct timespec tim, tim1, tim2;

void Delay_sec(int sec)
{
 tim.tv_sec = sec;
 tim.tv_nsec = 0;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}

void Delay_mls(int milisec)
{
 tim.tv_sec = 0;
 tim.tv_nsec = milisec * 1000000;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}

void Delay_mcs(int microsec)
{
 tim.tv_sec = 0;
 tim.tv_nsec = microsec * 1000;
     while(nanosleep(&tim,&tim)==-1)
          continue;
}




//******************************************************************
// on Raspberry PI 3 B+ the shortest delay is few microseconds

void Delay_ns(int nanosec)
{
clock_gettime(CLOCK_REALTIME, &tim1);
clock_gettime(CLOCK_REALTIME, &tim2);
while(tim2.tv_nsec < (tim1.tv_nsec + nanosec))
 {
    clock_gettime(CLOCK_REALTIME, &tim2);
    if(tim2.tv_nsec < tim1.tv_nsec)
       tim2.tv_nsec= tim2.tv_nsec+1000000000;
 }

}
