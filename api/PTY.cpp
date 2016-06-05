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
#include <pty.h>

#include "PTY.h"

PTY::PTY() {
    _masterfd = -1;
    _slavefd = -1;
}

void PTY::begin(unsigned long baud) {
    begin();
}

void PTY::begin(unsigned long baud, byte config) {
    begin();
}

void PTY::begin() {
    struct termios options;
    if (_masterfd >= 0) {
        end();
    }
    openpty(&_masterfd, &_slavefd, _dev, NULL, NULL);
    fcntl(_masterfd, F_SETFL, 0);
    tcgetattr(_masterfd, &_savedOptions);
    tcgetattr(_masterfd, &options);
    cfmakeraw(&options);
    options.c_cflag |= (CLOCAL | CREAD);
    if (tcsetattr(_masterfd, TCSANOW, &options) != 0) {
        fprintf(stderr, "Can't set up PTY\n");
    }
}

void PTY::end() {
    if (_masterfd > -1) {
        tcsetattr(_masterfd, TCSANOW, &_savedOptions);

        close(_masterfd);
    }
    _masterfd = -1;
}

int PTY::available(void) {
    if (_masterfd < 0) {
        return 0;
    }

    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(_masterfd, &rfds);
    tv.tv_sec = 0;
    tv.tv_usec = 1;
    int retval = select(_masterfd+1, &rfds, NULL, NULL, &tv);
    if (retval) {
        return 1;
    }
    return 0;
}

int PTY::peek(void) {
    
    return -1;
}

int PTY::read(void) {
    if (!available()) {
        return -1;
    }
    uint8_t c;
    if (::read(_masterfd, &c, 1) <= 0) {
        return -1;
    }
    return c;
}

void PTY::flush() {
}

size_t PTY::write(uint8_t c) {
    if (_masterfd < 0) {
        return 0;
    }
    ::write(_masterfd, &c, 1);
    return 1;
}

PTY::operator bool() {
    return true;
}

char *PTY::getDeviceName() {
    return _dev;
}

