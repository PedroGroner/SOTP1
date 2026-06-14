#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#define MATRIZ_LINHA 10000
#define MATRIZ_COLUNA 10000
#define BLOCO_LINHA 100
#define BLOCO_COLUNA 100

int **matriz;
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

int main (void){

    matriz = alocar_matrizes(MATRIZ_LINHA, MATRIZ_COLUNA);
    

    if (matriz == NULL){
        return 1;
    }

    preencheMatrizes();
    clock_t inicio_Contagem = clock();
    BuscaSerial();
    clock_t fim_contagem = clock();
    double tempototal = (double)(fim_contagem - inicio_Contagem) / CLOCKS_PER_SEC;
    printf("Tempo da busca serial: %f segundos \n", tempototal);

}