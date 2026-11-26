# EP3 — Problema do Caixeiro Viajante: respostas

> **Arquivos anexados a esta entrega** (enviados separadamente, todos no mesmo nível):
> `ep3.h`, `primEP3.c`, `doubleTreeEP3.c`, `gulosoNAOEOPRINCIPAL.c`, `comparacaoEntreOsDois.c`, `testeEP3.c`, `README.md`, `relatorio_double_tree.pdf`.
> Cada resposta abaixo indica quais desses arquivos a sustentam.

## 1. Que algoritmo aproximativo você implementou?

Implementamos a heurística **Double Tree** (também chamada de "aproximação por MST") para o Problema do Caixeiro Viajante **métrico/euclidiano**, usando o **algoritmo de Prim** para construir a árvore geradora mínima (MST).

- Prim: [primEP3.c](primEP3.c)
- Double Tree + DFS + custo da rota: [doubleTreeEP3.c](doubleTreeEP3.c)
- Estruturas: [ep3.h](ep3.h)

O vizinho mais próximo ([gulosoNAOEOPRINCIPAL.c](gulosoNAOEOPRINCIPAL.c)) **não é o algoritmo proposto**. Ele só serve de referência para comparação.

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
1. **w(MST) ≤ OPT.** Removendo uma aresta da rota ótima, sobra um caminho que passa por todas as cidades, e esse caminho é uma árvore geradora. A MST é a árvore geradora mais barata, então custa no máximo isso.
2. **Duplicar a MST custa 2·w(MST).** Com cada aresta duplicada, todos os vértices ficam com grau par, então existe um passeio fechado (euleriano) que passa por todas as cidades e custa exatamente 2·w(MST).
3. **Os atalhos não aumentam o custo.** Pulamos as cidades repetidas, indo direto para a próxima ainda não visitada (é a pré-ordem da DFS). Pela **desigualdade triangular**, d(u,w) ≤ d(u,v) + d(v,w), então cada atalho custa no máximo o trecho que ele substitui.

A garantia **vale apenas para custos métricos**. A distância euclidiana usada no EP é uma métrica, então a garantia se aplica.

**Verificação empírica:** na grade regular completa com espaçamento 10, o ótimo é conhecido (n × 10):

| Instância | OPT | Double Tree | Razão DT/OPT |
|---|---|---|---|
| Grade 10×10 (n = 100) | 1 000 | 1 715,603 | **1,72** |
| Grade 100×100 (n = 10 000) | 100 000 | 197 015,051 | **1,97** |

As duas razões ficam abaixo de 2, como a teoria garante. A grade 100×100 chega perto do limite.

**Referências:**
- GeeksforGeeks (prova do fator 2): <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>
- Wikipedia — *Travelling salesman problem* (heurísticas para o caso métrico): <https://en.wikipedia.org/wiki/Travelling_salesman_problem>

**📎 Arquivos anexados para esta resposta:**
- `relatorio_double_tree.pdf`: teorema e demonstração do fator 2 (Seção 1.3, "Por que o Double Tree funciona") e tabela de resultados brutos (Seção 3.1), de onde saem os valores da grade
- `comparacaoEntreOsDois.c`: gera a grade regular (`gerarCidades`, caso `GRADE`, espaçamento 10) usada na verificação empírica

## 3. Explique o algoritmo.

O grafo é completo: toda cidade liga-se a todas as outras, e o peso de cada aresta é a distância euclidiana entre as duas cidades.

**Passo 1 — MST com Prim (`Prim`, [primEP3.c](primEP3.c))**
- Começa pela cidade 0.
- Mantém três vetores: `cidadesVisitadas`, `menorDistancia[i]` (a aresta mais barata que liga a cidade *i* à árvore) e `pai[i]` (o vértice da árvore na outra ponta dessa aresta).
- Em cada uma das n−1 iterações:
  1. Escolhe, entre as cidades fora da árvore, a de menor `menorDistancia`.
  2. Inclui essa cidade na árvore.
  3. Atualiza `menorDistancia` e `pai` das cidades restantes.
- Para economizar `sqrt`, as comparações usam a **distância ao quadrado**, que preserva a ordem. A raiz só é tirada no fim, para somar o peso da MST.
- Resultado: o vetor `pai[]`, que representa a MST.

