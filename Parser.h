#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Parser {
	FILE *xml;
public:
	void setXML(FILE *rxml);
	int loadXML(const char *path);
	int nextInt(const char *tag, int tabs);
	char getChar();
	void ungetChar(char c);
	void nextString(char *dest, const char *tag);
	int skipChars(int n);
	int skipStr(const char *str);
	int peof();
	int perr();
	void close();
};
