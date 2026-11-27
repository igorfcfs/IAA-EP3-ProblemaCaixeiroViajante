/*
 * Testes de corretude do Prim + Double Tree (e do guloso de referencia).
 *
 * Compilar (ver README.md):
 *   gcc -std=c11 -O3 -march=native -ffp-contract=off testeEP3.c primEP3.c doubleTreeEP3.c \
 *       gulosoNAOEOPRINCIPAL.c -o testes -lm
 * Executar:
 *   ./testes               testes de corretude
 *   ./testes desempenho    mede o Prim original x o otimizado (~2 min)
 *   ./testes otimo         compara DT e guloso com o otimo exato (forca
 *                          bruta), 500 instancias por n = 5..10 (~2 s)
 *
 * O que e verificado:
 *  1. Casos de borda: n = 1, 2, 3, pontos repetidos e pontos colineares.
 *  2. Prim x Prim de referencia (versao original, duas varreduras por
 *     passo): o vetor de pais deve ser IDENTICO.
 *  3. Prim x Kruskal (algoritmo independente): mesmo peso de MST.
 *  4. Rotas do Double Tree e do guloso sao permutacoes validas.
 *  5. Garantia teorica: DT <= 2 * MST.
 *  6. Para n <= 9, comparacao com o otimo exato (forca bruta):
 *     MST <= OPT e DT <= 2 * OPT.
 * O programa termina com codigo 0 se tudo passar e 1 caso contrario.
 */

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include "ep3.h"

static int falhas = 0;
static int verificacoes = 0;

static void verificar(int condicao, const char *descricao, int n) {
    verificacoes++;
    if (!condicao) {
        falhas++;
        printf("  FALHOU: %s (n = %d)\n", descricao, n);
    }
}

static int iguais(double a, double b) {
    return fabs(a - b) <= 1e-9 * (1.0 + fabs(a) + fabs(b));
}

/* ---------- Prim de referencia (implementacao original do grupo) ---------- */
static MST *primReferencia(const Cidades *c) {
    int n = c->numCidades;
    int *visitada = calloc((size_t)n, sizeof(int));
    double *dist = malloc(sizeof(double) * (size_t)n);
    MST *t = malloc(sizeof(MST));
    t->pai = malloc(sizeof(int) * (size_t)n);
    for (int i = 1; i < n; i++) {
        dist[i] = euclideanDisSquared(c->cidades[0], c->cidades[i]);
        t->pai[i] = 0;
    }
    dist[0] = 0;
    t->pai[0] = -1;
    visitada[0] = 1;
    for (int k = 1; k < n; k++) {
        int j = -1;
        double menor = DBL_MAX;
        for (int i = 0; i < n; i++)
            if (!visitada[i] && menor > dist[i]) { menor = dist[i]; j = i; }
        visitada[j] = 1;
        for (int i = 0; i < n; i++) {
            double d = euclideanDisSquared(c->cidades[j], c->cidades[i]);
            if (!visitada[i] && d < dist[i]) { dist[i] = d; t->pai[i] = j; }
        }
    }
    t->pesoTotal = 0;
    for (int i = 1; i < n; i++) t->pesoTotal += sqrt(dist[i]);
    free(visitada);
    free(dist);
    return t;
}

/* ---------- Kruskal com union-find (verificacao independente) ---------- */
typedef struct { int a, b; double w; } Aresta;

static int compararArestas(const void *x, const void *y) {
    double d = ((const Aresta *)x)->w - ((const Aresta *)y)->w;
    return (d > 0) - (d < 0);
}

static int raiz(int *p, int x) {
    while (p[x] != x) x = p[x] = p[p[x]];
    return x;
}

static double kruskal(const Cidades *c) {
    int n = c->numCidades, m = 0;
    if (n < 2) return 0.0;
    Aresta *e = malloc(sizeof(Aresta) * (size_t)n * (size_t)(n - 1) / 2);
    int *p = malloc(sizeof(int) * (size_t)n);
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            e[m++] = (Aresta){i, j, euclideanDis(c->cidades[i], c->cidades[j])};
    qsort(e, (size_t)m, sizeof(Aresta), compararArestas);
    for (int i = 0; i < n; i++) p[i] = i;
    double total = 0.0;
    for (int k = 0; k < m; k++) {
        int a = raiz(p, e[k].a), b = raiz(p, e[k].b);
        if (a != b) { p[a] = b; total += e[k].w; }
    }
    free(e);
    free(p);
    return total;
}

/* ---------- Otimo exato por forca bruta (n <= 9) ---------- */
static double melhorRota;
static int permutacao[16], usada[16];

static void buscaExaustiva(const Cidades *c, int prof, double custo) {
    int n = c->numCidades;
    if (custo >= melhorRota) return;
    if (prof == n) {
        double t = custo + euclideanDis(c->cidades[permutacao[n - 1]], c->cidades[0]);
        if (t < melhorRota) melhorRota = t;
        return;
    }
    for (int i = 1; i < n; i++) {
        if (usada[i]) continue;
        usada[i] = 1;
        permutacao[prof] = i;
        buscaExaustiva(c, prof + 1,
                       custo + euclideanDis(c->cidades[permutacao[prof - 1]], c->cidades[i]));
        usada[i] = 0;
    }
}

