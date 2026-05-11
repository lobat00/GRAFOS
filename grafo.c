/*
 * ============================================================
 *  GRAFOS EM C
 *  Entrada Manual + DFS + Ciclos + Desenho ASCII
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

#define RESET   "\x1b[0m"
#define VERMELHO "\x1b[31m"
#define VERDE    "\x1b[32m"
#define AMARELO  "\x1b[33m"
#define AZUL     "\x1b[34m"
#define MAGENTA_PISCANDO   "\x1b[5;95m"


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
        printf(VERMELHO "Erro de memoria.\n" RESET);
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
        printf(VERMELHO "Erro de memoria.\n" RESET);
        exit(1);
    }

    g->V = V;

    g->adj = (ListaAdj *)calloc(V, sizeof(ListaAdj));

    if (!g->adj) {
        printf(VERMELHO "Erro de memoria.\n" RESET);
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
 * Impressão lista de adjacência
 * ============================================================
 */

void imprimir_grafo(Grafo *g, int dirigido) {

    printf("\n========================================\n");

    if (dirigido)
        printf(VERDE "GRAFO DIRIGIDO\n" RESET);
    else
        printf(VERDE "GRAFO NAO-DIRIGIDO\n" RESET);

    printf("========================================\n");

    for (int v = 0; v < g->V; v++) {

        printf(AMARELO "[%d] -> " RESET, v);

        for (No *cur = g->adj[v].cabeca;
             cur;
             cur = cur->prox) {

            printf("%d ", cur->destino);
        }

        printf("\n");
    }
}

/* ============================================================
 * Desenho ASCII do grafo
 * ============================================================
 */

void desenhar_grafo(Grafo *g, int dirigido) {

    printf("\n========================================\n");
    printf(VERDE "DESENHO DO GRAFO\n" RESET);
    printf("========================================\n\n");

    int matriz[MAX_VERTICES][MAX_VERTICES];

    memset(matriz, 0, sizeof(matriz));

    /* monta matriz */

    for (int u = 0; u < g->V; u++) {

        for (No *cur = g->adj[u].cabeca;
             cur;
             cur = cur->prox) {

            matriz[u][cur->destino] = 1;
        }
    }

    /* imprime arestas */

    for (int i = 0; i < g->V; i++) {

        for (int j = 0; j < g->V; j++) {

            if (matriz[i][j]) {

                /* evita duplicar aresta
                   em não-dirigido */

                if (!dirigido && i > j)
                    continue;

                if (dirigido)
                    printf("(%d) " AMARELO "----->" RESET " (%d)\n", i, j);
                else
                    printf("(%d) " AMARELO "-------" RESET " (%d)\n", i, j);
            }
        }
    }

    printf("\n");
}

/* ============================================================
 * DFS
 * ============================================================
 */

void dfs(Grafo *g, int v, int visitado[]) {

    visitado[v] = 1;

    printf(AMARELO "Visitando vertice " RESET "%d\n", v);

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

            printf(AZUL "\nComponente %d:\n" RESET,
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

    printf(AZUL "Ordem topologica:\n" RESET);

    int primeiro = 1;

    while (frente < traseira) {

        int u = fila[frente++];

        if (!primeiro) {
            printf(AZUL "--> " RESET);
        }

        printf(AMARELO "%d " RESET, u);

        primeiro = 0;

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

        printf("VERMELHO \nCiclo detectado.\n" RESET);
        printf(VERMELHO "Ordenacao topologica impossivel.\n" RESET);

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

    setlocale(LC_ALL, "");

#ifdef _WIN32

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#endif

    while (1) {

        int opcao;

        printf("\n========================================\n");
        printf(VERDE "SISTEMA DE GRAFOS EM C\n" RESET);
        printf("========================================\n");

        printf(AZUL "1 - Criar e analisar grafo\n" RESET);
        printf(AZUL "2 - Limpar tela\n" RESET);
        printf(AZUL "3 - Sair\n" RESET);

        printf(AMARELO "\nEscolha: " RESET);
        scanf("%d", &opcao);

        /* ================================================ */

        if (opcao == 3) {

            printf("\nEncerrando programa...\n");
            break;
        }

        /* ================================================ */

        if (opcao == 2) {

#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
            continue;
        }

        /* ================================================ */

        if (opcao != 1) {

            printf(VERMELHO "\nOpcao invalida.\n" RESET);
            continue;
        }

        /* ================================================ */

        int V;
        int E;
        int dirigido;

        printf(AZUL "\nDigite a quantidade de: " RESET);
        printf(AMARELO "\nVertices: " RESET);
        scanf("%d", &V);

        if (V <= 0 || V > MAX_VERTICES) {

            printf(VERMELHO "Quantidade invalida.\n" RESET);

            continue;
        }

        printf(AMARELO "Arestas: " RESET);
        scanf("%d", &E);

        printf(AZUL "\nTipo do grafo:\n" RESET);
        printf("0 - Nao-dirigido\n");
        printf("1 - Dirigido\n");

        printf(AMARELO "\nEscolha: " RESET);
        scanf("%d", &dirigido);

        Grafo *g = criar_grafo(V);

        printf(AZUL "\nDigite as arestas:\n" RESET);
        printf("Formato: origem destino\n\n");

        for (int i = 0; i < E; i++) {

            int u;
            int v;

            printf(AMARELO "Aresta %d: " RESET, i + 1);

            scanf("%d %d", &u, &v);

            if (u < 0 || u >= V ||
                v < 0 || v >= V) {

                printf("Vertice invalido.\n");

                i--;

                continue;
            }

            if (dirigido)
                adicionar_aresta(g, u, v);
            else
                adicionar_aresta_nao_dir(g, u, v);
        }

        /* ================================================
         * IMPRESSÃO
         * ================================================
         */

        imprimir_grafo(g, dirigido);

        /* ================================================
         * DESENHO ASCII
         * ================================================
         */

        desenhar_grafo(g, dirigido);

        /* ================================================
         * DFS / COMPONENTES
         * ================================================
         */

        printf("\n========================================\n");
        printf(VERDE "DFS / COMPONENTES\n" RESET);
        printf("========================================\n");

        int componentes = contar_componentes(g);

        printf(AMARELO "\nTotal de componentes: " RESET "%d\n", componentes);

        /* ================================================
         * GRAFO DIRIGIDO
         * ================================================
         */

        if (dirigido) {

            int in_degree[MAX_VERTICES];

            calcular_in_degree(g, in_degree);

            printf("\n========================================\n");
            printf(VERDE "IN-DEGREE\n" RESET);
            printf("========================================\n");

            for (int v = 0; v < V; v++) {

            printf(AMARELO "Vertice " RESET "%d " AMARELO "->" RESET " %d\n",
                       v,
                       in_degree[v]);
            }

            printf("\n========================================\n");
            printf(VERDE "DETECCAO DE CICLO\n" RESET);
            printf("========================================\n");

            if (detectar_ciclo(g))
                printf(VERMELHO "O grafo possui ciclo.\n" RESET);
            else
                printf(AMARELO "O grafo NAO possui ciclo.\n" RESET);

            printf("\n========================================\n");
            printf(VERDE "ORDENACAO TOPOLOGICA\n" RESET);
            printf("========================================\n");

            ordenacao_topologica(g);
        }

        liberar_grafo(g);

        printf(MAGENTA_PISCANDO "\nPressione ENTER para continuar..." RESET);

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