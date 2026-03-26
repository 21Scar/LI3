#ifndef PASSAGEIROS_H
#define PASSAGEIROS_H

/**
 * @file passageiros.h
 * @brief Estrutura e operações associadas a passageiros.
 *
 * Define o tipo abstrato `Passageiros` (passageiro individual) e disponibiliza
 * funções de construção/destruição, getters/setters dos seus campos e
 * funcionalidades auxiliares usadas por algumas queries (ex.: gastos por semana).
 */

#include <stdbool.h>
#include <glib.h>

/**
 * @brief Estrutura opaca que representa um passageiro.
 */
typedef struct passageiros Passageiros;

/**
 * @brief Cria um passageiro a partir de campos CSV e adiciona ao catálogo.
 *
 * Esta função é tipicamente usada como callback do parser genérico.
 *
 * @param passengers_data Array de strings com os campos do CSV.
 * @param passageiro_catalog Catálogo/estrutura onde o passageiro será armazenado.
 *
 * @return 0 em caso de sucesso, ou valor não-zero em caso de erro/entrada inválida.
 */
int criarPassageiros(char **passengers_data, void *passageiro_catalog);

/**
 * @brief Destrói (liberta) um passageiro.
 *
 * @param passageiro Passageiro a destruir.
 *
 * @return void.
 */
void destruirPassageiros(Passageiros* passageiro);

/**
 * @brief Obtém o número de documento do passageiro.
 * @param passageiro Passageiro.
 * @return Número de documento.
 */
int getPassengerDocumentNumber(const Passageiros* passageiro);

/**
 * @brief Obtém o primeiro nome do passageiro.
 * @param passageiro Passageiro.
 * @return String com o primeiro nome.
 */
char* getFirstName(const Passageiros* passageiro);

/**
 * @brief Obtém o último nome do passageiro.
 * @param passageiro Passageiro.
 * @return String com o último nome.
 */
char* getLastName(const Passageiros* passageiro);

/**
 * @brief Obtém a data de nascimento do passageiro.
 * @param passageiro Passageiro.
 * @return String com a data de nascimento.
 */
char* getDob(const Passageiros* passageiro);

/**
 * @brief Obtém a nacionalidade do passageiro.
 * @param passageiro Passageiro.
 * @return String com a nacionalidade.
 */
char* getNationality(const Passageiros* passageiro);

/**
 * @brief Obtém o género do passageiro.
 * @param passageiro Passageiro.
 * @return String com o género.
 */
char* getGender(const Passageiros* passageiro);

/**
 * @brief Obtém o email do passageiro.
 * @param passageiro Passageiro.
 * @return String com o email.
 */
char* getEmail(const Passageiros* passageiro);

/**
 * @brief Obtém o telefone do passageiro.
 * @param passageiro Passageiro.
 * @return String com o telefone.
 */
char* getPhone(const Passageiros* passageiro);

/**
 * @brief Obtém a morada do passageiro.
 * @param passageiro Passageiro.
 * @return String com a morada.
 */
char* getAddress(const Passageiros* passageiro);

/**
 * @brief Obtém o URL/caminho da foto do passageiro.
 * @param passageiro Passageiro.
 * @return String com a foto.
 */
char* getPhoto(const Passageiros* passageiro);

/**
 * @brief Define o número de documento do passageiro.
 * @param passageiro Passageiro.
 * @param document_number Número de documento.
 * @return void.
 */
void setPassengerDocumentNumber(Passageiros* passageiro, int document_number);

/**
 * @brief Define o primeiro nome do passageiro.
 * @param passageiro Passageiro.
 * @param first_name Primeiro nome.
 * @return void.
 */
void setFirstName(Passageiros* passageiro, char* first_name);

/**
 * @brief Define o último nome do passageiro.
 * @param passageiro Passageiro.
 * @param last_name Último nome.
 * @return void.
 */
void setLastName(Passageiros* passageiro,char* last_name);

/**
 * @brief Define a data de nascimento do passageiro.
 * @param passageiro Passageiro.
 * @param dob Data de nascimento.
 * @return void.
 */
void setDob(Passageiros* passageiro,  char* dob);

/**
 * @brief Define a nacionalidade do passageiro.
 * @param passageiro Passageiro.
 * @param nationality Nacionalidade.
 * @return void.
 */
void setNationality(Passageiros* passageiro,  char* nationality);

/**
 * @brief Define o género do passageiro.
 * @param passageiro Passageiro.
 * @param gender Género.
 * @return void.
 */
void setGender(Passageiros* passageiro,  char* gender);

/**
 * @brief Define o email do passageiro.
 * @param passageiro Passageiro.
 * @param email Email.
 * @return void.
 */
void setEmail(Passageiros* passageiro,  char* email);

/**
 * @brief Define o telefone do passageiro.
 * @param passageiro Passageiro.
 * @param phone Telefone.
 * @return void.
 */
void setPhone(Passageiros* passageiro,  char* phone);

/**
 * @brief Define a morada do passageiro.
 * @param passageiro Passageiro.
 * @param address Morada.
 * @return void.
 */
void setAddress(Passageiros* passageiro, char* address);

/**
 * @brief Define o URL/caminho da foto do passageiro.
 * @param passageiro Passageiro.
 * @param photo Foto.
 * @return void.
 */
void setPhoto(Passageiros* passageiro,  char* photo);

/**
 * @brief Adiciona gasto associado a uma semana.
 * @param passageiro Passageiro.
 * @param semana_id Identificador da semana.
 * @param gasto Valor a adicionar.
 * @return void.
 */
void adicionarGastoSemana(Passageiros* passageiro, int semana_id, double gasto);

/**
 * @brief Obtém o gasto total de uma semana.
 * @param passageiro Passageiro.
 * @param semana_id Identificador da semana.
 * @return Gasto total nessa semana.
 */
double obterGastoSemana(Passageiros* passageiro, int semana_id);

/**
 * @brief Lista as semanas onde existe gasto associado.
 *
 * @param passageiro Passageiro.
 * @return Lista (`GList`) de apontadores para `int`.
 */
GList* obterSemanasComGasto(Passageiros* passageiro);

/**
 * @brief Lista as semanas onde existe gasto associado (em array).
 *
 * Alternativa a `obterSemanasComGasto` que devolve um array de `int`, evitando
 * problemas de gestão de memória associados a listas.
 *
 * @param passageiro Passageiro.
 * @param count Output: número de semanas no array.
 * @return Array de `int` com as semanas.
 */
int* obterSemanasComGasto_array(Passageiros* passageiro, int* count);

#endif