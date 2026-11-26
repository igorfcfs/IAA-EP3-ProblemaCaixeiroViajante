#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "ep3.h"
MST* Prim(Cidades* cidadesArr){
    if(cidadesArr->numCidades==0)return NULL;
    int* cidadesVisitadas=calloc(cidadesArr->numCidades,sizeof(int));
    double* menorDistancia=malloc(sizeof(double)*cidadesArr->numCidades);
    for(int i=1;i<cidadesArr->numCidades;i++)menorDistancia[i]=euclideanDisSquared(cidadesArr->cidades[0], cidadesArr->cidades[i]);
    menorDistancia[0]=0;
    MST* minimumSTree=malloc(sizeof(MST));
    minimumSTree->pesoTotal=0.0;
    minimumSTree->pai=malloc(sizeof(int)*cidadesArr->numCidades);
    minimumSTree->pai[0]=-1;
    for(int i=1;i<cidadesArr->numCidades;i++)minimumSTree->pai[i]=0;

    cidadesVisitadas[0]=1;


    int i;
    int j;
    for(int k=1;k<cidadesArr->numCidades;k++){
        j=-1;
        double menor=DBL_MAX;
        for(i=0;i<cidadesArr->numCidades;i++){
            double distancia=menorDistancia[i];
            if(cidadesVisitadas[i]!=1 && menor>distancia){
                menor=distancia;
                j=i;
            }
        }
    if(j==-1){
        free(minimumSTree->pai);
        free(minimumSTree);
        free(menorDistancia);
        free(cidadesVisitadas);
        return NULL;

    }

    cidadesVisitadas[j]=1;
    for(i=0;i<cidadesArr->numCidades;i++){
        double distancia=euclideanDisSquared(cidadesArr->cidades[j], cidadesArr->cidades[i]);
        if(cidadesVisitadas[i]!=1 && distancia<menorDistancia[i]){
            menorDistancia[i]=distancia;
            minimumSTree->pai[i]=j;
        }
    }

    }
    for(i=0;i<cidadesArr->numCidades;i++)
    minimumSTree->pesoTotal+=sqrt(menorDistancia[i]);
    free(menorDistancia);
    free(cidadesVisitadas);
    return minimumSTree;

}


