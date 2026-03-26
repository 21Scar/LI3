#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "voos.h"
#include "voosCatalog.h"
#include "aeronavesCatalog.h"
#include "aeronaves.h"

struct voosCatalog
{
    GHashTable* voos ;
    AeronavesCatalog* aeronavesCatalog; // para depois atualizar o contador voos 
    AeroportosCatalog* aeroportosCatalog; // para atualizar contadores de partidas por data
    ReservasCatalog* reservasCatalog; // para contar passageirosAdiciona o campo na estrutura privada struct voosCatalog:
    GHashTable* AirlineStats; // Q5 
    GPtrArray* airlineAtraso; // Q5 ordenar 
};

struct airlineStats{
    double sumAtrasos;
    int countVoos;
}; 

struct airlineAtraso{
    char* airline;
    double atrasoMedio;
    int contadorVoos; 
};

static void free_airline_atraso(gpointer data) {
    AirlineAtraso* x = (AirlineAtraso*)data;
    if (!x) return;
    if (x->airline) g_free(x->airline);
    g_free(x);
}

VoosCatalog* criarVoosCatalog()
{
    VoosCatalog* catalog = (VoosCatalog*)malloc(sizeof(VoosCatalog));
    if (!catalog)
    {
        fprintf(stderr, "Erro ao alocar memoria para o catalogo de voos\n");
        return NULL;
    }
    catalog->voos = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free_voo);
    catalog->aeronavesCatalog = NULL;
    catalog->aeroportosCatalog = NULL;
    catalog->reservasCatalog = NULL;
    catalog->AirlineStats = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, free);
    catalog->airlineAtraso = NULL;
    return catalog;

}

void setAeronavesCatalog(VoosCatalog* catalog, AeronavesCatalog* ac) {
    if (!catalog) return;
    catalog->aeronavesCatalog = ac;    
}

void setAeroportosCatalog(VoosCatalog* catalog, AeroportosCatalog* apc) {
    if (!catalog) return;
    catalog->aeroportosCatalog = apc;
}

void setReservasCatalog(VoosCatalog* catalog, ReservasCatalog* rc) {
    if (!catalog) return;
    catalog->reservasCatalog = rc;
}

void destruirVoosCatalog(VoosCatalog* catalog)
{
    if (!catalog) return;
    g_hash_table_destroy(catalog->voos);
    g_hash_table_destroy(catalog->AirlineStats);
    if (catalog->airlineAtraso) {
        g_ptr_array_set_free_func(catalog->airlineAtraso, free_airline_atraso);
        g_ptr_array_free(catalog->airlineAtraso, TRUE);
    }
    free(catalog);
}

