#include "Parser.h"

void throwErr(xmlError code, const char *aux_msg) {
	throwErr(code);
	printf("Auxiliar: %s\n", aux_msg);
	exit(-1);
}

void throwErr(xmlError code) {
	printf("Got error code %d: ", (int)code);
	switch (code) {
		case fileNotFound:
			printf("File not found!");
			break;
		case noMemory:
			printf("Out of memory (HEAP OVERFLOW)");
			break;
		case wrongIndentation:
			printf("Wrong indentation");
			break;
		case wrongTag:
			printf("Wrong tag");
			break;
		case badAttributeTag:
			printf("Bad Attribute tag");
			break;
		case badAttributeVal:
			printf("Bad Attribute value");
			break;
		case illegalSize:
			printf("Badly sized string");
		default:
			printf("Unknown error code");
			break;
	}
	printf("\n");
}

xmlNode *start(const char *path) {
	int endLock = 0;
	FILE *xml = fopen(path, "r");
	if (!xml) throwErr(fileNotFound, path);
	
	xmlNode *toDelete = parseXML(xml, "", 0, &endLock); 
	//printf("Parsing complete [%s]\n", toDelete->tag);
	xmlNode *toReturn = toDelete->inside; //Skip the master class XML
	free(toDelete);
	return toReturn;
}

int readTag(FILE *xml, char *dest, attribNode **attribs) { //Implement attribute reading here
	char c, tag[CTAG];
	
	int isCloser = 0, cpointer = 0, tpointer = 0;
	if ((c = fgetc(xml)) != '<') {
		printf("Expected a (<), got a (%c)\n", c);
		ungetc(c, xml);
		return -1;
	}
	
	c = fgetc(xml);
	
	int readingTags = 0;
	
	attribNode *current;
	attribNode *first = NULL;
	attribNode *prev = NULL;

	int firstAttrib = 1;
	if (c == '/') { isCloser = 1; } else { dest[cpointer++] = c; } 
	while ((c = fgetc(xml)) != '>') { 
		if (!readingTags) {
			if (c == ' ') {
				readingTags = 1;
				//dest[cpointer] = '\0';
			} else {
				dest[cpointer++] = c; 
			}
		} else {
			//if (!firstAttrib) { if ( fgetc(xml) != ' ' ) { throwErr(badAttributeTag, "Expected ' '"); } }
			current = (attribNode*)malloc(sizeof(attribNode));
			if (!current) { throwErr(noMemory); }
			if (!first) first = current;
			
			if (firstAttrib) {
				current->tag[0] = c;
				tpointer = 1;
			} else {
				tpointer = 0;
			}
			while ((c = fgetc(xml)) != '=') {
				//printf("Appending %c @ %d\n", c, tpointer);
				current->tag[tpointer++] = c;
			}
			if (tpointer >= CTAG) {
				throwErr(illegalSize, "Wrong tag size for attribute");
			}
			
			//printf("Ending with \\0 @ %d\n", tpointer);
			current->tag[tpointer] = '\0';
			
			//printf("Result: %s\n", current->tag);
			
			tpointer = 0;
			if (fgetc(xml) != '\"') throwErr(badAttributeTag, "Expected \"");
			
			while ((c = fgetc(xml)) != '\"') {
				current->val[tpointer++] = c;
			}
			
			if (tpointer >= CIMD) {
				throwErr(illegalSize, "Wrong immed size for attribute");
			}
			
			current->val[tpointer] = '\0';
			
			if (prev) prev->next = current;
			prev = current;
			
			firstAttrib = 0;
		}
	}
	
	if (cpointer >= CTAG) {
		throwErr(illegalSize, "Wrong tag size for node");
	}
			
	dest[cpointer] = '\0';
	
	*attribs = first;
	return isCloser;
}

xmlNode *createNewNode() {
	xmlNode *created = (xmlNode*)malloc(sizeof(xmlNode));
	if (!created) throwErr(noMemory);
	return created;
}

