# Busca de Números Primos em Matrizes Utilizando Threads (Pthreads)

## Descrição

Este projeto realiza a comparação do resultado de busca **serial** e **paralela** de números primos em uma matriz de grandes dimensões utilizando a biblioteca **Pthreads** em linguagem C. Permitindo que seja analisado em quais cenários o uso de múltiplas threads é superior (ou não) ao processo serial. 

O programa gera uma matriz preenchida com números aleatórios (de 0 a 32000) e realiza a contagem de números primos presentes nela, permitindo comparar o desempenho da execução sequencial com a execução paralela utilizando múltiplas threads.

## Objetivos

- Implementar uma busca serial por números primos.
- Implementar uma busca paralela utilizando threads POSIX (Pthreads).
- Avaliar o ganho de desempenho (speedup) obtido com paralelismo.
- Comparar tempos de execução para diferentes quantidades de threads.

## Configuração da Matriz

Para analisar o resultado em diferentes cenários, é necessário a alteração das diretivas que definem o tamanho máximo da Matriz a ser percorrida (MATRIZ_LINHA, MATRIZ_COLUNA) e dos macroblocos a serem utilizados pelas threads (BLOCO_LINHA, BLOCO_COLUNA). Vale ressaltar que a alteração desse valores afeta diretamente no tempo de execução dos dois tipos de busca (Serial, Paralela).

```c
#define MATRIZ_LINHA 10000
#define MATRIZ_COLUNA 10000
#define BLOCO_LINHA 1000
#define BLOCO_COLUNA 1000
```

## Funcionamento

### Busca Serial

Percorre toda a matriz e contabiliza os números primos encontrados.

### Busca Paralela

Divide a matriz em macroblocos que é processado pela thread, utilizando mutexes para sincronização e atualização segura dos resultados.

## Compilação

### Linux

```bash
gcc main.c -o busca_primos -pthread -lm
```

### Windows (MinGW)

```bash
gcc main.c -o busca_primos.exe -pthread -lm
```

## Execução

### Linux

```bash
./busca_primos
```

### Windows

```bash
busca_primos.exe
```

## Bibliotecas Utilizadas

- stdio.h
- stdlib.h
- math.h
- time.h
- pthread.h

## Autores

- Pedro Henrique Groner
- José Otávio de S. Lessa
- Inácio Almeida

Disciplina: Sistemas Operacionais  
Período: 2026/1
