#include "reservas.h"
#include "reservasCatalog.h"
#include "validation.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct reservas {
    char* reservation_id;
    char* flight_ids;
    char* document_number;
    char* seat;
    double price;
    char* extra_luggage;
    char* priority_boarding;
    char* qr_code;
};

int criarReserva(char ** reservations_data, void *reserva_catalog) {
    if(!validate_reservation(reservations_data,8)) {
        return 1; // Dados inválidos
    }

    Reservas* reserva = (Reservas*)malloc(sizeof(Reservas));

    setReservationId(reserva,reservations_data[0]);
    setFlightIds(reserva,reservations_data[1]);
    setReservationDocumentNumber(reserva, reservations_data[2]);
    setSeat(reserva, reservations_data[3]);
    setPrice(reserva, atof(reservations_data[4]));
    setExtraLuggage(reserva, reservations_data[5]);
    setPriorityBoarding(reserva, reservations_data[6]);
    setQrCode(reserva, reservations_data[7]);
    
    /* Inserir reserva no catálogo */
    ReservasCatalog* catalog = (ReservasCatalog*)reserva_catalog;
    if (catalog) {
        inserirReserva(catalog, reserva);
    }
    
    return 0; // Sucesso
}

void free_reserva(void* reserva_ptr) {
    Reservas* reserva = (Reservas*)reserva_ptr;

    free(reserva->reservation_id);
    free(reserva->flight_ids);
    free(reserva->document_number);
    free(reserva->seat);
    free(reserva->extra_luggage);
    free(reserva->priority_boarding);
    free(reserva->qr_code);
    free(reserva);
}

char* getReservationId(Reservas* reserva) {
    return strdup(reserva->reservation_id);
}

char* getFlightIds(Reservas* reserva) {
    return strdup(reserva->flight_ids);
}

char* getReservationDocumentNumber(Reservas* reserva) {
    return strdup(reserva->document_number);
}

/* Getter interno para callbacks de performance - NÃO liberar resultado */
const char* getReservationDocumentNumber_internal(Reservas* reserva) {
    return reserva->document_number;
}

const char* getFlightIds_internal(Reservas* reserva) {
    return reserva->flight_ids;
}

char* getSeat(Reservas* reserva) {
    return strdup(reserva->seat);
}

double getPrice(Reservas* reserva) {
    return reserva->price;
}

char* hasExtraLuggage(Reservas* reserva) {
    return strdup(reserva->extra_luggage);
}

char* hasPriorityBoarding(Reservas* reserva) {
    return strdup(reserva->priority_boarding);
}

char* getQrCode(Reservas* reserva) {
    return strdup(reserva->qr_code);
}

void setReservationId(Reservas* reserva, char* reservation_id) {
    reserva->reservation_id = strdup(reservation_id);
}

void setFlightIds(Reservas* reserva, char* flight_ids) {
    reserva->flight_ids = strdup(flight_ids);
}

void setReservationDocumentNumber(Reservas* reserva, char* document_number) {
    reserva->document_number = strdup(document_number);
}

void setSeat(Reservas* reserva, char* seat) {
    reserva->seat = strdup(seat);
}

void setPrice(Reservas* reserva, double price) {
    reserva->price = price;
}

void setExtraLuggage(Reservas* reserva, char* extra_luggage) {
    reserva->extra_luggage = strdup(extra_luggage);
}

void setPriorityBoarding(Reservas* reserva, char* priority_boarding) {
    reserva->priority_boarding = strdup(priority_boarding);
}

void setQrCode(Reservas* reserva, char* qr_code) {
    reserva->qr_code = strdup(qr_code);
}