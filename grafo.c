/*
 * ============================================================
 *  GRAFOS EM C - Entrada Manual + DFS + DAG + Ciclos
 * ============================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define MAX_VERTICES 100

/* ============================================================
 * Estruturas
 * ============================================================
 */

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    No *cabeca;
} ListaAdj;

typedef struct {
    int V;
    ListaAdj *adj;
} Grafo;

/* ============================================================
 * Criação de nós
 * ============================================================
 */

No *criar_no(int destino) {

    No *novo = (No *)malloc(sizeof(No));

    if (!novo) {
        printf("Erro de memória.\n");
        exit(1);
    }

    novo->destino = destino;
    novo->prox = NULL;

    return novo;
}

/* ============================================================
 * Criação do grafo
 * ============================================================
 */

Grafo *criar_grafo(int V) {

    Grafo *g = (Grafo *)malloc(sizeof(Grafo));

    if (!g) {
        printf("Erro de memória.\n");
        exit(1);
    }

    g->V = V;

    g->adj = (ListaAdj *)calloc(V, sizeof(ListaAdj));

    if (!g->adj) {
        printf("Erro de memória.\n");
        exit(1);
    }

    return g;
}

/* ============================================================
 * Inserção de arestas
 * ============================================================
 */

void adicionar_aresta(Grafo *g, int u, int v) {

    No *novo = criar_no(v);

    novo->prox = g->adj[u].cabeca;

    g->adj[u].cabeca = novo;
}

void adicionar_aresta_nao_dir(Grafo *g, int u, int v) {

    adicionar_aresta(g, u, v);

    adicionar_aresta(g, v, u);
}

/* ============================================================
 * Liberar memória
 * ============================================================
 */

void liberar_grafo(Grafo *g) {

    for (int i = 0; i < g->V; i++) {

        No *cur = g->adj[i].cabeca;

        while (cur) {

            No *tmp = cur;

            cur = cur->prox;

            free(tmp);
        }
    }

    free(g->adj);

    free(g);
}

/* ============================================================
 * Impressão do grafo
 * ============================================================
 */

void imprimir_grafo(Grafo *g, int dirigido) {

    printf("\n========================================\n");

    if (dirigido)
        printf("GRAFO DIRIGIDO\n");
    else
        printf("GRAFO NÃO-DIRIGIDO\n");

    printf("========================================\n");

    for (int v = 0; v < g->V; v++) {

        printf("[%d] -> ", v);

        for (No *cur = g->adj[v].cabeca;
             cur;
             cur = cur->prox) {

            printf("%d ", cur->destino);
        }

        printf("\n");
    }
}

/* ============================================================
 * DFS
 * ============================================================
 */

void dfs(Grafo *g, int v, int visitado[]) {

    visitado[v] = 1;

    printf("Visitando vértice %d\n", v);

    for (No *cur = g->adj[v].cabeca;
         cur;
         cur = cur->prox) {

        int u = cur->destino;

        if (!visitado[u]) {
            dfs(g, u, visitado);
        }
    }
}

/* ============================================================
 * Componentes conexas
 * ============================================================
 */

int contar_componentes(Grafo *g) {

    int visitado[MAX_VERTICES] = {0};

    int componentes = 0;

    for (int v = 0; v < g->V; v++) {

        if (!visitado[v]) {

            printf("\nComponente %d:\n",
                   componentes + 1);

            dfs(g, v, visitado);

            componentes++;
        }
    }

    return componentes;
}

/* ============================================================
 * In-degree
 * ============================================================
 */

void calcular_in_degree(Grafo *g,
                         int in_degree[]) {

    memset(in_degree, 0,
           g->V * sizeof(int));

    for (int u = 0; u < g->V; u++) {

        for (No *cur = g->adj[u].cabeca;
             cur;
             cur = cur->prox) {

            in_degree[cur->destino]++;
        }
    }
}

/* ============================================================
 * Ordenação topológica
 * ============================================================
 */

int ordenacao_topologica(Grafo *g) {

    int in_degree[MAX_VERTICES];

    calcular_in_degree(g, in_degree);

    int fila[MAX_VERTICES];

    int frente = 0;
    int traseira = 0;

    for (int v = 0; v < g->V; v++) {

        if (in_degree[v] == 0) {
            fila[traseira++] = v;
        }
    }

    int contagem = 0;

    printf("\nOrdem topológica:\n");

    while (frente < traseira) {

        int u = fila[frente++];

        printf("%d ", u);

        contagem++;

        for (No *cur = g->adj[u].cabeca;
             cur;
             cur = cur->prox) {

            int v = cur->destino;

            in_degree[v]--;

            if (in_degree[v] == 0) {
                fila[traseira++] = v;
            }
        }
    }

    printf("\n");

    if (contagem != g->V) {

        printf("\nCiclo detectado.\n");
        printf("Ordenação topológica impossível.\n");

        return 0;
    }

    return 1;
}

