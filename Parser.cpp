#include "Parser.h"

void Parser::setXML(FILE *rxml) {
	xml = rxml;
}

int Parser::loadXML(const char *path) {
	xml = fopen(path, "r");
	if (!xml) return 0;
	return 1;
}

int Parser::nextInt(const char *tag, int tabs) {
	char temp[30];
	temp[0] = '\0';
	while (tabs) {
		strcat(temp, "\t");
		tabs--;
	}
	strcat(temp, "<");
	strcat(temp, tag);
	strcat(temp, ">");
	strcat(temp, "%d");
	strcat(temp, "</");
	strcat(temp, tag);
	strcat(temp, ">\n");
	int r;
	fscanf(xml, temp, &r);
	return r;
}

void Parser::nextString(char *dest, const char *tag) {
	char curr;
	int pos = 0;
	while (curr != '>') 
		fscanf(xml, "%c", &curr);
		
	while (curr != '<') {
		fscanf(xml, "%c", &curr);
		dest[pos++] = curr;
	}
	dest[pos - 1] = '\0';
	
	while (curr != '\n') 
		fscanf(xml, "%c", &curr);
}

int Parser::skipChars(int n) {
	char garb;
	while (n) {
		fscanf(xml, "%c", &garb);
		//printf("Discarding %c...\n", garb);
		n--;
	}
	if (feof(xml)) {
		//printf("Reached EOF\n"); 
		return -1;
	}
	return 0;
}

int Parser::skipStr(const char *str) {
	int n = strlen(str);
	return skipChars(n);
}

int Parser::peof() {
	return feof(xml);
}

int Parser::perr() {
	return ferror(xml);
}

char Parser::getChar() {
	char cc;
	fscanf(xml, "%c", &cc);
	return cc;
}

void Parser::ungetChar(char c) {
	ungetc(c, xml);
}

void Parser::close() {
	fclose(xml);
}
