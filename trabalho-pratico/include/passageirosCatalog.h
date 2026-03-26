#ifndef PASSAGEIROSCATALOG_H
#define PASSAGEIROSCATALOG_H

#include "passageiros.h"
#include <glib.h>

/**
 * @file passageirosCatalog.h
 * @brief Interface do catálogo de passageiros.
 *
 * Este módulo gere uma coleção de passageiros, permitindo inserir e procurar
 * passageiros por número de documento, iterar sobre todos os registos e
 * atualizar gastos por semana com base em reservas/voos.
 */

/**
 * @brief Estrutura opaca do catálogo de passageiros.
 */
typedef struct passageirosCatalog PassageirosCatalog;

/**
 * @brief Declaração adiantada do catálogo de voos.
 */
typedef struct voosCatalog VoosCatalog;

/**
 * @brief Declaração adiantada do catálogo de reservas.
 */
typedef struct reservasCatalog ReservasCatalog;

/**
 * @brief Cria um novo catálogo de passageiros.
 * @return Apontador para o catálogo criado.
 */
PassageirosCatalog* criarPassageirosCatalog();

/**
 * @brief Destrói um catálogo de passageiros e liberta os seus recursos.
 * @param catalog Catálogo a destruir.
 * @return void.
 */
void destruirPassageirosCatalog(PassageirosCatalog* catalog);

/**
 * @brief Insere um passageiro no catálogo.
 * @param catalog Catálogo destino.
 * @param passageiro Passageiro a inserir.
 * @return void.
 */
void inserirPassageiro(PassageirosCatalog* catalog, Passageiros* passageiro);

/**
 * @brief Procura um passageiro pelo número de documento.
 * @param catalog Catálogo.
 * @param document_number Número de documento.
 * @return Apontador para `Passageiros` se existir, caso contrário `NULL`.
 */
Passageiros* buscarPassageiro(PassageirosCatalog* catalog, int document_number);

/**
 * @brief Atualiza os gastos por semana em cada passageiro com base nas reservas.
 *
 * @param passageirosCatalog Catálogo de passageiros a atualizar.
 * @param reservasCatalog Catálogo de reservas (fonte de informação).
 * @param voosCatalog Catálogo de voos (para cruzamento de dados, quando necessário).
 * @return void.
 */
void atualizarGastoPassageiros(PassageirosCatalog* passageirosCatalog, ReservasCatalog* reservasCatalog, VoosCatalog* voosCatalog);

/**
 * @brief Itera sobre todos os passageiros do catálogo.
 *
 * Wrapper para iteração interna do catálogo (ex.: `g_hash_table_foreach`).
 *
 * @param catalog Catálogo.
 * @param func Função callback chamada por elemento.
 * @param user_data Dados extra passados ao callback.
 * @return void.
 */
void passageiros_foreach(PassageirosCatalog* catalog, GHFunc func, gpointer user_data);

#endif