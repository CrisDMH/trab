#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

struct lista_t *lista_cria () 
{
  struct lista_t *lst = (struct lista_t *) malloc(sizeof(struct lista_t));

  if (lst == NULL) 
    return NULL; // retorna null para caso de erro
  lst->prim = NULL;
  lst->ult = NULL;
  lst->tamanho = 0;

  return lst;
}

struct lista_t *lista_destroi(struct lista_t *lst) 
{
  if (!lst)
    return NULL;  
  struct item_t *auxiliar = lst->prim;
  while (auxiliar != NULL) 
  {
    struct item_t *prox = auxiliar->prox;
    free(auxiliar); // remove os itens
    auxiliar = prox;
  }
  free(lst);  // libera a memória após remover os itens
  
  return NULL;
}

int lista_insere(struct lista_t *lst, int item, int pos) 
{  
  if (!lst) 
    return -1; // retorna -1 em erro
  
  struct item_t *auxiliar = (struct item_t *) malloc(sizeof(struct item_t));
  if (!auxiliar) 
    return -1;
    
  auxiliar->valor = item;
  auxiliar->ant = NULL;
  auxiliar->prox = NULL;

  if (pos == 0 || lst->tamanho == 0) // insere no começo da lista
  {
    if (lst->prim == NULL)  
      lst->prim = lst->ult = auxiliar;
    else 
    {
      auxiliar->prox = lst->prim;
      lst->prim->ant = auxiliar;
      lst->prim = auxiliar;
    }
  } 
  else if (pos == -1 || pos >= lst->tamanho) // insere no final da lista
  {
    auxiliar->ant = lst->ult;
    if (lst->ult) lst->ult->prox = auxiliar;
    lst->ult = auxiliar;
    if (lst->prim == NULL) lst->prim = auxiliar;
  } 
  else // insere no meio da lista
  {  
    struct item_t *atual = lst->prim;
    for (int i = 0; i < pos; i++) 
      atual = atual->prox;

    auxiliar->prox = atual;
    auxiliar->ant = atual->ant;
    if (atual->ant)
      atual->ant->prox = auxiliar;
    atual->ant = auxiliar;
  }
  lst->tamanho++;

  return lst->tamanho;
}

int lista_retira(struct lista_t *lst, int *item, int pos) 
{
  if (!lst || !item || lst->tamanho == 0) // retorna -1 em erro
    return -1;

  struct item_t *auxiliar = lst->prim;

  if (pos == 0) // retira do começo da lista
  {
    *item = auxiliar->valor;
    lst->prim = auxiliar->prox;
    if (lst->prim) 
      lst->prim->ant = NULL;
    if (lst->ult == auxiliar) 
      lst->ult = NULL;
  } 
  else if (pos == -1 || pos >= lst->tamanho - 1) // retira do final da lista
  { 
    auxiliar = lst->ult;
    *item = auxiliar->valor;
    lst->ult = auxiliar->ant;
    if (lst->ult) 
      lst->ult->prox = NULL;
    if (lst->prim == auxiliar) 
      lst->prim = NULL;
  } 
  else // retira do meio da lista
  {  
    for (int i = 0; i < pos; i++) 
      auxiliar = auxiliar->prox;
        
    *item = auxiliar->valor;
    if (auxiliar->ant) 
      auxiliar->ant->prox = auxiliar->prox;
    if (auxiliar->prox) 
      auxiliar->prox->ant = auxiliar->ant;
  }

  free(auxiliar);
  lst->tamanho--;

  return lst->tamanho;
}

int lista_consulta(struct lista_t *lst, int *item, int pos) 
{
  if (!(lst) || !(item) || lst->tamanho == 0 || pos >= lst->tamanho) 
    return -1;
  
  struct item_t *auxiliar;
  if (pos == -1)
    auxiliar = lst->ult;
  else
  {
    auxiliar = lst->prim;  
    for (int i = 0; i < pos; i++) // percorre a lista até que a posição 
      auxiliar = auxiliar->prox;  // seja encontrada
  }
  *item = auxiliar->valor; // guarda o valor da posição

  return lst->tamanho;
}

int lista_procura(struct lista_t *lst, int valor) 
{
  if (lst == NULL)
    return -1; // retorna -1 com erro
  
  struct item_t *auxiliar = lst->prim;
  int posição = 0;

  while (auxiliar) 
  {
    if (auxiliar->valor == valor)
      return posição; // retorna a posição do valor indicado
    auxiliar = auxiliar->prox;
    posição++;
  }

  return -1; // retorna -1 caso não acha
}

int lista_tamanho(struct lista_t *lst) 
{
  if (lst)
    return lst->tamanho;
  else
    return -1; // retorna -1 se dá erro
}

void lista_imprime(struct lista_t *lst) 
{
  if (!lst)
    return;
  struct item_t *atual = lst->prim;
  int primeiro = 1;
  while (atual != NULL) 
  {
    if (!primeiro) printf(" ");
    printf("%d", atual->valor);
    primeiro = 0;
    atual = atual->prox;
  }
}