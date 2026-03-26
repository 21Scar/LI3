#ifndef RESERVAS_H
#define RESERVAS_H

/**
 * @file reservas.h
 * @brief Estrutura e operações associadas a reservas.
 *
 * Define o tipo abstrato `Reservas` (reserva individual) e disponibiliza
 * funções de construção/destruição e getters/setters dos seus campos.
 */

#include <stdbool.h>

/**
 * @brief Estrutura opaca que representa uma reserva.
 */
typedef struct reservas Reservas;

/**
 * @brief Cria uma reserva a partir de campos CSV e adiciona ao catálogo.
 *
 * Esta função é tipicamente usada como callback do parser genérico.
 *
 * @param reservations_data Array de strings com os campos do CSV.
 * @param reserva_catalog Catálogo/estrutura onde a reserva será armazenada.
 *
 * @return 0 em caso de sucesso, ou valor não-zero em caso de erro/entrada inválida.
 */
int criarReserva(char ** reservations_data, void *reserva_catalog);

/**
 * @brief Função auxiliar para libertar uma reserva (compatível com callbacks GLib).
 *
 * @param reserva_ptr Apontador genérico para uma `Reservas`.
 * @return void.
 */
void free_reserva(void* reserva_ptr);

/**
 * @brief Obtém o identificador da reserva.
 * @param reserva Reserva.
 * @return String com o ID da reserva.
 */
char* getReservationId(Reservas* reserva);

/**
 * @brief Obtém a lista de IDs de voo associados à reserva.
 * @param reserva Reserva.
 * @return String com os IDs de voo.
 */
char* getFlightIds(Reservas* reserva);

/**
 * @brief Obtém o número de documento associado à reserva.
 * @param reserva Reserva.
 * @return String com o número de documento.
 */
char* getReservationDocumentNumber(Reservas* reserva);

/**
 * @brief Getter interno: número de documento (ponteiro const).
 *
 * Usado em callbacks/rotinas de performance para evitar cópias.
 *
 * @param reserva Reserva.
 * @return Ponteiro const para a string do número de documento.
 */
const char* getReservationDocumentNumber_internal(Reservas* reserva);

/**
 * @brief Getter interno: IDs de voo (ponteiro const).
 *
 * Usado em callbacks/rotinas de performance para evitar cópias.
 *
 * @param reserva Reserva.
 * @return Ponteiro const para a string com os IDs de voo.
 */
const char* getFlightIds_internal(Reservas* reserva);

/**
 * @brief Obtém o lugar (seat) associado à reserva.
 * @param reserva Reserva.
 * @return String com o lugar.
 */
char* getSeat(Reservas* reserva);

/**
 * @brief Obtém o preço da reserva.
 * @param reserva Reserva.
 * @return Preço.
 */
double getPrice(Reservas* reserva);

/**
 * @brief Indica se a reserva tem bagagem extra.
 * @param reserva Reserva.
 * @return String com o indicador.
 */
char* hasExtraLuggage(Reservas* reserva);

/**
 * @brief Indica se a reserva tem embarque prioritário.
 * @param reserva Reserva.
 * @return String com o indicador.
 */
char* hasPriorityBoarding(Reservas* reserva);

/**
 * @brief Obtém o QR code associado à reserva.
 * @param reserva Reserva.
 * @return String com o QR code.
 */
char* getQrCode(Reservas* reserva);

/**
 * @brief Define o identificador da reserva.
 * @param reserva Reserva.
 * @param reservation_id ID da reserva.
 * @return void.
 */
void setReservationId(Reservas* reserva, char* reservation_id);

/**
 * @brief Define a lista de IDs de voo da reserva.
 * @param reserva Reserva.
 * @param flight_ids IDs de voo.
 * @return void.
 */
void setFlightIds(Reservas* reserva, char* flight_ids);

/**
 * @brief Define o número de documento associado à reserva.
 * @param reserva Reserva.
 * @param document_number Número de documento.
 * @return void.
 */
void setReservationDocumentNumber(Reservas* reserva, char* document_number);

/**
 * @brief Define o lugar (seat) associado à reserva.
 * @param reserva Reserva.
 * @param seat Lugar.
 * @return void.
 */
void setSeat(Reservas* reserva, char* seat);

/**
 * @brief Define o preço da reserva.
 * @param reserva Reserva.
 * @param price Preço.
 * @return void.
 */
void setPrice(Reservas* reserva, double price);

/**
 * @brief Define se existe bagagem extra.
 * @param reserva Reserva.
 * @param extra_luggage Indicador.
 * @return void.
 */
void setExtraLuggage(Reservas* reserva, char* extra_luggage);

/**
 * @brief Define se existe embarque prioritário.
 * @param reserva Reserva.
 * @param priority_boarding Indicador.
 * @return void.
 */
void setPriorityBoarding(Reservas* reserva, char* priority_boarding);

/**
 * @brief Define o QR code associado à reserva.
 * @param reserva Reserva.
 * @param qr_code QR code.
 * @return void.
 */
void setQrCode(Reservas* reserva, char* qr_code);

#endif