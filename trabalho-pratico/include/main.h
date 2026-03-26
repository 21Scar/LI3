#ifndef MAIN_H
#define MAIN_H

#include "voosCatalog.h"
#include "reservasCatalog.h"
#include "passageirosCatalog.h"
#include "aeronavesCatalog.h"
#include "aeroportosCatalog.h"

/**
 * @file main.h
 * @brief Interface do módulo principal do programa.
 *
 * Define o tipo abstrato `Program` e as operações de alto nível para:
 * - inicializar o sistema e carregar os catálogos a partir do dataset;
 * - executar as queries de um ficheiro de comandos;
 * - libertar toda a memória associada.
 */

#define OUTPUT_DIR_NAME "resultados"
/**
 * @brief Nome da diretoria onde são guardados os outputs.
 */

/**
 * @brief Formato do nome do ficheiro de output por comando.
 *
 * Tipicamente usado com `snprintf` para produzir `commandN_output.txt`.
 */
#define OUTPUT_FILE_NAME "command%d_output.txt"

/**
 * @brief Tamanho inicial do buffer usado na leitura de linhas.
 */
#define BUFFER_SIZE 200

/**
 * @brief Estrutura opaca que agrega os catálogos e estado global do programa.
 */
typedef struct program Program;

/**
 * @brief Inicializa o programa e carrega os dados do dataset.
 *
 * @param arguments Argumentos de linha de comando do programa principal.
 * Espera-se que `arguments[1]` contenha o caminho para a pasta do dataset.
 *
 * @return Apontador para a estrutura `Program` inicializada, ou `NULL` em caso
 * de erro.
 */
Program* initialize_program(char **arguments);

/**
 * @brief Executa as queries descritas no ficheiro de comandos.
 *
 * @param program Instância do programa com os catálogos carregados.
 * @param arguments Argumentos de linha de comando do programa principal.
 * Espera-se que `arguments[2]` contenha o caminho para o ficheiro de comandos.
 *
 * @return void.
 */
void run_program(Program *program, char **arguments);

/**
 * @brief Liberta todos os recursos do programa.
 *
 * @param program Instância do programa a libertar.
 *
 * @return void.
 */
void free_program(Program *program);

/**
 * @brief Obtém o catálogo de passageiros do programa.
 * @param program Instância do programa.
 * @return Apontador para `PassageirosCatalog`.
 */
PassageirosCatalog* getPassageirosCatalog(Program* program);

/**
 * @brief Obtém o catálogo de aeronaves do programa.
 * @param program Instância do programa.
 * @return Apontador para `AeronavesCatalog`.
 */
AeronavesCatalog* getAeronavesCatalog(Program* program);

/**
 * @brief Obtém o catálogo de aeroportos do programa.
 * @param program Instância do programa.
 * @return Apontador para `AeroportosCatalog`.
 */
AeroportosCatalog* getAeroportosCatalog(Program* program);

/**
 * @brief Obtém o catálogo de voos do programa.
 * @param program Instância do programa.
 * @return Apontador para `VoosCatalog`.
 */
VoosCatalog* getVoosCatalog(Program* program);

/**
 * @brief Obtém o catálogo de reservas do programa.
 * @param program Instância do programa.
 * @return Apontador para `ReservasCatalog`.
 */
ReservasCatalog* getReservasCatalog(Program* program);

#endif