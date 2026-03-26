#ifndef AEROPORTOS_H
#define AEROPORTOS_H
#include <stdbool.h>
#include <glib.h>

/**
 * @file aeroportos.h
 * @brief Interface do módulo de aeroportos.
 *
 * Define o tipo abstrato `Aeroportos` e as operações para criar, libertar,
 * consultar e alterar dados de um aeroporto.
 *
 * O módulo inclui ainda estruturas auxiliares (ex.: tabela por data) para
 * contagem de partidas e contadores de passageiros (chegadas/partidas).
 */

typedef struct aeroportos Aeroportos;

/**
 * @brief Cria e insere um aeroporto no catálogo.
 *
 * Constrói um aeroporto a partir dos campos lidos do CSV (`airports.csv`) e
 * insere-o no catálogo fornecido.
 *
 * @param airports_data Array de strings com os campos do registo (linha CSV).
 * @param aeroporto_catalog Catálogo onde o aeroporto será inserido.
 *
 * @return 0 em caso de sucesso; diferente de 0 se o registo for inválido ou se
 * ocorrer algum erro de criação/inserção.
 */
int criarAeroporto(char **airports_data, void *aeroporto_catalog);

/**
 * @brief Liberta uma instância de aeroporto.
 *
 * Liberta os campos alocados dinamicamente (incluindo estruturas auxiliares)
 * e a própria estrutura.
 *
 * @param aeroporto_ptr Apontador genérico para o aeroporto a libertar.
 *
 * @return void.
 */
void free_aeroporto(void* aeroporto_ptr);

/**
 * @brief Obtém o código IATA do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende o código.
 *
 * @return String com o código. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getCode(Aeroportos* aeroporto);

/**
 * @brief Obtém o nome do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende o nome.
 *
 * @return String com o nome. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getName(Aeroportos* aeroporto);

/**
 * @brief Obtém a cidade do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende a cidade.
 *
 * @return String com a cidade. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getCity(Aeroportos* aeroporto);

/**
 * @brief Obtém o país do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende o país.
 *
 * @return String com o país. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getCountry(Aeroportos* aeroporto);

/**
 * @brief Obtém a latitude do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende a latitude.
 *
 * @return Latitude.
 */
double getLatitude(Aeroportos* aeroporto);

/**
 * @brief Obtém a longitude do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende a longitude.
 *
 * @return Longitude.
 */
double getLongitude(Aeroportos* aeroporto);

/**
 * @brief Obtém o código ICAO do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende o ICAO.
 *
 * @return String com o ICAO. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getIcao(Aeroportos* aeroporto);

/**
 * @brief Obtém o tipo do aeroporto.
 *
 * @param aeroporto Aeroporto do qual se pretende o tipo.
 *
 * @return String com o tipo. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getType(Aeroportos* aeroporto);

/**
 * @brief Define o código IATA.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param code Novo código.
 *
 * @return void.
 */
void setCode(Aeroportos* aeroporto, char* code);

/**
 * @brief Define o nome do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param name Novo nome.
 *
 * @return void.
 */
void setName(Aeroportos* aeroporto, char* name);

/**
 * @brief Define a cidade do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param city Nova cidade.
 *
 * @return void.
 */
void setCity(Aeroportos* aeroporto, char* city);

/**
 * @brief Define o país do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param country Novo país.
 *
 * @return void.
 */
void setCountry(Aeroportos* aeroporto, char* country);

/**
 * @brief Define a latitude do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param latitude Nova latitude.
 *
 * @return void.
 */
void setLatitude(Aeroportos* aeroporto, double latitude);

/**
 * @brief Define a longitude do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param longitude Nova longitude.
 *
 * @return void.
 */
void setLongitude(Aeroportos* aeroporto, double longitude);

/**
 * @brief Define o código ICAO do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param icao Novo ICAO.
 *
 * @return void.
 */
void setIcao(Aeroportos* aeroporto, char* icao);

/**
 * @brief Define o tipo do aeroporto.
 *
 * @param aeroporto Aeroporto a modificar.
 * @param type Novo tipo.
 *
 * @return void.
 */
void setType(Aeroportos* aeroporto, char* type);

/* Contadores de partidas por data (YYYY-MM-DD) */

/**
 * @brief Incrementa o contador de partidas numa data.
 *
 * Regista uma partida associada a uma determinada data (formato `YYYY-MM-DD`).
 *
 * @param aeroporto Aeroporto a atualizar.
 * @param date Data no formato `YYYY-MM-DD`.
 *
 * @return void.
 */
void addDepartureOnDate(Aeroportos* aeroporto, const char* date);

/**
 * @brief Obtém o total de partidas entre duas datas.
 *
 * Soma as partidas registadas no intervalo inclusivo `[start_date, end_date]`.
 *
 * @param aeroporto Aeroporto a consultar.
 * @param start_date Data inicial (`YYYY-MM-DD`).
 * @param end_date Data final (`YYYY-MM-DD`).
 *
 * @return Número total de partidas no intervalo.
 */
int getDeparturesBetweenDates(Aeroportos* aeroporto, const char* start_date, const char* end_date);

/* Opcional: acesso para iteração/controlos internos */

/**
 * @brief Acede à tabela interna de partidas por data.
 *
 * Útil para iteração/diagnóstico. O chamador não deve libertar a hash table
 * devolvida.
 *
 * @param aeroporto Aeroporto do qual se pretende a estrutura interna.
 *
 * @return Apontador para `GHashTable` com contagens por data.
 */
GHashTable* getDeparturesByDate(Aeroportos* aeroporto);

/* Contadores de passageiros (chegadas e partidas) */

/**
 * @brief Incrementa o contador de passageiros a chegar.
 *
 * @param aeroporto Aeroporto a atualizar.
 * @param count Quantidade a adicionar.
 *
 * @return void.
 */
void addArrivingPassengers(Aeroportos* aeroporto, int count);

/**
 * @brief Incrementa o contador de passageiros a partir.
 *
 * @param aeroporto Aeroporto a atualizar.
 * @param count Quantidade a adicionar.
 *
 * @return void.
 */
void addDepartingPassengers(Aeroportos* aeroporto, int count);

/**
 * @brief Obtém o total de passageiros a chegar.
 *
 * @param aeroporto Aeroporto a consultar.
 *
 * @return Total de passageiros a chegar.
 */
int getArrivingPassengers(Aeroportos* aeroporto);

/**
 * @brief Obtém o total de passageiros a partir.
 *
 * @param aeroporto Aeroporto a consultar.
 *
 * @return Total de passageiros a partir.
 */
int getDepartingPassengers(Aeroportos* aeroporto);

#endif