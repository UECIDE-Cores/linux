/*
  wiring_digital.c - digital input and output functions
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

  Modified 28 September 2010 by Mark Sproul

  $Id: wiring.c 248 2007-02-03 15:36:30Z mellis $
*/

#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"

#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

static const char *gpio_root = "/sys/class/gpio";

static void exportGpio(int gpio) {
	char temp[100];
	sprintf(temp, "%s/export", gpio_root);
	int fd = open(temp, O_WRONLY);
	if (fd < 0) {
		return;
	}
	sprintf(temp, "%d\n", gpio);
	write(fd, temp, strlen(temp));
	close(fd);
}

boolean isGpioExported(int gpio) {
	char temp[100];
	struct stat sb;
	sprintf(temp, "%s/gpio%d", gpio_root, gpio);

	if (stat(temp, &sb) == 0) {
		return true;
	}
	return false;
}

static int openGpio(int gpio) {
	char temp[100];
	sprintf(temp, "%s/gpio%d/value", gpio_root, gpio);
	return open(temp, O_RDWR);
}

void pinMode(uint8_t pin, uint8_t mode) {
	if (pin >= NUM_GPIO) {
		return;
	}
	struct _pin *p = &(_pins_gpio[pin]);

    if (p->gpio == __NOT_A_PIN) {
        return;
    }

	if (!isGpioExported(p->gpio)) {
		exportGpio(p->gpio);
	}

	if (p->thread != -1) {
		p->data = -1;
		pthread_join(p->thread, NULL);
		p->thread = -1;
	}

	char temp[100];
	sprintf(temp, "%s/gpio%d/direction", gpio_root, p->gpio);
	int fd = open(temp, O_RDWR);
	if (fd < 0) {
		return;
	}

	switch (mode) {
		case INPUT_PULLUP:
		case INPUT:
			write(fd, "in\n", 3);
			break;
		case OUTPUT:
			write(fd, "out\n", 4);
			break;
	}
	close(fd);
}

void digitalWrite(uint8_t pin, uint8_t val) {
	if (pin >= NUM_GPIO) {
		return;
	}
	struct _pin *p = &(_pins_gpio[pin]);
    if (p->gpio == __NOT_A_PIN) {
        return;
    }

	if (p->thread != -1) {
		p->data = -1;
		pthread_join(p->thread, NULL);
		p->thread = -1;
	}
	char temp[100];
	sprintf(temp, "%s/gpio%d/value", gpio_root, p->gpio);
	int fd = open(temp, O_WRONLY);
	if (fd < 0) {
		return;
	}
	write(fd, val ? "1" : "0", 1);
	close(fd);
}

int digitalRead(uint8_t pin) {
	if (pin >= NUM_GPIO) {
		return 0;
	}
	struct _pin *p = &(_pins_gpio[pin]);
    if (p->gpio == __NOT_A_PIN) {
        return 0;
    }

	if (p->thread != -1) {
		p->data = -1;
		pthread_join(p->thread, NULL);
		p->thread = -1;
	}
	char temp[100];

	sprintf(temp, "%s/gpio%d/value", gpio_root, p->gpio);
	int fd = open(temp, O_RDWR);
	if (fd < 0) {
		return 0;
	}

	char c;
	read(fd, &c, 1);
	close(fd);
	if (c == '1') {
		return 1;
	}
	return 0;
}
