#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <ctype.h>
#include "reservas.h"
#include "reservasCatalog.h"
#include "voosCatalog.h"
#include "voos.h"
#include "aeroportosCatalog.h"
#include "aeroportos.h"

struct reservasCatalog
{
    GHashTable* reservas ;
};

ReservasCatalog* criarReservasCatalog() {
    ReservasCatalog* catalog = (ReservasCatalog*)malloc(sizeof(ReservasCatalog));
    if (!catalog) {
        fprintf(stderr, "Erro ao alocar memoria para o catalogo de reservas\n");
        return NULL;
    }
    catalog->reservas = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_reserva);
    return catalog;
}

void destruirReservasCatalog(ReservasCatalog* catalog) {
    g_hash_table_destroy(catalog->reservas);
    free(catalog);
}
void inserirReserva(ReservasCatalog* catalog, Reservas* reserva) {
    char *tmp = getReservationId(reserva);   // strdup feito pelo getter
    char *key = strdup(tmp);                 // cópia usada pela hash table
    free(tmp);                               // libertar a cópia do getter

    g_hash_table_insert(catalog->reservas, key, reserva);
}

/* Callback para iterar reservas e contar passageiros por voo/aeroporto */
struct conta_passageiros_cb {
    VoosCatalog* voosCatalog;
    AeroportosCatalog* aeroportosCatalog;
};

// DAN: VERIFICAR COM MUSK SE ISTO FICA AQUI 
static void conta_passageiros_callback(gpointer key, gpointer value, gpointer user_data) {
    Reservas* reserva = (Reservas*)value;
    struct conta_passageiros_cb* data = (struct conta_passageiros_cb*)user_data;
    
    char* flight_ids_str = getFlightIds(reserva);
    // printf("Flight IDs: %s\n", flight_ids_str); // Debug print

    if (!flight_ids_str || flight_ids_str[0] == '\0') {
        if (flight_ids_str) free(flight_ids_str);
        return;
    }

    /* Parse flight IDs (separated by ; or ,) */
    char* copy = strdup(flight_ids_str);
    char* save_ptr = NULL;
    char* flight_id = strtok_r(copy, ";,", &save_ptr);

    while (flight_id != NULL) {
        char* tok = flight_id;

        // Remover espaços iniciais
        while (*tok && isspace((unsigned char)*tok)) tok++;

        // Remover espaços finais
        char* end_tok = tok + strlen(tok) - 1;
        while(end_tok >= tok && isspace((unsigned char)*end_tok)) {
            *end_tok = '\0';
            end_tok--;
        }

        // Remover [] 
        if(*tok == '[') tok++; 
        size_t len = strlen(tok);
        if(len > 0 && tok[len - 1] == ']'){
            tok[len - 1] = '\0';
            len--;
        }

        // Remover ' '
        len = strlen(tok);
        if(len >= 2){
            if((tok[0] == '\'' && tok[len - 1] == '\'')){
                tok[len - 1] = '\0';
                tok++;
                len -= 2;
            }
        }

        // Skip se ficou vazio 
        if (*tok == '\0') {
            flight_id = strtok_r(NULL, ";,", &save_ptr);
            continue;
        }
        
        Voos* voo = buscarVoo(data->voosCatalog, tok);
        if (voo) {
            char* status = getStatus(voo);
            if (status && strcmp(status, "Cancelled") != 0) {
                char* origin = getOrigin(voo);
                char* destination = getDestination(voo);
                
                if (origin && origin[0] != '\0') {
                    Aeroportos* ap_origin = buscarAeroporto(data->aeroportosCatalog, origin);
                    if (ap_origin) {
                        addDepartingPassengers(ap_origin, 1);
                    }
                }
                
                if (destination && destination[0] != '\0') {
                    Aeroportos* ap_dest = buscarAeroporto(data->aeroportosCatalog, destination);
                    if (ap_dest) {
                        addArrivingPassengers(ap_dest, 1);
                    }
                }
                
                if (origin) free(origin);
                if (destination) free(destination);
            }
            if (status) free(status);
        }
        // Proximo token 
        flight_id = strtok_r(NULL, ";,", &save_ptr);
    }
    
    free(copy);
    free(flight_ids_str);
}

void atualizarContagemPassageiros(ReservasCatalog* reservasCatalog, VoosCatalog* voosCatalog, AeroportosCatalog* aeroportosCatalog) {
    if (!reservasCatalog || !voosCatalog || !aeroportosCatalog) return;
    
    struct conta_passageiros_cb data;
    data.voosCatalog = voosCatalog;
    data.aeroportosCatalog = aeroportosCatalog;
    
    g_hash_table_foreach(reservasCatalog->reservas, conta_passageiros_callback, &data);
}

void reservas_foreach(ReservasCatalog* catalog, GHFunc func, gpointer user_data) {
    if (!catalog || !func) return;
    g_hash_table_foreach(catalog->reservas, func, user_data);
}
