/*
  wiring_analog.c - analog input and output
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

#include <pthread.h>

#include "wiring_private.h"
#include "pins_arduino.h"

static const char *gpio_root = "/sys/class/gpio";

uint8_t analog_reference = DEFAULT;

void analogReference(uint8_t mode)
{
	analog_reference = mode;
}

const int _pwm_mul = 3;

void *pwmThreadHandler(void *pinptr) {
	char temp[100];
	int pin = *(int *)pinptr;
	if (pin >= NUM_GPIO) {
		return NULL;
	}
	struct _pin *p = &_pins_gpio[pin];
    if (p->gpio == __NOT_A_PIN) {
        return NULL;
    }

	sprintf(temp, "%s/gpio%d/value", gpio_root, p->gpio);
	int fd = open(temp, O_RDWR);
	if (fd < 0) {
		return NULL;
	}
	while (p->data >= 0) {
		int val = p->data;
		if (val <= 0) {
			write(fd, "0\n", 2);
			usleep(255 * _pwm_mul);
			continue;
		}
		if (val >= 255) {
			write(fd, "1\n", 2);
			usleep(255 * _pwm_mul);
			continue;
		}
		write(fd, "1\n", 2);
		usleep(val * _pwm_mul);
		write(fd, "0\n", 2);
		usleep((255 * _pwm_mul) - (val * _pwm_mul));
	}
	close(fd);
	return NULL;
}

void analogWrite(uint8_t pin, int val)
{
    if (pin >= NUM_GPIO) {
        return;
    }
	pinMode(pin, OUTPUT);
	struct _pin *p = &_pins_gpio[pin];
    if (p->gpio == __NOT_A_PIN) {
        return;
    }

	p->data = val;
	if (p->thread == -1) {
		pthread_create(&(p->thread), NULL, &pwmThreadHandler, (void *)&pin);
	}
}

int analogRead(uint8_t pin) {
#ifdef USE_BOARD_ANALOG_READ
    return __board_analog_read(pin);
#endif
    return 0;
}
