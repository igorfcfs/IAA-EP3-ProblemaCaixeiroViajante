#ifndef EP3_H
#define EP3_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct{
    double x;
    double y;
}CidadeCoord;

/* Vetor de cidades com tamanho dinamico: so ocupa memoria para as
   numCidades cidades efetivamente usadas (ver criarCidades). */
typedef struct{
    CidadeCoord* cidades;
    int numCidades;
}Cidades;

typedef struct{
    int* pai;          /* pai[v] na arvore; pai[raiz] = -1 */
    double pesoTotal;  /* soma dos comprimentos das arestas da MST */
} MST;

typedef struct{
    int* caminho;
    int tamanho;
    double distanciaTotal;
    double pesoMST;    /* peso da MST usada (limite inferior do otimo); 0 se nao se aplica */
} Rota;

static inline double euclideanDis(CidadeCoord cidade1,CidadeCoord cidade2){
    return sqrt((cidade1.x-cidade2.x)*(cidade1.x-cidade2.x)+(cidade1.y-cidade2.y)*(cidade1.y-cidade2.y));
}
static inline double euclideanDisSquared(CidadeCoord cidade1,CidadeCoord cidade2){
    return (cidade1.x-cidade2.x)*(cidade1.x-cidade2.x)+(cidade1.y-cidade2.y)*(cidade1.y-cidade2.y);
}

/* Alocacao e liberacao das estruturas */
Cidades* criarCidades(int n);
void liberarCidades(Cidades* cidadesArr);
void liberarMST(MST* arvore);
void liberarRota(Rota* rota);

/* Algoritmo proposto: Prim + Double Tree */
MST* Prim(const Cidades* cidadesArr);
void DFS(int numCidades, const int* inicioFilhos, const int* filhos, int* visitasOrdem, int* k);
double calcularDistanciaTotal(const Cidades* cidadesArr, const int* visitasOrdem);
Rota* doubleTree(const Cidades* cidadesArr);

/* Referencia (nao e o algoritmo proposto) */
Rota* gulosoVizinhoMaisProximo(const Cidades* cidadesArr);

#endif
