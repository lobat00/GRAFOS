/*
 * ============================================================
 *  GRAFOS EM C - Implementação Prática Completa
 * ============================================================
 *  Conteúdo:
 *   1. DFS para percorrer componentes
 *   2. Contagem de componentes conexas
 *   3. Cálculo de in-degree
 *   4. Ordenação topológica (Kahn's Algorithm)
 *   5. Detecção de ciclos em digrafos (DFS com coloração)
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── Estruturas ─────────────────────────────────────────── */

#define MAX_VERTICES 100

/* Lista de adjacência */
typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    No *cabeca;
} ListaAdj;

typedef struct {
    int V;               /* número de vértices            */
    ListaAdj *adj;       /* vetor de listas de adjacência */
} Grafo;

/* ─── Auxiliares de lista ────────────────────────────────── */

No *criar_no(int destino) {
    No *novo = (No *)malloc(sizeof(No));
    novo->destino = destino;
    novo->prox    = NULL;
    return novo;
}

/* ─── Criação / destruição ───────────────────────────────── */

Grafo *criar_grafo(int V) {
    Grafo *g = (Grafo *)malloc(sizeof(Grafo));
    g->V   = V;
    g->adj = (ListaAdj *)calloc(V, sizeof(ListaAdj));
    return g;
}

void adicionar_aresta(Grafo *g, int u, int v) {
    /* Insere v na lista de u (digrafo: só uma direção) */
    No *novo = criar_no(v);
    novo->prox      = g->adj[u].cabeca;
    g->adj[u].cabeca = novo;
}

void adicionar_aresta_nao_dir(Grafo *g, int u, int v) {
    adicionar_aresta(g, u, v);
    adicionar_aresta(g, v, u);
}

void liberar_grafo(Grafo *g) {
    for (int i = 0; i < g->V; i++) {
        No *cur = g->adj[i].cabeca;
        while (cur) { No *tmp = cur; cur = cur->prox; free(tmp); }
    }
    free(g->adj);
    free(g);
}

/* ─── 1. DFS ─────────────────────────────────────────────── */
/*
 * Percorre o grafo a partir do vértice `inicio` usando busca em
 * profundidade e marca os vértices visitados em `visitado[]`.
 */
void dfs(Grafo *g, int v, int visitado[]) {
    visitado[v] = 1;
    printf("  visitando vértice %d\n", v);

    for (No *cur = g->adj[v].cabeca; cur; cur = cur->prox) {
        if (!visitado[cur->destino])
            dfs(g, cur->destino, visitado);
    }
}

/* ─── 2. Contagem de componentes conexas ─────────────────── */
/*
 * Funciona para grafos NÃO-DIRIGIDOS.
 * Cada vez que encontra um vértice não visitado, inicia nova DFS
 * → nova componente.
 */
int contar_componentes(Grafo *g) {
    int visitado[MAX_VERTICES] = {0};
    int componentes = 0;

    for (int v = 0; v < g->V; v++) {
        if (!visitado[v]) {
            printf("\n  [Componente %d] raiz = %d\n", componentes + 1, v);
            dfs(g, v, visitado);
            componentes++;
        }
    }
    return componentes;
}

/* ─── 3. Cálculo de in-degree ────────────────────────────── */
/*
 * in-degree[v] = número de arestas que CHEGAM em v.
 * Percorre todas as listas de adjacência e incrementa o destino.
 */
void calcular_in_degree(Grafo *g, int in_degree[]) {
    memset(in_degree, 0, g->V * sizeof(int));

    for (int u = 0; u < g->V; u++)
        for (No *cur = g->adj[u].cabeca; cur; cur = cur->prox)
            in_degree[cur->destino]++;
}

/* ─── 4. Ordenação Topológica (Kahn) ─────────────────────── */
/*
 * Algoritmo de Kahn usando fila (BFS com in-degree):
 *   1. Enfileira todos os vértices com in-degree 0.
 *   2. Remove um vértice da fila, adiciona ao resultado,
 *      decrementa in-degree dos vizinhos; enfileira novos zeros.
 *   3. Se ao final nem todos os vértices foram processados,
 *      o grafo tem ciclo → ordenação topológica impossível.
 *
 * Retorna 1 se sucesso, 0 se detectou ciclo.
 */
int ordenacao_topologica(Grafo *g) {
    int in_degree[MAX_VERTICES];
    calcular_in_degree(g, in_degree);

    /* Fila simples com array */
    int fila[MAX_VERTICES];
    int frente = 0, traseira = 0;

    for (int v = 0; v < g->V; v++)
        if (in_degree[v] == 0)
            fila[traseira++] = v;

    int contagem = 0;
    printf("  Ordem topológica: ");

    while (frente < traseira) {
        int u = fila[frente++];
        printf("%d ", u);
        contagem++;

        for (No *cur = g->adj[u].cabeca; cur; cur = cur->prox) {
            int v = cur->destino;
            if (--in_degree[v] == 0)
                fila[traseira++] = v;
        }
    }
    printf("\n");

    if (contagem != g->V) {
        printf("  *** Ciclo detectado: ordenação topológica impossível! ***\n");
        return 0;
    }
    return 1;
}

