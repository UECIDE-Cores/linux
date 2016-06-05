/*
  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
  
  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "wiring_private.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#define    BOTHER 0010000

#ifdef __SERIAL_0
HardwareSerial Serial(__SERIAL_0);
#endif

#ifdef __SERIAL_1
HardwareSerial Serial1(__SERIAL_1);
#endif

#ifdef __SERIAL_2
HardwareSerial Serial2(__SERIAL_2);
#endif

#ifdef __SERIAL_3
HardwareSerial Serial3(__SERIAL_3);
#endif

#ifdef __SERIAL_4
HardwareSerial Serial4(__SERIAL_4);
#endif

#ifdef __SERIAL_5
HardwareSerial Serial5(__SERIAL_5);
#endif

HardwareSerial Console("/dev/tty");

#include "HardwareSerial.h"

void serialEventRun(void) {
#ifdef serialEvent_implemented
  if (Serial.available()) serialEvent();
#endif
#ifdef consoleEvent_implemented
  if (Console.available()) consoleEvent();
#endif
}

HardwareSerial::HardwareSerial(const char *device) {
    _dev = device;
    _fd = -1;
}

void HardwareSerial::begin(unsigned long baud) {
    begin(baud, SERIAL_8N1);
}

void HardwareSerial::begin(unsigned long baud, byte config) {
    struct termios options;
    if (_fd >= 0) {
        end();
    }
    _fd = open(_dev, O_RDWR|O_NOCTTY);
    fcntl(_fd, F_SETFL, 0);
    tcgetattr(_fd, &_savedOptions);
    tcgetattr(_fd, &options);
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag &= ~CBAUD;
    if (strcmp(_dev, "/dev/tty") == 0) {
        options.c_lflag |= ISIG;
    }
    options.c_cflag |= BOTHER;
    options.c_ispeed = baud;
    options.c_ospeed = baud;
    if (tcsetattr(_fd, TCSANOW, &options) != 0) {
        fprintf(stderr, "Can't set up serial\n");
    }
}

void HardwareSerial::end() {
    if (_fd > -1) {
        tcsetattr(_fd, TCSANOW, &_savedOptions);

        close(_fd);
    }
    _fd = -1;
}

int HardwareSerial::available(void) {
    if (_fd < 0) {
        return 0;
    }

    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(_fd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    int retval = select(_fd+1, &rfds, NULL, NULL, &tv);
    if (retval) {
        return 1;
    }
    return 0;
}

int HardwareSerial::peek(void) {
    
    return -1;
}

int HardwareSerial::read(void) {
    if (!available()) {
        return -1;
    }
    uint8_t c;
    if (::read(_fd, &c, 1) <= 0) {
        return -1;
    }
    return c;
}

void HardwareSerial::flush() {
}

size_t HardwareSerial::write(uint8_t c) {
    if (_fd < 0) {
        return 0;
    }
    ::write(_fd, &c, 1);
    return 1;
}

HardwareSerial::operator bool() {
	return true;
}


