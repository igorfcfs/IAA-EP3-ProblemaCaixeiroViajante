
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ep3.h"

static void executarTeste(int n) {
    printf("\n========================================\n");
    printf("Teste com %d cidades\n", n);
    printf("========================================\n");

    Cidades *cidades = malloc(sizeof(*cidades));

    if (cidades == NULL) {
        printf("Erro ao alocar a estrutura Cidades.\n");
        return;
    }

    cidades->numCidades = n;

    // Gera coordenadas aleatorias entre 0 e 1000
    for (int i = 0; i < n; i++) {
        cidades->cidades[i].x =
            ((double)rand() / RAND_MAX) * 1000.0;

        cidades->cidades[i].y =
            ((double)rand() / RAND_MAX) * 1000.0;
    }

    clock_t inicio = clock();
    Rota *rota = doubleTree(cidades);
    clock_t fim = clock();

    if (rota == NULL) {
        printf("Erro: doubleTree retornou NULL.\n");
        free(cidades);
        return;
    }

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

    int rotaValida = (rota->tamanho == n);
    int *visitada = calloc((size_t)n, sizeof(*visitada));

    if (visitada == NULL) {
        printf("Erro ao alocar vetor de validacao.\n");
        rotaValida = 0;
    } else {
        for (int i = 0; i < rota->tamanho; i++) {
            int cidade = rota->caminho[i];

            if (cidade < 0 || cidade >= n || visitada[cidade]) {
                rotaValida = 0;
                break;
            }

            visitada[cidade] = 1;
        }

        free(visitada);
    }

    printf("Distancia total: %.6f\n", rota->distanciaTotal);
    printf("Tamanho da rota: %d\n", rota->tamanho);
    printf("Tempo de execucao: %.6f segundos\n", tempo);
    printf("Validade da rota: %s\n",
           rotaValida ? "OK" : "INVALIDA");

    free(rota->caminho);
    free(rota);
    free(cidades);
}

int main(void) {
    // Semente fixa para permitir reproducibilidade
    srand(20261125);

    const int tamanhos[] = {
        10,
        100,
        500,
        1000,
        10000,
        100000,
        200000
    };

    const int quantidadeTestes =
        (int)(sizeof(tamanhos) / sizeof(tamanhos[0]));

    for (int i = 0; i < quantidadeTestes; i++) {
        executarTeste(tamanhos[i]);
    }

    return 0;
}
