#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define MATRIZ_LINHA 10000
#define MATRIZ_COLUNA 10000
#define BLOCO_LINHA 1000
#define BLOCO_COLUNA 1000

// =================
// VARIÁVEIS GLOBAIS
// =================

int **matriz;

int totalPrimosParalelo = 0;
int totalPrimo = 0;

int blocoAtual = 0; 

pthread_mutex_t mutex_bloco;
pthread_mutex_t mutex_soma;

// =================
// STRUCT DE RESULTADOS
// =================

typedef struct {
    int    num_threads;
    double tempo;
    int    total_primos;
} ResultadoBenchmark;


// =================
// FUNÇÕES PRINCIPAIS
// =================

/*
* Preenche a matriz (global) com valores aleatórios.
*/
void preencherMatriz(){
    srand(12345);

    for ( int i = 0; i < MATRIZ_LINHA; i++){
        for ( int j = 0; j < MATRIZ_COLUNA; j++){
            matriz[i][j] = rand() % 32000; 
        }
    }
}

/*
* Verifica se o número é primo.
*
* @param n Número a ser comparado
* @return 1 para Primo; 0 para Não-primo
*/
int ehPrimo(int n){
    int limite;

    if (n < 2) {
        return 0;
    }

    limite = sqrt(n);

    for (int i = 2; i <= limite; i++){
        if (n % i == 0){
            return 0;
        }
    }
    return 1;
}

/*
* Aloca dinamicamente a matriz (global) com os tamanhos definidos no #define.
*
* @param m Número de linhas da matriz.
* @param n Número de colunas da matriz.
*/
int **alocar_matrizes (int m, int n){

   int **v;
   int i;

   if (m < 1 || n < 1){
    printf("Erro: parametro invalido \n");
    return NULL;
   }

   v = calloc(m, sizeof(int*));
   if ( v == NULL){
    printf("Erro: memoria insuficiente ");
    return NULL;
   }
   
   for ( i = 0; i < m; i++){
    v[i] = calloc(n, sizeof(int));
    if (v[i] == NULL){
        printf("ERRO: memoria insuficiente");
        return NULL;
    }
   }
   return (v);
}

/*
* Libera a matriz (global) da memória.
*
* @param m Número de linhas da matriz.
* @param n Número de colunas da matriz.
*/
void Liberar_matriz(int m, int n, int **v){
    if (v == NULL) return;
    if (m < 1|| n < 1){
        printf("Erro: parametro invalido");
        return;
    }

    for (int i=0; i < m; i++) free (v[i]);

    free (v);
}

/*
* Faz a busca serial por números primos na matriz (global).
* Altera valor da variável global totalPrimo (global).
*/
void BuscaSerial() {
    totalPrimo = 0;
    for (int i = 0; i < MATRIZ_LINHA; i++){
        for (int j = 0; j < MATRIZ_COLUNA; j++){
            if (ehPrimo(matriz[i][j])){
                totalPrimo++;
            }
        }
    }
}

/*
* Define o bloco de trabalho da thread na matriz (global).
* Altera valor da variável global totalPrimoParalelo (global).
* @param arg Ponteiro para argumentos da thread (não utilizado) (pthread_create precisa disso).
*/
void *BuscaThread(void *arg) {
    int total_blocos_linha = (MATRIZ_COLUNA + BLOCO_COLUNA - 1) / BLOCO_COLUNA;
    int total_blocos_coluna = (MATRIZ_LINHA + BLOCO_LINHA - 1) / BLOCO_LINHA;
    int total_blocos = total_blocos_linha * total_blocos_coluna;

    while (1){
        pthread_mutex_lock(&mutex_bloco);
        int bolsoBloco = blocoAtual;
        blocoAtual++;
        pthread_mutex_unlock(&mutex_bloco);

        if(bolsoBloco >= total_blocos) break;

        int start_i = (bolsoBloco / total_blocos_linha) * BLOCO_LINHA;
        int start_j = (bolsoBloco % total_blocos_linha) * BLOCO_COLUNA;
        int end_i = start_i + BLOCO_LINHA > MATRIZ_LINHA ? MATRIZ_LINHA : start_i + BLOCO_LINHA;
        int end_j = start_j + BLOCO_COLUNA > MATRIZ_COLUNA ? MATRIZ_COLUNA : start_j + BLOCO_COLUNA;
        int local_primos = 0;
        for (int i = start_i; i < end_i; i++) {
            for (int j = start_j; j < end_j; j++) {
                if (ehPrimo(matriz[i][j])) local_primos++;
            }
        }
        
        pthread_mutex_lock(&mutex_soma);
        totalPrimosParalelo += local_primos;
        pthread_mutex_unlock(&mutex_soma);
    }
    return NULL;
}

/*
* Faz a busca paralela por números primos na matriz (global).
* Altera valor da variável global totalPrimoParalelo (global).
* @param num_threads Número de threads a serem utilizadas na busca paralela.
*/
void BuscaParalela(int num_threads) {
    pthread_t threads[num_threads];

    totalPrimosParalelo = 0;
    blocoAtual = 0;

    pthread_mutex_init(&mutex_bloco, NULL);
    pthread_mutex_init(&mutex_soma, NULL);

    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, BuscaThread, NULL);
    }
    
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex_bloco);
    pthread_mutex_destroy(&mutex_soma);
}


// =================
// FUNÇÕES DE INTERAÇÃO COM O USUÁRIO
// =================
/*
* Função de menu para interação com o usuário.
*/
int menu(){
    int opcao;
    printf("Escolha uma opção:\n");
    printf("1 - Busca Serial\n");
    printf("2 - Busca Paralela\n");
    printf("3 - Comparar (Apenas após ambos os benchmarks forem executados)\n");
    printf("0 - Sair\n");

    scanf("%d", &opcao);
    return opcao;
}

