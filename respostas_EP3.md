# EP3 — Problema do Caixeiro Viajante: respostas

> **Arquivos anexados a esta entrega** (enviados separadamente, todos no mesmo nível):
> `ep3.h`, `primEP3.c`, `doubleTreeEP3.c`, `gulosoNAOEOPRINCIPAL.c`, `comparacaoEntreOsDois.c`, `testeEP3.c`, `README.md`, `relatorio_double_tree.pdf`.
> Cada resposta abaixo indica quais desses arquivos a sustentam.

## 1. Que algoritmo aproximativo você implementou?

Implementamos a heurística **Double Tree** (também chamada de "aproximação por MST") para o Problema do Caixeiro Viajante **métrico/euclidiano**. A árvore geradora mínima (MST) é construída com o **algoritmo de Prim**.

- Prim: [primEP3.c](primEP3.c)
- Double Tree, DFS e custo da rota: [doubleTreeEP3.c](doubleTreeEP3.c)
- Estruturas e protótipos: [ep3.h](ep3.h)

O vizinho mais próximo ([gulosoNAOEOPRINCIPAL.c](gulosoNAOEOPRINCIPAL.c)) **não é o algoritmo proposto**. Ele serve apenas de referência na comparação.

**Referências:**
- GeeksforGeeks — *Approximate solution for Travelling Salesman Problem using MST*: <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>
- P. Feofiloff (IME-USP) — *Algoritmo de Prim*: <https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/prim.html>
- Wikipedia — *Prim's algorithm*: <https://en.wikipedia.org/wiki/Prim%27s_algorithm>

**📎 Arquivos anexados para esta resposta:**
- `primEP3.c`: algoritmo de Prim (MST)
- `doubleTreeEP3.c`: heurística Double Tree
- `ep3.h`: estruturas e protótipos usados pelos dois
- `relatorio_double_tree.pdf`: Seções 0 e 1 (definição do algoritmo proposto)

## 2. Qual é a estimativa de erro do algoritmo?

O Double Tree é uma **2-aproximação**: a rota produzida custa **no máximo o dobro da rota ótima**.

$$w(\text{Double Tree}) \le 2\,w(\text{MST}) \le 2\,\text{OPT}$$

Ou seja, o erro relativo é de **no máximo 100%** acima do ótimo.

**Por quê:**
1. **w(MST) ≤ OPT.** Removendo uma aresta da rota ótima, sobra um caminho que passa por todas as cidades. Esse caminho é uma árvore geradora, e a MST é a árvore geradora mais barata, então custa no máximo isso.
2. **Duplicar a MST custa 2·w(MST).** Com cada aresta duplicada, todos os vértices ficam com grau par. Logo existe um passeio fechado (euleriano) que passa por todas as cidades e custa exatamente 2·w(MST).
3. **Os atalhos não aumentam o custo.** As cidades repetidas são puladas, indo direto para a próxima ainda não visitada (é a pré-ordem da DFS). Pela **desigualdade triangular**, d(u,w) ≤ d(u,v) + d(v,w), então cada atalho custa no máximo o trecho que substitui.

A garantia **vale apenas para custos métricos**. A distância euclidiana usada no EP é uma métrica, então a garantia se aplica.

**Verificação empírica.** Medimos a distância ao ótimo de três formas:

- **Grade regular com espaçamento 10.** O ótimo é conhecido (n × 10):

  | Instância | OPT | Double Tree | Razão DT/OPT |
  |---|---|---|---|
  | Grade 10×10 (n = 100) | 1 000 | 1 715,603 | **1,72** |
  | Grade 100×100 (n = 10 000) | 100 000 | 197 015,051 | **1,97** |

  As duas razões ficam abaixo de 2. A grade chega perto do limite porque a MST vira um "pente", e os saltos de volta da DFS custam quase tanto quanto refazer cada ramo.

- **MST como limite inferior.** Como w(MST) ≤ OPT, a razão DT/MST é um limite superior para DT/OPT. Nas 30 instâncias principais (até 100 000 cidades), DT/MST ficou entre 1,07 e 1,99.

- **Ótimo exato por força bruta** (n = 5 a 10, 500 instâncias aleatórias por tamanho): o pior caso observado foi **1,51·OPT**. A média ficou de 5% a 17% acima do ótimo (ver resposta 5).

**Referências:**
- GeeksforGeeks (prova do fator 2): <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>
- Wikipedia — *Travelling salesman problem* (heurísticas para o caso métrico): <https://en.wikipedia.org/wiki/Travelling_salesman_problem>

