#ifndef AERONAVES_H
#define AERONAVES_H
#include <stdbool.h>

/**
 * @file aeronaves.h
 * @brief Interface do módulo de aeronaves.
 *
 * Define o tipo abstrato `Aeronaves` e as operações para criar, libertar,
 * consultar e alterar dados de uma aeronave.
 */

typedef struct aeronaves Aeronaves;

/**
 * @brief Cria e insere uma aeronave no catálogo.
 *
 * Constrói uma aeronave a partir dos campos lidos do CSV e insere-a no catálogo
 * fornecido.
 *
 * @param aircrafts_data Array de strings com os campos do registo (linha CSV).
 * @param aeronave_catalog Catálogo onde a aeronave será inserida.
 *
 * @return 0 em caso de sucesso; diferente de 0 se o registo for inválido ou se
 * ocorrer algum erro de criação/inserção.
 */
int criarAeronave(char **aircrafts_data, void *aeronave_catalog);

/**
 * @brief Liberta uma instância de aeronave.
 *
 * Liberta os campos alocados dinamicamente e a própria estrutura.
 *
 * @param aeronave_ptr Apontador genérico para a aeronave a libertar.
 *
 * @return void.
 */
void free_aeronave(void* aeronave_ptr);

/**
 * @brief Obtém o identificador da aeronave.
 *
 * @param aeronave Aeronave da qual se pretende o identificador.
 *
 * @return String com o identificador. Dependendo da implementação, pode ser uma
 * cópia alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getIdentifier(Aeronaves* aeronave);

/**
 * @brief Obtém o fabricante da aeronave.
 *
 * @param aeronave Aeronave da qual se pretende o fabricante.
 *
 * @return String com o fabricante. Dependendo da implementação, pode ser uma
 * cópia alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getManufacturer(Aeronaves* aeronave);

/**
 * @brief Obtém o modelo da aeronave.
 *
 * @param aeronave Aeronave da qual se pretende o modelo.
 *
 * @return String com o modelo. Dependendo da implementação, pode ser uma cópia
 * alocada (nesse caso o chamador deve libertar com `free()`).
 */
char* getModel(Aeronaves* aeronave);

/**
 * @brief Obtém o ano de fabrico.
 *
 * @param aeronave Aeronave da qual se pretende o ano.
 *
 * @return Ano de fabrico.
 */
int getYear(Aeronaves* aeronave);

/**
 * @brief Obtém a capacidade da aeronave.
 *
 * @param aeronave Aeronave da qual se pretende a capacidade.
 *
 * @return Capacidade (ex.: número de lugares).
 */
int getCapacity(Aeronaves* aeronave);

/**
 * @brief Obtém o alcance (range) da aeronave.
 *
 * @param aeronave Aeronave da qual se pretende o alcance.
 *
 * @return Alcance (unidade conforme o dataset).
 */
int getRange(Aeronaves* aeronave);

/**
 * @brief Obtém o número de voos associados à aeronave.
 *
 * @param aeronave Aeronave da qual se pretende o contador de voos.
 *
 * @return Número de voos contabilizados.
 */
int getNVoos(Aeronaves* aeronave);

/**
 * @brief Define o identificador da aeronave.
 *
 * @param aeronave Aeronave a modificar.
 * @param identifier Novo identificador.
 *
 * @return void.
 */
void setIdentifier(Aeronaves* aeronave,char* identifier);

/**
 * @brief Define o fabricante da aeronave.
 *
 * @param aeronave Aeronave a modificar.
 * @param manufacturer Novo fabricante.
 *
 * @return void.
 */
void setManufacturer(Aeronaves* aeronave, char* manufacturer);

/**
 * @brief Define o modelo da aeronave.
 *
 * @param aeronave Aeronave a modificar.
 * @param model Novo modelo.
 *
 * @return void.
 */
void setModel(Aeronaves* aeronave, char* model);

/**
 * @brief Define o ano de fabrico.
 *
 * @param aeronave Aeronave a modificar.
 * @param year Novo ano.
 *
 * @return void.
 */
void setYear(Aeronaves* aeronave, int year);

/**
 * @brief Define a capacidade da aeronave.
 *
 * @param aeronave Aeronave a modificar.
 * @param capacity Nova capacidade.
 *
 * @return void.
 */
void setCapacity(Aeronaves* aeronave, int capacity);

/**
 * @brief Define o alcance (range) da aeronave.
 *
 * @param aeronave Aeronave a modificar.
 * @param range Novo alcance.
 *
 * @return void.
 */
void setRange(Aeronaves* aeronave, int range);

/**
 * @brief Define o número de voos associados à aeronave.
 *
 * @param aeronave Aeronave a modificar.
 * @param n_voos Novo valor do contador.
 *
 * @return void.
 */
void setNVoos(Aeronaves* aeronave, int n_voos);

#endif

