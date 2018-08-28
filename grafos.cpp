#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Logger.h"
#include "Parser.h"

#define MAX 10
#define INF 999
#define NAOMEMBRO 0
#define MEMBRO 1

#define NNOS 50
#define MAX_NOME 30

#define LINUX
struct tnode {
	int no;
	int peso;
	char nome[MAX_NOME];
	struct tnode *prox;
};

typedef struct tnode node;

int parseXML(const char *path, char aliases[][MAX_NOME], int idalias[], node *nos[MAX], Logger l);
void getBetweenTags(FILE *fd, char *dest);
void skipChars(FILE *fd, int n);

void dij(node *nos[MAX], int orig, int dest, int path[], int *pd);
void insert(node *nos[MAX], int pos, int conn, int peso, char nome[MAX_NOME]);
int getPeso(node *nos[MAX], int from, int to);
void printPath(node *nos[MAX], int path[MAX], int dest, int peso);

int main() {
	Logger l;
	l.setLevel(config);
	//l.logString("This is being logged as fine!", fine);
	//l.logString("This is being logged as info!", info);
	//l.logString("This is being logged as error!", error);
	
	//------------------------ Creating adjacency matrix ------------------------
	node *nos[MAX];
	for (int i = 0; i < MAX; i++)
		nos[i] = NULL;

	char aliases[NNOS][MAX_NOME];
	int  idalias[NNOS];

	//FILE *xml = fopen("db.xml", "r");
	//if (!xml) {
		//l.logString("Error loading XML, is it really there? (/db.xml)", error);
		//exit(0);
	//}
	
	int POIs = parseXML("db.xml", aliases, idalias, nos, l);
	
	int path[MAX];
	int pesoTotal;

	int orig = 0;
	int dest = 9;
	
	int i;
	printf("\n\tPontos de interesse: \n\n");
	for (i = 0; i < POIs; i++)
		printf("\t\t%d. %s\n", i+1, aliases[i]);

	printf("Digite a origem:  ");
	scanf("%i", &orig);
	
	sprintf(l.buffer, "Orig was %d, now is %d", orig, idalias[orig - 1]);
	l.logB(config);
	
	orig = idalias[orig - 1];
	
	printf("Digite o destino: ");
	scanf("%i", &dest);
	
	sprintf(l.buffer, "Dest was %d, now is %d", dest, idalias[dest - 1]);
	l.logB(config);
	
	dest = idalias[dest - 1];

	dij(nos, orig, dest, path, &pesoTotal);

	printPath(nos, path, dest, pesoTotal);

	#ifndef LINUX
	system("pause");
	#endif
	return 0;
}

