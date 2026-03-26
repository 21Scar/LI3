#ifndef QUERYS_H
#define QUERYS_H

#include "aeroportosCatalog.h"
#include "aeronavesCatalog.h"
#include "main.h"
#include <glib.h>

/**
 * @file querys.h
 * @brief Interface das queries do trabalho prático.
 *
 * Este módulo concentra as funções responsáveis por executar as queries
 * pedidas no enunciado, produzindo outputs por comando.
 *
 * O output de cada comando é tipicamente gravado em ficheiros
 * `resultados/commandN_output.txt`.
 */

/**
 * @brief Estrutura auxiliar (opaca) para agregação de voos por aeronave.
 */
typedef struct aeronaveVoos AeronaveVoos; 

/**
 * @brief Estrutura auxiliar (opaca) para agregação de partidas por aeroporto.
 */
typedef struct dadosAeroportoPartidas DadosAeroportoPartidas;

/**
 * @brief Contexto (opaco) usado para recolha de gastos.
 */
typedef struct recolher_gastos_ctx RecolherGastosCtx;

/**
 * @brief Estrutura auxiliar (opaca) com gasto de passageiro por semana.
 */
typedef struct PassageiroGastoSemana PassageiroGastoSemana;

/**
 * @brief Executa uma query por número.
 *
 * Função de dispatch: interpreta `query_number` e encaminha para a
 * implementação correspondente.
 *
 * @param program Instância do programa com os catálogos carregados.
 * @param query_number Número da query (ex.: 1, 2, 3, ...).
 * @param arguments Argumentos (string) da query, tal como no ficheiro de comandos.
 * @param command_number Índice do comando (usado para nome do ficheiro de output).
 * @param s_existe Flag que indica se o formato com separador `;` é usado (modo "S").
 *
 * @return void.
 */
void run_query(Program* program, int query_number, char* arguments, int command_number, int s_existe);

/**
 * @brief Query 1: lista o resumo de um aeroporto (ou equivalente).
 *
 * @param program Instância do programa.
 * @param arguments Argumentos da query.
 * @param command_number Índice do comando.
 * @param s_existe Flag do modo "S".
 * @return void.
 */
void listarResumoAeroporto(Program* program, char* arguments, int command_number, int s_existe);

/**
 * @brief Query 2: lista o top de aeronaves.
 *
 * @param program Instância do programa.
 * @param arguments Argumentos da query.
 * @param command_number Índice do comando.
 * @param s_existe Flag do modo "S".
 * @return void.
 */
void listarTopAeronaves(Program* program, char* arguments, int command_number, int s_existe);

/**
 * @brief Callback para recolher pares (key,value) para uma estrutura linear.
 *
 * Tipicamente usado com `g_hash_table_foreach`.
 *
 * @param key Chave.
 * @param value Valor.
 * @param user_data Estrutura destino (contexto do chamador).
 * @return void.
 */
void adicionarAoArray(gpointer key, gpointer value, gpointer user_data);

/**
 * @brief Função de comparação usada na ordenação de aeronaves/voos.
 *
 * @param a Elemento A.
 * @param b Elemento B.
 * @return Inteiro no formato esperado por GLib (`<0`, `0`, `>0`).
 */
gint compararVoosNaves(gconstpointer a, gconstpointer b);

/**
 * @brief Guarda o output de um comando no ficheiro respetivo.
 *
 * @param command_number Índice do comando.
 * @param output Conteúdo a escrever.
 * @return void.
 */
void save_output(int command_number, const char* output);

/**
 * @brief Query 3: lista o aeroporto com mais partidas.
 *
 * @param program Instância do programa.
 * @param arguments Argumentos da query.
 * @param command_number Índice do comando.
 * @param s_existe Flag do modo "S".
 * @return void.
 */
void listarAeroportoMaisPartidas(Program* program, char* arguments, int command_number, int s_existe);

/**
 * @brief Query 4: lista passageiro(s) top por semanas.
 *
 * @param program Instância do programa.
 * @param arguments Argumentos da query.
 * @param command_number Índice do comando.
 * @param s_existe Flag do modo "S".
 * @return void.
 */
void listarPassageiroTopSemanas(Program* program, char* arguments, int command_number, int s_existe);

/**
 * @brief Query 5: lista companhias por atraso médio.
 *
 * @param program Instância do programa.
 * @param arguments Argumentos da query.
 * @param command_number Índice do comando.
 * @param s_existe Flag do modo "S".
 * @return void.
 */
void listarCompanhiasAtrasoMedio(Program* program, char* arguments, int command_number, int s_existe); 

/**
 * @brief Query 6: destino mais comum por nacionalidade.
 *
 * @param program Instância do programa.
 * @param arguments Argumentos da query.
 * @param command_number Índice do comando.
 * @param s_existe Flag do modo "S".
 * @return void.
 */
void listarDestinoMaisComumPorNacionalidade(Program* program, char* arguments, int command_number, int s_existe);

/**
 * @brief Liberta estruturas em cache usadas pela Query 6.
 *
 * Deve ser chamada no final do programa (antes do exit) para evitar leaks.
 *
 * @return void.
 */
void q6_cleanup(void);

#endif