/* ============================================================
 * Detecção de ciclo
 * ============================================================
 */

#define BRANCO 0
#define CINZA 1
#define PRETO 2

int dfs_ciclo(Grafo *g,
              int v,
              int cor[]) {

    cor[v] = CINZA;

    for (No *cur = g->adj[v].cabeca;
         cur;
         cur = cur->prox) {

        int u = cur->destino;

        if (cor[u] == CINZA) {

            printf("\nCiclo encontrado: %d -> %d\n",
                   v, u);

            return 1;
        }

        if (cor[u] == BRANCO &&
            dfs_ciclo(g, u, cor)) {

            return 1;
        }
    }

    cor[v] = PRETO;

    return 0;
}

int detectar_ciclo(Grafo *g) {

    int cor[MAX_VERTICES];

    memset(cor, BRANCO, sizeof(cor));

    for (int v = 0; v < g->V; v++) {

        if (cor[v] == BRANCO) {

            if (dfs_ciclo(g, v, cor)) {
                return 1;
            }
        }
    }

    return 0;
}

/* ============================================================
 * MAIN
 * ============================================================
 */

int main() {

    /* UTF-8 Linux */
    setlocale(LC_ALL, "");

#ifdef _WIN32

    /* UTF-8 Windows */
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#endif

    while (1) {

        int opcao;

        printf("\n========================================\n");
        printf("SISTEMA DE GRAFOS EM C\n");
        printf("========================================\n");

        printf("1 - Criar e analisar grafo\n");
        printf("2 - Limpar tela\n");
        printf("3 - Sair\n");

        printf("\nEscolha: ");
        scanf("%d", &opcao);

        /* ====================================================
         * SAIR
         * ====================================================
         */

        if (opcao == 3) {

            printf("\nEncerrando programa...\n");

            break;
        }

        /* ====================================================
         * LIMPAR TELA
         * ====================================================
         */

        if (opcao == 2) {

#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif

            continue;
        }

        /* ====================================================
         * OPÇÃO INVÁLIDA
         * ====================================================
         */

        if (opcao != 1) {

            printf("\nOpção inválida.\n");

            continue;
        }

        /* ====================================================
         * CRIAÇÃO DO GRAFO
         * ====================================================
         */

        int V;
        int E;
        int dirigido;

        printf("\nNúmero de vértices: ");
        scanf("%d", &V);

        if (V <= 0 || V > MAX_VERTICES) {

            printf("Quantidade inválida.\n");

            continue;
        }

        printf("Número de arestas: ");
        scanf("%d", &E);

        printf("\nTipo do grafo:\n");
        printf("1 - Dirigido\n");
        printf("0 - Não-dirigido\n");

        printf("Escolha: ");
        scanf("%d", &dirigido);

        Grafo *g = criar_grafo(V);

        printf("\nDigite as arestas:\n");
        printf("Formato: origem destino\n\n");

        for (int i = 0; i < E; i++) {

            int u;
            int v;

            printf("Aresta %d: ", i + 1);

            scanf("%d %d", &u, &v);

            if (u < 0 || u >= V ||
                v < 0 || v >= V) {

                printf("Vértice inválido.\n");

                i--;

                continue;
            }

            if (dirigido)
                adicionar_aresta(g, u, v);
            else
                adicionar_aresta_nao_dir(g, u, v);
        }

        /* ====================================================
         * IMPRESSÃO
         * ====================================================
         */

        imprimir_grafo(g, dirigido);

        /* ====================================================
         * DFS / COMPONENTES
         * ====================================================
         */

        printf("\n========================================\n");
        printf("DFS / COMPONENTES\n");
        printf("========================================\n");

        int componentes = contar_componentes(g);

        printf("\nTotal de componentes: %d\n",
               componentes);

        /* ====================================================
         * GRAFO DIRIGIDO
         * ====================================================
         */

        if (dirigido) {

            int in_degree[MAX_VERTICES];

            calcular_in_degree(g, in_degree);

            printf("\n========================================\n");
            printf("IN-DEGREE\n");
            printf("========================================\n");

            for (int v = 0; v < V; v++) {

                printf("Vértice %d -> %d\n",
                       v,
                       in_degree[v]);
            }

            /* ================================================
             * CICLO
             * ================================================
             */

            printf("\n========================================\n");
            printf("DETECÇÃO DE CICLO\n");
            printf("========================================\n");

            if (detectar_ciclo(g))
                printf("O grafo possui ciclo.\n");
            else
                printf("O grafo NÃO possui ciclo.\n");

            /* ================================================
             * ORDENAÇÃO TOPOLOGICA
             * ================================================
             */

            printf("\n========================================\n");
            printf("ORDENAÇÃO TOPOLOGICA\n");
            printf("========================================\n");

            ordenacao_topologica(g);
        }

        liberar_grafo(g);

        printf("\nPressione ENTER para continuar...");

        getchar();
        getchar();

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    return 0;
}