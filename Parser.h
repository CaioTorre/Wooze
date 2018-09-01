#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CTAG 20
#define CIMD 30

enum xmlError { fileNotFound = 1, noMemory, wrongIndentation, wrongTag, badAttributeTag, badAttributeVal };
enum xmlNodeType { parent = 0, immediate };

struct at {
	char tag[CTAG];
	char val[CIMD];
	struct at *next;
};

typedef struct at attribNode;

struct tp {
	char tag[CTAG];
	xmlNodeType type;
	struct at *attributes;
	struct tp *inside;
	char immed[CIMD];
	struct tp *next;
};

typedef struct tp xmlNode;

xmlNode *start(const char *path);
int readTag(FILE *xml, char *dest, attribNode *attribs);
xmlNode *createNewNode();
xmlNode *parseXML(FILE *xml, const char *parentTag, int tabDepth, int *masterClose);
void printHierarchy(xmlNode *current, int indentation);
void printAttributes(attribNode *current);

void throwErr(xmlError code);
void throwErr(xmlError code, const char *aux_msg);

int findTagInParent(xmlNode *parent, const char *tag, char *dest);
int getIntTag(xmlNode *parent, const char *tag);
xmlNode *findNestedParent(xmlNode *parent, const char *tag);
