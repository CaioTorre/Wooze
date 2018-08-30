#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CTAG 20
#define CIMD 30

enum xmlError { fileNotFound = 1, noMemory, wrongIndentation, wrongTag };
enum xmlNodeType { parent = 0, immediate };

struct tp {
	char tag[CTAG];
	xmlNodeType type;
	struct tp *inside;
	char immed[CIMD];
	struct tp *next;
};

typedef struct tp xmlNode;

xmlNode *start(const char *path);
int readTag(FILE *xml, char *dest);
xmlNode *createNewNode();
xmlNode *parseXML(FILE *xml, const char *parentTag, int tabDepth, int *masterClose);
void printHierarchy(xmlNode *current, int indentation);
void throwErr(xmlError code);

int findTagInParent(xmlNode *parent, const char *tag, char *dest);
int getIntTag(xmlNode *parent, const char *tag);
xmlNode *findNestedParent(xmlNode *parent, const char *tag);
