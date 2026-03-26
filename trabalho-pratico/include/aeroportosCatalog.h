#ifndef AEROPORTOSCATALOG_H
#define AEROPORTOSCATALOG_H

#include "aeroportos.h"
#include <glib.h>

/**
 * @file aeroportosCatalog.h
 * @brief Interface do catálogo de aeroportos.
 *
 * Este módulo gere um conjunto de estruturas `Aeroportos` indexadas pelo seu
 * código (tipicamente IATA), fornecendo operações para criar/destruir o catálogo,
 * inserir e procurar aeroportos, e iterar sobre todos os elementos.
 */

typedef struct aeroportosCatalog AeroportosCatalog;

/**
 * @brief Cria um novo catálogo de aeroportos.
 *
 * Inicializa as estruturas internas necessárias (ex.: tabelas hash GLib).
 *
 * @param void (sem parâmetros).
 *
 * @return Apontador para o catálogo criado, ou `NULL` em caso de erro de
 * alocação.
 */
AeroportosCatalog* criarAeroportosCatalog();

/**
 * @brief Destrói um catálogo de aeroportos.
 *
 * Liberta o catálogo e todos os recursos associados (incluindo aeroportos
 * armazenados, se o catálogo for o proprietário desses objetos).
 *
 * @param catalog Catálogo a destruir.
 *
 * @return void.
 */
void destruirAeroportosCatalog(AeroportosCatalog* catalog);

/**
 * @brief Insere um aeroporto no catálogo.
 *
 * Insere a instância `aeroporto` no catálogo, indexando-a pelo seu código.
 *
 * @param catalog Catálogo onde inserir.
 * @param aeroporto Aeroporto a inserir.
 *
 * @return void.
 */
void inserirAeroporto(AeroportosCatalog* catalog, Aeroportos* aeroporto);

/**
 * @brief Procura um aeroporto no catálogo pelo código.
 *
 * @param catalog Catálogo onde procurar.
 * @param code Código do aeroporto (ex.: IATA).
 *
 * @return Apontador para o aeroporto encontrado, ou `NULL` se não existir.
 */
Aeroportos* buscarAeroporto(AeroportosCatalog* catalog, const char* code);

/**
 * @brief Itera sobre todos os aeroportos do catálogo.
 *
 * Chama a função `func` para cada par (chave, valor) existente no catálogo.
 *
 * @param catalog Catálogo a iterar.
 * @param func Função callback no formato `GHFunc` (GLib) a executar por elemento.
 * @param user_data Dados do utilizador passados para a callback.
 *
 * @return void.
 */
void aeroportos_foreach(AeroportosCatalog* catalog, GHFunc func, gpointer user_data);


#endif