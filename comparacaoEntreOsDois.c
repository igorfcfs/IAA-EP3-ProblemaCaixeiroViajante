
/*
 * Comparacao entre Double Tree e Guloso do Vizinho Mais Proximo.
 *
 * Casos:
 * 1. Aleatorio uniforme
 * 2. Clusters
 * 3. Grade regular
 * 4. Circulos concentricos
 * 5. Espiral
 *
 * Compile:
 * gcc -march=native -O3 comparacaoEntreOsDois.c \
 *     primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o teste
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "ep3.h"

#define PI 3.14159265358979323846

typedef enum {
    ALEATORIO,
    CLUSTERS,
    GRADE,
    CIRCULOS,
    ESPIRAL
} TipoTeste;

static inline double segundos(clock_t inicio, clock_t fim) {
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

static int validarRota(const Rota *rota, int n) {
    if (rota == NULL || rota->caminho == NULL || rota->tamanho != n)
        return 0;

    unsigned char *visitada = calloc((size_t)n, sizeof(*visitada));
    if (visitada == NULL) return 0;

    int valida = 1;

    for (int i = 0; i < n; i++) {
        int cidade = rota->caminho[i];

        if (cidade < 0 || cidade >= n || visitada[cidade]) {
            valida = 0;
            break;
        }

        visitada[cidade] = 1;
    }

    free(visitada);
    return valida;
}

static inline void liberarRota(Rota *rota) {
    if (rota != NULL) {
        free(rota->caminho);
        free(rota);
    }
}

static const char *nomeTeste(TipoTeste tipo) {
    switch (tipo) {
        case ALEATORIO: return "Aleatorio uniforme";
        case CLUSTERS:  return "Clusters";
        case GRADE:     return "Grade regular";
        case CIRCULOS:  return "Circulos concentricos";
        case ESPIRAL:   return "Espiral";
        default:        return "Desconhecido";
    }
}

/* Gera uma coordenada aleatoria entre 0 e 1. */
static double aleatorio01(void) {
    return (double)rand() / RAND_MAX;
}

/* Gera as cidades de acordo com o caso escolhido. */
static void gerarCidades(Cidades *cidades, int n, TipoTeste tipo) {
    cidades->numCidades = n;

    switch (tipo) {

        case ALEATORIO:
            /* Distribuicao uniforme em uma regiao 1000 x 1000. */
            for (int i = 0; i < n; i++) {
                cidades->cidades[i].x = aleatorio01() * 1000.0;
                cidades->cidades[i].y = aleatorio01() * 1000.0;
            }
            break;

        case CLUSTERS: {
            /* Cinco agrupamentos distribuidos pelo plano. */
            const double centros[5][2] = {
                {100, 100},
                {900, 100},
                {500, 500},
                {100, 900},
                {900, 900}
            };

            for (int i = 0; i < n; i++) {
                int grupo = rand() % 5;

                cidades->cidades[i].x =
                    centros[grupo][0] + (aleatorio01() - 0.5) * 100.0;

                cidades->cidades[i].y =
                    centros[grupo][1] + (aleatorio01() - 0.5) * 100.0;
            }
            break;
        }

        case GRADE: {
            /* Pontos igualmente espacados em uma grade. */
            int lado = (int)ceil(sqrt((double)n));

            for (int i = 0; i < n; i++) {
                int linha = i / lado;
                int coluna = i % lado;

                cidades->cidades[i].x = coluna * 10.0;
                cidades->cidades[i].y = linha * 10.0;
            }
            break;
        }

        case CIRCULOS: {
            /* Quatro circulos concentricos. */
            const double raios[4] = {100.0, 200.0, 300.0, 400.0};

            for (int i = 0; i < n; i++) {
                int circulo = i % 4;
                int posicao = i / 4;
                int quantidade = (n + 3 - circulo) / 4;

                double angulo = 2.0 * PI * posicao / quantidade;
                double raio = raios[circulo];

                cidades->cidades[i].x =
                    500.0 + raio * cos(angulo);

                cidades->cidades[i].y =
                    500.0 + raio * sin(angulo);
            }
            break;
        }

        case ESPIRAL: {
            /* Espiral com quatro voltas. */
            for (int i = 0; i < n; i++) {
                double t = (n > 1) ? (double)i / (n - 1) : 0.0;

                double angulo = t * 8.0 * PI;
                double raio = 20.0 + 400.0 * t;

                cidades->cidades[i].x =
                    500.0 + raio * cos(angulo);

                cidades->cidades[i].y =
                    500.0 + raio * sin(angulo);
            }
            break;
        }
    }
}

