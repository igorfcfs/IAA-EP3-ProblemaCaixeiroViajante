# EP3 — Introdução à Análise de Algoritmos

**Problema do Caixeiro Viajante: Prim + Double Tree**

> **O GULOSO NÃO É O ALGORITMO PROPOSTO. ELE SERVE APENAS COMO MÉTRICA PARA O DOUBLE TREE + PRIM E NÃO DEVE SER AVALIADO.**

## Arquivos

| Arquivo | Conteúdo |
|---|---|
| `ep3.h` | estruturas (`Cidades`, `MST`, `Rota`) e protótipos |
| `primEP3.c` | algoritmo de Prim (MST) e alocação das estruturas |
| `doubleTreeEP3.c` | Double Tree: listas de filhos, DFS e custo da rota |
| `gulosoNAOEOPRINCIPAL.c` | vizinho mais próximo (apenas referência) |
| `comparacaoEntreOsDois.c` | programa de comparação (gera os resultados do relatório) |
| `testeEP3.c` | testes de corretude |
| `relatorio_double_tree.tex` / `.pdf` | relatório |

## Como compilar

Requisito: GCC (no Windows, MinGW-w64). Rode os comandos dentro desta pasta.

1. Programa de comparação:

   ```sh
   gcc -std=c11 -O3 -march=native -ffp-contract=off comparacaoEntreOsDois.c primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o teste -lm
   ```

2. Testes de corretude:

   ```sh
   gcc -std=c11 -O3 -march=native -ffp-contract=off testeEP3.c primEP3.c doubleTreeEP3.c gulosoNAOEOPRINCIPAL.c -o testes -lm
   ```

Sobre as opções:

| Opção | Efeito |
|---|---|
| `-O3 -march=native` | otimização máxima para o processador da máquina |
| `-ffp-contract=off` | impede o compilador de fundir multiplicação e soma (FMA). Sem isso, o arredondamento das distâncias pode mudar de um computador ou compilador para outro e, nos casos com empates (círculos, grade), a rota muda. |
| `-lm` | biblioteca matemática (`sqrt`, `cos`, `sin`) |

## Como executar

No PowerShell, use `.\` antes do nome; no Git Bash ou no Linux, use `./`.

| Comando | O que faz |
|---|---|
| `.\testes` | roda os testes de corretude (alguns segundos). Termina com `TODOS OS TESTES PASSARAM.` e código de saída 0. |
| `.\testes desempenho` | mede o Prim original contra o Prim otimizado com n = 10.000, 30.000 e 100.000 (cerca de 2 minutos). |
| `.\testes otimo` | compara Double Tree e guloso com o ótimo exato (força bruta), em 500 instâncias para cada n = 5..10 (cerca de 2 segundos). |
| `.\teste 10000` | comparação rápida, só com tamanhos até 10.000 (segundos). |
| `.\teste` | comparação completa, até 100.000 cidades (alguns minutos; os casos de 100.000 dominam o tempo). |
| `.\teste > saida.txt` | grava toda a saída num arquivo. |

Saída do programa de comparação:

- um bloco para cada combinação de distribuição e tamanho, com tempo médio, distância, validade da rota, peso da MST e as razões DT/MST e guloso/MST;
- um `RESUMO` final separado por `;`, que pode ser aberto no Excel ou no LibreOffice;
- a `PARTE 2`: 2.000 instâncias aleatórias pequenas por tamanho, com a porcentagem de vitórias de cada algoritmo.

Os tempos são medidos com relógio de parede de alta resolução. Execuções rápidas são repetidas até somar 0,25 s, e o valor mostrado é a média. As distâncias são determinísticas (sementes fixas); os tempos variam com a máquina e com a carga do computador.

## Organização do grupo

| Integrante | Função |
|---|---|
| Matheus Resende | Implementação do código |
| Guilherme Costa | Implementação e pesquisa |
| Ezequiel Nogueira | Pesquisa e relatório |
| Igor Casita | Relatório |

## Computador usado nas medições do relatório

| Componente | Especificação |
|---|---|
| Processador (CPU) | 13th Gen Intel Core i7-1355U (10 núcleos: 2 de desempenho + 8 de eficiência; 12 threads) |
| Memória RAM total | 16.454.352 kB (~15,69 GB) |
| Sistema operacional | Microsoft Windows 11 Home Single Language (v10.0.26200), 64 bits |
| Compilador C | GCC 13.2.0 (MinGW-w64, x86_64-ucrt-posix-seh) |

**Leia o relatório ([`relatorio_double_tree.pdf`](relatorio_double_tree.pdf)) para mais detalhes.**
