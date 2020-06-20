/* Data de submissão:
 * Nome: Igor Peterossi Lopes
 * RA: 174929
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define maxprocess 4       // Definimos o número máximo de processos simultâneos
#define MAP_ANONYMOUS 0x20 // Define usado para contornar o fato do compilador do meu IDE não identificar esse define nativamente

/* Função que identifica se o número é primo, retornando o número se ele for 
   primo e -1 se ele não for primo*/
int checarSePrimo(int numero)
{

  // Casos base
  if ((numero == 1) || (numero == 0))
  {
    return -1;
  }

  for (int i = 2; i <= numero / 2; ++i)
  {

    // Se o número não é primo
    if (numero % i == 0)
    {
      numero = -1;
      break;
    }
  }

  return numero;
}

/* Função que recebe a string de entrada com os números a serem análisados e extrai desta 
   cada número e os salva em um vetor */
int receberEntrada(int **numeros)
{

  char entrada[300]; // Armazena a entrada do programa como uma string
  char *p;
  int tam = 0; // Tamanho do vetor que armazena os números

  fgets(entrada, sizeof(entrada), stdin); // Recebemos a string de entrada

  p = strtok(entrada, " \n");     // Selecionamos o primeiro número da entrada
  for (tam = 0; p != NULL; tam++) // Extraímos os números da string
  {
    *numeros = (int *)realloc(*numeros, (sizeof(int) * (tam + 1))); // Alocamos mais espaço para o vetor

    (*numeros)[tam] = atoi(p); // Salvamos no vetor o número convertido para int

    p = strtok(NULL, " \n"); // Selecionamos o próximo número
  }

  return tam;
}

int main(int argc, char *argv[])
{
  int *numeros = NULL; // Vetor que armazena os números a serem avaliados
  int totNums = 0;     // Total de números a serem avaliados
  int numAval = 0;     // Número para testar se é primo
  int *totPrimos;      // Indica o total de números primos identificados

  pid_t pidFilho;     // PID do processo filho
  int totProcess = 0; // Total de processos executando no momento
  int status;         // Usado na função wait, indica o status atual do processo

  // Definimos as flags de proteção e visibilidade da memória
  int protecao = PROT_READ | PROT_WRITE;         // A memória pode ser lida e escrita
  int visibilidade = MAP_SHARED | MAP_ANONYMOUS; // A memória é compartilhada e privada

  totPrimos = (int *)mmap(NULL, sizeof(int), protecao, visibilidade, 0, 0); // Alocamos o espaço necessários na memória compartilhada
  (*totPrimos) = 0;                                                         // Incializamos a variável
  totNums = receberEntrada(&numeros);                                       // Recebemos as entradas do programa

  for (int i = 0; i < totNums; i++)
  {
    totProcess++; // Incrementamos o contador de processos

    for (; totProcess >= maxprocess; totProcess--)
    {
      wait(&status);
    }

    pidFilho = fork(); // Criamos um novo processo

    if (pidFilho == 0) // Se estamos no processo filho
    {
      numAval = checarSePrimo(numeros[i]); // Determinamos se o número é primo

      if (numAval != -1) // Se o número for primo
      {
        (*totPrimos)++;
      }

      exit(0); // Encerramos o processo
    }
  }

  while ((wait(&status) > 0)); // Esperamos o encerramento qualquer processo filho que ainda esteja em execução

  printf("%d\n", *totPrimos); // Printamos o total de números primos encontrados

  return 0;
}
