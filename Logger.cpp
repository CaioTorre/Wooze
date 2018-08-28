#include "Logger.h"

void Logger::logString(const char *msg, level mlvl) {
	if (lvl <= mlvl) {
		char al[6][10];
		strcpy(al[0], "Fine");
		strcpy(al[1], "Config");
		strcpy(al[2], "Info");
		strcpy(al[3], "Warning");
		strcpy(al[4], "Error");
		strcpy(al[5], "Severe");
	
		printf("[%s]: %s\n", al[mlvl - 1], msg);
	}
}

void Logger::logB(level mlvl) {
	if (lvl <= mlvl) {
		char al[6][10];
		strcpy(al[0], "Fine");
		strcpy(al[1], "Config");
		strcpy(al[2], "Info");
		strcpy(al[3], "Warning");
		strcpy(al[4], "Error");
		strcpy(al[5], "Severe");
	
		printf("[%s]: %s\n", al[mlvl - 1], buffer);
	}
}

void Logger::setLevel(level mlvl) {
	if (mlvl > 0 && mlvl < 7)
		lvl = mlvl;
}
