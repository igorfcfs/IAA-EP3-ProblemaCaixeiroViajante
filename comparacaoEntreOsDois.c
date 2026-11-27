/*
 * Comparacao entre Double Tree (Prim + DFS) e Guloso do Vizinho Mais Proximo.
 *
 * Parte 1 - cinco distribuicoes geometricas x seis tamanhos:
 *   1. Aleatorio uniforme   2. Clusters   3. Grade regular
 *   4. Circulos concentricos   5. Espiral
 * Parte 2 - muitas instancias aleatorias pequenas: em quantas cada
 *   algoritmo encontra a rota mais curta.
 *
 * Compilar (ver README.md):
 *   gcc -std=c11 -O3 -march=native -ffp-contract=off comparacaoEntreOsDois.c \
 *       primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o teste -lm
 *
 * Executar:
 *   ./teste            (tamanhos ate 100000 - leva alguns minutos)
 *   ./teste 10000      (so tamanhos <= 10000 - poucos segundos)
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "ep3.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define PI 3.14159265358979323846

/* Tempo minimo acumulado por medicao: execucoes rapidas sao repetidas ate
   atingir esse total, e o tempo informado e a media por execucao. */
#define TEMPO_MINIMO_MEDICAO 0.25

typedef enum {
    ALEATORIO,
    CLUSTERS,
    GRADE,
    CIRCULOS,
    ESPIRAL
} TipoTeste;

