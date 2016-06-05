#ifndef _WIRING_PINS_H
#define _WIRING_PINS_H

#include <pthread.h>

#define __NOT_A_PIN -1

struct _pin {
	int gpio;
	pthread_t thread;
	int data;
};

#endif
