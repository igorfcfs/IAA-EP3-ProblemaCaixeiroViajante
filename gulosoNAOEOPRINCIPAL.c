#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "ep3.h"

/*
 * CODIGO ORIGINAL DO CHAT GPT, SERVE APENAS COMO METRICA PARA COMPARAR COM
 * O ALGORITMO ORIGINAL (primEP3.c e doubleTreeEP3.c), NAO LEVAR EM
 * CONSIDERACAO NA CORRECAO.
 *
 * Ajustes feitos para a comparacao ser justa com o Prim:
 *  - a busca do vizinho mais proximo compara distancias AO QUADRADO
 *    (a raiz e monotona, logo a cidade escolhida e a mesma; o Prim ja
 *    fazia isso, e o guloso pagava uma sqrt por comparacao);
 *  - adaptado a estrutura Cidades com vetor dinamico.
 */
Rota *gulosoVizinhoMaisProximo(const Cidades *cidadesArr) {
    if (cidadesArr == NULL || cidadesArr->numCidades <= 0)
        return NULL;

    int n = cidadesArr->numCidades;

    Rota *rota = malloc(sizeof(*rota));
    if (rota == NULL) return NULL;

    rota->caminho = malloc((size_t)n * sizeof(*rota->caminho));
    unsigned char *visitada = calloc((size_t)n, sizeof(*visitada));

    if (rota->caminho == NULL || visitada == NULL) {
        free(rota->caminho);
        free(visitada);
        free(rota);
        return NULL;
    }

    int atual = 0;
    rota->caminho[0] = atual;
    visitada[atual] = 1;
    rota->tamanho = 1;
    rota->pesoMST = 0.0;

    for (int passo = 1; passo < n; passo++) {
        int proxima = -1;
        double menor = DBL_MAX;
        const double ax = cidadesArr->cidades[atual].x;
        const double ay = cidadesArr->cidades[atual].y;

        for (int i = 0; i < n; i++) {
            if (!visitada[i]) {
                double dx = ax - cidadesArr->cidades[i].x;
                double dy = ay - cidadesArr->cidades[i].y;
                double d = dx * dx + dy * dy;

                if (d < menor) {
                    menor = d;
                    proxima = i;
                }
            }
        }

        if (proxima == -1) {
            free(visitada);
            free(rota->caminho);
            free(rota);
            return NULL;
        }

        rota->caminho[passo] = proxima;
        visitada[proxima] = 1;
        atual = proxima;
        rota->tamanho++;
    }

    double total = 0.0;
    for (int i = 1; i < n; i++) {
        int a = rota->caminho[i - 1];
        int b = rota->caminho[i];
        double dx = cidadesArr->cidades[a].x - cidadesArr->cidades[b].x;
        double dy = cidadesArr->cidades[a].y - cidadesArr->cidades[b].y;
        total += sqrt(dx * dx + dy * dy);
    }

    /* Fecha o ciclo, retornando a cidade inicial. */
    {
        int ultima = rota->caminho[n - 1];
        int primeira = rota->caminho[0];
        double dx = cidadesArr->cidades[ultima].x -
                    cidadesArr->cidades[primeira].x;
        double dy = cidadesArr->cidades[ultima].y -
                    cidadesArr->cidades[primeira].y;
        total += sqrt(dx * dx + dy * dy);
    }

    rota->distanciaTotal = total;
    free(visitada);
    return rota;
}
