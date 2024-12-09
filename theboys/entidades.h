#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"
#include "lista.h"
#include "conjunto.h"

#define N_HEROIS 50
#define N_BASES 10
#define N_MISSOES 5256
#define N_HABILIDADES 10
#define T_FIM_DO_MUNDO 525600
#define N_TAMANHO_MUNDO 20000
#define T_INICIO 0

#define CHEGA 1
#define ESPERA 2
#define DESISTE 3
#define AVISA 4
#define ENTRA 5
#define SAI 6
#define VIAJA 7
#define MORRE 8
#define MISSAO 9
#define FIM 10

struct heroi_t
{
  int ID;
  struct cjto_t *habilidades;
  int paciencia;
  int velocidade;
  int experiencia;
  int IDbase; // ID da base que ele está
  int status; // vivo ou morto
} ;

struct base_t
{
  int ID;
  int lotacao; // numero maximo de herois na base
  struct cjto_t *presentes; // herois presentes na base
  struct lista_t *espera; // fila de espera da base
  int x_base;  // coordenadas da base
  int y_base;
  int filaMax;
  int missoes_frequentadas; 
} ;

struct missao_t
{
  int ID;
  struct cjto_t *habilidades;
  int perigo;
  int status; // cumprida ou não cumprida
  int x_missao; // coordenadas da missao
  int y_missao;
  int tentativas;
} ;

struct mundo_t
{
  struct heroi_t herois[N_HEROIS];
  struct base_t bases[N_BASES];
  struct missao_t missoes[N_MISSOES];
  int nHabilidades;
  int tamanhoMundo;
  int relogio;
  int eventos_tratados;
  int missoes_cumpridas;
  int max_tentativas;
  int min_tentativas;
  int total_tentativas;
} ;

struct evento_t
{
  int tempo;
  int IDheroi;
  int IDbase;
  int destino;
  int IDmissao;
  int tipo;
} ;

long aleat (long min, long max);

struct mundo_t *criar_mundo();

//função que cria os eventos e insere na LEF
void criar_evento(struct fprio_t *LEF, int tempo, int tipo,
                  int ID_heroi, int ID_base, int destino, int ID_missao);

void chega (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base);

void espera (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base);

void desiste (struct fprio_t *LEF, int tempo, int ID_heroi, int ID_base);

void avisa (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_base);

void entra (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base);

void sai (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base);

void viaja (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base_destino);

void morre (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base, int IDmissao);

void missao (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_missao);

void fim (struct mundo_t *mundo);

void eventos_iniciais (struct fprio_t *LEF);