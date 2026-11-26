#define N 10000000
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
typedef struct{
    double x;
    double y;
}CidadeCoord;

typedef struct{
    CidadeCoord cidades[N];
    int numCidades;
}Cidades;

typedef struct{
    int* pai;
    double pesoTotal;
} MST;
typedef struct{
    int* caminho;
    int tamanho;
    double distanciaTotal;
} Rota;
typedef struct Node{
    int val;
    struct Node* prox;
}Node;

static inline double euclideanDis(CidadeCoord cidade1,CidadeCoord cidade2){
    return sqrt((cidade1.x-cidade2.x)*(cidade1.x-cidade2.x)+(cidade1.y-cidade2.y)*(cidade1.y-cidade2.y));
}
static inline double euclideanDisSquared(CidadeCoord cidade1,CidadeCoord cidade2){
    return (cidade1.x-cidade2.x)*(cidade1.x-cidade2.x)+(cidade1.y-cidade2.y)*(cidade1.y-cidade2.y);
}
MST* Prim(Cidades* cidadesArr);
void DFS(int numCidades,Node** filhos, int* visitasOrdem, int* k);
double calcularDistanciaTotal(Cidades* cidadesArr, int* visitasOrdem);
Rota* doubleTree(Cidades* cidadesArr);
Rota* gulosoVizinhoMaisProximo(Cidades *cidadesArr);
