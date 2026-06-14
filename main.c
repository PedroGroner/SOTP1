#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define MATRIZ_LINHA 10000
#define MATRIZ_COLUNA 10000
#define BLOCO_LINHA 1000
#define BLOCO_COLUNA 1000

int **matriz;
int totalPrimosParalelo = 0;
int blocoAtual = 0; 
pthread_mutex_t mutex_bloco;
pthread_mutex_t mutex_soma;
// rapaziada, o serial ta sendo bem mais rapido que o paralelo, eu imagino que isso seja comum
// levando em conta que tenho um processador parrudo, qualquer coisa 
// alterem o codigo com o nucleo de voces, se forem diferente de 8 no caso, imagino que se for o msm nao vá mudar mt coisa

// funcoes 

void preencheMatrizes(){
    srand(12345);

    for ( int i = 0; i < MATRIZ_LINHA; i++){
        for ( int j = 0; j < MATRIZ_COLUNA; j++){
            matriz[i][j] = rand() % 32000; 
        }
    }
}

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

void Libera_matrizes(int m, int n, int **v){

    int i;

    if (v == NULL) return;
    if (m < 1|| n < 1){
        printf("Erro: parametro invalido");
        return;
    }

    for (i=0; i < m; i++) free (v[i]);

    free (v);

}

void BuscaSerial(){
    int totalPrimo = 0;
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

int main (void){

    matriz = alocar_matrizes(MATRIZ_LINHA, MATRIZ_COLUNA);


    if (matriz == NULL){
        return 1;
    }

    preencheMatrizes();

    // serial
    clock_t inicio_Contagem = clock();
    BuscaSerial();
    clock_t fim_contagem = clock();
    double tempototal = (double)(fim_contagem - inicio_Contagem) / CLOCKS_PER_SEC;
    printf("Tempo da busca serial: %f segundos \n", tempototal);

    // paralela
    int num_threads = 8;
    pthread_t threads[num_threads];
    pthread_mutex_init(&mutex_bloco, NULL);
    pthread_mutex_init(&mutex_soma, NULL);

    inicio_Contagem = clock();
    for(int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, BuscaParalela, NULL);
    }
    
    for(int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    fim_contagem = clock();
    tempototal = (double)(fim_contagem - inicio_Contagem) / CLOCKS_PER_SEC;
    printf("Tempo da busca paralela: %f segundos \n", tempototal);

    pthread_mutex_destroy(&mutex_bloco);
    pthread_mutex_destroy(&mutex_soma);

    return 0;

}