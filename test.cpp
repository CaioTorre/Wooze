#include <stdlib.h>
#include <stdio.h>

#include "Parser.h"

int main() {
	printf("Iniciando...\n");
	xmlNode *raiz = start("db.xml"); 
	printf("XML avaliado com sucesso!\n\n");
	printHierarchy(raiz, 0);
	return 0;
}
