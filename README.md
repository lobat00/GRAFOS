# GRAFOS

# Grafos em C — Implementação Prática

Implementação didática de algoritmos clássicos sobre grafos em linguagem C pura, sem dependências externas. O código cobre os cinco tópicos fundamentais listados abaixo usando uma representação por **lista de adjacência**.

---

## Conteúdo

| # | Algoritmo | Função principal |
|---|-----------|-----------------|
| 1 | DFS — Busca em Profundidade | `dfs()` |
| 2 | Contagem de Componentes Conexas | `contar_componentes()` |
| 3 | Cálculo de In-degree | `calcular_in_degree()` |
| 4 | Ordenação Topológica (Kahn) | `ordenacao_topologica()` |
| 5 | Detecção de Ciclos em Digrafos | `detectar_ciclo()` / `dfs_ciclo()` |

---

## Pré-requisitos

- Compilador C99 ou superior (GCC, Clang, MSVC)
- Nenhuma biblioteca externa

---

## Compilação e execução

```bash
# GCC
gcc -std=c99 -Wall -o grafos grafos.c

# Clang
clang -std=c99 -Wall -o grafos grafos.c

# Executar
./grafos
```

No Windows (MinGW):
```bash
gcc -std=c99 -Wall -o grafos.exe grafos.c
grafos.exe
```

---

## Estrutura de dados

O grafo é representado por **lista de adjacência**, composta por três tipos:

```
No          — nó da lista encadeada (destino + ponteiro para o próximo)
ListaAdj    — cabeça de cada lista (um por vértice)
Grafo       — número de vértices V + vetor de ListaAdj
```

A constante `MAX_VERTICES` (padrão: 100) define o limite de vértices para arrays estáticos auxiliares (visitado, cor, in-degree). O grafo em si aloca memória dinamicamente via `malloc`/`calloc`.

---

## Descrição dos algoritmos

### 1. DFS — Busca em Profundidade

```c
void dfs(Grafo *g, int v, int visitado[]);
```

Percorre recursivamente todos os vértices alcançáveis a partir de `v`, marcando cada um em `visitado[]`. Complexidade **O(V + E)**.

### 2. Componentes Conexas

```c
int contar_componentes(Grafo *g);
```

Aplica-se a **grafos não-dirigidos**. Varre todos os vértices; cada vértice ainda não visitado dispara uma nova DFS que "pinta" sua componente inteira. O número de DFS disparadas é o número de componentes.

**Exemplo usado no código:**
```
Componente 1: 0 — 1 — 2
Componente 2: 3 — 4
Componente 3: 5 — 6
Resultado: 3 componentes
```

### 3. In-degree

```c
void calcular_in_degree(Grafo *g, int in_degree[]);
```

Para cada aresta `u → v` encontrada nas listas de adjacência, incrementa `in_degree[v]`. O resultado é o grau de entrada de cada vértice no digrafo.

### 4. Ordenação Topológica — Algoritmo de Kahn

```c
int ordenacao_topologica(Grafo *g);  // retorna 1 = ok, 0 = ciclo
```

Funciona exclusivamente em **DAGs** (grafos dirigidos acíclicos):

1. Calcula o in-degree de todos os vértices.
2. Enfileira os vértices com `in_degree == 0`.
3. Retira um vértice da fila, imprime, decrementa o in-degree de seus vizinhos; novos zeros entram na fila.
4. Se ao final `contagem < V`, há ciclo e a ordenação é impossível.

**Exemplo usado no código:**
```
0 → 1 → 3
0 → 2 → 3
        2 → 4

Ordem topológica válida: 0 2 4 1 3
```

### 5. Detecção de Ciclos em Digrafos

```c
int detectar_ciclo(Grafo *g);  // retorna 1 = ciclo, 0 = acíclico
```

Usa DFS com **três cores** por vértice:

| Cor | Valor | Significado |
|-----|-------|-------------|
| BRANCO | 0 | Não visitado |
| CINZA | 1 | Na pilha de recursão atual |
| PRETO | 2 | Totalmente processado |

Se durante a DFS for encontrada uma aresta para um vértice **CINZA**, trata-se de uma **aresta de retorno** — prova de que existe um ciclo.

**Exemplo usado no código:**
```
0 → 1 → 2 → 0   ← ciclo fechado
3 → 4

Aresta de retorno detectada: 2 → 0
```

---

## Saída esperada

```
╔══════════════════════════════════════════════════╗
║        GRAFOS EM C — Demonstração Completa       ║
╚══════════════════════════════════════════════════╝

━━━ TESTE 1: Componentes Conexas (grafo não-dirigido) ━━━

=== Grafo Não-Dirigido (7 vértices) ===
  [0] -> 1
  [1] -> 2 0
  ...

DFS por componentes:
  [Componente 1] raiz = 0
    visitando vértice 0 ...
  [Componente 2] raiz = 3 ...
  [Componente 3] raiz = 5 ...

  Total de componentes conexas: 3

━━━ TESTE 2: In-degree + Ordenação Topológica (DAG) ━━━
  ...
  In-degrees:
    vértice 0: in-degree = 0
    vértice 1: in-degree = 1
    ...
  Ordem topológica: 0 2 4 1 3
  Detecção de ciclo: Sem ciclo ✓

━━━ TESTE 3: Detecção de Ciclo em Digrafo ━━━
  Aresta de retorno: 2 -> 0  (CICLO!)
  Resultado: CICLO DETECTADO ✓
  *** Ciclo detectado: ordenação topológica impossível! ***
```

---

## Funções utilitárias

| Função | Descrição |
|--------|-----------|
| `criar_grafo(V)` | Aloca um grafo com `V` vértices |
| `adicionar_aresta(g, u, v)` | Aresta dirigida `u → v` |
| `adicionar_aresta_nao_dir(g, u, v)` | Aresta bidirecional `u ↔ v` |
| `imprimir_grafo(g, titulo)` | Imprime todas as listas de adjacência |
| `liberar_grafo(g)` | Libera toda a memória alocada |

---

## Complexidade

| Algoritmo | Tempo | Espaço |
|-----------|-------|--------|
| DFS | O(V + E) | O(V) |
| Componentes conexas | O(V + E) | O(V) |
| In-degree | O(V + E) | O(V) |
| Ordenação topológica (Kahn) | O(V + E) | O(V) |
| Detecção de ciclos | O(V + E) | O(V) |

---

## Limitações conhecidas

- `MAX_VERTICES` é 100; arrays auxiliares locais usam esse limite. Para grafos maiores, basta aumentar a constante ou substituir por alocação dinâmica.
- A DFS é recursiva; grafos muito densos com centenas de milhares de vértices podem causar estouro de pilha. Para esses casos, a versão iterativa com pilha explícita é recomendada.
- Não há verificação de arestas duplicadas na inserção