**📎 Arquivos anexados para esta resposta:**
- `relatorio_double_tree.pdf`: teorema e demonstração do fator 2 (Seção 1.3, "Por que o Double Tree funciona"), comparação com o ótimo exato (Seção 2.4.1), tabela de resultados brutos (Seção 3.1) e razões DT/MST (Seção 3.3)
- `comparacaoEntreOsDois.c`: gera a grade regular (`gerarCidades`, caso `GRADE`, espaçamento 10) e imprime as razões DT/MST
- `testeEP3.c`: força bruta (`otimoExato`) e verificação automática de DT ≤ 2·MST e DT ≤ 2·OPT

## 3. Explique o algoritmo.

O grafo é completo: cada cidade liga-se a todas as outras, e o peso de uma aresta é a distância euclidiana entre as duas cidades.

**Passo 1 — MST com Prim (`Prim`, [primEP3.c](primEP3.c))**
- Começa pela cidade 0. As coordenadas são copiadas para dois vetores contíguos, `x[]` e `y[]`, o que favorece o acesso sequencial à memória.
- Mantém `menorDistancia[v]` (a aresta mais barata que liga *v* à árvore), `pai[v]` (o vértice da árvore na outra ponta dessa aresta) e a lista `restantes` das cidades que ainda estão fora da árvore.
- Em cada passo, **uma única varredura** de `restantes`:
  1. atualiza `menorDistancia` e `pai` em relação à cidade que acabou de entrar;
  2. na mesma passagem, escolhe a cidade de menor `menorDistancia`. Em caso de empate, vence o menor índice, então a árvore é idêntica à do Prim clássico.
- A cidade escolhida sai de `restantes` por troca com o último elemento, em O(1). Assim cada passo varre só as cidades que faltam.
- As comparações usam a **distância ao quadrado**, que preserva a ordem e economiza `sqrt`. A raiz só é tirada no fim, para somar o peso da MST.
- Resultado: o vetor `pai[]`, que representa a MST, e o seu peso.

**Passo 2 — Filhos de cada vértice (`doubleTree`)**
- Converte `pai[]` em listas de filhos guardadas em **três vetores contíguos**, em vez de uma lista encadeada com uma alocação por nó:
  1. conta os filhos de cada vértice;
  2. calcula a soma de prefixos em `inicioFilhos`;
  3. preenche `filhos`.
- Os filhos de *v* ficam em `filhos[inicioFilhos[v] .. inicioFilhos[v+1]-1]`.

**Passo 3 — DFS em pré-ordem (`DFS`)**
- Busca em profundidade **iterativa, com pilha de inteiros**, a partir da cidade 0. Os filhos são empilhados em ordem inversa para serem visitados em ordem crescente.
- A ordem em que as cidades são visitadas pela primeira vez (pré-ordem) já é o passeio na árvore duplicada com os atalhos aplicados. Por isso a duplicação das arestas não precisa ser montada explicitamente.

**Passo 4 — Fechar o ciclo (`calcularDistanciaTotal`)**
- Soma as distâncias entre cidades consecutivas da pré-ordem e a distância da última de volta à primeira.
- Retorna a `Rota`: sequência, tamanho, custo total e peso da MST, que serve de limite inferior para o ótimo.

**Referências:**
- Feofiloff (Prim): <https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/prim.html>
- Wikipedia (Prim): <https://en.wikipedia.org/wiki/Prim%27s_algorithm>
- GeeksforGeeks (MST → DFS → rota): <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>

**📎 Arquivos anexados para esta resposta:**
- `primEP3.c`: Passo 1 (função `Prim`), além de `criarCidades`, `liberarCidades` e `liberarMST`
- `doubleTreeEP3.c`: Passos 2 a 4 (funções `doubleTree`, `DFS` e `calcularDistanciaTotal`) e `liberarRota`
- `ep3.h`: estruturas `Cidades`, `MST` e `Rota` e as funções `euclideanDis` e `euclideanDisSquared`
- `relatorio_double_tree.pdf`: Seções 1.2 e 1.3 (explicação detalhada, com trechos de código)

## 4. Qual a complexidade de tempo e de espaço do algoritmo?

### Tempo: **Θ(n²)**

| Etapa | Custo |
|---|---|
| Prim com vetor, sem heap: no passo *k* varre n−k cidades, Σ(n−k) = n(n−1)/2 | Θ(n²) |
| Montagem dos vetores de filhos (contagem, prefixos, preenchimento) | Θ(n) |
| DFS (n vértices, n−1 arestas) | Θ(n) |
| Custo do ciclo | Θ(n) |
| **Total** | **Θ(n²)** |

O grafo completo tem Θ(n²) arestas, e qualquer algoritmo que examine todas elas gasta Ω(n²). Por isso o Prim com vetor é a escolha assintoticamente ótima nesta representação. Com heap binário, ficaria O(E log V) = O(n² log n), que é pior.