xmlNode *parseXML(FILE *xml, const char *parentTag, int tabDepth, int *masterClose) {//const char *path) {
	int i;
	char c;
	attribNode *attribs;
	for (i = 0; i < tabDepth; i++) {
		if ((c = fgetc(xml)) != '\t') { throwErr(wrongIndentation, "Tabbing incorrect"); }
	}
	char current_tag[CTAG], closing_tag[CTAG];
	xmlNode *first;
	int char_pos;
	int tag_type = readTag(xml, current_tag, &attribs);
	int closing_type, force_closing;
	if (tag_type == -1) throwErr(wrongTag);
	
	xmlNode *current = createNewNode();
	current->attributes = attribs;
	first = current;
	
	strcpy(current->tag, current_tag);
	
	while (!(*masterClose)) {
		if ((c = fgetc(xml)) == '\n') { //Is parent node
			//printf("Tag %s is a parent\n", current->tag);
			current->type = parent;
			current->inside = parseXML( xml, current_tag, tabDepth + 1, masterClose );
		} else { //Is immediate node
			current->type = immediate;
			current->inside = NULL;
			
			char_pos = 0;
			if (c != '<') { 
				current->immed[char_pos++] = c;
				while ((c = fgetc(xml)) != '<') { current->immed[char_pos++] = c; }
			} 
			if (char_pos++ >= CIMD) {
				throwErr(illegalSize, "Wrong immed size for node");
			}
			
			current->immed[char_pos] = '\0';
			
			ungetc('<', xml);
			closing_type = readTag(xml, closing_tag, &attribs); //Should be closing tag
			
			if (attribs) throwErr(badAttributeTag, "Closing tag should not have attributes");
			
			if (closing_type == 0) throwErr(wrongTag);
			if (closing_type == -1) throwErr(wrongTag);
			if (strcmp(closing_tag, current_tag)) throwErr(wrongTag);
			
			c = fgetc(xml);
			if (c != '\n') throwErr(wrongTag); //Skip LN
		}
		
		if (!strcmp(parentTag, "")) { //Master tag doesnt have a next one
			current->next = NULL;
			return current;
		}
		
		c = fgetc(xml);
		if (feof(xml)) { //End of file
			current->next = NULL;
			return first;
		}
		ungetc(c, xml);
		
		for (i = 0; i < tabDepth - 1; i++) {
			if (fgetc(xml) != '\t') { printf("Wrong indentation (%d/%d)", i, tabDepth - 1); throwErr(wrongIndentation); }
		}

		//if (!(*masterClose)) {
			if ((c = fgetc(xml)) == '<') { //Probably is a closing tag
				ungetc(c, xml);
				force_closing = 1;
			} else { //Probably is a ->next (should have gotten another tab)
				if ((c = fgetc(xml)) != '<') {  throwErr(wrongIndentation, "Expected new tag!"); }
				ungetc(c, xml);
				force_closing = 0;
			}
			
			tag_type = readTag(xml, current_tag, &attribs);
			if (tag_type == -1) throwErr(wrongTag);
			
			if (tag_type != force_closing) throwErr(wrongTag);
			
			if (tag_type == 1) {
				//if (!strcmp(current_tag, "XML")) {
					//*masterClose = 1;
					//return first;
				//} else {
					if (!strcmp(parentTag, current_tag)) { 
						if (fgetc(xml) != '\n') throwErr(wrongTag); 
						return first; 
					} else { 
						throwErr(wrongTag); 
					}	
				//}
			} //else
			
			//if (!(*masterClose)) {
			xmlNode *nextNode = createNewNode();
			
			current->next = nextNode;
			current = nextNode;
			current->attributes = attribs;
			
			strcpy(current->tag, current_tag);
			//}
		//} else {
			//return first;
		//}
	}
}

void printHierarchy(xmlNode *current, int indentation) {
	int i;
	while (current) {
		if (current->type == parent) {
			for (i = 0; i < indentation; i++) printf("|\t");
			printf("{%s", current->tag);
			printAttributes(current->attributes);
			printf("}\n");
			printHierarchy(current->inside, indentation + 1);
		} else {
			for (i = 0; i < indentation; i++) printf("|\t");
			printf("[%s", current->tag);
			printAttributes(current->attributes);
			if (strcmp(current->immed, ""))
				printf("]: %s\n", current->immed);
			else
				printf("]\n");
		}
		
		current = current->next;
	}
}

void printAttributes(attribNode *current) {
	while (current) {
		printf(" (%s)=\"%s\"", current->tag, current->val);
		current = current->next;
	}
}

int findTagInParent(xmlNode *parent, const char *tag, char *dest) {
	xmlNode *current = parent->inside;
	while (current) {
		if (!strcmp(tag, current->tag)) { strcpy(dest, current->immed); return 0; }
		current = current->next;
	}
	printf("Could not find tag %s in parent...\n", tag);
	return -1;
}

int getIntAtt(xmlNode *node, const char *tag) {
	attribNode *aux = node->attributes;
	int t;
	while (aux && strcmp(tag, aux->tag)) aux = aux->next;
	if (!aux) throwErr(noSuchAttribute, tag); else {
		sscanf(aux->val, "%d", &t);
		return t;
	}
	return -1;
}

int getStrAtt(xmlNode *node, const char *tag, char dest[]) {
	attribNode *aux = node->attributes;
	while (aux && strcmp(tag, aux->tag)) aux = aux->next;
	if (!aux) { throwErr(noSuchAttribute, tag); return 0; } else {
		strcpy(dest, aux->val);
		return 1;
	} 
}

int getIntTag(xmlNode *parent, const char *tag) {
	char buffer[CIMD];
	int res;
	//printf("Current parent is of type %s...\n", parent->tag);
	xmlNode *current = parent->inside;
	while (current) {
		if (!strcmp(tag, current->tag)) { 
			strcpy(buffer, current->immed); 
			sscanf(buffer, "%d", &res);
			return res;
		}
		current = current->next;
	}
	printf("Could not find tag %s for int...\n", tag);
	return -1;
}

xmlNode *findNestedParent(xmlNode *parent, const char *tag) {
	xmlNode *current = parent->inside;
	while (current) {
		if (!strcmp(tag, current->tag)) { return current; }
		current = current->next;
	}
	printf("Could not find tag %s for nested...\n", tag);
	return NULL;
}
