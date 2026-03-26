#ifndef PARSERGENERICO_H
#define PARSERGENERICO_H

/**
 * @file parserGenerico.h
 * @brief Funções genéricas de parsing de CSV e logging de entradas inválidas.
 *
 * Este módulo fornece:
 * - um parser CSV genérico que itera por linhas, separa campos e delega o
 *   armazenamento/validação a um callback;
 * - utilitários para escrita de campos com aspas e para registar linhas
 *   inválidas em ficheiros de erros.
 */
/**
 * @brief Callback para processar e armazenar os campos de uma linha CSV.
 *
 * O callback recebe um array de strings (campos já separados) e um apontador
 * genérico para o catálogo/estrutura onde deve guardar os dados.
 *
 * @param fields Array de strings com os campos da linha.
 * @param data_catalog Estrutura/catálogo onde os dados devem ser armazenados.
 *
 * @return Valor inteiro que indica sucesso/insucesso (convenção definida pela
 * implementação do callback e pelo chamador).
 */
typedef int(StoreData)(char**, void*);

/**
 * @brief Faz parse de um CSV e armazena os dados via callback.
 *
 * Lê o ficheiro CSV linha a linha, separa os campos e chama `store_data` para
 * validar e inserir a entrada no catálogo.
 *
 * @param csv Ficheiro CSV já aberto para leitura.
 * @param store_data Callback que processa uma linha (campos) e guarda no catálogo.
 * @param data_catalog Estrutura/catálogo de destino.
 * @param csv_name Nome lógico do CSV (usado para logging/diagnóstico).
 *
 * @return 0 em caso de sucesso, ou um valor não-zero em caso de erro.
 */
int parse_csv_and_store_data(FILE* csv, StoreData* store_data, void* data_catalog, const char *csv_name);

/**
 * @brief Escreve um campo num ficheiro, escapando/quotando quando necessário.
 *
 * @param file Ficheiro de destino.
 * @param field Campo a escrever.
 *
 * @return void.
 */
void write_field_with_quotes(FILE *file, const char *field);

/**
 * @brief Regista uma entrada inválida num ficheiro de erros do respetivo CSV.
 *
 * @param fields Campos parseados da linha.
 * @param field_count Número de campos disponíveis em `fields`.
 * @param csv_name Nome lógico do CSV (ex.: "flights").
 * @param header Linha de cabeçalho a escrever no ficheiro de erros (se aplicável).
 * @param header_written Apontador para flag que indica se o cabeçalho já foi escrito.
 * @param raw_line Linha original (raw) para referência, quando disponível.
 *
 * @return void.
 */
void log_invalid_entry(char **fields, int field_count, const char *csv_name, const char *header, int *header_written, const char *raw_line);

/**
 * @brief Remove aspas exteriores de um campo, se existirem.
 *
 * @param field Campo a normalizar (string mutável).
 *
 * @return Apontador para a string resultante (pode ser o mesmo apontador).
 */
char* remove_quotes(char* field);

#endif
