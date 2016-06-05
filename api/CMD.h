#include <Arduino.h>

class CMDClass {
	private:
		int _argc;
		char **_argv;
        public:
		void setArguments(int c, char **v);
                int getNumberOfArguments();
                char *getArgument(int no);
};

extern CMDClass CMD;
