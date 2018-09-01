#include <stdlib.h>
#include <stdio.h>

#include "Parser.h"

int main(int argv, char *argc[]) {
	printf("Iniciando...\n");
	xmlNode *raiz = start(argc[1]); 
	printf("XML avaliado com sucesso!\n\n");
	printHierarchy(raiz, 0);
	return 0;
}