/* ─── 5. Detecção de ciclos em digrafos (DFS + coloração) ── */
/*
 * Cada vértice tem uma cor:
 *   BRANCO (0) = não visitado
 *   CINZA  (1) = em processamento (na pilha de recursão)
 *   PRETO  (2) = totalmente processado
 *
 * Se durante a DFS encontramos uma aresta para um vértice CINZA,
 * encontramos uma aresta de retorno → ciclo!
 */
#define BRANCO 0
#define CINZA  1
#define PRETO  2

int dfs_ciclo(Grafo *g, int v, int cor[]) {
    cor[v] = CINZA;

    for (No *cur = g->adj[v].cabeca; cur; cur = cur->prox) {
        int u = cur->destino;
        if (cor[u] == CINZA) {
            printf("  Aresta de retorno: %d -> %d  (CICLO!)\n", v, u);
            return 1;          /* ciclo encontrado */
        }
        if (cor[u] == BRANCO && dfs_ciclo(g, u, cor))
            return 1;
    }

    cor[v] = PRETO;
    return 0;
}

int detectar_ciclo(Grafo *g) {
    int cor[MAX_VERTICES];
    memset(cor, BRANCO, sizeof(cor));

    for (int v = 0; v < g->V; v++)
        if (cor[v] == BRANCO)
            if (dfs_ciclo(g, v, cor))
                return 1;

    return 0;
}

/* ─── Impressão do grafo ─────────────────────────────────── */

void imprimir_grafo(Grafo *g, const char *titulo) {
    printf("\n=== %s (%d vértices) ===\n", titulo, g->V);
    for (int v = 0; v < g->V; v++) {
        printf("  [%d] ->", v);
        for (No *cur = g->adj[v].cabeca; cur; cur = cur->prox)
            printf(" %d", cur->destino);
        printf("\n");
    }
}

/* ─── main ───────────────────────────────────────────────── */

int main(void) {

    printf("╔══════════════════════════════════════════════════╗\n");
    printf("║        GRAFOS EM C — Demonstração Completa       ║\n");
    printf("╚══════════════════════════════════════════════════╝\n");

    /* ── Teste 1: Grafo não-dirigido — componentes conexas ── */
    printf("\n\n━━━ TESTE 1: Componentes Conexas (grafo não-dirigido) ━━━\n");

    Grafo *gnd = criar_grafo(7);
    /* Componente 1: 0-1-2 */
    adicionar_aresta_nao_dir(gnd, 0, 1);
    adicionar_aresta_nao_dir(gnd, 1, 2);
    /* Componente 2: 3-4 */
    adicionar_aresta_nao_dir(gnd, 3, 4);
    /* Componente 3: 5-6 */
    adicionar_aresta_nao_dir(gnd, 5, 6);

    imprimir_grafo(gnd, "Grafo Não-Dirigido");

    printf("\nDFS por componentes:\n");
    int total = contar_componentes(gnd);
    printf("\n  Total de componentes conexas: %d\n", total);
    liberar_grafo(gnd);

    /* ── Teste 2: Digrafo sem ciclo — in-degree + topo ─────── */
    printf("\n\n━━━ TESTE 2: In-degree + Ordenação Topológica (DAG) ━━━\n");
    /*
     *  0 → 1 → 3
     *  0 → 2 → 3
     *  2 → 4
     */
    Grafo *dag = criar_grafo(5);
    adicionar_aresta(dag, 0, 1);
    adicionar_aresta(dag, 0, 2);
    adicionar_aresta(dag, 1, 3);
    adicionar_aresta(dag, 2, 3);
    adicionar_aresta(dag, 2, 4);

    imprimir_grafo(dag, "DAG (sem ciclo)");

    int in_degree[MAX_VERTICES];
    calcular_in_degree(dag, in_degree);
    printf("\n  In-degrees:\n");
    for (int v = 0; v < dag->V; v++)
        printf("    vértice %d: in-degree = %d\n", v, in_degree[v]);

    printf("\n");
    ordenacao_topologica(dag);

    printf("\n  Detecção de ciclo: ");
    printf(detectar_ciclo(dag) ? "CICLO DETECTADO\n" : "Sem ciclo ✓\n");
    liberar_grafo(dag);

    /* ── Teste 3: Digrafo COM ciclo ────────────────────────── */
    printf("\n\n━━━ TESTE 3: Detecção de Ciclo em Digrafo ━━━\n");
    /*
     *  0 → 1 → 2 → 0   (ciclo!)
     *  3 → 4
     */
    Grafo *gcirc = criar_grafo(5);
    adicionar_aresta(gcirc, 0, 1);
    adicionar_aresta(gcirc, 1, 2);
    adicionar_aresta(gcirc, 2, 0);   /* fecha o ciclo */
    adicionar_aresta(gcirc, 3, 4);

    imprimir_grafo(gcirc, "Digrafo com ciclo");

    printf("\n  Buscando ciclos com DFS (coloração):\n");
    if (detectar_ciclo(gcirc))
        printf("  Resultado: CICLO DETECTADO ✓\n");
    else
        printf("  Resultado: Sem ciclo\n");

    printf("\n  Tentando ordenação topológica:\n  ");
    ordenacao_topologica(gcirc);
    liberar_grafo(gcirc);

    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("  Fim da demonstração.\n\n");
    return 0;
}