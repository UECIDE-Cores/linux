#include <CMD.h>

CMDClass CMD;

int CMDClass::getNumberOfArguments() {
	return _argc;
}

char *CMDClass::getArgument(int no) {
	if (no >= _argc) {
		return NULL;
	}
	return _argv[no];
}

void CMDClass::setArguments(int c, char **v) {
	_argc = c;
	_argv = v;
}
