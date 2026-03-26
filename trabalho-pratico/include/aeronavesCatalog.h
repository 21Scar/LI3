#ifndef AERONAVESCATALOG_H
#define AERONAVESCATALOG_H

#include "aeronaves.h"
#include <glib.h>

/**
 * @file aeronavesCatalog.h
 * @brief Interface do catálogo de aeronaves.
 *
 * O catálogo armazena e disponibiliza operações sobre todas as aeronaves lidas
 * a partir do dataset, permitindo inserção, pesquisa e iteração. Também expõe
 * uma estrutura auxiliar de ranking (usada por queries) via `GArray`.
 */

typedef struct aeronavesCatalog AeronavesCatalog;
typedef struct aeronaveRanking AeronaveRanking;

/**
 * @brief Cria um catálogo de aeronaves vazio.
 *
 * Aloca e inicializa a estrutura do catálogo e as estruturas internas
 * necessárias (ex.: tabela de hash).
 *
 * @return Apontador para o catálogo criado, ou NULL em caso de falha de
 * alocação.
 */
AeronavesCatalog* criarAeronavesCatalog();

/**
 * @brief Destrói um catálogo de aeronaves.
 *
 * Liberta toda a memória associada ao catálogo, incluindo as aeronaves
 * armazenadas e estruturas auxiliares.
 *
 * @param catalog Catálogo a destruir.
 *
 * @return void.
 */
void destruirAeronavesCatalog(AeronavesCatalog* catalog);

/**
 * @brief Insere uma aeronave no catálogo.
 *
 * @param catalog Catálogo onde inserir.
 * @param aeronave Aeronave a inserir (passa a ser gerida pelo catálogo).
 *
 * @return void.
 */
void inserirAeronave(AeronavesCatalog* catalog, Aeronaves* aeronave);

/**
 * @brief Procura uma aeronave no catálogo pelo identificador.
 *
 * @param catalog Catálogo onde procurar.
 * @param identifier Identificador da aeronave.
 *
 * @return Apontador para a aeronave se existir; NULL caso contrário.
 */
Aeronaves* buscarAeronave(AeronavesCatalog* catalog, const char* identifier);

/**
 * @brief Itera sobre todas as aeronaves do catálogo.
 *
 * Wrapper para percorrer a estrutura interna do catálogo, chamando `func` para
 * cada elemento.
 *
 * @param catalog Catálogo a percorrer.
 * @param func Função callback a invocar por elemento.
 * @param user_data Contexto adicional passado para o callback.
 *
 * @return void.
 */
void aeronaves_foreach(AeronavesCatalog* catalog, GHFunc func, gpointer user_data);

/**
 * @brief Constrói/atualiza o array de ranking de aeronaves.
 *
 * Tipicamente cria um `GArray` com elementos `AeronaveRanking`, usado por
 * queries que necessitam de ordenação/consulta eficiente.
 *
 * @param catalog Catálogo onde construir o array.
 *
 * @return void.
 */
void arrayAeronaves (AeronavesCatalog* catalog);

/**
 * @brief Obtém o `GArray` de ranking/ordenação de aeronaves.
 *
 * @param catalog Catálogo do qual obter o array.
 *
 * @return Apontador para o `GArray` interno, ou NULL se ainda não foi
 * construído.
 */
GArray* getAeronavesArray(AeronavesCatalog* catalog);

/**
 * @brief Obtém o identificador associado a um elemento de ranking.
 *
 * @param ar Elemento de ranking.
 *
 * @return String com o identificador (ponteiro gerido pelo elemento; não deve
 * ser libertado pelo chamador).
 */
const char* getAeronaveRank_Id(const AeronaveRanking* ar);

/**
 * @brief Obtém a aeronave associada a um elemento de ranking.
 *
 * @param ar Elemento de ranking.
 *
 * @return Apontador para `Aeronaves` associada.
 */
Aeronaves* getAeronaveRank_Aeronave(const AeronaveRanking* ar);


#endif