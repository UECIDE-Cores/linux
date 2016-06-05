/*
  wiring.c - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id$
*/

#include "wiring_private.h"
#include <unistd.h>
#include <sys/time.h>

struct timeval bootTime;

unsigned long millis()
{
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    uint64_t then = bootTime.tv_sec * 1000 + bootTime.tv_usec / 1000;
    uint64_t now = currentTime.tv_sec * 1000 + currentTime.tv_usec / 1000;
    now = now - then;
    
    return now;
}

unsigned long micros() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec * 100000) + now.tv_usec;
}

void delay(unsigned long ms)
{
    usleep(ms * 1000);
}

void delayMicroseconds(unsigned int us)
{
    usleep(us);
}

extern void nativeInit();

void init() {
    gettimeofday(&bootTime, NULL);
    nativeInit();
}

