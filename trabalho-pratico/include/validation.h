#ifndef VALIDATION_H
#define VALIDATION_H

/**
 * @file validation.h
 * @brief Validação de linhas/fields dos CSVs e utilitários de parsing.
 *
 * Este módulo expõe validadores por entidade (voos, aeroportos, aeronaves,
 * passageiros, reservas) e validadores auxiliares (datas, listas, emails,
 * códigos de aeroporto, etc.).
 *
 * Algumas validações dependem de catálogos já carregados (ex.: validar se um
 * `flight_id` existe ao validar reservas). Para isso, o módulo permite registar
 * apontadores para os catálogos relevantes.
 */

#include <time.h>
#include "voosCatalog.h"
#include "aeronavesCatalog.h"
#include "passageirosCatalog.h"
#include "aeroportosCatalog.h"

/**
 * @brief Valida os campos de um voo.
 * @param fields Campos da linha.
 * @param field_count Número de campos.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_voo(char **fields, int field_count);

/**
 * @brief Valida os campos de um aeroporto.
 * @param fields Campos da linha.
 * @param field_count Número de campos.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_airports(char **fields, int field_count);

/**
 * @brief Valida os campos de uma aeronave.
 * @param fields Campos da linha.
 * @param field_count Número de campos.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_aircrafts(char **fields, int field_count);

/**
 * @brief Valida os campos de um passageiro.
 * @param fields Campos da linha.
 * @param field_count Número de campos.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_passengers(char **fields, int field_count);

/**
 * @brief Valida os campos de uma reserva.
 * @param fields Campos da linha.
 * @param field_count Número de campos.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_reservation(char **fields, int field_count);

/**
 * @brief Valida o formato de uma lista (ex.: "[a,b]").
 * @param list String da lista.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_list_format(const char *list);

/**
 * @brief Valida um ID de voo.
 * @param flight_id ID de voo.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_flight_id(const char *flight_id);

/**
 * @brief Valida uma lista de IDs de voo.
 * @param list String com lista de IDs.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_flight_id_list(const char *list);

/**
 * @brief Valida uma data/hora no formato esperado pelo dataset.
 * @param datetime String de data/hora.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_datetime(const char *datetime);

/**
 * @brief Converte uma string de data/hora para `time_t`.
 * @param datetime String de data/hora.
 * @return Valor `time_t` correspondente (convenção definida na implementação).
 */
time_t datetime_to_time(const char *datetime);

/**
 * @brief Valida o tipo de aeroporto.
 * @param type String do tipo.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_airport_type(const char *type);

/**
 * @brief Valida latitude e longitude em conjunto.
 * @param lat_str Latitude.
 * @param lon_str Longitude.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_coordinates_pair(const char *lat_str, const char *lon_str);

/**
 * @brief Valida códigos de aeroporto IATA (3 letras) e ICAO (4 letras).
 * @param iata Código IATA.
 * @param icao Código ICAO.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_airport_codes(const char *iata, const char *icao);

/**
 * @brief Valida o ano de uma aeronave.
 * @param year_str Ano em string.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_aircraft_year(const char *year_str);

/**
 * @brief Valida um número de documento.
 * @param doc_num Número de documento.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_document_number(const char *doc_num);

/**
 * @brief Valida um endereço de email.
 * @param email Email.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_email(const char *email);

/**
 * @brief Valida o género.
 * @param gender Género.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_gender(const char *gender);

/**
 * @brief Valida um ID de reserva.
 * @param res_id ID de reserva.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_reservation_id(const char *res_id);

/**
 * @brief Valida uma data de nascimento.
 * @param date Data.
 * @return 1 se válido, 0 caso contrário.
 */
int validate_date_dob(const char *date);

/**
 * @brief Regista o catálogo de voos para validações que dependem dele.
 *
 * Usado, por exemplo, para validar se IDs de voo existem ao validar reservas.
 *
 * @param catalog Catálogo de voos.
 * @return void.
 */
void setVoosCatalogForValidation(VoosCatalog* catalog);

/**
 * @brief Regista o catálogo de aeronaves para validações que dependem dele.
 * @param catalog Catálogo de aeronaves.
 * @return void.
 */
void setAeronavesCatalogForValidation(AeronavesCatalog* catalog);

/**
 * @brief Regista o catálogo de passageiros para validações que dependem dele.
 *
 * Usado, por exemplo, para validar se o passageiro existe ao validar reservas.
 *
 * @param catalog Catálogo de passageiros.
 * @return void.
 */
void setPassageirosCatalogForValidation(PassageirosCatalog* catalog);

/**
 * @brief Regista o catálogo de aeroportos para validações que dependem dele.
 *
 * Usado, por exemplo, para validar origem/destino ao validar voos.
 *
 * @param catalog Catálogo de aeroportos.
 * @return void.
 */
void setAeroportosCatalogForValidation(AeroportosCatalog* catalog);

#endif