static void executarTeste(int n, TipoTeste tipo) {
    printf("\n===============================================\n");
    printf("%s - %d cidades\n", nomeTeste(tipo), n);
    printf("===============================================\n");

    Cidades *cidades = malloc(sizeof(*cidades));

    if (cidades == NULL) {
        printf("Erro ao alocar Cidades.\n");
        return;
    }

    gerarCidades(cidades, n, tipo);

    clock_t inicioDT = clock();
    Rota *dt = doubleTree(cidades);
    clock_t fimDT = clock();

    clock_t inicioG = clock();
    Rota *g = gulosoVizinhoMaisProximo(cidades);
    clock_t fimG = clock();

    double tempoDT = segundos(inicioDT, fimDT);
    double tempoG = segundos(inicioG, fimG);

    int validoDT = validarRota(dt, n);
    int validoG = validarRota(g, n);

    printf("\n%-27s %16s %16s\n",
           "Metrica", "Double Tree", "Vizinho Proximo");

    printf("---------------------------------------------------------------\n");

    printf("%-27s %16.6f %16.6f\n",
           "Tempo (segundos)", tempoDT, tempoG);

    printf("%-27s %16.3f %16.3f\n",
           "Distancia total",
           validoDT ? dt->distanciaTotal : NAN,
           validoG ? g->distanciaTotal : NAN);

    printf("%-27s %16s %16s\n",
           "Rota valida",
           validoDT ? "SIM" : "NAO",
           validoG ? "SIM" : "NAO");

    if (validoDT && validoG) {
        double diferenca = g->distanciaTotal - dt->distanciaTotal;

        double percentual = dt->distanciaTotal > 0.0
            ? 100.0 * diferenca / dt->distanciaTotal
            : 0.0;

        printf("\nDiferenca (Guloso - Double Tree): %.3f\n",
               diferenca);

        printf("Diferenca percentual sobre Double Tree: %.2f%%\n",
               percentual);

        if (diferenca < 0.0)
            printf("Vizinho Proximo encontrou a menor rota.\n");
        else if (diferenca > 0.0)
            printf("Double Tree encontrou a menor rota.\n");
        else
            printf("Os algoritmos encontraram rotas de mesmo custo.\n");

    } else {
        printf("\nNao foi possivel comparar as distancias: rota invalida.\n");
    }

    liberarRota(dt);
    liberarRota(g);
    free(cidades);
}

int main(void) {
    const int tamanhos[] = {
        10, 100, 500, 1000, 10000, 100000
    };

    const int quantidade =
        (int)(sizeof(tamanhos) / sizeof(tamanhos[0]));

    const TipoTeste tipos[] = {
        ALEATORIO,
        CLUSTERS,
        GRADE,
        CIRCULOS,
        ESPIRAL
    };

    const int quantidadeTipos =
        (int)(sizeof(tipos) / sizeof(tipos[0]));

    for (int t = 0; t < quantidadeTipos; t++) {
        for (int i = 0; i < quantidade; i++) {

            /*
             * Semente reproduzivel para cada combinacao
             * de distribuicao e tamanho.
             */
            srand(20261121u + (unsigned int)(t * 1000000 + tamanhos[i]));

            executarTeste(tamanhos[i], tipos[t]);
        }
    }

    return 0;
}
