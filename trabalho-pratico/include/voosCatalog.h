#ifndef VOOSCATALOG_H
#define VOOSCATALOG_H

#include "voos.h"
#include "aeronavesCatalog.h"
#include "aeroportosCatalog.h"
#include "reservasCatalog.h"
#include <glib.h>

/**
 * @file voosCatalog.h
 * @brief Interface do catálogo de voos.
 *
 * Este módulo gere uma coleção de voos e fornece operações de inserção/consulta,
 * bem como funções auxiliares usadas por queries (ex.: agregações por aeronave,
 * partidas por aeroporto entre datas, e estatísticas por companhia).
 */

/**
 * @brief Estrutura opaca do catálogo de voos.
 */
typedef struct voosCatalog VoosCatalog;

/**
 * @brief Estrutura auxiliar (opaca) para estatísticas por companhia.
 */
typedef struct airlineStats AirlineStats;

/**
 * @brief Estrutura auxiliar (opaca) para atraso médio por companhia.
 */
typedef struct airlineAtraso AirlineAtraso;

/**
 * @brief Cria um novo catálogo de voos.
 * @return Apontador para o catálogo criado.
 */
VoosCatalog* criarVoosCatalog();

/**
 * @brief Destrói um catálogo de voos e liberta os seus recursos.
 * @param catalog Catálogo a destruir.
 * @return void.
 */
void destruirVoosCatalog(VoosCatalog* catalog);

/**
 * @brief Insere um voo no catálogo.
 * @param catalog Catálogo destino.
 * @param voo Voo a inserir.
 * @return void.
 */
void inserirVoo(VoosCatalog* catalog, Voos* voo);

/**
 * @brief Regista o catálogo de aeronaves (dependência) no catálogo de voos.
 * @param catalog Catálogo de voos.
 * @param aeronavesCatalog Catálogo de aeronaves.
 * @return void.
 */
void setAeronavesCatalog(VoosCatalog* catalog, AeronavesCatalog* aeronavesCatalog);

/**
 * @brief Regista o catálogo de aeroportos (dependência) no catálogo de voos.
 * @param catalog Catálogo de voos.
 * @param aeroportosCatalog Catálogo de aeroportos.
 * @return void.
 */
void setAeroportosCatalog(VoosCatalog* catalog, AeroportosCatalog* aeroportosCatalog);

/**
 * @brief Regista o catálogo de reservas (dependência) no catálogo de voos.
 * @param catalog Catálogo de voos.
 * @param reservasCatalog Catálogo de reservas.
 * @return void.
 */
void setReservasCatalog(VoosCatalog* catalog, ReservasCatalog* reservasCatalog);

/**
 * @brief Procura um voo pelo seu ID.
 * @param catalog Catálogo.
 * @param id ID do voo.
 * @return Apontador para `Voos` se existir, caso contrário `NULL`.
 */
Voos* buscarVoo(VoosCatalog* catalog, const char* id);

/**
 * @brief Agrupa voos por aeronave para um determinado fabricante.
 *
 * @param voosCatalog Catálogo de voos.
 * @param fabricante Fabricante a filtrar.
 * @return `GHashTable*` com a agregação produzida (convenção definida na implementação).
 */
GHashTable* voosPorAeronave(VoosCatalog* voosCatalog, const char* fabricante);

/**
 * @brief Callback auxiliar para agregação de voos por aeronave.
 *
 * Tipicamente usado com `g_hash_table_foreach`.
 *
 * @param key Chave.
 * @param value Valor.
 * @param user_data Contexto do chamador.
 * @return void.
 */
void voosNavesCallback(gpointer key, gpointer value, gpointer user_data);

/**
 * @brief Calcula partidas por aeroporto num intervalo de datas.
 *
 * @param voosCatalog Catálogo de voos.
 * @param start_date Data inicial.
 * @param end_date Data final.
 * @return `GHashTable*` com contagens por aeroporto (convenção definida na implementação).
 */
GHashTable* partidasPorAeroportoEntreDatas(VoosCatalog* voosCatalog, const char* start_date, const char* end_date);

/**
 * @brief Função de comparação usada na ordenação de `AirlineAtraso`.
 * @param a Elemento A.
 * @param b Elemento B.
 * @return Inteiro no formato esperado por GLib (`<0`, `0`, `>0`).
 */
gint compararAirlineAtraso(gconstpointer a, gconstpointer b);

/**
 * @brief Constrói/atualiza o array de atrasos médios por companhia.
 * @param catalog Catálogo de voos.
 * @return void.
 */
void construirAirlineAtrasoArray(VoosCatalog* catalog);

/**
 * @brief Obtém o array com atrasos médios por companhia.
 * @param catalog Catálogo de voos.
 * @return `GPtrArray*` com elementos `AirlineAtraso*`.
 */
GPtrArray* getAirlineAtrasoArray(VoosCatalog* catalog);

/**
 * @brief Obtém a companhia (airline) de um item `AirlineAtraso`.
 * @param item Item.
 * @return String com a companhia.
 */
char* get_AirlineAtraso_airline(AirlineAtraso* item);

/**
 * @brief Obtém o atraso médio de um item `AirlineAtraso`.
 * @param item Item.
 * @return Atraso médio.
 */
double get_AirlineAtraso_atrasoMedio(AirlineAtraso* item);

/**
 * @brief Obtém o número de voos considerados num item `AirlineAtraso`.
 * @param item Item.
 * @return Contador de voos.
 */
int get_AirlineAtraso_contadorVoos(AirlineAtraso* item);

#endif