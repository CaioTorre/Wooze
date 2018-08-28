#include <stdlib.h>
#include <stdio.h>
#include <string.h>

enum level {fine = 1, config, info, warning, error, severe};

class Logger {
	level lvl;
public:
	char buffer[100];
	void logString(const char *msg, level mlvl);
	void logB(level mlvl);
	void setLevel(level mlvl);
};
