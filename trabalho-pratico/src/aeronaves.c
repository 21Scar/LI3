#include "aeronaves.h"
#include "validation.h"
#include "aeronavesCatalog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct aeronaves {
    char* identifier;
    char* manufacturer;
    char* model;
    int year;
    int capacity;
    int range;
    int n_voos; // util para query 2
};

int criarAeronave(char **aircrafts_data, void *aeronave_catalog) {
    
    if (!validate_aircrafts(aircrafts_data, 6)) {
        return 1; // Dados inválidos
    }

    Aeronaves* aeronave = (Aeronaves*)calloc(1, sizeof(Aeronaves));
    if (!aeronave) {
        fprintf(stderr, "Erro ao alocar memoria para aeronave\n");
        return 1;
    }

    setIdentifier(aeronave, aircrafts_data[0]);
    setManufacturer(aeronave, aircrafts_data[1]);
    setModel(aeronave, aircrafts_data[2]);
    setYear(aeronave, atoi(aircrafts_data[3]));
    setCapacity(aeronave, atoi(aircrafts_data[4]));
    setRange(aeronave, atoi(aircrafts_data[5]));
    setNVoos(aeronave, 0);

    /* Inserir a aeronave no catálogo fornecido */
    AeronavesCatalog* catalog = (AeronavesCatalog*)aeronave_catalog;
    if (catalog != NULL) {
        inserirAeronave(catalog, aeronave);
    } else {
        /* Se não houver catálogo, libera a aeronave e sinaliza erro */
        free_aeronave(aeronave);
        return 1;
    }

    return 0; // Sucesso
}

void free_aeronave(void* aeronave_ptr) {
    Aeronaves* aeronave = (Aeronaves*)aeronave_ptr;

    free(aeronave->identifier);
    free(aeronave->manufacturer);
    free(aeronave->model);
    free(aeronave);
}

char* getIdentifier(Aeronaves* aeronave) {
    return strdup(aeronave->identifier);
}

char* getManufacturer(Aeronaves* aeronave) {
    return strdup(aeronave->manufacturer);
}

char* getModel(Aeronaves* aeronave) {
    return strdup(aeronave->model);
}

int getYear(Aeronaves* aeronave) {
    return aeronave->year;
}

int getCapacity(Aeronaves* aeronave) {
    return aeronave->capacity;
}

int getRange(Aeronaves* aeronave) {
    return aeronave->range;
}

int getNVoos(Aeronaves* aeronave) {
    return aeronave->n_voos;
}

void setIdentifier(Aeronaves* aeronave, char* identifier) {
    if (aeronave->identifier) {
        free(aeronave->identifier);
    }
    aeronave->identifier = strdup(identifier);
}

void setManufacturer(Aeronaves* aeronave, char* manufacturer) {
    if (aeronave->manufacturer) {
        free(aeronave->manufacturer);
    }
    aeronave->manufacturer = strdup(manufacturer);
}

void setModel(Aeronaves* aeronave, char* model) {
    if (aeronave->model) {
        free(aeronave->model);
    }
    aeronave->model = strdup(model);
}

void setYear(Aeronaves* aeronave, int year) {
    aeronave->year = year;
}

void setCapacity(Aeronaves* aeronave, int capacity) {
    aeronave->capacity = capacity;
}

void setRange(Aeronaves* aeronave, int range) {
    aeronave->range = range;
}

void setNVoos(Aeronaves* aeronave, int n_voos) {
    aeronave->n_voos = n_voos;
}