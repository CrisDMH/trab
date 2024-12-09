#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"

struct fprio_t *fprio_cria ()
{
  struct fprio_t *f = (struct fprio_t *) malloc(sizeof(struct fprio_t));

  if (f == NULL) 
    return NULL; // retorna null para caso de erro
  f->prim = NULL;
  f->num = 0;

  return f;
}

struct fprio_t *fprio_destroi (struct fprio_t *f) 
{
  if (!f) 
    return NULL;

  struct fpnodo_t *auxiliar = f->prim;
  while (auxiliar != NULL) 
  {
    struct fpnodo_t *prox = auxiliar->prox;
    free(auxiliar->item); // remove os itens
    free(auxiliar);       
    auxiliar = prox;
  }
  free(f); // libera a memória após remover itens

  return NULL;
}

int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio) 
{
  if (!f || !item)
    return -1; 
  
  struct fpnodo_t *repetido = f->prim;
  while (repetido != NULL) 
  {
    if (repetido->item == item) 
      return -1; // retorna erro se o item é repetido
    repetido = repetido->prox;
  }  

  struct fpnodo_t *auxiliar = (struct fpnodo_t *)malloc(sizeof(struct fpnodo_t));
  if (!auxiliar) 
    return -1; // retorna -1 quando ocorre erro na alocação
  
  auxiliar->item = item;
  auxiliar->tipo = tipo;
  auxiliar->prio = prio;
  auxiliar->prox = NULL;
  
  struct fpnodo_t *atual = f->prim; // insere na ordem de prioridade
  struct fpnodo_t *ant = NULL;

  while (atual && (atual->prio < prio || (atual->prio == prio && atual->tipo <= tipo))) 
  {
    ant = atual;
    atual = atual->prox;
  }

  if (!ant) // insere no começo
  {
    auxiliar->prox = f->prim;
    f->prim = auxiliar;
  } 
  else 
  {
    ant->prox = auxiliar;
    auxiliar->prox = atual;
  }
  f->num++;

  return f->num;
}

void *fprio_retira (struct fprio_t *f, int *tipo, int *prio) 
{
  if (!f || !f->prim || !tipo || !prio)
    return NULL; // retornando null para caso de fila vazia, erro ou tipo nulo
    
  struct fpnodo_t *auxiliar = f->prim;
  void *item = auxiliar->item;

  if (tipo != NULL) 
    *tipo = auxiliar->tipo;
  if (prio != NULL)
    *prio = auxiliar->prio;

  f->prim = auxiliar->prox;

  free(auxiliar);
  f->num--;

  return item;
}

int fprio_tamanho (struct fprio_t *f) 
{
  if (f)
    return f->num;
  else
    return -1; // retorna -1 se dá erro

}

void fprio_imprime (struct fprio_t *f) 
{
  if (!f->prim || !f) 
    return;

  struct fpnodo_t *atual = f->prim;
  while (atual != NULL) 
  {
    printf("(%d %d)", atual->tipo, atual->prio);
    if (atual->prox != NULL) 
      printf(" ");
    atual = atual->prox;
  }
}