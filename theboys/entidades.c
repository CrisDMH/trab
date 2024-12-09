#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "entidades.h"
#include "conjunto.h"
#include "lista.h"
#include "fprio.h"

struct fprio_t *LEF;

long aleat (long min, long max)
{
  return (rand () % ((max - min) + 1)) + (min);
}

struct mundo_t *criar_mundo()
{
  struct mundo_t *mundo = malloc(sizeof(struct mundo_t));
  
  if (!mundo) 
    return NULL;

  mundo->nHabilidades = N_HABILIDADES;
  mundo->tamanhoMundo = N_TAMANHO_MUNDO;
  mundo->relogio = T_INICIO;
  mundo->eventos_tratados = 0;
  mundo->missoes_cumpridas = 0;
  mundo->max_tentativas = 0;
  mundo->min_tentativas = 0;
  mundo->total_tentativas = 0;

  for (int i = 0; i < N_BASES; i++) // inicializa as bases
  {
    mundo->bases[i].ID = i;
    mundo->bases[i].lotacao = aleat(3, 10);
    mundo->bases[i].presentes = cjto_cria(N_HEROIS);
    mundo->bases[i].espera = lista_cria();
    mundo->bases[i].x_base = aleat(0, N_TAMANHO_MUNDO);
    mundo->bases[i].y_base = aleat(0, N_TAMANHO_MUNDO);
    mundo->bases[i].filaMax = 0;
    mundo->bases[i].missoes_frequentadas = 0; 
  }

  for (int i = 0; i < N_HEROIS; i++) // inicializa os herois
  {
    mundo->herois[i].ID = i;
    mundo->herois[i].habilidades = cjto_aleat(aleat(1, 3),N_HABILIDADES);
    mundo->herois[i].paciencia = aleat(0, 100);
    mundo->herois[i].velocidade = aleat(50, 5000);
    mundo->herois[i].experiencia = 0;
    mundo->herois[i].status = 1; // status do heroi = vivo
    mundo->herois[i].IDbase = aleat(0, N_BASES - 1);

    int base_id = mundo->herois[i].IDbase;
    cjto_insere(mundo->bases[base_id].presentes, i);
  }

  for (int i = 0; i < N_MISSOES; i++) // inicializa as missoes
  {
    mundo->missoes[i].ID = i;
    mundo->missoes[i].habilidades = cjto_aleat(aleat(6, 10), N_HABILIDADES);
    mundo->missoes[i].perigo = aleat(0, 100);
    mundo->missoes[i].status = 0; // status da missao = nao feita
    mundo->missoes[i].x_missao = aleat(0, N_TAMANHO_MUNDO);
    mundo->missoes[i].y_missao = aleat(0, N_TAMANHO_MUNDO);
    mundo->missoes[i].tentativas = 0;
  }

  return mundo;
}

// função que cria o evento e insere na LEF
void criar_evento (struct fprio_t *LEF, int tempo, int tipo,
                  int ID_heroi, int ID_base, int destino, int ID_missao) 
{
  struct evento_t *evento = malloc(sizeof(struct evento_t));
  evento->tempo = tempo;
  evento->tipo = tipo;
  evento->IDheroi = ID_heroi;
  evento->IDbase = ID_base;
  evento->destino = destino;
  evento->IDmissao = ID_missao;

  fprio_insere(LEF, evento, tipo, tempo);
}

void chega (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base)
{
  struct heroi_t *heroi = &mundo->herois[ID_heroi];
  struct base_t *base = &mundo->bases[ID_base];

  heroi->IDbase = ID_base; // atualiza a base de H

  int espera = 0;
  int fila_tamanho = lista_tamanho(base->espera);

  if (cjto_card(base->presentes) < base->lotacao && fila_tamanho == 0) 
  {
    espera = 1; // espera = true pois não há ninguém na fila
  } 
  else 
  {
      if (heroi->paciencia > 10 * fila_tamanho)
        espera = 1;
      else
        espera = 0;
  }

  if (espera) // cria e insere ESPERA na LEF
  {
    criar_evento(LEF, tempo, ESPERA, ID_heroi, ID_base, -1, -1);
    printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) ESPERA\n",
         tempo, ID_heroi, ID_base, cjto_card(base->presentes), base->lotacao);
  } 
  else // cria e insere DESISTE na LEF
  {
    criar_evento(LEF, tempo, DESISTE, ID_heroi, ID_base, -1, -1);
    printf("%6d: CHEGA  HEROI %2d BASE %d (%2d/%2d) DESISTE\n",
         tempo, ID_heroi, ID_base, cjto_card(base->presentes), base->lotacao);
  }
}

