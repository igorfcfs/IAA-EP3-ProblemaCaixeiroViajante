#include <stdio.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <stdlib.h>
#include "ep3.h"


void DFS(int numCidades,Node** filhos, int* visitasOrdem, int* k){
    Node** pilha=calloc(numCidades ,sizeof(Node*));
    int topo=0;


    visitasOrdem[(*k)++]=0;
    Node* p = filhos[0];
    while (p) {
        pilha[topo++] = p;
        p = p->prox;
    }
    while(topo != 0){
        Node* temp = pilha[--topo];
        visitasOrdem[(*k)++] = temp->val;
        p = filhos[temp->val];

    while(p){
        pilha[topo++] = p;
        p = p->prox;
    }
}
    free(pilha);

}
double calcularDistanciaTotal(Cidades* cidadesArr, int* visitasOrdem){
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
Rota* doubleTree(Cidades* cidadesArr){
    if(!cidadesArr) return NULL;
    MST* minimunSTree=Prim(cidadesArr);
    if(minimunSTree==NULL) return NULL;
    Node** filhos=calloc(cidadesArr->numCidades, sizeof(Node*));
    for(int i=1;i<cidadesArr->numCidades;i++){
            Node* filho=calloc(1,sizeof(Node));
            filho->val=i;
            filho->prox=filhos[minimunSTree->pai[i]];
            filhos[minimunSTree->pai[i]]=filho;
    }


    Rota* rota=malloc(sizeof(Rota));
    rota->caminho = malloc(sizeof(int) * cidadesArr->numCidades);
    int k=0;


    DFS(cidadesArr->numCidades,filhos, rota->caminho, &k);
    rota->distanciaTotal=calcularDistanciaTotal(cidadesArr,rota->caminho);
    rota->tamanho=k;

    free(minimunSTree->pai);
    free(minimunSTree);
    for(int i=0;i<cidadesArr->numCidades;i++){
        Node* atual=filhos[i];
        while(atual){
            Node* proximo=atual->prox;
            free(atual);
            atual=proximo;
        }

    }
    free(filhos);
    return rota;


}