/* Relogio de parede monotono e de alta resolucao. */
static double agora(void) {
#ifdef _WIN32
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER t;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart / (double)freq.QuadPart;
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
#endif
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

/* Gera uma coordenada aleatoria entre 0 e 1.
   Obs.: no Windows RAND_MAX = 32767, entao ha 32768 valores possiveis. */
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

typedef Rota *(*Algoritmo)(const Cidades *);

/* Executa o algoritmo quantas vezes forem necessarias para acumular
   TEMPO_MINIMO_MEDICAO segundos; devolve a rota da ultima execucao e
   grava em *tempoMedio o tempo medio por execucao. */
static Rota *medir(Algoritmo alg, const Cidades *cidades,
                   double *tempoMedio, int *repeticoes) {
    Rota *rota = NULL;
    int reps = 0;
    double total = 0.0;

    do {
        liberarRota(rota);
        double inicio = agora();
        rota = alg(cidades);
        total += agora() - inicio;
        reps++;
    } while (total < TEMPO_MINIMO_MEDICAO && rota != NULL);

    *tempoMedio = total / reps;
    *repeticoes = reps;
    return rota;
}

/* Linha do resumo final (uma por combinacao distribuicao x tamanho). */
typedef struct {
    TipoTeste tipo;
    int n;
    double tempoDT, tempoG, distDT, distG, pesoMST;
    int valido;
} Resultado;

static Resultado executarTeste(int n, TipoTeste tipo) {
    Resultado r = {tipo, n, NAN, NAN, NAN, NAN, NAN, 0};

    printf("\n===============================================\n");
    printf("%s - %d cidades\n", nomeTeste(tipo), n);
    printf("===============================================\n");

    Cidades *cidades = criarCidades(n);

    if (cidades == NULL) {
        printf("Erro ao alocar Cidades.\n");
        return r;
    }

    gerarCidades(cidades, n, tipo);

    int repDT, repG;
    Rota *dt = medir(doubleTree, cidades, &r.tempoDT, &repDT);
    Rota *g = medir(gulosoVizinhoMaisProximo, cidades, &r.tempoG, &repG);

    int validoDT = validarRota(dt, n);
    int validoG = validarRota(g, n);

    printf("\n%-27s %16s %16s\n",
           "Metrica", "Double Tree", "Vizinho Proximo");

    printf("---------------------------------------------------------------\n");

    printf("%-27s %16.9f %16.9f\n",
           "Tempo medio (segundos)", r.tempoDT, r.tempoG);

    printf("%-27s %16d %16d\n",
           "Execucoes medidas", repDT, repG);

    printf("%-27s %16.3f %16.3f\n",
           "Distancia total",
           validoDT ? dt->distanciaTotal : NAN,
           validoG ? g->distanciaTotal : NAN);

    printf("%-27s %16s %16s\n",
           "Rota valida",
           validoDT ? "SIM" : "NAO",
           validoG ? "SIM" : "NAO");

    if (validoDT && validoG) {
        r.valido = 1;
        r.distDT = dt->distanciaTotal;
        r.distG = g->distanciaTotal;
        r.pesoMST = dt->pesoMST;

        /* Mesma convencao da tabela do relatorio: positivo = guloso melhor. */
        double vantagem = 100.0 * (r.distDT - r.distG) / r.distDT;

        printf("\nPeso da MST (limite inferior do otimo): %.3f\n", r.pesoMST);
        printf("Razao Double Tree / MST: %.3f   (garantia: <= 2)\n",
               r.distDT / r.pesoMST);
        printf("Razao Vizinho Proximo / MST: %.3f\n", r.distG / r.pesoMST);
        printf("Vantagem do guloso (DT - G) / DT: %+.2f%%\n", vantagem);

        if (fabs(r.distDT - r.distG) <= 1e-9 * r.distDT)
            printf("Os algoritmos encontraram rotas de mesmo custo.\n");
        else if (r.distG < r.distDT)
            printf("Vizinho Proximo encontrou a menor rota.\n");
        else
            printf("Double Tree encontrou a menor rota.\n");

    } else {
        printf("\nNao foi possivel comparar as distancias: rota invalida.\n");
    }

    liberarRota(dt);
    liberarRota(g);
    liberarCidades(cidades);
    return r;
}

/* Parte 2: em instancias aleatorias pequenas, quem encontra a rota menor? */
static void experimentoInstanciasPequenas(void) {
    const int tamanhos[] = {5, 10, 20, 40, 80, 160};
    const int instancias = 2000;

    printf("\n\n###############################################\n");
    printf("PARTE 2 - %d instancias aleatorias por tamanho\n", instancias);
    printf("###############################################\n");
    printf("%6s %12s %12s %10s %18s\n",
           "n", "DT vence", "NN vence", "Empate", "Maior vitoria DT");

    for (size_t t = 0; t < sizeof(tamanhos) / sizeof(tamanhos[0]); t++) {
        int n = tamanhos[t];
        int vitoriasDT = 0, vitoriasNN = 0, empates = 0;
        double maiorVitoriaDT = 0.0;
        Cidades *cidades = criarCidades(n);
        if (cidades == NULL) return;

        for (int r = 0; r < instancias; r++) {
            srand(1000u * (unsigned int)n + (unsigned int)r);
            gerarCidades(cidades, n, ALEATORIO);
            Rota *dt = doubleTree(cidades);
            Rota *g = gulosoVizinhoMaisProximo(cidades);
            if (dt && g) {
                double dif = g->distanciaTotal - dt->distanciaTotal;
                if (fabs(dif) <= 1e-9 * dt->distanciaTotal) {
                    empates++;
                } else if (dif > 0) {
                    vitoriasDT++;
                    if (dif / g->distanciaTotal > maiorVitoriaDT)
                        maiorVitoriaDT = dif / g->distanciaTotal;
                } else {
                    vitoriasNN++;
                }
            }
            liberarRota(dt);
            liberarRota(g);
        }

        printf("%6d %11.1f%% %11.1f%% %9.1f%% %17.1f%%\n", n,
               100.0 * vitoriasDT / instancias,
               100.0 * vitoriasNN / instancias,
               100.0 * empates / instancias,
               100.0 * maiorVitoriaDT);
        liberarCidades(cidades);
    }
    printf("(Maior vitoria DT = quanto a rota do DT foi menor que a do guloso, em %%.)\n");
}

int main(int argc, char **argv) {
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

    /* Argumento opcional: maior tamanho a testar (padrao 100000). */
    int nMaximo = 100000;
    if (argc > 1) {
        nMaximo = atoi(argv[1]);
        if (nMaximo <= 0) {
            fprintf(stderr, "Uso: %s [tamanho_maximo]\n", argv[0]);
            return 1;
        }
    }

    Resultado resultados[5 * 6];
    int total = 0;

    for (int t = 0; t < quantidadeTipos; t++) {
        for (int i = 0; i < quantidade; i++) {
            if (tamanhos[i] > nMaximo) continue;

            /*
             * Semente reproduzivel para cada combinacao
             * de distribuicao e tamanho.
             */
            srand(20261121u + (unsigned int)(t * 1000000 + tamanhos[i]));

            resultados[total++] = executarTeste(tamanhos[i], tipos[t]);
        }
    }

    printf("\n\n###############################################\n");
    printf("RESUMO (separado por ';')\n");
    printf("###############################################\n");
    printf("distribuicao;n;tempo_dt_s;tempo_guloso_s;dist_dt;dist_guloso;"
           "peso_mst;dt_mst;guloso_mst;vantagem_guloso_pct;valido\n");
    for (int i = 0; i < total; i++) {
        Resultado *r = &resultados[i];
        printf("%s;%d;%.9f;%.9f;%.3f;%.3f;%.3f;%.4f;%.4f;%.2f;%s\n",
               nomeTeste(r->tipo), r->n, r->tempoDT, r->tempoG,
               r->distDT, r->distG, r->pesoMST,
               r->distDT / r->pesoMST, r->distG / r->pesoMST,
               100.0 * (r->distDT - r->distG) / r->distDT,
               r->valido ? "SIM" : "NAO");
    }

    experimentoInstanciasPequenas();

    return 0;
}
