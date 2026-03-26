#include "voos.h"
#include "voosCatalog.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "validation.h"
#include "voosCatalog.h"
#include <glib.h>


struct voos {
    char* id;
    char* departure;
    char* actual_departure;
    char* arrival;
    char* actual_arrival;
    char* gate;
    char* status;
    char* origin;
    char* destination;
    char* aircraft;
    char* airline;
    char* tracking_url;
    time_t departure_ts; 
    time_t actual_departure_ts;
};

int criarVoo(char **flights_data, void *voo_catalog) {
    if(!validate_voo(flights_data, 12)) {
        return 1; // Dados inválidos
    }

    Voos* voo = (Voos*)malloc(sizeof(Voos));

    setId(voo, flights_data[0]);
    setDeparture(voo, flights_data[1]);
    setActualDeparture(voo, flights_data[2]);
    setArrival(voo, flights_data[3]);
    setActualArrival(voo, flights_data[4]);
    setGate(voo, flights_data[5]);
    setStatus(voo, flights_data[6]);
    setOrigin(voo, flights_data[7]);
    setDestination(voo, flights_data[8]);
    setAircraft(voo, flights_data[9]);
    setAirline(voo, flights_data[10]);
    setTrackingUrl(voo, flights_data[11]);

    if(validate_datetime(voo->departure)){
        voo->departure_ts = datetime_to_time(voo->departure);
    } else {
        voo->departure_ts = 0;
    }

    if(validate_datetime(voo->actual_departure)){
        voo->actual_departure_ts = datetime_to_time(voo->actual_departure);
    } else {
        voo->actual_departure_ts = 0;
    }

    inserirVoo((VoosCatalog*)voo_catalog, voo);

    return 0; // Sucesso
}


void free_voo(void *voo_ptr){
    Voos* voo = (Voos*)voo_ptr;

    free(voo->id);
    free(voo->departure);
    free(voo->actual_departure);
    free(voo->arrival);
    free(voo->actual_arrival);
    free(voo->gate);
    free(voo->status);
    free(voo->origin);
    free(voo->destination);
    free(voo->aircraft);
    free(voo->airline);
    free(voo->tracking_url);
    free(voo);
}

char* getId( Voos* voo) {
    return strdup(voo->id);
}

char* getDeparture( Voos* voo) {
    return strdup(voo->departure);
}

char* getActualDeparture( Voos* voo) {
    return strdup(voo->actual_departure);
}

char* getArrival( Voos* voo) {
    return strdup(voo->arrival);
}
 char* getActualArrival( Voos* voo) {
    return strdup(voo->actual_arrival);
}
 char* getGate( Voos* voo) {
    return strdup(voo->gate);
}
 char* getStatus( Voos* voo) {
    return strdup(voo->status);
}
 char* getOrigin( Voos* voo) {
    return strdup(voo->origin);
}
 char* getDestination( Voos* voo) {
    return strdup(voo->destination);
}
 char* getAircraft( Voos* voo) {
    return strdup(voo->aircraft);
}
 char* getAirline( Voos* voo) {
    return strdup(voo->airline);
}
 char* getTrackingUrl( Voos* voo) {
    return strdup(voo->tracking_url);
}

time_t getDepartureTs(Voos* voo) {
    return voo->departure_ts;
}

time_t getActualDepartureTs(Voos* voo) {
    return voo->actual_departure_ts;
}

/* Getters internos (const, sem malloc) para performance */
const char* getStatus_internal(Voos* voo) {
    return voo->status;
}

const char* getDeparture_internal(Voos* voo) {
    return voo->departure;
}

const char* getOrigin_internal(Voos* voo) {
    return voo->origin;
}

const char* getDestination_internal(Voos* voo) {
    return voo->destination;
}

void setId(Voos* voo, char* id) {
    voo->id = strdup(id);
}

void setDeparture(Voos* voo, char* departure) {
    voo->departure = strdup(departure);
}

void setActualDeparture(Voos* voo, char* actual_departure) {
    voo->actual_departure = strdup(actual_departure);
}

void setArrival(Voos* voo, char* arrival) {
    voo->arrival = strdup(arrival);
}

void setActualArrival(Voos* voo, char* actual_arrival) {
    voo->actual_arrival = strdup(actual_arrival);
}

void setGate(Voos* voo, char* gate) {
    voo->gate = strdup(gate);
}

void setStatus(Voos* voo, char* status) {
    voo->status = strdup(status);
}

void setOrigin(Voos* voo, char* origin) {
    voo->origin = strdup(origin);
}

void setDestination(Voos* voo, char* destination) {
    voo->destination = strdup(destination);
}

void setAircraft(Voos* voo, char* aircraft) {
    voo->aircraft = strdup(aircraft);
}

void setAirline(Voos* voo, char* airline) {
    voo->airline = strdup(airline);
}

void setTrackingUrl(Voos* voo, char* tracking_url) {
    voo->tracking_url = strdup(tracking_url);
}