Os tempos medidos confirmam o crescimento quadrático. No caso aleatório, de n = 10 000 para n = 100 000 (10× mais cidades), o tempo passou de 0,077 s para 8,28 s (≈ 107×).

A versão atual do Prim faz uma varredura por passo, em vez de duas, e percorre só as cidades restantes. A complexidade é a mesma, mas a constante é menor: com n = 100 000 aleatórias, o Prim caiu de 44,3 s para 8,9 s (5×).

**Para ir abaixo de Θ(n²)**, é preciso usar a geometria do plano. A MST euclidiana está contida na triangulação de Delaunay, que tem O(n) arestas, e com ela o Double Tree inteiro cai para **O(n log n)**. Essa alternativa está discutida, mas não implementada (Seção 1.6 do relatório).

### Espaço: **Θ(n)**
- Entrada: `Cidades` é alocada com exatamente n posições (`criarCidades(n)`).
- Prim: `x`, `y`, `menorDistancia`, `pai` e `restantes`, cada um com n posições.
- Double Tree: `inicioFilhos`, `filhos`, `proximaPos`, a pilha da DFS e o vetor da rota, todos com Θ(n) posições.
- **Não se armazena a matriz de distâncias**, que ocuparia Θ(n²). Cada distância é recalculada quando necessária.

**Referências:**
- Wikipedia — *Prim's algorithm*, seção "Time complexity" (O(V²) com matriz/vetor): <https://en.wikipedia.org/wiki/Prim%27s_algorithm#Time_complexity>
- Feofiloff (Prim e sua implementação quadrática): <https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/prim.html>
- GeeksforGeeks (complexidade da aproximação por MST): <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>

**📎 Arquivos anexados para esta resposta:**
- `primEP3.c`: o laço que custa Θ(n²)
- `doubleTreeEP3.c`: as etapas lineares (vetores de filhos, DFS e custo do ciclo)
- `ep3.h`: estruturas com tamanho dinâmico
- `relatorio_double_tree.pdf`:
  - análise de complexidade: Seções 1.2 e 1.3;
  - limites e alternativas: Seção 1.6;
  - tempos medidos: Seção 3.1;
  - gráficos log-log com a referência n²: Seção 3.4;
  - ganho da otimização do Prim: Seção 3.5.

## 5. Exemplo de compilação, uso e comparação

### Compilação e execução

