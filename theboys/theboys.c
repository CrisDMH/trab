// programa principal do projeto "The Boys - 2024/2"
// Autor: Cristiano de Moura Helferich, GRR 20245340

// seus #includes vão aqui

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "entidades.h"
#include "lista.h"
#include "fprio.h"
#include "conjunto.h"

// seus #defines vão aqui

#define N_HEROIS 50
#define N_BASES 10
#define N_MISSOES 5256
#define N_HABILIDADES 10
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define T_INICIO 0

// minimize o uso de variáveis globais

// programa principal
int main ()
{
  int tipo;
  int prio;
  struct fprio_t *LEF = fprio_cria();

  srand(0);

  // iniciar o mundo
  struct mundo_t *mundo = criar_mundo();

  //iniciar os eventos inicias
  eventos_iniciais(LEF);

  // executar o laço de simulação
  while (mundo->relogio <T_FIM_DO_MUNDO && fprio_tamanho(LEF) > 0)
  {
    struct evento_t *evento = (struct evento_t*)LEF->prim->item; 
    fprio_retira(LEF, &tipo, &prio);
    mundo->relogio = evento->tempo;
    mundo->eventos_tratados++;

     switch (tipo)
    {
    case CHEGA:  
      chega(LEF, mundo, evento->tempo, evento->IDheroi, evento->IDbase);
      break;
    case ESPERA: 
      espera(LEF, mundo, evento->tempo, evento->IDheroi, evento->IDbase);
      break;
    case DESISTE: 
      desiste(LEF, evento->tempo, evento->IDheroi, evento->IDbase);
      break;
    case AVISA:  
      avisa(LEF, mundo, evento->tempo, evento->IDbase);
      break;
    case VIAJA:  
      viaja(LEF, mundo, evento->tempo, evento->IDheroi, evento->destino);
      break;
    case ENTRA:  
      entra(LEF, mundo, evento->tempo, evento->IDheroi, evento->IDbase);
      break;
    case SAI:  
      sai(LEF, mundo, evento->tempo, evento->IDheroi, evento->IDbase);
      break;
    case MORRE:  
      morre(LEF, mundo, evento->tempo, evento->IDheroi, evento->IDbase, evento->IDmissao);
      break;
     case MISSAO:  
      missao(LEF, mundo, evento->tempo, evento->IDmissao);
      break;
    case FIM:
      fim(mundo);
      break;
    }

  free(evento);
  evento = NULL;
  }
  
  fprio_destroi(LEF);
  free(mundo);
  mundo = NULL;
  LEF = NULL;

  return (0);
}