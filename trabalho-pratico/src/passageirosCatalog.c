#include <stdio.h>
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "passageiros.h"
#include "passageirosCatalog.h"
#include "reservas.h"
#include "voos.h"
#include "voosCatalog.h"

/*
// Calcular início da semana (domingo) como YYYYMMDD usando libc time
static int calcular_semana_iso(const char* date_str) {
    if (!date_str || strlen(date_str) < 10) return -1;

    struct tm tmv = {0};
    if (sscanf(date_str, "%4d-%2d-%2d", &tmv.tm_year, &tmv.tm_mon, &tmv.tm_mday) != 3) return -1;
    tmv.tm_year -= 1900;
    tmv.tm_mon -= 1;
    tmv.tm_isdst = -1;

    time_t t = mktime(&tmv);
    if (t == (time_t)-1) return -1;

    // mktime normaliza a struct e preenche tm_wday
    // tmv.tm_wday agora tem o dia da semana: 0=Sunday .. 6=Saturday
    t -= (time_t)(tmv.tm_wday) * 86400;

    struct tm wk;
    if (!localtime_r(&t, &wk)) return -1;

    int year = wk.tm_year + 1900;
    int month = wk.tm_mon + 1;
    int day = wk.tm_mday;
    return year * 10000 + month * 100 + day;
}
*/

struct passageirosCatalog
{
    GHashTable* passageiros ;
};

PassageirosCatalog* criarPassageirosCatalog()
{
    PassageirosCatalog* catalog = (PassageirosCatalog*)malloc(sizeof(PassageirosCatalog));
    if (!catalog)
    {
        fprintf(stderr, "Erro ao alocar memoria para o catalogo de passageiros\n");
        return NULL;
    }
    catalog->passageiros = g_hash_table_new_full(g_int_hash, g_int_equal, free, (GDestroyNotify)destruirPassageiros);
    return catalog;
}
void destruirPassageirosCatalog(PassageirosCatalog* catalog)
{
    g_hash_table_destroy(catalog->passageiros);
    free(catalog);
}

void inserirPassageiro(PassageirosCatalog* catalog, Passageiros* passageiro)
{
    int* key = malloc(sizeof(int));
    *key = getPassengerDocumentNumber(passageiro);
    g_hash_table_insert(catalog->passageiros, key, passageiro);
}

Passageiros* buscarPassageiro(PassageirosCatalog* catalog, int document_number)
{
    if (!catalog) return NULL;
    int key = document_number;
    return (Passageiros*) g_hash_table_lookup(catalog->passageiros, &key);
}
/*
// Estrutura de contexto para callback
struct atualiza_gasto_ctx {
    PassageirosCatalog* passageirosCatalog;
    VoosCatalog* voosCatalog;
};

// Callback para processar cada reserva - OTIMIZADO com encapsulamento mantido
static void processa_reserva_gasto(gpointer key, gpointer value, gpointer user_data) {
    (void)key;
    Reservas* reserva = (Reservas*)value;
    struct atualiza_gasto_ctx* ctx = (struct atualiza_gasto_ctx*)user_data;

    // Usa getter interno (const) - sem malloc/free
    const char* doc_str = getReservationDocumentNumber_internal(reserva);
    if (!doc_str) return;
    
    int documento = atoi(doc_str);
    
    Passageiros* passageiro = buscarPassageiro(ctx->passageirosCatalog, documento);
    if (!passageiro) return;
    
    double preco = getPrice(reserva);
    
    // Usa getter interno (const) - sem malloc/free
    const char* flight_ids_str = getFlightIds_internal(reserva);
    if (!flight_ids_str || flight_ids_str[0] == '\0') return;
    
    // Parse flight IDs com buffer local - removendo Python list notation
    const char* ptr = flight_ids_str;
    char flight_id_buf[64];
    
    while (*ptr) {
        // Skip delimiters: [ , ; ' " e espaço
        while (*ptr && (*ptr == '[' || *ptr == ']' || *ptr == ',' || *ptr == ';' || 
                        *ptr == '\'' || *ptr == '"' || *ptr == ' ')) ptr++;
        if (!*ptr) break;
        
        // Extract flight ID até encontrar delimitador
        int i = 0;
        while (*ptr && *ptr != '[' && *ptr != ']' && *ptr != ',' && *ptr != ';' && 
               *ptr != '\'' && *ptr != '"' && *ptr != ' ' && i < 63) {
            flight_id_buf[i++] = *ptr++;
        }
        flight_id_buf[i] = '\0';
        
        if (i == 0) continue;
        
        Voos* voo = buscarVoo(ctx->voosCatalog, flight_id_buf);
        if (!voo) continue;
        
        const char* status = getStatus_internal(voo);
        if (status && strcmp(status, "Cancelled") == 0) continue;
        
        const char* departure = getDeparture_internal(voo);
        if (departure && strlen(departure) >= 10) {
            char data_voo[11];
            memcpy(data_voo, departure, 10);
            data_voo[10] = '\0';
            
            int semana = calcular_semana_iso(data_voo);
            if (semana > 0) {
                adicionarGastoSemana(passageiro, semana, preco);
            }
        }
    }
}

// Atualizar gastos por semana em cada passageiro
void atualizarGastoPassageiros(PassageirosCatalog* passageirosCatalog, ReservasCatalog* reservasCatalog, VoosCatalog* voosCatalog) {
    if (!passageirosCatalog || !reservasCatalog || !voosCatalog) return;
    
    struct atualiza_gasto_ctx ctx;
    ctx.passageirosCatalog = passageirosCatalog;
    ctx.voosCatalog = voosCatalog;
    
    // Precisa de acesso à função reservas_foreach
    extern void reservas_foreach(ReservasCatalog* catalog, GHFunc func, gpointer user_data);
    reservas_foreach(reservasCatalog, processa_reserva_gasto, &ctx);
}
*/

/* Iterar sobre todos os passageiros */
void passageiros_foreach(PassageirosCatalog* catalog, GHFunc func, gpointer user_data) {
    if (!catalog || !func) return;
    g_hash_table_foreach(catalog->passageiros, func, user_data);
}
