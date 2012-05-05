#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>

#define NITER 10
#define BUFF_TAM 5

typedef struct {
  int BUFFER[BUFF_TAM];
  int in;
  int out;
  int contador;
  sem_t lleno;
  sem_t vacio;
  sem_t mutex;
} std_entorno;

std_entorno shared;


void imprimir_buffer()
{
    int j;
    for(j=0;j<BUFF_TAM;j++)
    {
      if(shared.in>shared.out)
      {
        if(j<shared.out) printf("_");
        else if(j>=shared.out && j<shared.in) printf("*");
        else if(j>=shared.in) printf("_");
      }
      if(shared.in<shared.out)
      {
        if(j<shared.in) printf("*");
        else if(j>=shared.in && j<shared.out) printf("_");
        else if(j>=shared.out) printf("*");
      }
      if(shared.in==shared.out)
      {
        if(shared.contador==0) printf("_");
        else printf("*");
      }
    }
    printf(" %d", shared.contador);
    if(shared.contador == 1) printf(" elemento");
    else printf(" elementos\n", shared.contador);
    printf("\n");
    return;
}

void *Productor(void *arg){
  int item;
  int i;
  for(i = 0; i<NITER; i++)
  { 
    item = i;
    sem_wait(&shared.vacio);
    sem_wait(&shared.mutex);
    shared.BUFFER[shared.in] = item;
    shared.in = (shared.in+1)%BUFF_TAM;
    shared.contador++;
    printf("Productor item(%d)\t\t\t\t",item);
    imprimir_buffer();   
    sem_post(&shared.mutex);
    sem_post(&shared.lleno);     

    if(rand()%2)sleep(1);
  }
  return NULL;
}

void *Consumidor(void *arg){
  int item;
  int i;
  for(i=0;i<NITER; i++)
  {
    sem_wait(&shared.lleno);
    sem_wait(&shared.mutex);
    item = shared.BUFFER[shared.out];
    shared.out = (shared.out+BUFF_TAM+1)%BUFF_TAM;
    shared.contador--;
    printf("\t\t\tConsumidor item(%d)\t",item);
    imprimir_buffer();
    sem_post(&shared.mutex);
    sem_post(&shared.vacio);
    if(rand()%2)sleep(1);
  }
  return NULL;
}


int main()
{
  srand((unsigned)time(NULL));
  pthread_t tProductor, tConsumidor;

  shared.in = 0; 
  shared.out = 0; 
  shared.contador = 0;
  sem_init(&shared.lleno, 0, 0);
  sem_init(&shared.vacio, 0, 5);
  sem_init(&shared.mutex, 0, 1);

  pthread_create(&tProductor, NULL, Productor, NULL);
  pthread_create(&tConsumidor, NULL, Consumidor, NULL);

  pthread_exit(NULL);

}


