#define _CRT_SECURE_NO_WARNINGS

#define COMPACT_XML

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

void dij(xmlNode *raiz, int orig, int dest, int path[], int *pd);

int getPeso(xmlNode *raiz, int from, int to);
void getNome(xmlNode *raiz, int from, int to, char dest[]);
int cGetPeso(xmlNode *raiz, int from, int to);
int cGetNome(xmlNode *raiz, int from, int to, char dest[]);

void printPath(xmlNode *raiz, int path[MAX], int dest, int peso);

int main(int argc, char *argv[]) {
	#ifdef COMPACT_XML
		printf("Using newest compact-style XML file\n");
	#endif
	Logger l;
	if (argc > 1) {
		int lv;
		sscanf(argv[1], "%d", &lv);
		l.setLevel((level) lv);
	} else {
		l.setLevel(info);
	}
	
	char aliases[NNOS][MAX_NOME];
	int  idalias[NNOS];
	
	xmlNode *raiz = start("db.xml");
	xmlNode *aux = raiz;
	char buffer[CIMD];
	int POIs = 0;
	
	#ifndef COMPACT_XML
	while (aux) {
		findTagInParent(aux, "nome", buffer);
		if (strcmp(buffer, "NULL")) {
			strcpy(aliases[POIs], buffer);
			findTagInParent(aux, "id", buffer);
			sscanf(buffer, "%d", &(idalias[POIs++]));
		}
		aux = aux->next;
	}
	#endif
	
	#ifdef COMPACT_XML
	while (aux) {
		getStrAtt(aux, "nome", buffer);
		if (strcmp(buffer, "NULL")) {
			strcpy(aliases[POIs], buffer);
			idalias[POIs++] = getIntAtt(aux, "id");
		}
		aux = aux->next;
	}
	#endif
	
	int path[MAX];
	int pesoTotal;

	int orig = 0;
	int dest = 9;
	
	int i;
	printf("\n\tPontos de interesse: \n\n");
	for (i = 0; i < POIs; i++)
		printf("\t\t%d. %s\n", i+1, aliases[i]);

	printf("\nDigite a origem:  ");
	scanf("%i", &orig);
	
	sprintf(l.buffer, "Orig was %d, now is %d", orig, idalias[orig - 1]);
	l.logB(config);
	
	orig = idalias[orig - 1];
	
	printf("Digite o destino: ");
	scanf("%i", &dest);
	
	sprintf(l.buffer, "Dest was %d, now is %d", dest, idalias[dest - 1]);
	l.logB(config);
	
	dest = idalias[dest - 1];

	dij(raiz, orig, dest, path, &pesoTotal);

	l.logString("Dijstra is done!", config);
	printPath(raiz, path, dest, pesoTotal);

	#ifndef LINUX
	system("pause");
	#endif
	return 0;
}

void dij(xmlNode *raiz, int orig, int dest, int path[], int *pd) {
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
				#ifdef COMPACT_XML
				novadist = dc + cGetPeso(raiz, corrente, i);
				#else
				novadist = dc +  getPeso(raiz, corrente, i);
				#endif
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

int cGetPeso(xmlNode *raiz, int from, int to) {
	xmlNode *aux = raiz;
	int cfrom, cto;
	do {
		cfrom = getIntAtt(aux, "id");
		if (cfrom != from) aux = aux->next; 
	} while (aux && cfrom != from);
	
	aux = aux->inside; //Get first road
	if (!aux) return INF; //If there are no roads starting at 'from'
	
	do {
		cto = getIntAtt(aux, "id");
		if (cto != to) aux = aux->next;
	} while (aux && cto != to);
	
	if (aux) {
		int attempt = getIntAtt(aux, "peso");
		if (attempt >= 0) { return attempt; }
		printf("Got a negative weight (probably a failed get)");
		return INF;
	}
	//printf("No aux\n");
	return INF;
}

int getPeso(xmlNode *raiz, int from, int to) {
	xmlNode *aux = raiz;
	int cfrom, cto;
	char buffer[CIMD];
	do {
		cfrom = getIntTag(aux, "id");
		if (cfrom != from) aux = aux->next;
	} while (aux && cfrom != from);
	
	if (!aux) return INF; //Ideally this wouldnt happen if all nodes connections > 0
	
	aux = findNestedParent(aux, "conns")->inside;
	
	do {
		cto = getIntTag(aux, "id");
		if (cto != to) { aux = aux->next; }
	} while (aux && cto != to);
	
	if (aux) return getIntTag(aux, "peso");
	return INF;
}

int cGetNome(xmlNode *raiz, int from, int to, char dest[]) {
	xmlNode *aux = raiz;
	int cfrom, cto;
	do {
		cfrom = getIntAtt(aux, "id");
		if (cfrom != from) aux = aux->next;
	} while (aux && cfrom != from);
		
	aux = aux->inside; //Get first road
	if (!aux) return INF; //If there are no roads starting at 'from'
	
	do {
		cto = getIntAtt(aux, "id");
		if (cto != to) aux = aux->next;
	} while (aux && cto != to);
	
	if (aux) {
		if (getStrAtt(aux, "nome", dest)) 
			return 1;
		return -1;
	}
	
	printf("Path is invalid!\n");
	exit(5);
}

void getNome(xmlNode *raiz, int from, int to, char dest[]) {
	xmlNode *aux = raiz;
	int cfrom, cto;
	char buffer[CIMD];
	while (aux && cfrom != from) {
		cfrom = getIntTag(aux, "id");
		if (cfrom != from) aux = aux->next;
	}
	
	aux = findNestedParent(aux, "conns")->inside;
	while (aux && cto != to) {
		cto = getIntTag(aux, "id");
		if (cto != to) aux = aux->next;
	}
	
	if (aux) {
		findTagInParent(aux, "nome", dest);
	} else {
		printf("Path is invalid!\n");
		exit(5);
	}
}

void printPath(xmlNode *raiz, int path[MAX], int dest, int peso) {
	int rev[MAX + 1];
	int point = 1;

	char curr[MAX_NOME];
	char last[MAX_NOME];
	
	int i = 0;
	printf("\n\tCaminho de menor peso (%d)\n", peso);
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
		#ifdef COMPACT_XML
		cGetNome(raiz, rev[point], rev[point - 1], curr);
		#else
		 getNome(raiz, rev[point], rev[point - 1], curr);
		#endif
		if (strcmp(curr, last)) {
			printf("\t\t%d. %s\n", l, curr);
			l++;
			strcpy(last, curr);
		}
		point--;
	}
	printf("\n");
}