static double otimoExato(const Cidades *c) {
    int n = c->numCidades;
    if (n == 1) return 0.0;
    melhorRota = DBL_MAX;
    for (int i = 0; i < n; i++) usada[i] = 0;
    permutacao[0] = 0;
    usada[0] = 1;
    buscaExaustiva(c, 1, 0.0);
    return melhorRota;
}

static int rotaValida(const Rota *r, int n) {
    if (r == NULL || r->caminho == NULL || r->tamanho != n) return 0;
    int *vista = calloc((size_t)n, sizeof(int));
    int ok = 1;
    for (int i = 0; i < n && ok; i++) {
        int v = r->caminho[i];
        ok = v >= 0 && v < n && !vista[v];
        if (ok) vista[v] = 1;
    }
    free(vista);
    return ok;
}

/* Roda todas as verificacoes sobre uma instancia. */
static void testarInstancia(const Cidades *c) {
    int n = c->numCidades;
    MST *t = Prim(c);
    MST *ref = primReferencia(c);
    Rota *dt = doubleTree(c);
    Rota *g = gulosoVizinhoMaisProximo(c);

    verificar(t != NULL, "Prim devolveu NULL", n);
    verificar(dt != NULL && g != NULL, "algoritmo devolveu NULL", n);
    if (!t || !dt || !g) goto fim;

    int paisIguais = 1;
    for (int i = 0; i < n; i++) paisIguais &= (t->pai[i] == ref->pai[i]);
    verificar(paisIguais, "arvore diferente do Prim de referencia", n);
    verificar(iguais(t->pesoTotal, kruskal(c)), "peso da MST difere do Kruskal", n);
    verificar(rotaValida(dt, n), "rota do Double Tree invalida", n);
    verificar(rotaValida(g, n), "rota do guloso invalida", n);
    verificar(dt->distanciaTotal <= 2.0 * t->pesoTotal + 1e-9,
              "Double Tree violou DT <= 2*MST", n);
    verificar(iguais(dt->pesoMST, t->pesoTotal), "pesoMST da rota incorreto", n);

    if (n <= 9) {
        double opt = otimoExato(c);
        verificar(t->pesoTotal <= opt + 1e-9, "MST maior que o otimo", n);
        verificar(dt->distanciaTotal <= 2.0 * opt + 1e-9, "DT maior que 2*OPT", n);
        verificar(g->distanciaTotal >= opt - 1e-9, "guloso abaixo do otimo", n);
        verificar(dt->distanciaTotal >= opt - 1e-9, "DT abaixo do otimo", n);
    }

fim:
    liberarMST(t);
    liberarMST(ref);
    liberarRota(dt);
    liberarRota(g);
}

/* ---------- Modo opcional: desempenho do Prim original x otimizado ---------- */
#ifdef _WIN32
#include <windows.h>
static double agora(void) {
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER t;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&t);
    return (double)t.QuadPart / (double)freq.QuadPart;
}
#else
#include <time.h>
static double agora(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec * 1e-9;
}
#endif

static void compararDesempenho(void) {
    const int tamanhos[] = {10000, 30000, 100000};
    printf("Desempenho: Prim original (2 varreduras de n por passo) x otimizado\n");
    printf("%-10s %8s %14s %14s %9s\n", "Pontos", "n", "Original (s)", "Otimizado (s)", "Ganho");
    for (int tipo = 0; tipo < 2; tipo++) {
        for (int t = 0; t < 3; t++) {
            int n = tamanhos[t];
            Cidades *c = criarCidades(n);
            srand(42);
            int lado = (int)ceil(sqrt((double)n));
            for (int i = 0; i < n; i++) {
                if (tipo == 0) {
                    c->cidades[i].x = (double)rand() / RAND_MAX * 1000.0;
                    c->cidades[i].y = (double)rand() / RAND_MAX * 1000.0;
                } else {
                    c->cidades[i].x = (i % lado) * 10.0;
                    c->cidades[i].y = (i / lado) * 10.0;
                }
            }
            double t0 = agora();
            MST *a = primReferencia(c);
            double t1 = agora();
            MST *b = Prim(c);
            double t2 = agora();
            int iguaisPais = 1;
            for (int i = 0; i < n; i++) iguaisPais &= (a->pai[i] == b->pai[i]);
            printf("%-10s %8d %14.3f %14.3f %8.1fx%s\n",
                   tipo == 0 ? "aleatorio" : "grade", n, t1 - t0, t2 - t1,
                   (t1 - t0) / (t2 - t1), iguaisPais ? "" : "  (ARVORES DIFERENTES!)");
            fflush(stdout);
            liberarMST(a);
            liberarMST(b);
            liberarCidades(c);
        }
    }
}

/* Compara DT e guloso com o otimo exato (forca bruta) em instancias
 * pequenas: e a comparacao com o algoritmo de tentativa e erro. */