void inserirVoo(VoosCatalog* catalog, Voos* voo)
{
    char *tmp = getId(voo);    // strdup do getter
    char *key = strdup(tmp);   // cópia da key
    free(tmp);                 // libertar cópia do getter 

    g_hash_table_insert(catalog->voos, key, voo);

    if(!catalog->aeronavesCatalog && !catalog->aeroportosCatalog) return; // se não estiver ligado a nenhum, não faz nada e continua a inserção
    
    char *status = getStatus(voo);
    if (!status) return;
    if (strcmp(status, "Cancelled") == 0) {
        free(status);
        return;
    }

    char *aeronave_id = getAircraft(voo);

    if (catalog->aeronavesCatalog) {
        Aeronaves* aeronave = buscarAeronave(catalog->aeronavesCatalog, aeronave_id);
           if(aeronave){
            setNVoos(aeronave, getNVoos(aeronave) + 1);
        }
    }

    free(status);
    free(aeronave_id);

    /* Atualizar contadores de partidas por data no aeroporto de origem */
    if (catalog->aeroportosCatalog) {
        char* origin = getOrigin(voo);
        if (origin && origin[0] != '\0') {
            char* actual_dep = getActualDeparture(voo);
            if (actual_dep && strlen(actual_dep) >= 10) {
                char date_part[11];
                strncpy(date_part, actual_dep, 10);
                date_part[10] = '\0';
                Aeroportos* ap = buscarAeroporto(catalog->aeroportosCatalog, origin);
                if (ap) {
                    addDepartureOnDate(ap, date_part);
                }
                free(actual_dep);
            } else if (actual_dep) {
                free(actual_dep);
            }
        }
        if (origin) free(origin);
    }

    // Filtrar voos Delayed
    char* status_atual = getStatus(voo);
    
    if(strcmp(status_atual, "Delayed") != 0) {
        free(status_atual);
        return;
    }

    time_t dep_planeada = getDepartureTs(voo);
    time_t dep_real = getActualDepartureTs(voo);
    
    // Calcular atraso em minutos
    double atraso = difftime (dep_real, dep_planeada) / 60.0;

    // Obter a companhia do voo atual para a hash 
    free(status_atual);
    char *airline_key = getAirline(voo);
    AirlineStats* stats = g_hash_table_lookup(catalog->AirlineStats, airline_key);

    if(!stats){ 
        stats = g_new0(AirlineStats, 1);
        stats->sumAtrasos = atraso;
        stats->countVoos = 1;
        g_hash_table_insert(catalog->AirlineStats, strdup(airline_key), stats);  
        free(airline_key);  
    } else {
        stats->sumAtrasos += atraso;
        stats->countVoos += 1;
        free(airline_key); // já existe na hash
    }
}

Voos* buscarVoo(VoosCatalog* catalog, const char* id) {
    if (!catalog || !id) return NULL;
    return g_hash_table_lookup(catalog->voos, id);
}

// Callback data for voosPorAeronave
struct voos_por_aeronave_cb {
    GHashTable* counts;
    VoosCatalog* catalog;
    const char* fabricante; // optional filter by manufacturer
};

static void voos_por_aeronave_callback(gpointer key, gpointer value, gpointer user_data) {
    Voos* voo = (Voos*)value;
    struct voos_por_aeronave_cb* data = (struct voos_por_aeronave_cb*)user_data;
    GHashTable* contadores = data->counts;
    VoosCatalog* vc = data->catalog;

    char *status = getStatus(voo);
    if (status == NULL) return;
    if (strcmp(status, "Cancelled") == 0) {
        free(status);
        return;
    }
    free(status);

    char* aeronave_id = getAircraft(voo);
    if (!aeronave_id) return;

    // Verificar filtro por fabricante, se for o caso
    if (data->fabricante != NULL && data->fabricante[0] != '\0') {
        if (!vc->aeronavesCatalog) {
            free(aeronave_id);
            return;
        }
        Aeronaves* a = buscarAeronave(vc->aeronavesCatalog, aeronave_id);
        if (!a) {
            free(aeronave_id);
            return;
        }
        char* manuf = getManufacturer(a);
        if (g_strcmp0(manuf, data->fabricante) != 0) {
            free(manuf);
            free(aeronave_id);
            return;
        }
        free(manuf);
    }

    int* contador = g_hash_table_lookup(contadores, aeronave_id);
    if (contador) {
        (*contador)++;
        free(aeronave_id);
    } else {
        int* novo = (int*)malloc(sizeof(int));
        *novo = 1;
        g_hash_table_insert(contadores, strdup(aeronave_id), novo);
        free(aeronave_id);
    }
}

GHashTable* voosPorAeronave(VoosCatalog* voosCatalog, const char* fabricante) {
    if (!voosCatalog) return NULL;
    GHashTable* contadores = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    struct voos_por_aeronave_cb data;
    data.counts = contadores;
    data.catalog = voosCatalog;
    data.fabricante = fabricante;

    g_hash_table_foreach(voosCatalog->voos, voos_por_aeronave_callback, &data);

    return contadores;
}

// Nova callback e função para contar partidas por aeroporto entre duas datas (inclusive)
struct partida_cb_data {
    const char* start;
    const char* end;
    GHashTable* counts;
};

