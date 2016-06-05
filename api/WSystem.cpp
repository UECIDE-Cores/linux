#include <Arduino.h>

#include <signal.h>

void nativeCleanup() {
#ifdef __SERIAL_0
    Serial.end();
#endif
#ifdef __SERIAL_1
    Serial1.end();
#endif
#ifdef __SERIAL_2
    Serial2.end();
#endif
#ifdef __SERIAL_3
    Serial3.end();
#endif
#ifdef __SERIAL_4
    Serial4.end();
#endif
#ifdef __SERIAL_5
    Serial5.end();
#endif
    Console.end();
}

void intHandler(int sig) {
    nativeCleanup();
    exit(0);
}

void quitHandler(int sig) {
    nativeCleanup();
    exit(0);
}

void nativeInit() {
    signal(SIGINT, intHandler);
    signal(SIGQUIT, quitHandler);
    atexit(&nativeCleanup);
}