static void compararComOtimo(void) {
    const int instancias = 500;
    srand(2024);
    printf("Comparacao com o otimo exato (forca bruta), %d instancias por n\n", instancias);
    printf("pontos uniformes em [0,1000]^2; razao = custo da rota / custo otimo\n\n");
    printf("%4s %10s %12s %12s %10s %12s %12s\n", "n", "DT otimo", "DT/OPT medio",
           "DT/OPT max", "NN otimo", "NN/OPT medio", "NN/OPT max");
    for (int n = 5; n <= 10; n++) {
        int otimoDT = 0, otimoNN = 0;
        double somaDT = 0, maxDT = 0, somaNN = 0, maxNN = 0;
        for (int it = 0; it < instancias; it++) {
            Cidades *c = criarCidades(n);
            for (int i = 0; i < n; i++) {
                c->cidades[i].x = (double)rand() / RAND_MAX * 1000.0;
                c->cidades[i].y = (double)rand() / RAND_MAX * 1000.0;
            }
            double opt = otimoExato(c);
            Rota *dt = doubleTree(c);
            Rota *g = gulosoVizinhoMaisProximo(c);
            double rd = dt->distanciaTotal / opt, rg = g->distanciaTotal / opt;
            somaDT += rd; somaNN += rg;
            if (rd > maxDT) maxDT = rd;
            if (rg > maxNN) maxNN = rg;
            otimoDT += rd <= 1.0 + 1e-9;
            otimoNN += rg <= 1.0 + 1e-9;
            liberarRota(dt);
            liberarRota(g);
            liberarCidades(c);
        }
        printf("%4d %9.1f%% %12.4f %12.4f %9.1f%% %12.4f %12.4f\n", n,
               100.0 * otimoDT / instancias, somaDT / instancias, maxDT,
               100.0 * otimoNN / instancias, somaNN / instancias, maxNN);
    }
}

int main(int argc, char **argv) {
    if (argc > 1 && strcmp(argv[1], "otimo") == 0) {
        compararComOtimo();
        return 0;
    }
    if (argc > 1 && strcmp(argv[1], "desempenho") == 0) {
        compararDesempenho();
        return 0;
    }

    printf("Testes de corretude - EP3 (Prim + Double Tree)\n\n");

    /* 1. Casos de borda */
    printf("[1] Casos de borda\n");
    {
        Cidades *c = criarCidades(9);
        const double pts[][2] = {{0, 0}, {3, 4}, {6, 8}, {0, 0}, {1, 1},
                                 {1, 1}, {1, 1}, {9, 0}, {2, 7}};
        for (int n = 1; n <= 9; n++) {
            c->numCidades = n;
            for (int i = 0; i < n; i++) {
                c->cidades[i].x = pts[i][0];
                c->cidades[i].y = pts[i][1];
            }
            testarInstancia(c);
        }
        /* todos os pontos no mesmo lugar */
        for (int i = 0; i < 9; i++) c->cidades[i].x = c->cidades[i].y = 5.0;
        c->numCidades = 9;
        testarInstancia(c);
        /* pontos colineares */
        for (int i = 0; i < 9; i++) { c->cidades[i].x = (i * 7) % 9; c->cidades[i].y = 0; }
        testarInstancia(c);
        liberarCidades(c);

        Cidades vazio = {NULL, 0};
        verificar(Prim(&vazio) == NULL && doubleTree(&vazio) == NULL,
                  "entrada vazia deveria devolver NULL", 0);
        verificar(doubleTree(NULL) == NULL, "entrada NULL deveria devolver NULL", 0);
    }
    printf("    %d verificacoes, %d falhas ate aqui\n", verificacoes, falhas);

    /* 2-6. Instancias aleatorias (coordenadas inteiras forcam empates) */
    printf("[2] Instancias aleatorias: 300 pequenas (n <= 9) e 200 medias (n <= 400)\n");
    {
        Cidades *c = criarCidades(400);
        srand(7);
        for (int rep = 0; rep < 500; rep++) {
            int n = (rep < 300) ? 1 + rand() % 9 : 10 + rand() % 391;
            c->numCidades = n;
            for (int i = 0; i < n; i++) {
                c->cidades[i].x = rand() % 100;
                c->cidades[i].y = rand() % 100;
            }
            testarInstancia(c);
        }
        /* algumas com coordenadas reais */
        for (int rep = 0; rep < 50; rep++) {
            int n = 2 + rand() % 300;
            c->numCidades = n;
            for (int i = 0; i < n; i++) {
                c->cidades[i].x = (double)rand() / RAND_MAX * 1000.0;
                c->cidades[i].y = (double)rand() / RAND_MAX * 1000.0;
            }
            testarInstancia(c);
        }
        liberarCidades(c);
    }

    printf("\nTotal: %d verificacoes, %d falhas.\n", verificacoes, falhas);
    printf(falhas == 0 ? "TODOS OS TESTES PASSARAM.\n" : "HA TESTES FALHANDO.\n");
    return falhas == 0 ? 0 : 1;
}
