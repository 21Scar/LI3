#include "aeroportos.h"
#include "validation.h"
#include "aeroportosCatalog.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

struct aeroportos {
    char* code;
    char* name;
    char* city;
    char* country;
    double latitude;
    double longitude;
    char* icao;
    char* type;
    /* Contadores: partidas por data YYYY-MM-DD */
    GHashTable* departures_by_date; // key: strdup(date), value: int* count
    /* Contadores: passageiros que chegam e partem */
    int arriving_passengers;
    int departing_passengers;
};

int criarAeroporto(char **airports_data, void *aeroporto_catalog) {

    if(!validate_airports(airports_data, 8)) {
        return 1; // Dados inválidos
    }

    Aeroportos* aeroporto = (Aeroportos*)calloc(1, sizeof(Aeroportos));
    if (!aeroporto) {
        fprintf(stderr, "Erro ao alocar memoria para aeroporto\n");
        return 1;
    }

    setCode(aeroporto, airports_data[0]);
    setName(aeroporto, airports_data[1]);
    setCity(aeroporto, airports_data[2]);
    setCountry(aeroporto, airports_data[3]);
    setLatitude(aeroporto, atof(airports_data[4]));
    setLongitude(aeroporto, atof(airports_data[5]));
    setIcao(aeroporto, airports_data[6]);
    setType(aeroporto, airports_data[7]);

    /* Inicializar hash de partidas por data */
    aeroporto->departures_by_date = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    aeroporto->arriving_passengers = 0;
    aeroporto->departing_passengers = 0;


    /* Inserir no catalogo */
    AeroportosCatalog* catalog = (AeroportosCatalog*)aeroporto_catalog;
    if (catalog != NULL) {
        inserirAeroporto(catalog, aeroporto);
    } else {
        free_aeroporto(aeroporto);
        return 1;
    }

    return 0; // Sucesso
}

void free_aeroporto(void* aeroporto_ptr) {
    Aeroportos* aeroporto = (Aeroportos*)aeroporto_ptr;

    free(aeroporto->code);
    free(aeroporto->name);
    free(aeroporto->city);
    free(aeroporto->country);
    free(aeroporto->icao);
    free(aeroporto->type);
    if (aeroporto->departures_by_date) {
        g_hash_table_destroy(aeroporto->departures_by_date);
    }
    free(aeroporto);
}

char* getCode(Aeroportos* aeroporto) {
    return strdup(aeroporto->code);
}

char* getName(Aeroportos* aeroporto) {
    return strdup(aeroporto->name);
}

char* getCity(Aeroportos* aeroporto) {
    return strdup(aeroporto->city);
}

char* getCountry(Aeroportos* aeroporto) {
    return strdup(aeroporto->country);
}

double getLatitude(Aeroportos* aeroporto) {
    return aeroporto->latitude;
}

double getLongitude(Aeroportos* aeroporto) {
    return aeroporto->longitude;
}

char* getIcao(Aeroportos* aeroporto) {
    return strdup(aeroporto->icao);
}

char* getType(Aeroportos* aeroporto) {
    return strdup(aeroporto->type);
}

void setCode(Aeroportos* aeroporto, char* code) {
    aeroporto->code = strdup(code);
}
void setName(Aeroportos* aeroporto, char* name) {
    aeroporto->name = strdup(name);
}

void setCity(Aeroportos* aeroporto, char* city) {
    aeroporto->city = strdup(city);
}

void setCountry(Aeroportos* aeroporto, char* country) {
    aeroporto->country = strdup(country);
}

void setLatitude(Aeroportos* aeroporto, double latitude) {
    aeroporto->latitude = latitude;
}

void setLongitude(Aeroportos* aeroporto, double longitude) {
    aeroporto->longitude = longitude;
}

void setIcao(Aeroportos* aeroporto, char* icao) {
    aeroporto->icao = strdup(icao);
}

void setType(Aeroportos* aeroporto, char* type) {
    aeroporto->type = strdup(type);
}


/* Atualizar contador de partidas numa data */
void addDepartureOnDate(Aeroportos* aeroporto, const char* date) {
    if (!aeroporto || !date) return;
    if (!aeroporto->departures_by_date) {
        aeroporto->departures_by_date = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
    }
    int* c = (int*)g_hash_table_lookup(aeroporto->departures_by_date, date);
    if (c) {
        (*c)++;
    } else {
        int* novo = (int*)malloc(sizeof(int));
        *novo = 1;
        g_hash_table_insert(aeroporto->departures_by_date, strdup(date), novo);
    }
}

/* Obter total de partidas entre duas datas inclusivas */
int getDeparturesBetweenDates(Aeroportos* aeroporto, const char* start_date, const char* end_date) {
    if (!aeroporto || !start_date || !end_date) return 0;
    if (!aeroporto->departures_by_date) return 0;

    int total = 0;
    GHashTableIter iter;
    gpointer key, val;
    g_hash_table_iter_init(&iter, aeroporto->departures_by_date);
    while (g_hash_table_iter_next(&iter, &key, &val)) {
        const char* d = (const char*)key;
        if (strcmp(d, start_date) < 0) continue;
        if (strcmp(d, end_date) > 0) continue;
        total += *((int*)val);
    }
    return total;
}

GHashTable* getDeparturesByDate(Aeroportos* aeroporto) {
    if (!aeroporto) return NULL;
    return aeroporto->departures_by_date;
}

/* Adicionar passageiros que chegam */
void addArrivingPassengers(Aeroportos* aeroporto, int count) {
    if (!aeroporto || count <= 0) return;
    aeroporto->arriving_passengers += count;
}

/* Adicionar passageiros que partem */
void addDepartingPassengers(Aeroportos* aeroporto, int count) {
    if (!aeroporto || count <= 0) return;
    aeroporto->departing_passengers += count;
}

/* Obter total de passageiros que chegaram */
int getArrivingPassengers(Aeroportos* aeroporto) {
    if (!aeroporto) return 0;
    return aeroporto->arriving_passengers;
}

/* Obter total de passageiros que partiram */
int getDepartingPassengers(Aeroportos* aeroporto) {
    if (!aeroporto) return 0;
    return aeroporto->departing_passengers;
}