int parseXML(const char *path, char aliases[][MAX_NOME], int adalias[], node *nos[MAX], Logger l) {
	int aliases_pointer = 0;
	int cid, cto, cpeso;
	int cont_rua;
	char cval[MAX_NOME], cchar, c;
	
	//int count = 2;
	
	Parser p;
	//p.xml = xml;
	//p.setXML(xml);
	p.loadXML(path);
	while (!p.peof()) {
		p.skipStr("<node>\n");
		//fscanf(xml, "\t<id>%d</id>\n", &cid);
		cid = p.nextInt("id", 1);
		//l.logString("Got current ID", config);
		//getBetweenTags(xml, cval);
		p.nextString(cval, "nome");
		
		sprintf(l.buffer, "Got node %d - %s", cid, cval);
		l.logB(config);
		
		if (strcmp(cval, "NULL")) {
			strcpy(aliases[aliases_pointer], cval);
			adalias[aliases_pointer] = cid;
			aliases_pointer++;
		}
		
		//fscanf(xml, "\t<conns>\n");
		//fscanf(xml, "\t\t<rua>\n");
		p.skipStr("\t<conns>\n\t\t<rua>\n");
		
		cont_rua = 1;
		
		while (cont_rua) {
			//fscanf(xml, "\t\t\t<id>%d</id>\n", &cto);
			cto = p.nextInt("id", 3);
			//fscanf(xml, "\t\t\t<peso>%d</peso>\n", &cpeso);
			cpeso = p.nextInt("peso", 3);
			//fscanf(xml, "\t\t\t");
			p.skipStr("\t\t\t");
			
			//getBetweenTags(xml, cval);
			p.nextString(cval, "nome");
			
			insert(nos, cid, cto, cpeso, cval);
			
			sprintf(l.buffer,"\tInserting from %d to %d, weighed %d, named %s", cid, cto, cpeso, cval);
			l.logB(config);
			
			p.skipStr("\t\t</rua>\n\t");
			//fscanf(xml, "\t\t</rua>\n");
			//fscanf(xml, "\t%c", &cchar);
			//fscanf(xml, "%c", &cchar);
			cchar = p.getChar();
			if (cchar == '<') { 
				cont_rua = 0; 
				sprintf(l.buffer, "\t\tNo more conns (%c)", cchar);
				l.logB(config);
			} else {
				//skipChars(xml, 6);
				p.skipStr("<rua>\n");
				//fscanf(xml, "<rua>\n");
				sprintf(l.buffer, "\t\tGot another conn");
				l.logB(config);
			}
		}
		l.logString("\t\tFinished getting conns...", config);
		//if (p.skipStr("/conns>\n</node>\n") == -1) break;
		p.skipStr("/conns>\n</node>\n");
		c = p.getChar();
		if (!p.peof()) p.ungetChar(c);
		//printf("Foo is now %d\n", foo);
		//fscanf(xml, "/conns>\n");
		//fscanf(xml, "</node>\n");
	}
	p.close();
	return aliases_pointer;
}
/*
void skipChars(FILE *fd, int n) {
	char garb;
	while (n) {
		fscanf(fd, "%c", &garb);
		n--;
	}
}

void getBetweenTags(FILE *fd, char *dest) {
	//int valid = 0;
	int pos = 0;
	char curr;
	//char intag[MAX_NOME];
	//char temp[MAX_NOME];
	
	//intag[0] = '<';
	//intag[1] = '\0';
	//strcpy(intag, tag);
	//strcat(intag, ">");
	while (curr != '>') 
		fscanf(fd, "%c", &curr);
		
	while (curr != '<') {
		fscanf(fd, "%c", &curr);
		dest[pos++] = curr;
	}
	dest[pos - 1] = '\0';
	
	while (curr != '\n') 
		fscanf(fd, "%c", &curr);
}
*/
void dij(node *nos[MAX], int orig, int dest, int path[], int *pd) {
	int dist[MAX];
	int corrente, i, k = 0, dc;
	int perm[MAX];
	int menordist, novadist;
	
	for (i = 0; i < MAX; i++) {
		perm[i] = NAOMEMBRO;
		dist[i] = INF;
		path[i] = -1;
	}
	
	perm[orig] = MEMBRO;
	dist[orig] = 0;
	corrente = orig;

	while (corrente != dest) {
		menordist = INF;
		dc = dist[corrente];
		for (i = 0; i < MAX; i++) {
			if (perm[i] == NAOMEMBRO) {
				novadist = dc + getPeso(nos, corrente, i);
				if (novadist < dist[i]) {
					dist[i] = novadist;
					path[i] = corrente;
				}
				if (dist[i] < menordist) {
					menordist = dist[i];
					k = i;
				}
			}
		}
		corrente = k;
		perm[corrente] = MEMBRO;
	}
	*pd = dist[dest];
}

void insert(node *nos[MAX], int pos, int conn, int peso, char nome[MAX_NOME]) {
	if (nos[pos]) {
		node *atual = nos[pos];
		while (atual->prox != NULL) atual = atual->prox;
		node *novo = (node*)malloc(sizeof(node));
		novo->no = conn;
		novo->peso = peso;
		strcpy(novo->nome, nome);
		novo->prox = NULL;
		atual->prox = novo;
	} else {
		node *novo = (node*)malloc(sizeof(node));
		novo->no = conn;
		novo->peso = peso;
		strcpy(novo->nome, nome);
		novo->prox = NULL;
		nos[pos] = novo;
	}
}

int getPeso(node *nos[MAX], int from, int to) {
	node *atual = nos[from];
	while (atual != NULL) {
		if (atual->no == to) 
			return atual->peso;
		atual = atual->prox;
	}
	return INF;
}

void getNome(node *nos[MAX], int from, int to, char dest[]) {
	node *atual = nos[from];
	while (atual != NULL) {
		if (atual->no == to) {
			strcpy(dest, atual->nome);
			return;
		}
		atual = atual->prox;
	}
	printf("Path invalid!\n");
	exit(5); 
}

void printPath(node *nos[MAX], int path[MAX], int dest, int peso) {
	int rev[MAX + 1];
	int point = 1;

	char curr[MAX_NOME];
	char last[MAX_NOME];
	
	int i = 0;
	printf("\n");
	rev[0] = dest;
	i = dest;
	while (path[i] != -1) {
		rev[point] = path[i];
		point++;
		i = path[i];
	}
	point--;

	int l = 1;
	while (point > 0) {
		getNome(nos, rev[point], rev[point - 1], curr);
		if (strcmp(curr, last)) {
			printf("\t%d. %s\n", l, curr);
			l++;
			strcpy(last, curr);
		}
		point--;
	}
}