**Passo 2 — Lista de filhos (`doubleTree`)**
- Converte `pai[]` em listas de adjacência: para cada cidade *i* ≠ 0, insere *i* na lista de filhos de `pai[i]`.

**Passo 3 — DFS em pré-ordem (`DFS`)**
- Faz uma busca em profundidade **iterativa, com pilha**, a partir da cidade 0.
- A ordem em que as cidades são visitadas pela primeira vez (pré-ordem) é exatamente o passeio na árvore duplicada já com os atalhos aplicados. Por isso a duplicação das arestas não precisa ser montada explicitamente.

**Passo 4 — Fechar o ciclo (`calcularDistanciaTotal`)**
- Soma as distâncias entre cidades consecutivas da pré-ordem.
- Soma também a distância da última cidade de volta à primeira.
- Retorna a `Rota` (sequência, tamanho e custo total).

**Referências:**
- Feofiloff (Prim): <https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/prim.html>
- Wikipedia (Prim): <https://en.wikipedia.org/wiki/Prim%27s_algorithm>
- GeeksforGeeks (MST → DFS → rota): <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>

**📎 Arquivos anexados para esta resposta:**
- `primEP3.c`: Passo 1 (função `Prim`)
- `doubleTreeEP3.c`: Passos 2 a 4 (funções `doubleTree`, `DFS` e `calcularDistanciaTotal`)
- `ep3.h`: estruturas `Cidades`, `MST`, `Rota`, `Node` e as funções `euclideanDis` e `euclideanDisSquared`
- `relatorio_double_tree.pdf`: Seções 1.2 e 1.3 (explicação detalhada, com trechos de código)

## 4. Qual a complexidade de tempo e de espaço do algoritmo?

### Tempo: **Θ(n²)**

| Etapa | Custo |
|---|---|
| Prim com vetor, sem heap (n iterações × 2 varreduras de n) | Θ(n²) |
| Montagem das listas de filhos | Θ(n) |
| DFS (n vértices, n−1 arestas) | Θ(n) |
| Custo do ciclo | Θ(n) |
| **Total** | **Θ(n²)** |

No grafo completo há Θ(n²) arestas. Por isso o Prim com vetor é a escolha assintoticamente ótima aqui: com heap binário, ficaria O(E log V) = O(n² log n), que é pior.

Os tempos medidos confirmam o crescimento quadrático. De n = 10 000 para n = 100 000 (10× mais cidades), o tempo do caso aleatório passou de 0,496 s para 50,266 s (≈ 100×).

### Espaço: **Θ(n) auxiliar**
- Prim: `cidadesVisitadas`, `menorDistancia` e `pai`, cada um com n posições.
- Double Tree: vetor de listas de filhos, n−1 nós, pilha da DFS e vetor da rota, todos Θ(n).
- **Não se armazena a matriz de distâncias.** Cada distância é recalculada quando necessária.

> Observação de implementação: `Cidades` usa um vetor estático de tamanho `N = 10 000 000` ([ep3.h](ep3.h)). Por isso a entrada ocupa cerca de 160 MB, seja qual for o n usado. A memória **auxiliar** do algoritmo continua Θ(n).

**Referências:**
- Wikipedia — *Prim's algorithm*, seção "Time complexity" (O(V²) com matriz/vetor): <https://en.wikipedia.org/wiki/Prim%27s_algorithm#Time_complexity>
- Feofiloff (Prim e sua implementação quadrática): <https://users.ime.usp.br/~pf/algoritmos_para_grafos/aulas/prim.html>
- GeeksforGeeks (complexidade da aproximação por MST): <https://www.geeksforgeeks.org/dsa/approximate-solution-for-travelling-salesman-problem-using-mst/>

**📎 Arquivos anexados para esta resposta:**
- `primEP3.c`: os dois laços aninhados que custam Θ(n²)
- `doubleTreeEP3.c`: as etapas lineares (listas de filhos, DFS e custo do ciclo)
- `ep3.h`: definição de `N` e do vetor estático de `Cidades` (observação sobre memória)
- `relatorio_double_tree.pdf`: análise de complexidade (Seções 1.2 e 1.3), tempos medidos (Tabela de resultados) e gráficos log-log com a referência O(n²) (Seção 3.3)

