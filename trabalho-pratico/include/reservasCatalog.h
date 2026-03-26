#ifndef RESERVASCATALOG_H
#define RESERVASCATALOG_H

#include "reservas.h"
#include <glib.h>

/**
 * @file reservasCatalog.h
 * @brief Interface do catálogo de reservas.
 *
 * Este módulo gere uma coleção de reservas, permitindo inserir e iterar sobre
 * as mesmas, bem como executar operações de agregação/atualização usadas por
 * algumas queries (ex.: contagem de passageiros por aeroporto).
 */

/**
 * @brief Estrutura opaca do catálogo de reservas.
 */
typedef struct reservasCatalog ReservasCatalog;

/**
 * @brief Declaração adiantada do catálogo de voos.
 */
typedef struct voosCatalog VoosCatalog;

/**
 * @brief Declaração adiantada do catálogo de aeroportos.
 */
typedef struct aeroportosCatalog AeroportosCatalog;

/**
 * @brief Cria um novo catálogo de reservas.
 * @return Apontador para o catálogo criado.
 */
ReservasCatalog* criarReservasCatalog();

/**
 * @brief Destrói um catálogo de reservas e liberta os seus recursos.
 * @param catalog Catálogo a destruir.
 * @return void.
 */
void destruirReservasCatalog(ReservasCatalog* catalog);

/**
 * @brief Insere uma reserva no catálogo.
 * @param catalog Catálogo destino.
 * @param reserva Reserva a inserir.
 * @return void.
 */
void inserirReserva(ReservasCatalog* catalog, Reservas* reserva);
//Reservas* buscarReserva(ReservasCatalog* catalog, const char* reservation_id);

/**
 * @brief Atualiza contadores de passageiros nos aeroportos com base nas reservas.
 *
 * @param reservasCatalog Catálogo de reservas (fonte de informação).
 * @param voosCatalog Catálogo de voos (para cruzamento de dados).
 * @param aeroportosCatalog Catálogo de aeroportos a atualizar.
 * @return void.
 */
void atualizarContagemPassageiros(ReservasCatalog* reservasCatalog, VoosCatalog* voosCatalog, AeroportosCatalog* aeroportosCatalog);

/**
 * @brief Itera sobre todas as reservas do catálogo.
 *
 * Wrapper para iteração interna do catálogo (ex.: `g_hash_table_foreach`).
 *
 * @param catalog Catálogo.
 * @param func Função callback chamada por elemento.
 * @param user_data Dados extra passados ao callback.
 * @return void.
 */
void reservas_foreach(ReservasCatalog* catalog, GHFunc func, gpointer user_data);

#endif