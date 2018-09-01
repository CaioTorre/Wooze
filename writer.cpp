#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 30

#define LINUX

#define COMPACT_XML

void getContents(char *str);
void purgeLN (char *str);

int main() {
	int cont = 2;
	int addNewCon = 1;
	char contents[40];
	int value, weight;
	FILE *fd = fopen("db.xml", "a");
	
	if (!fd) {
		printf("Erro de leitura!");
		exit(1);
	}
	

	int sz;
	fseek(fd, 0L, SEEK_END);
	sz = ftell(fd);
	
	if (!sz)
		fprintf(fd, "<?xml version=\"1.0\" encoding=\"UTF8\">\n");
		
	rewind(fd);

	
	do {
		if (cont == 1) {
			printf("Adicionar novo (0/1)?");
			scanf("%d", &cont);
		}
		if (cont) {
			printf("Id do novo no? ");
			scanf("%d", &value);
			
			printf("Nome do no (NULL para esquina): ");
			getContents(contents);
			
			#ifdef COMPACT_XML
				fprintf(fd, "\t<node id=\"%d\" nome=\"%s\">\n", value, contents);
			#else
				fprintf(fd, "\t<node>\n\t\t<id>%d</id>\n\t\t<nome>%s</nome>\n\t\t<conns>\n", value, contents); 
			#endif
			do {
				printf("\tNo de destino..: ");
				scanf("%d", &value);

				printf("\tPeso do caminho: ");
				scanf("%d", &weight);

				printf("\tNome da rua....: ");
				getContents(contents);

				#ifdef COMPACT_XML
					fprintf(fd, "\t\t<rua id=\"%d\" peso=\"%d\" nome=\"%s\"></rua>\n", value, weight, contents);
				#else
					fprintf(fd, "\t\t\t<rua>\n\t\t\t\t<id>%d</id>\n\t\t\t\t<peso>%d</peso>\n\t\t\t\t<nome>%s</nome>\n\t\t\t</rua>\n", value, weight, contents);
				#endif
				
				printf("\n\tAdicionar nova rua saindo deste no (0/1)? ");
				scanf("%d", &addNewCon);
			} while (addNewCon);
			#ifndef COMPACT_XML
				fprintf(fd, "\t\t</conns>\n");
			#endif
			fprintf(fd, "\t</node>\n");
			cont = 1;
		}
	} while (cont);
	fclose(fd);
	return 0;
}

void getContents(char *str) {
	#ifdef LINUX
		__fpurge(stdin);
	#else
		fflush(stdin);
	#endif
	fgets(str, MAX_NOME, stdin);
	purgeLN(str);
}

void purgeLN(char *str) {
	if ((strlen(str) > 0) && (str[strlen(str) - 1] == '\n'))
        str[strlen(str) - 1] = '\0';
}