## 5. Exemplo de compilação, uso e comparação

### Compilação e execução

```bash
gcc -march=native -O3 comparacaoEntreOsDois.c primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o teste
./teste
```

O programa gera cidades em 5 distribuições (aleatória, clusters, grade, círculos concêntricos e espiral) e 6 tamanhos (10 a 100 000 cidades). As sementes são fixas, então os resultados são reproduzíveis. Para cada caso, o programa roda o Double Tree e o vizinho mais próximo sobre a **mesma instância**, valida as duas rotas e imprime tempo, distância e diferença percentual.

Há também [testeEP3.c](testeEP3.c), que roda só o Double Tree (10 a 200 000 cidades aleatórias) e valida a rota.

### Uso como função

```c
Cidades *c = malloc(sizeof(*c));
c->numCidades = n;            /* preencher c->cidades[i].x / .y */
Rota *r = doubleTree(c);      /* r->caminho, r->tamanho, r->distanciaTotal */
```

### Comparação com tentativa e erro (força bruta): **não feita, e por quê**

Não implementamos a força bruta. Ela enumera as (n−1)!/2 rotas possíveis e custa **O(n!)**:
- n = 15 → ≈ 4,4 × 10¹⁰ rotas;
- n = 20 → ≈ 6 × 10¹⁶ rotas.

Com isso, ela fica inviável muito antes dos tamanhos que testamos (até 100 000 cidades). Em vez de comparar contra um ótimo que não dá para calcular nesses tamanhos, fizemos duas coisas:
1. comparamos com outra heurística de mesma complexidade, Θ(n²): o **vizinho mais próximo**;
2. medimos o erro real contra o **ótimo conhecido** da grade regular (seção 2).

### Resultado da comparação (Double Tree × vizinho mais próximo, 30 instâncias)

| Distribuição (n = 100 000) | Tempo DT (s) | Tempo guloso (s) | Distância DT | Distância gulosa |
|---|---|---|---|---|
| Aleatória | 50,266 | 25,663 | 319 919,748 | 277 713,643 |
| Clusters | 50,905 | 25,756 | 75 206,083 | 65 583,300 |
| Grade | 20,742 | 9,816 | 1 990 505,041 | 1 004 850,029 |
| Círculos | 18,222 | 9,490 | 8 859,103 | 6 882,934 |
| Espiral | 20,594 | 9,749 | 5 952,882 | 5 952,882 |

- As **30 rotas** de cada algoritmo foram validadas: cada cidade aparece exatamente uma vez.
- O guloso fez rotas **menores em 24 casos**. Houve **6 empates** (espiral com n ≥ 500, círculos e espiral com n = 10). O Double Tree **não venceu estritamente em nenhum caso**.
- O guloso também foi **cerca de 2× mais rápido**. As duas complexidades são Θ(n²), mas o Double Tree tem constante maior (Prim + listas + DFS).

**Conclusão:** o Double Tree oferece **garantia teórica** (≤ 2·OPT), que o vizinho mais próximo não tem. Nos testes, porém, o guloso teve **melhor desempenho prático**. A garantia limita o pior caso. Ela não promete vencer outras heurísticas em instâncias típicas.

Tabela completa, gráficos log-log e discussão: [relatorio_double_tree.pdf](relatorio_double_tree.pdf).

**📎 Arquivos anexados para esta resposta:**
- Para **compilar e rodar a comparação**, os 6 arquivos abaixo precisam ficar **na mesma pasta**:
  - `comparacaoEntreOsDois.c`: programa principal (`main`) da comparação
  - `primEP3.c`
  - `doubleTreeEP3.c`
  - `gulosoNAOEOPRINCIPAL.c`: vizinho mais próximo, usado só como referência
  - `ep3.h`
- `testeEP3.c`: teste isolado do Double Tree. Compile com `gcc -O3 testeEP3.c primEP3.c doubleTreeEP3.c -o testeDT`
- `README.md`: comando de compilação, divisão do grupo e ambiente de hardware dos testes
- `relatorio_double_tree.pdf`: Seção 2 (metodologia) e Seção 3 (resultados das 30 instâncias, gráficos e conclusão)