No PowerShell, dentro da pasta com os arquivos (no Git Bash/Linux, troque `.\` por `./`):

```powershell
# programa de comparação Double Tree x vizinho mais próximo
gcc -std=c11 -O3 -march=native -ffp-contract=off comparacaoEntreOsDois.c primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o teste -lm

# testes de corretude e comparação com a força bruta
gcc -std=c11 -O3 -march=native -ffp-contract=off testeEP3.c primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o testes -lm

.\testes               # testes de corretude (termina com "TODOS OS TESTES PASSARAM.")
.\testes otimo         # DT e guloso x ótimo exato, n = 5..10 (~2 s)
.\testes desempenho    # Prim original x otimizado (~2 min)
.\teste 10000          # comparação rápida, até 10 000 cidades
.\teste > saida.txt    # comparação completa, até 100 000 cidades (~4 min)
```

A opção `-ffp-contract=off` impede o compilador de fundir operações de ponto flutuante. Sem ela, o arredondamento muda de uma máquina para outra, os desempates (grade, círculos) mudam e as rotas deixam de ser reproduzíveis.

O programa de comparação gera cidades em 5 distribuições (aleatória, clusters, grade, círculos concêntricos e espiral) e 6 tamanhos (10 a 100 000 cidades). As sementes são fixas, então os resultados são reproduzíveis. Para cada caso, o programa:
- roda o Double Tree e o vizinho mais próximo sobre a **mesma instância**;
- valida as duas rotas;
- imprime o tempo médio (com relógio de alta resolução, repetindo as execuções rápidas), a distância, a diferença percentual e as razões DT/MST e guloso/MST.

No fim, imprime um resumo separado por `;` e um experimento com 2 000 instâncias pequenas por tamanho.

### Uso como função

```c
#include "ep3.h"

Cidades *c = criarCidades(n);            /* preencher c->cidades[i].x e .y */
Rota *r = doubleTree(c);                 /* r->caminho, r->tamanho, r->distanciaTotal, r->pesoMST */
printf("custo = %.3f (MST = %.3f)\n", r->distanciaTotal, r->pesoMST);
liberarRota(r);
liberarCidades(c);
```

### Comparação com tentativa e erro (força bruta)

A força bruta enumera as (n−1)! ordens de visita a partir da cidade 0 e custa **O(n!)**. Com n = 20, isso já são ≈ 1,2 × 10¹⁷ ordens, então ela é inviável nos tamanhos da comparação principal. Ela serve, porém, como **referência exata** em instâncias pequenas.

O `testeEP3.c` implementa a busca exaustiva (com poda) e a usa de duas formas:
- **Nos testes automáticos:** para n ≤ 9, verifica que MST ≤ OPT e DT ≤ 2·OPT. As 5 754 verificações passaram.
- **Em `.\testes otimo`:** compara as duas heurísticas com o ótimo em 500 instâncias aleatórias para cada n:

| n | DT ótimo | DT/OPT médio | DT/OPT máx. | Guloso ótimo | Guloso/OPT médio | Guloso/OPT máx. |
|---|---|---|---|---|---|---|
| 5 | 37,0% | 1,055 | 1,338 | 45,8% | 1,031 | 1,215 |
| 6 | 22,6% | 1,090 | 1,377 | 32,6% | 1,059 | 1,328 |
| 7 | 13,8% | 1,115 | 1,432 | 23,8% | 1,075 | 1,365 |
| 8 | 11,6% | 1,137 | 1,474 | 22,6% | 1,084 | 1,446 |
| 9 | 5,8% | 1,162 | 1,506 | 17,4% | 1,099 | 1,412 |
| 10 | 5,8% | 1,171 | 1,452 | 12,2% | 1,104 | 1,516 |

Nas 3 000 instâncias, o Double Tree ficou bem abaixo do limite de 2·OPT: o pior caso foi 1,51·OPT. O guloso fica, em média, mais perto do ótimo, mas não tem garantia. Com n = 10, o pior caso dele (1,52) superou o do Double Tree.

### Resultado da comparação (Double Tree × vizinho mais próximo, 30 instâncias)

Tempos medidos num i7-1355U, Windows 11, GCC 13.2. A tabela mostra os casos com n = 100 000:

| Distribuição (n = 100 000) | Tempo DT (s) | Tempo guloso (s) | Distância DT | Distância gulosa |
|---|---|---|---|---|
| Aleatória | 8,28 | 21,10 | 319 918,737 | 277 712,137 |
| Clusters | 9,25 | 24,72 | 75 204,596 | 65 733,761 |
| Grade | 5,76 | 6,75 | 1 990 505,041 | 1 004 850,029 |
| Círculos | 6,90 | 6,41 | 8 865,548 | 6 882,934 |
| Espiral | 4,59 | 6,69 | 5 952,882 | 5 952,882 |

- **Validade:** as **30 rotas** de cada algoritmo foram validadas; cada cidade aparece exatamente uma vez.
- **Distância:** o guloso fez rotas **menores em 25 casos**. Houve **5 empates**, todos na espiral (n = 10 e n ≥ 500). Nessas 30 instâncias, o Double Tree não venceu estritamente nenhuma vez.
- **Instâncias pequenas:** nas 2 000 instâncias aleatórias por tamanho, o Double Tree **vence em cerca de 1 de cada 4** para n entre 10 e 20, com rotas até 28% mais curtas. Essa frequência cai para menos de 5% com n = 160.
- **Tempo:** com 100 000 cidades, o Double Tree foi **mais rápido em 4 das 5 distribuições**. As duas complexidades são Θ(n²). Nos dados aleatórios, o guloso perde tempo com o desvio imprevisível do teste `visitada`, que o processador não consegue antecipar.

**Conclusão:** o Double Tree oferece **garantia teórica** (≤ 2·OPT), que o vizinho mais próximo não tem. Na prática, ficou no máximo 51% acima do ótimo nas instâncias pequenas. Nas instâncias grandes, o guloso produziu rotas menores, e o Double Tree foi, em geral, mais rápido. A garantia limita o pior caso; ela não promete vencer outras heurísticas em instâncias típicas.

A tabela completa, os gráficos log-log e a discussão estão no [relatorio_double_tree.pdf](relatorio_double_tree.pdf).

**📎 Arquivos anexados para esta resposta:**
- Os 5 arquivos abaixo precisam ficar **na mesma pasta** para compilar e rodar a comparação:
  - `comparacaoEntreOsDois.c`: programa principal (`main`) da comparação
  - `primEP3.c`
  - `doubleTreeEP3.c`
  - `gulosoNAOEOPRINCIPAL.c`: vizinho mais próximo, usado só como referência
  - `ep3.h`
- `testeEP3.c`: testes de corretude, comparação com a força bruta (`otimo`) e medição do Prim (`desempenho`). Usa os mesmos arquivos `.c` e o `ep3.h`.
- `README.md`: comandos de compilação e execução, divisão do grupo e ambiente de hardware dos testes
- `relatorio_double_tree.pdf`:
  - compilação e metodologia: Seção 2;
  - testes e força bruta: Seção 2.4;
  - resultados das 30 instâncias, gráficos e conclusão: Seção 3.
