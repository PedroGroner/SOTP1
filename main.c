#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define MATRIZ_LINHA 10000
#define MATRIZ_COLUNA 10000
#define BLOCO_LINHA 1000
#define BLOCO_COLUNA 1000
#define NUM_THREADS 4

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
// FUNÇÕES
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
* @return 1 para Primo, 0 para Não-primo
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

void Liberar_matriz(int m, int n, int **v){
    if (v == NULL) return;
    if (m < 1|| n < 1){
        printf("Erro: parametro invalido");
        return;
    }

    for (int i;=0; i < m; i++) free (v[i]);

    free (v);
}

void BuscaSerial(){
    totalPrimo = 0;
    for (int i = 0; i < MATRIZ_LINHA; i++){
        for (int j = 0; j < MATRIZ_COLUNA; j++){
            if (ehPrimo(matriz[i][j])){
                totalPrimo++;
            }
        }
    }
}

void *BuscaParalela( void *arg){
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

int menu(){
    int opcao;
    printf("Escolha uma opção:\n");
    printf("1 - Busca Serial\n");
    printf("2 - Busca Paralela\n");
    printf("3 - Comparar\n");
    printf("0 - Sair\n");

    scanf("%d", &opcao);
    return opcao;
}

int main (void){
    struct timespec ini_s,ini_p, fim_s, fim_p;
    
    int num_threads = 8;
    pthread_t threads[num_threads];

    // Alocando matriz (MATRIZ_LINHA x MATRIZ_COLUNA) de numeros aleatorios
    matriz = alocar_matrizes(MATRIZ_LINHA, MATRIZ_COLUNA);
    if (matriz == NULL){
        return 1;
    }

    preencherMatriz();

    int opcao = menu();
    while (opcao != 0) {
        switch (opcao) {
            case 1:
                printf("Busca Serial selecionada.\n");
                printf("Processando...\n");

                // serial
                clock_gettime(CLOCK_MONOTONIC, &ini_s);

                BuscaSerial();

                clock_gettime(CLOCK_MONOTONIC, &fim_s);

                double tempototal_s = (double)(fim_s.tv_sec - ini_s.tv_sec) + (fim_s.tv_nsec - ini_s.tv_nsec) / 1e9;
                printf("Tempo da busca serial: %f segundos \n", tempototal_s);
                printf("Total de primos encontrados na busca serial: %d\n", totalPrimo);
                printf("Pressione ENTER para sair...");
                while (getchar() != '\n'); // limpa buffer
                break;
            case 2:
                printf("Busca Paralela selecionada.\n");
                printf("Processando...\n");

                // paralela
                pthread_mutex_init(&mutex_bloco, NULL);
                pthread_mutex_init(&mutex_soma, NULL);

                clock_gettime(CLOCK_MONOTONIC, &ini_p);
                for(int i = 0; i < num_threads; i++) {
                    pthread_create(&threads[i], NULL, BuscaParalela, NULL);
                }
                
                for(int i = 0; i < num_threads; i++) {
                    pthread_join(threads[i], NULL);
                }
                clock_gettime(CLOCK_MONOTONIC, &fim_p);
                double tempototal_p = (double)(fim_p.tv_sec - ini_p.tv_sec) + (fim_p.tv_nsec - ini_p.tv_nsec) / 1e9;
                printf("Tempo da busca paralela: %f segundos \n", tempototal_p);
                printf("Total de primos encontrados na busca paralela: %d\n", totalPrimosParalelo);
                printf("Pressione ENTER para sair...");
                while (getchar() != '\n'); // limpa buffer
                getchar();  
                break;
            case 3:
                printf("Comparar selecionada.\n");
                    printf("Tempo da busca serial: %f segundos \n", tempototal_s);
                    printf("Tempo da busca paralela: %f segundos \n", tempototal_p);
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
        opcao = menu();
    }

    Liberar_matriz(MATRIZ_LINHA, MATRIZ_COLUNA, matriz);

    pthread_mutex_destroy(&mutex_bloco);
    pthread_mutex_destroy(&mutex_soma);

    return 0;
}