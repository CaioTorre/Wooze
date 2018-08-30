#include "Parser.h"

void throwErr(xmlError code) {
	printf("Exiting with error code %d...\n", (int)code);
	exit(-1);
}

xmlNode *start(const char *path) {
	int endLock = 0;
	FILE *xml = fopen(path, "r");
	if (!xml) throwErr(fileNotFound);
	
	xmlNode *toDelete = parseXML(xml, "", 0, &endLock); 
	printf("Parsing complete [%s]\n", toDelete->tag);
	xmlNode *toReturn = toDelete->inside; //Skip the master class XML
	free(toDelete);
	return toReturn;
}

int readTag(FILE *xml, char *dest) {
	char c, tag[CTAG];
	int isCloser = 0, cpointer = 0;
	if ((c = fgetc(xml)) != '<') {
		ungetc(c, xml);
		return -1;
	}
	
	c = fgetc(xml);
	
	if (c == '/') { isCloser = 1; } else { dest[cpointer++] = c; }
	while ((c = fgetc(xml)) != '>') { dest[cpointer++] = c; }
	dest[cpointer] = '\0';
	
	return isCloser;
}

xmlNode *createNewNode() {
	xmlNode *created = (xmlNode*)malloc(sizeof(xmlNode));
	if (!created) throwErr(noMemory);
	return created;
}

xmlNode *parseXML(FILE *xml, const char *parentTag, int tabDepth, int *masterClose) {//const char *path) {
	int i;
	for (i = 0; i < tabDepth; i++) {
		if (fgetc(xml) != '\t') { throwErr(wrongIndentation); }
	}
	char current_tag[CTAG], closing_tag[CTAG], c;
	xmlNode *first;
	int char_pos;
	int tag_type = readTag(xml, current_tag);
	int closing_type;
	if (tag_type == -1) throwErr(wrongTag);
	
	xmlNode *current = createNewNode();
	first = current;
	
	strcpy(current->tag, current_tag);
	
	while (!(*masterClose)) { //Will probably fail in bad XMLs
		if ((c = fgetc(xml)) == '\n') { //Is parent node
			current->type = parent;
			current->inside = parseXML( xml, current_tag, tabDepth + 1, masterClose );
		} else { //Is immediate node
			current->type = immediate;
			current->inside = NULL;
			
			char_pos = 1;
			current->immed[0] = c;
			while ((c = fgetc(xml)) != '<') { current->immed[char_pos++] = c; }
			current->immed[char_pos] = '\0';
			
			ungetc('<', xml);
			closing_type = readTag(xml, closing_tag);
			
			if (closing_type == 0) throwErr(wrongTag);
			if (closing_type == -1) throwErr(wrongTag);
			if (strcmp(closing_tag, current_tag)) throwErr(wrongTag);
			
			c = fgetc(xml);
			if (c != '\n') throwErr(wrongTag); //Skip LN
		}
		
		for (i = 0; i < tabDepth - 1; i++) {
			if (fgetc(xml) != '\t') { printf("Wrong indentation (%d/%d)", i, tabDepth - 1); throwErr(wrongIndentation); }
		}
		
		if (!(*masterClose)) {
			if ((c = fgetc(xml)) == '<') { //Probably is a closing tag
				ungetc(c, xml);
			} else { //Probably is a ->next
				if ((c = fgetc(xml)) != '<') { printf("Didnt get a new tag! (%c)\n", c); throwErr(wrongIndentation); }
				ungetc(c, xml);
			}
			
			tag_type = readTag(xml, current_tag);
			if (tag_type == -1) throwErr(wrongTag);
			
			if (tag_type == 1) {
				if (!strcmp(current_tag, "XML")) {
					*masterClose = 1;
					return first;
				} else {
					if (!strcmp(parentTag, current_tag)) { 
						if (fgetc(xml) != '\n') throwErr(wrongTag); 
						return first; 
					} else { 
						throwErr(wrongTag); 
					}	
				}
			} //else
			
			if (!(*masterClose)) {
				xmlNode *nextNode = createNewNode();
				
				current->next = nextNode;
				current = nextNode;
				
				strcpy(current->tag, current_tag);
			}
		} else {
			return first;
		}
	}
}

void printHierarchy(xmlNode *current, int indentation) {
	int i;
	while (current) {
		if (current->type == parent) {
			for (i = 0; i < indentation; i++) printf("|\t");
			printf("{%s}\n", current->tag);
			printHierarchy(current->inside, indentation + 1);
		} else {
			for (i = 0; i < indentation; i++) printf("|\t");
			printf("[%s] %s\n", current->tag, current->immed);
		}
		current = current->next;
	}
}