void espera (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base) 
{
  struct base_t *base = &mundo->bases[ID_base];

  // joga o heroi para o final da lista de espera da base
  lista_insere(base->espera, ID_heroi, -1);

  // cria e insere AVISA na LEF
  criar_evento(LEF, tempo, AVISA, -1, ID_base, -1, -1);

  printf("%6d: ESPERA  HEROI %2d BASE %d (%2d)\n",
         tempo, ID_heroi, ID_base, lista_tamanho(base->espera));
}

void desiste (struct fprio_t *LEF, int tempo, int ID_heroi, int ID_base) 
{
  int ID_base_destino;

  do
    ID_base_destino = aleat(0, N_BASES- 1);
  while (ID_base_destino == ID_base);

  // cria e insere VIAJA na LEF
  criar_evento(LEF, tempo, VIAJA, ID_heroi, ID_base_destino, -1, -1);

  printf("%6d: DESISTE  HEROI %2d BASE %2d\n",
         tempo, ID_heroi, ID_base);
}

void avisa (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_base) 
{ 
  struct base_t *base = &mundo->bases[ID_base];
  int heroi;
  
  if (mundo->bases[ID_base].filaMax < lista_tamanho(mundo->bases[ID_base].espera))
    mundo->bases[ID_base].filaMax = lista_tamanho(mundo->bases[ID_base].espera);
  // imprime como está a base e a fila de espera dela
  printf("%6d: AVISA  PORTEIRO BASE %d (%2d/%2d) FILA [ ",
         tempo, ID_base, cjto_card(base->presentes), base->lotacao);
  lista_imprime(base->espera);
  printf("]\n");

  // enquanto houver vaga na base e tiver heróis esperando na fila
  while (base->lotacao > cjto_card(base->presentes) 
         && lista_tamanho(base->espera) > 0) 
  {
    // retira o primeiro heroi da fila de espera
    lista_retira (base->espera, &heroi, 0);  
    base->lotacao++;

    // adiciona o heroi na base
    cjto_insere(base->presentes, heroi);

    // cria e insere ENTRA na LEF
    printf("%6d: AVISA  PORTEIRO BASE %d ADMITE %2d\n", tempo, ID_base, heroi);
    criar_evento(LEF, tempo, ENTRA, heroi, ID_base, -1, -1);
  }
}

void entra (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base)
{
  struct base_t *base = &mundo->bases[ID_base];

  // TPB = tempo de permanência na base
  int TPB = 15 + mundo->herois[ID_heroi].paciencia * (aleat(1, 20));

  // atualiza a base do herói
  mundo->herois[ID_heroi].IDbase = ID_base;

  // coloca o herói no conjunto de herois presentes na base
  cjto_insere(base->presentes, ID_heroi);

  // cria e insere SAI na LEF
  criar_evento(LEF, tempo + TPB, SAI, ID_heroi, ID_base, -1, -1);
  printf("%6d: ENTRA  HEROI %2d BASE %d (%2d/%2d) SAI %d\n", 
         tempo, ID_heroi, ID_base, cjto_card(base->presentes), base->lotacao, tempo + TPB);
}

void sai (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base)
{
  struct base_t *base = &mundo->bases[ID_base];
   int ID_base_destino;

  // retira o herói do conjunto de heróis presentes na base
  cjto_retira(base->presentes, ID_heroi);

  do
    ID_base_destino = aleat(0, N_BASES - 1);  
  while (ID_base_destino == ID_base);  

  // cria e insere VIAJA na LEF
  criar_evento(LEF, tempo, VIAJA, ID_heroi, -1, ID_base_destino, -1); 

  // cria e insere AVISA para o porteiro da base
  criar_evento(LEF, tempo, AVISA, -1, ID_base, -1, -1);
  
  printf("%6d: SAI  HEROI %2d BASE %d (%2d/%2d)\n", 
         tempo, ID_heroi, ID_base, cjto_card(base->presentes), base->lotacao);
}

