#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include "ep3.h"

void liberarRota(Rota* rota){
    if(rota==NULL) return;
    free(rota->caminho);
    free(rota);
}

/*
 * DFS iterativa em pre-ordem a partir da cidade 0.
 *
 * Os filhos de cada vertice v estao em filhos[inicioFilhos[v] ..
 * inicioFilhos[v+1]-1], em ordem crescente de indice (representacao
 * compacta, sem listas ligadas). Os filhos sao empilhados do maior para o
 * menor, de modo que o menor indice e visitado primeiro.
 *
 * Cada vertice e empilhado uma unica vez (tem um unico pai), logo a pilha
 * nunca passa de n-1 elementos e a DFS custa O(n).
 */
void DFS(int numCidades, const int* inicioFilhos, const int* filhos, int* visitasOrdem, int* k){
    int* pilha=malloc(sizeof(int)*(size_t)(numCidades>0?numCidades:1));
    if(pilha==NULL) return;  /* k fica menor que n: a rota sera recusada */
    int topo=0;

    pilha[topo++]=0;  /* raiz */
    while(topo!=0){
        int v=pilha[--topo];
        visitasOrdem[(*k)++]=v;  /* pre-ordem = passeio da arvore duplicada com atalhos */
        for(int c=inicioFilhos[v+1]-1;c>=inicioFilhos[v];c--)
            pilha[topo++]=filhos[c];
    }
    free(pilha);
}

double calcularDistanciaTotal(const Cidades* cidadesArr, const int* visitasOrdem){
    double distanciaTotal=0.0;
    CidadeCoord primeiraCidade=cidadesArr->cidades[visitasOrdem[0]];
    CidadeCoord anterior=primeiraCidade;
    for(int i=1;i<cidadesArr->numCidades;i++){
        CidadeCoord atual=cidadesArr->cidades[visitasOrdem[i]];
        distanciaTotal+=euclideanDis(anterior,atual);
        anterior=atual;
    }
    distanciaTotal+=euclideanDis(anterior,primeiraCidade);
    return distanciaTotal;
}

Rota* doubleTree(const Cidades* cidadesArr){
    if(cidadesArr==NULL || cidadesArr->numCidades<=0) return NULL;
    int n=cidadesArr->numCidades;

    MST* minimunSTree=Prim(cidadesArr);
    if(minimunSTree==NULL) return NULL;

    /* Listas de filhos em formato compacto (contagem + soma de prefixos):
       inicioFilhos[v] = posicao onde comecam os filhos de v em filhos[]. */
    int* inicioFilhos=calloc((size_t)n+1,sizeof(int));
    int* filhos=malloc(sizeof(int)*(size_t)n);
    int* proximaPos=malloc(sizeof(int)*(size_t)n);
    Rota* rota=malloc(sizeof(Rota));
    int* caminho=malloc(sizeof(int)*(size_t)n);
    if(!inicioFilhos || !filhos || !proximaPos || !rota || !caminho){
        free(inicioFilhos); free(filhos); free(proximaPos);
        free(rota); free(caminho);
        liberarMST(minimunSTree);
        return NULL;
    }

    for(int i=1;i<n;i++) inicioFilhos[minimunSTree->pai[i]+1]++;      /* conta filhos */
    for(int v=0;v<n;v++) inicioFilhos[v+1]+=inicioFilhos[v];         /* soma de prefixos */
    memcpy(proximaPos,inicioFilhos,sizeof(int)*(size_t)n);
    for(int i=1;i<n;i++) filhos[proximaPos[minimunSTree->pai[i]]++]=i; /* i crescente */

    int k=0;
    DFS(n,inicioFilhos,filhos,caminho,&k);

    rota->caminho=caminho;
    rota->tamanho=k;
    rota->pesoMST=minimunSTree->pesoTotal;
    rota->distanciaTotal=(k==n)?calcularDistanciaTotal(cidadesArr,caminho):NAN;

    free(inicioFilhos); free(filhos); free(proximaPos);
    liberarMST(minimunSTree);
    return rota;
}
