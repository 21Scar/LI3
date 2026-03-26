#ifndef VOOS_H
#define VOOS_H

/**
 * @file voos.h
 * @brief Estrutura e operações associadas a voos.
 *
 * Define o tipo abstrato `Voos` (voo individual) e disponibiliza funções de
 * construção/destruição, getters/setters dos campos e getters auxiliares para
 * timestamps e performance.
 */

#include <stdbool.h>
#include <time.h>

/**
 * @brief Estrutura opaca que representa um voo.
 */
typedef struct voos Voos;

/**
 * @brief Cria um voo a partir de campos CSV e adiciona ao catálogo.
 *
 * Esta função é tipicamente usada como callback do parser genérico.
 *
 * @param flights_data Array de strings com os campos do CSV.
 * @param voo_catalog Catálogo/estrutura onde o voo será armazenado.
 *
 * @return 0 em caso de sucesso, ou valor não-zero em caso de erro/entrada inválida.
 */
int criarVoo(char **flights_data, void *voo_catalog);

/**
 * @brief Função auxiliar para libertar um voo (compatível com callbacks GLib).
 *
 * @param voo_ptr Apontador genérico para um `Voos`.
 * @return void.
 */
void free_voo(void *voo_ptr);

/**
 * @brief Obtém o ID do voo.
 * @param voo Voo.
 * @return String com o ID.
 */
char* getId(Voos* voo);

/**
 * @brief Obtém a data/hora de partida prevista.
 * @param voo Voo.
 * @return String com a data/hora.
 */
char* getDeparture(Voos* voo);

/**
 * @brief Obtém a data/hora de partida real.
 * @param voo Voo.
 * @return String com a data/hora.
 */
char* getActualDeparture(Voos* voo);

/**
 * @brief Obtém a data/hora de chegada prevista.
 * @param voo Voo.
 * @return String com a data/hora.
 */
char* getArrival(Voos* voo);

/**
 * @brief Obtém a data/hora de chegada real.
 * @param voo Voo.
 * @return String com a data/hora.
 */
char* getActualArrival(Voos* voo);

/**
 * @brief Obtém a gate do voo.
 * @param voo Voo.
 * @return String com a gate.
 */
char* getGate(Voos* voo);

/**
 * @brief Obtém o estado do voo (ex.: Scheduled, Cancelled).
 * @param voo Voo.
 * @return String com o estado.
 */
char* getStatus(Voos* voo);

/**
 * @brief Obtém o aeroporto de origem.
 * @param voo Voo.
 * @return String com o código de origem.
 */
char* getOrigin(Voos* voo);

/**
 * @brief Obtém o aeroporto de destino.
 * @param voo Voo.
 * @return String com o código de destino.
 */
char* getDestination(Voos* voo);

/**
 * @brief Obtém o identificador da aeronave associada.
 * @param voo Voo.
 * @return String com o identificador da aeronave.
 */
char* getAircraft(Voos* voo);

/**
 * @brief Obtém o código/nome da companhia aérea.
 * @param voo Voo.
 * @return String com a companhia.
 */
char* getAirline(Voos* voo);

/**
 * @brief Obtém o URL de tracking do voo.
 * @param voo Voo.
 * @return String com o URL.
 */
char* getTrackingUrl(Voos* voo);

/**
 * @brief Obtém a partida prevista como timestamp.
 * @param voo Voo.
 * @return Timestamp (`time_t`).
 */
time_t getDepartureTs(Voos* voo);

/**
 * @brief Obtém a partida real como timestamp.
 * @param voo Voo.
 * @return Timestamp (`time_t`).
 */
time_t getActualDepartureTs(Voos* voo);

/**
 * @brief Getter interno: estado do voo (ponteiro const, sem alocação).
 * @param voo Voo.
 * @return Ponteiro const para a string do estado.
 */
const char* getStatus_internal(Voos* voo);

/**
 * @brief Getter interno: partida prevista (ponteiro const, sem alocação).
 * @param voo Voo.
 * @return Ponteiro const para a string da partida.
 */
const char* getDeparture_internal(Voos* voo);

/**
 * @brief Getter interno: origem (ponteiro const, sem alocação).
 * @param voo Voo.
 * @return Ponteiro const para a string da origem.
 */
const char* getOrigin_internal(Voos* voo);

/**
 * @brief Getter interno: destino (ponteiro const, sem alocação).
 * @param voo Voo.
 * @return Ponteiro const para a string do destino.
 */
const char* getDestination_internal(Voos* voo);

/**
 * @brief Define o ID do voo.
 * @param voo Voo.
 * @param id ID.
 * @return void.
 */
void setId(Voos* voo, char* id);

/**
 * @brief Define a partida prevista.
 * @param voo Voo.
 * @param departure Partida prevista.
 * @return void.
 */
void setDeparture(Voos* voo, char* departure);

/**
 * @brief Define a partida real.
 * @param voo Voo.
 * @param actual_departure Partida real.
 * @return void.
 */
void setActualDeparture(Voos* voo, char* actual_departure);

/**
 * @brief Define a chegada prevista.
 * @param voo Voo.
 * @param arrival Chegada prevista.
 * @return void.
 */
void setArrival(Voos* voo,char* arrival);

/**
 * @brief Define a chegada real.
 * @param voo Voo.
 * @param actual_arrival Chegada real.
 * @return void.
 */
void setActualArrival(Voos* voo,char* actual_arrival);

/**
 * @brief Define a gate.
 * @param voo Voo.
 * @param gate Gate.
 * @return void.
 */
void setGate(Voos* voo, char* gate);

/**
 * @brief Define o estado do voo.
 * @param voo Voo.
 * @param status Estado.
 * @return void.
 */
void setStatus(Voos* voo,char* status);

/**
 * @brief Define o aeroporto de origem.
 * @param voo Voo.
 * @param origin Origem.
 * @return void.
 */
void setOrigin(Voos* voo,char* origin);

/**
 * @brief Define o aeroporto de destino.
 * @param voo Voo.
 * @param destination Destino.
 * @return void.
 */
void setDestination(Voos* voo,char* destination);

/**
 * @brief Define a aeronave associada.
 * @param voo Voo.
 * @param aircraft Aeronave.
 * @return void.
 */
void setAircraft(Voos* voo,char* aircraft);

/**
 * @brief Define a companhia aérea.
 * @param voo Voo.
 * @param airline Companhia.
 * @return void.
 */
void setAirline(Voos* voo,char* airline);

/**
 * @brief Define o URL de tracking.
 * @param voo Voo.
 * @param tracking_url URL.
 * @return void.
 */
void setTrackingUrl(Voos* voo,char* tracking_url);



#endif