void viaja (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base_destino) 
{
  int dx, dy, distancia, duracao, tempo_para_chegar;
  struct heroi_t *heroi = &mundo->herois[ID_heroi];
  struct base_t *base_atual = &mundo->bases[heroi->IDbase];
  struct base_t base_destino;
  
  base_destino.ID = aleat(0, N_BASES - 1);
  base_destino.x_base = aleat(0, N_TAMANHO_MUNDO - 1);
  base_destino.y_base = aleat(0, N_TAMANHO_MUNDO - 1);

  dx = base_destino.x_base - base_atual->x_base;
  dy = base_destino.y_base - base_atual->y_base;
  distancia = (int)round(sqrt(dx * dx + dy * dy)); 
  duracao = distancia / mundo->herois[ID_heroi].velocidade;
  tempo_para_chegar = tempo + duracao;

  // cria e insere CHEGA na LEF
  criar_evento(LEF, tempo_para_chegar, CHEGA, ID_heroi, base_destino.ID, 0, 0);

  printf("%6d: VIAJA HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n", 
         tempo, ID_heroi, heroi->IDbase, base_destino.ID,
         distancia, heroi->velocidade, tempo_para_chegar);
    
  heroi->IDbase = ID_base_destino;
}

void morre (struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_heroi, int ID_base, int IDmissao)
{
  struct heroi_t *heroi = &mundo->herois[ID_heroi];
  struct base_t *base = &mundo->bases[ID_base];

  // já que o herói morre, retira da base
  cjto_retira(base->presentes, ID_heroi);

  // muda o status para morto
  heroi->status = 0;

  // cria e insere AVISA para o porteiro da base
  criar_evento(LEF, tempo, AVISA, -1, ID_base, -1, -1);

  printf("%6d: MORRE  HEROI %2d MISSAO %d\n", tempo, ID_heroi, IDmissao);
}

void missao(struct fprio_t *LEF, struct mundo_t *mundo, int tempo, int ID_missao)
{
  int i, j, distancia, base_mais_proxima = -1;
  int dx, dy, distanciaBMP = 0;
  struct cjto_t *cjto_auxiliar, *BMP_habilidades = NULL;

  mundo->missoes[ID_missao].tentativas++;

  printf("%6d: MISSAO %d TENT %d HAB REQ: [ ", tempo, ID_missao,
         mundo->missoes[ID_missao].tentativas);
  cjto_imprime(mundo->missoes[ID_missao].habilidades);
  printf(" ]\n");

  for (i = 0; i < N_BASES; i++) // encontra a base mais próxima
  {
    // calcula distancia com a formula da distancia entre pontos
    dx = mundo->bases[i].x_base - mundo->missoes[ID_missao].x_missao;
    dy = mundo->bases[i].y_base - mundo->missoes[ID_missao].y_missao;
    distancia = (int)round(sqrt(dx * dx + dy * dy));

    struct cjto_t *habilidades_herois = cjto_cria(N_HABILIDADES);

    for (j = 0; j < N_HEROIS; j++)
    {
      if (cjto_pertence(mundo->bases[i].presentes, j) &&
          mundo->herois[j].status == 1)
      {
        cjto_auxiliar = cjto_uniao(habilidades_herois, mundo->herois[j].habilidades);
        cjto_destroi(habilidades_herois);
        habilidades_herois = cjto_auxiliar;
      }
    }

    if (cjto_contem(habilidades_herois, mundo->missoes[ID_missao].habilidades))
    {
      if (base_mais_proxima == -1 || distancia < distanciaBMP)
      {
        base_mais_proxima = i;
        cjto_destroi(BMP_habilidades);
        BMP_habilidades = cjto_copia(habilidades_herois);

        distanciaBMP = distancia; // atualiza distância da base mais próxima
      }
    }
    cjto_destroi(habilidades_herois);
  }

  if (base_mais_proxima == -1)
  {
    printf("%6d: MISSAO %d IMPOSSIVEL\n", tempo, ID_missao);
    criar_evento(LEF, tempo + 24 * 60, MISSAO, 0, 0, 0, ID_missao);
    return;
  }

  mundo->missoes[ID_missao].status = 1;

  // atualiza estatísticas de tentativas
  if (mundo->missoes_cumpridas == 0)
    mundo->min_tentativas = mundo->missoes[ID_missao].tentativas;
  else if (mundo->missoes[ID_missao].tentativas < mundo->min_tentativas)
    mundo->min_tentativas = mundo->missoes[ID_missao].tentativas;

  if (mundo->missoes[ID_missao].tentativas > mundo->max_tentativas)
    mundo->max_tentativas = mundo->missoes[ID_missao].tentativas;

  mundo->total_tentativas += mundo->missoes[ID_missao].tentativas;
  mundo->missoes_cumpridas++;
  mundo->bases[base_mais_proxima].missoes_frequentadas++;

  for (j = 0; j < N_HEROIS; j++)
  {
    if (cjto_pertence(mundo->bases[base_mais_proxima].presentes, j))
    {
      int risco = mundo->missoes[ID_missao].perigo /
                  (mundo->herois[j].paciencia + mundo->herois[j].experiencia + 1);

      if (risco > aleat(0, 30))
      {
        criar_evento(LEF, tempo, MORRE, j, base_mais_proxima, 0, ID_missao);
        mundo->herois[j].status = 0;
      }
      else
      {
        mundo->herois[j].experiencia++;
      }
    }
  }

  printf("%6d: MISSAO %d CUMPRIDA BASE %d HABS: [ ",
         tempo, ID_missao, base_mais_proxima);
  cjto_imprime(BMP_habilidades);
  printf(" ]\n");

  cjto_destroi(BMP_habilidades);
}