static void partidas_callback(gpointer key, gpointer value, gpointer user_data) {
    Voos* voo = (Voos*)value;
    struct partida_cb_data* data = (struct partida_cb_data*)user_data;
    GHashTable* contadores = data->counts;

    char* status = getStatus(voo);
    if (status == NULL) return;
    if (strcmp(status, "Cancelled") == 0) {
        free(status);
        return;
    }
    free(status);

    char* actual_dep = getActualDeparture(voo);
    if (actual_dep == NULL) {
        return;
    }
    // Espera-se pelo menos "YYYY-MM-DD..." — extrair a parte da data
    if (strlen(actual_dep) < 10) {
        free(actual_dep);
        return;
    }
    char date_part[11];
    strncpy(date_part, actual_dep, 10);
    date_part[10] = '\0';
    free(actual_dep);

    // Comparação lexicográfica funciona para formato YYYY-MM-DD
    if (strcmp(date_part, data->start) < 0) return;
    if (strcmp(date_part, data->end) > 0) return;

    char* origin = getOrigin(voo);
    if (origin == NULL || origin[0] == '\0') {
        if (origin) free(origin);
        return;
    }

    int* contador = g_hash_table_lookup(contadores, origin);
    if (contador) {
        (*contador)++;
        free(origin);
    } else {
        int* novo = (int*)malloc(sizeof(int));
        *novo = 1;
        g_hash_table_insert(contadores, strdup(origin), novo);
        free(origin);
    }
}

GHashTable* partidasPorAeroportoEntreDatas(VoosCatalog* voosCatalog, const char* start_date, const char* end_date) {
    if (!voosCatalog || !start_date || !end_date) return NULL;

    GHashTable* contadores = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);

    struct partida_cb_data data;
    data.start = start_date;
    data.end = end_date;
    data.counts = contadores;

    g_hash_table_foreach(voosCatalog->voos, partidas_callback, &data);

    return contadores;
}

// Comparador para a query5 - ordenar airlines por atraso médio
gint compararAirlineAtraso(gconstpointer a, gconstpointer b){
    const AirlineAtraso* A = *((AirlineAtraso**)a);
    const AirlineAtraso* B = *((AirlineAtraso**)b);

    if (A->atrasoMedio < B->atrasoMedio) return 1;
    if (A->atrasoMedio > B->atrasoMedio) return -1;

    return g_strcmp0(A->airline, B->airline);
}


void construirAirlineAtrasoArray(VoosCatalog* catalog){
    if (!catalog) return;

    if (catalog->airlineAtraso) {
        g_ptr_array_set_free_func(catalog->airlineAtraso, free_airline_atraso);
        g_ptr_array_free(catalog->airlineAtraso, TRUE);
        catalog->airlineAtraso = NULL;
    }

    GPtrArray* array = g_ptr_array_new_with_free_func(free_airline_atraso);
    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, catalog->AirlineStats);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        const char* airline = (const char*)key;
        const AirlineStats* stats = (const AirlineStats*)value;

        if(stats->countVoos > 0){
            AirlineAtraso* node = g_new0(AirlineAtraso, 1);
            node->airline = g_strdup(airline);
            node->atrasoMedio = stats->sumAtrasos / stats->countVoos;
            node->contadorVoos = stats->countVoos;
            g_ptr_array_add(array, node);
        }
    }
    g_ptr_array_sort(array, compararAirlineAtraso);
    catalog->airlineAtraso = array;
}

GPtrArray* getAirlineAtrasoArray(VoosCatalog* catalog){
    return catalog->airlineAtraso;
}

char* get_AirlineAtraso_airline(AirlineAtraso* item){
    return item->airline;
};

double get_AirlineAtraso_atrasoMedio(AirlineAtraso* item){
    return item->atrasoMedio;
};

int get_AirlineAtraso_contadorVoos(AirlineAtraso* item){
    return item->contadorVoos;
};

