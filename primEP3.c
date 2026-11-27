#include <stdio.h>
#include <math.h>
#include <float.h>
#include <stdlib.h>
#include "ep3.h"

/* ------------------------------------------------------------------ */
/* Alocacao das estruturas                                              */
/* ------------------------------------------------------------------ */

Cidades* criarCidades(int n){
    if(n<0) return NULL;
    Cidades* cidadesArr=malloc(sizeof(Cidades));
    if(cidadesArr==NULL) return NULL;
    cidadesArr->numCidades=n;
    cidadesArr->cidades=malloc(sizeof(CidadeCoord)*(size_t)(n>0?n:1));
    if(cidadesArr->cidades==NULL){
        free(cidadesArr);
        return NULL;
    }
    return cidadesArr;
}

void liberarCidades(Cidades* cidadesArr){
    if(cidadesArr==NULL) return;
    free(cidadesArr->cidades);
    free(cidadesArr);
}

void liberarMST(MST* arvore){
    if(arvore==NULL) return;
    free(arvore->pai);
    free(arvore);
}

/* ------------------------------------------------------------------ */
/* Prim em O(n^2) sobre o grafo completo euclidiano                     */
/* ------------------------------------------------------------------ */
/*
 * Mesma ideia do Prim "por vetor de distancias" (sem fila de prioridade),
 * com tres otimizacoes de constante:
 *
 *  1. Coordenadas copiadas para dois vetores separados (x[] e y[]), o
 *     que deixa o acesso sequencial e permite vetorizacao pelo compilador.
 *  2. A atualizacao das distancias e a escolha do proximo vertice sao
 *     feitas na MESMA varredura (antes eram duas varreduras por passo).
 *  3. Os vertices ainda fora da arvore ficam em uma lista compacta
 *     (restantes[]); ao entrar na arvore, o vertice e removido trocando-o
 *     com o ultimo da lista. Assim, o passo k percorre apenas n-k
 *     vertices: cerca de n^2/2 iteracoes no total, em vez de 2n^2.
 *
 * Desempate: entre distancias iguais escolhe-se o MENOR indice, o mesmo
 * criterio da versao original (varredura crescente com comparacao
 * estrita). Por isso a arvore produzida e identica a da versao anterior.
 *
 * As distancias sao mantidas ao quadrado (a raiz e monotona, entao a
 * ordem das comparacoes nao muda); a raiz so e calculada no peso final.
 */
MST* Prim(const Cidades* cidadesArr){
    if(cidadesArr==NULL || cidadesArr->numCidades<=0) return NULL;
    int n=cidadesArr->numCidades;

    MST* minimumSTree=malloc(sizeof(MST));
    double* x=malloc(sizeof(double)*(size_t)n);
    double* y=malloc(sizeof(double)*(size_t)n);
    double* menorDistancia=malloc(sizeof(double)*(size_t)n);
    int* restantes=malloc(sizeof(int)*(size_t)n);
    int* pai=malloc(sizeof(int)*(size_t)n);
    if(!minimumSTree || !x || !y || !menorDistancia || !restantes || !pai){
        free(minimumSTree); free(x); free(y);
        free(menorDistancia); free(restantes); free(pai);
        return NULL;
    }

    for(int i=0;i<n;i++){
        x[i]=cidadesArr->cidades[i].x;
        y[i]=cidadesArr->cidades[i].y;
    }

    /* A arvore comeca na cidade 0; as demais ainda nao foram alcancadas. */
    int m=0;
    for(int i=1;i<n;i++){
        restantes[m++]=i;
        menorDistancia[i]=HUGE_VAL;
        pai[i]=0;
    }
    pai[0]=-1;
    menorDistancia[0]=0.0;

    int atual=0;  /* ultimo vertice que entrou na arvore */
    while(m>0){
        const double ax=x[atual], ay=y[atual];
        double menor=HUGE_VAL;
        int melhor=-1, posMelhor=-1;

        for(int q=0;q<m;q++){
            int v=restantes[q];
            double dx=x[v]-ax, dy=y[v]-ay;
            double d=dx*dx+dy*dy;
            /* atualizacao: o novo vertice da arvore oferece aresta mais barata? */
            if(d<menorDistancia[v]){
                menorDistancia[v]=d;
                pai[v]=atual;
            }
            /* escolha do proximo vertice (menor distancia, menor indice) */
            double dv=menorDistancia[v];
            if(dv<menor || (dv==menor && v<melhor)){
                menor=dv;
                melhor=v;
                posMelhor=q;
            }
        }

        if(melhor==-1){  /* coordenadas invalidas (NaN/infinito) */
            free(x); free(y); free(menorDistancia); free(restantes);
            free(pai); free(minimumSTree);
            return NULL;
        }

        restantes[posMelhor]=restantes[--m];  /* remove em O(1) */
        atual=melhor;
    }

    /* menorDistancia[v] guarda o quadrado da aresta (pai[v], v). */
    double pesoTotal=0.0;
    for(int i=1;i<n;i++) pesoTotal+=sqrt(menorDistancia[i]);

    minimumSTree->pai=pai;
    minimumSTree->pesoTotal=pesoTotal;
    free(x); free(y); free(menorDistancia); free(restantes);
    return minimumSTree;
}