void fim (struct mundo_t *mundo) 
{
  int i;
  int mortes = 0;

  printf("%6d: FIM\n", mundo->relogio);

  for (i = 0; i < N_HEROIS; i++) 
  {
    printf("HEROI %2d ", mundo->herois[i].ID);
    if (mundo->herois[i].status == 1)
      printf ("VIVO ");
    else
    {
      mortes++; // se o herói está morto adiciona nas mortes totais 
      printf ("MORTO ");
    }
    printf ("PAC %3d VEL %4d EXP %4d HABS [ ",
            mundo->herois[i].paciencia, mundo->herois[i].velocidade, mundo->herois[i].experiencia);
    cjto_imprime(mundo->herois[i].habilidades); // imprime as habilidades do heroi
    printf("]\n");
    cjto_destroi(mundo->herois[i].habilidades);
  }
  
  for (i = 0; i < N_BASES; i++) 
  {
    printf("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n",
            i, mundo->bases[i].lotacao, mundo->bases[i].filaMax, mundo->bases->missoes_frequentadas ); 
    cjto_destroi(mundo->bases[i].presentes);
    lista_destroi(mundo->bases[i].espera);

  }

  for (i = 0; i < N_MISSOES; i++) 
    cjto_destroi(mundo->missoes[i].habilidades);

  float taxa_cumprimento = ((float)mundo->missoes_cumpridas / N_MISSOES) * 100;
  float media_tentativas = mundo->missoes_cumpridas > 0 ? 
    (float)mundo->total_tentativas / mundo->missoes_cumpridas : 0;
  
  float taxa_mortalidade = (float)mortes / N_HEROIS * 100.0;

  printf("EVENTOS TRATADOS: %d\n", mundo->eventos_tratados);
  printf("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n",
         mundo->missoes_cumpridas, N_MISSOES, taxa_cumprimento);
  printf("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n",
         mundo->min_tentativas, mundo->max_tentativas, media_tentativas);
  printf("TAXA MORTALIDADE: %.1f%%\n", taxa_mortalidade);
}

void eventos_iniciais (struct fprio_t *LEF)
{
  int tempo, base;

  for (int i = 0; i < N_HEROIS; i++) // inicializa herois
  {
    tempo = aleat(0, 4320);
    base = aleat(0, N_BASES - 1);
    criar_evento(LEF, tempo, CHEGA, i, base, 0, 0);
  }

  for (int i = 0; i < N_MISSOES; i++)
  {
    tempo = aleat(0, T_FIM_DO_MUNDO);
    criar_evento(LEF, tempo, MISSAO, 0, 0, 0, i);
  }

  tempo = T_FIM_DO_MUNDO;
  criar_evento(LEF, tempo, FIM, 0, 0, 0, 0);
}