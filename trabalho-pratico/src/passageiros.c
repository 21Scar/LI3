#include "passageiros.h"
#include "passageirosCatalog.h"
#include "validation.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <glib.h>

struct passageiros{
    int document_number;
    char* first_name;
    char* last_name;
    char* dob ;
    char* nationality;
    char* gender;
    char* email;
    char* phone;
    char* address;
    char* photo;
    // GHashTable* gasto_por_semana; /* Query 4 only - commented out */
};

int criarPassageiros(char **passengers_data, void *passageiro_catalog) {
    if(!validate_passengers(passengers_data,10)) {
        return 1; // Dados inválidos
    }

    Passageiros* passageiro = (Passageiros*)malloc(sizeof(Passageiros));

    setPassengerDocumentNumber(passageiro, atoi(passengers_data[0]));
    setFirstName(passageiro, passengers_data[1]);
    setLastName(passageiro, passengers_data[2]);
    setDob(passageiro, passengers_data[3]);
    setNationality(passageiro, passengers_data[4]);
    setGender(passageiro, passengers_data[5]);
    setEmail(passageiro, passengers_data[6]);
    setPhone(passageiro, passengers_data[7]);
    setAddress(passageiro, passengers_data[8]);
    setPhoto(passageiro, passengers_data[9]);
    
    /* Query 4 only - commented out */
    // passageiro->gasto_por_semana = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, free);

    /* Inserir no catalogo */
    PassageirosCatalog* catalog = (PassageirosCatalog*)passageiro_catalog;
    if (catalog) {
        inserirPassageiro(catalog, passageiro);
    }

    return 0; // Sucesso
}

void destruirPassageiros(Passageiros* passageiro) {
    if (!passageiro) return;
    free(passageiro->first_name);
    free(passageiro->last_name);
    free(passageiro->dob);
    free(passageiro->nationality);
    free(passageiro->gender);
    free(passageiro->email);
    free(passageiro->phone);
    free(passageiro->address);
    free(passageiro->photo);
    /* Query 4 only - commented out */
    // if (passageiro->gasto_por_semana) {
    //     g_hash_table_destroy(passageiro->gasto_por_semana);
    // }
    free(passageiro);
}

int getPassengerDocumentNumber(const Passageiros* passageiro) {
    return passageiro->document_number;
}

char* getFirstName(const Passageiros* passageiro) {
    return strdup(passageiro->first_name);
}

char* getLastName(const Passageiros* passageiro) {
    return strdup(passageiro->last_name);
}

char* getDob(const Passageiros* passageiro) {
    return strdup(passageiro->dob);
}

char* getNationality(const Passageiros* passageiro) {
    return strdup(passageiro->nationality);
}

char* getGender(const Passageiros* passageiro) {
    return strdup(passageiro->gender);
}

char* getEmail(const Passageiros* passageiro) {
    return strdup(passageiro->email);
}

char* getPhone(const Passageiros* passageiro) {
    return strdup(passageiro->phone);
}

char* getAddress(const Passageiros* passageiro) {
    return strdup(passageiro->address);
}

char* getPhoto(const Passageiros* passageiro) {
    return strdup(passageiro->photo);
}

void setPassengerDocumentNumber(Passageiros* passageiro, int document_number) {
    passageiro->document_number = document_number;
}

void setFirstName(Passageiros* passageiro, char* first_name) {
    passageiro->first_name = strdup(first_name);
}

void setLastName(Passageiros* passageiro, char* last_name) {
    passageiro->last_name = strdup(last_name);
}

void setDob(Passageiros* passageiro, char* dob) {
    passageiro->dob = strdup(dob);
}

void setNationality(Passageiros* passageiro, char* nationality) {
    passageiro->nationality = strdup(nationality);
}

void setGender(Passageiros* passageiro, char* gender) {
    passageiro->gender = strdup(gender);
}

void setEmail(Passageiros* passageiro, char* email) {
    passageiro->email = strdup(email);
}

void setPhone(Passageiros* passageiro, char* phone) {
    passageiro->phone = strdup(phone);
}

void setAddress(Passageiros* passageiro, char* address) {
    passageiro->address = strdup(address);
}

void setPhoto(Passageiros* passageiro, char* photo) {
    passageiro->photo = strdup(photo);
}

/* Query 4 functions - commented out
void adicionarGastoSemana(Passageiros* passageiro, int semana_id, double gasto) {
    if (!passageiro || !passageiro->gasto_por_semana) return;
    
    double* gasto_atual = g_hash_table_lookup(passageiro->gasto_por_semana, GINT_TO_POINTER(semana_id));
    
    if (gasto_atual) {
        *gasto_atual += gasto;
    } else {
        double* novo_gasto = malloc(sizeof(double));
        *novo_gasto = gasto;
        g_hash_table_insert(passageiro->gasto_por_semana, GINT_TO_POINTER(semana_id), novo_gasto);
    }
}

double obterGastoSemana(Passageiros* passageiro, int semana_id) {
    if (!passageiro || !passageiro->gasto_por_semana) return 0.0;
    
    double* gasto = g_hash_table_lookup(passageiro->gasto_por_semana, GINT_TO_POINTER(semana_id));
    return gasto ? *gasto : 0.0;
}

GList* obterSemanasComGasto(Passageiros* passageiro) {
    if (!passageiro || !passageiro->gasto_por_semana) return NULL;
    return g_hash_table_get_keys(passageiro->gasto_por_semana);
}

// Obter semanas e converter para int array - para evitar memory leaks da GList
int* obterSemanasComGasto_array(Passageiros* passageiro, int* count) {
    if (!passageiro || !passageiro->gasto_por_semana) {
        *count = 0;
        return NULL;
    }
    
    GList* semanas = g_hash_table_get_keys(passageiro->gasto_por_semana);
    if (!semanas) {
        *count = 0;
        return NULL;
    }
    
    int len = g_list_length(semanas);
    int* result = malloc(len * sizeof(int));
    
    int i = 0;
    for (GList* l = semanas; l != NULL; l = l->next) {
        result[i++] = GPOINTER_TO_INT(l->data);
    }
    
    g_list_free(semanas);
    *count = len;
    return result;
}
*/