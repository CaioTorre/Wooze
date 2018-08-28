#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 30

#define LINUX

void getContents(char *str);
void purgeLN (char *str);

int main() {
	int cont = 2;
	int addNewCon = 1;
	char contents[40];
	int value;
	FILE *fd = fopen("db.xml", "a");
	
	if (!fd) {
		printf("Erro de leitura!");
		exit(1);
	}
	
	do {
		if (cont == 1) {
			printf("Adicionar novo?");
			scanf("%d", &cont);
		}
		if (cont) {
			fprintf(fd, "<node>\n");
			printf("Id do novo no? ");
			scanf("%d", &value);
			fprintf(fd, "\t<id>%d</id>\n", value);
			
			printf("Nome do no (NULL para esquina): ");
			getContents(contents);
			fprintf(fd, "\t<nome>%s</nome>\n", contents);
			fprintf(fd, "\t<conns>\n");
			do {
				fprintf(fd, "\t\t<rua>\n");
				printf("\tno de destino..: ");
				scanf("%d", &value);
				fprintf(fd, "\t\t\t<id>%d</id>\n", value);
				
				printf("\tPeso do caminho: ");
				scanf("%d", &value);
				fprintf(fd, "\t\t\t<peso>%d</peso>\n", value);
				
				printf("\tNome da rua....: ");
				getContents(contents);
				fprintf(fd, "\t\t\t<nome>%d</nome>\n", value);
				fprintf(fd, "\t\t</rua>\n");
				
				printf("\n\tAdicionar nova rua saindo deste no? ");
				scanf("%d", &addNewCon);
			} while (addNewCon);
			fprintf(fd, "\t</conns>\n");
			fprintf(fd, "</node>\n");
			cont = 1;
		}
	} while (cont);
	fclose(fd);
	return 0;
}

void getContents(char *str) {
	#ifdef LINUX
	__fpurge(stdin);
	#endif
	#ifndef LINUX
	fflush(stdin);
	#endif
	fgets(str, MAX_NOME, stdin);
	purgeLN(str);
}

void purgeLN(char *str) {
	if ((strlen(str) > 0) && (str[strlen(str) - 1] == '\n'))
        str[strlen(str) - 1] = '\0';
}