/*
* Função Limpar tela.
*/
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/*
* Função que mostra os resultados dos benchmarks feitos durante a execução.
*/
void printResultados(ResultadoBenchmark resultados_s, ResultadoBenchmark *resultados_p, int n_resultados) {
    printf("Resultados dos Benchmarks MATRIX(%d x %d) BLOCO(%d x %d):\n", MATRIZ_LINHA, MATRIZ_COLUNA, BLOCO_LINHA, BLOCO_COLUNA);
    printf("Threads | Tempo (s) | Total Primos | Speedup\n");
    printf(" Serial | %9.4f | %12d | %.2f%%\n", resultados_s.tempo, resultados_s.total_primos, 100.0);
    for (int i = 0; i < n_resultados; i++) {
        printf("%7d | %9.4f | %12d | %.2f%%\n",
            resultados_p[i].num_threads,
            resultados_p[i].tempo,
            resultados_p[i].total_primos,
            (resultados_p[i].tempo != 0) ? ((resultados_s.tempo / resultados_p[i].tempo) * 100) : 0
        );
    }
}

/*
* Função que adiciona o resultado do benchmark paralelo no ponteiro de resultados paralelo.
*/
ResultadoBenchmark *adicionarResultado(ResultadoBenchmark *resultados, int *n_resultados, int threads, double tempo, int primos) {
    ResultadoBenchmark *temp = realloc(resultados, (*n_resultados + 1) * sizeof(ResultadoBenchmark));
    
    if (temp == NULL) {
        printf("Erro ao alocar memória resultados paralelos foram limpos...\n");
        *n_resultados = 0;
        return NULL;
    }
    
    resultados = temp;
    resultados[*n_resultados] = (ResultadoBenchmark){
        .num_threads  = threads,
        .tempo        = tempo,
        .total_primos = primos
    };

    (*n_resultados)++;
    return resultados;
}


int main (){
    // Variáveis utilizadas para registro de tempo
    struct timespec ini_s,ini_p, fim_s, fim_p;
    double tempototal_s = 0.0, tempototal_p = 0.0;

    // Variáveis para registro dos resultados
    ResultadoBenchmark resultado_serial = {0};
    ResultadoBenchmark *resultados_paralelo = NULL;
    int n_resultados = 0;

    // Alocando matriz (MATRIZ_LINHA x MATRIZ_COLUNA) de numeros aleatorios
    matriz = alocar_matrizes(MATRIZ_LINHA, MATRIZ_COLUNA);
    if (matriz == NULL){
        return 1;
    }

    // Valores aleatórios até 32000
    preencherMatriz();

    int opcao = menu();
    while (opcao != 0) {
        switch (opcao) {
            // Opção 1: Busca Serial
            case 1:
                limparTela();
                printf("Busca Serial selecionada.\n");
                printf("Processando...\n");

                // Registrando tempo de início e fim da busca serial
                clock_gettime(CLOCK_MONOTONIC, &ini_s);
                BuscaSerial();
                clock_gettime(CLOCK_MONOTONIC, &fim_s);

                tempototal_s = (double)(fim_s.tv_sec - ini_s.tv_sec) + (fim_s.tv_nsec - ini_s.tv_nsec) / 1e9;

                printf("Tempo da busca serial: %f segundos \n", tempototal_s);
                printf("Total de primos encontrados na busca serial: %d\n", totalPrimo);

                // Salvando Serial
                resultado_serial = (ResultadoBenchmark){ .num_threads = 1, .tempo = tempototal_s, .total_primos = totalPrimo};
                break;
            
            // Opção 2: Busca Paralela
            case 2:
                limparTela();
                printf("Busca Paralela selecionada.\n");

                // Solicitando número de threads para a busca paralela
                int num_threads;
                printf("Digite o número de threads para a busca paralela: ");
                scanf("%d", &num_threads);

                printf("Processando...\n");

                // Registrando tempo de início e fim da busca paralela
                clock_gettime(CLOCK_MONOTONIC, &ini_p);
                BuscaParalela(num_threads);
                clock_gettime(CLOCK_MONOTONIC, &fim_p);

                tempototal_p = (double)(fim_p.tv_sec - ini_p.tv_sec) + (fim_p.tv_nsec - ini_p.tv_nsec) / 1e9;

                printf("Tempo da busca paralela: %f segundos \n", tempototal_p);
                printf("Total de primos encontrados na busca paralela: %d\n", totalPrimosParalelo);
                
                // Salvando no pointer de resultados
                resultados_paralelo = adicionarResultado(resultados_paralelo, &n_resultados, num_threads, tempototal_p, totalPrimosParalelo);
                break;

            // Opção 3: Comparar Buscas
            case 3:
                limparTela();
                printf("Comparar selecionada.\n");
                // Verificando se os resultados de busca serial e paralela estão disponíveis para comparação
                if (resultados_paralelo == NULL || n_resultados == 0 || resultado_serial.tempo == 0.0) {
                    printf("Execute a busca serial e paralela primeiro para obter os tempos de referência.\n");
                } else {
                    printResultados(resultado_serial, resultados_paralelo, n_resultados); 
                }
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
        opcao = menu();
    }

    // Liberando memória alocada para a matriz e resultados paralelos
    Liberar_matriz(MATRIZ_LINHA, MATRIZ_COLUNA, matriz);
    free(resultados_paralelo);

    return 0;
}