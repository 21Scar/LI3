#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include "querys.h"
#include "aeroportosCatalog.h"
#include "aeronavesCatalog.h"
#include "main.h"
#include "aeronaves.h"
#include "voosCatalog.h"
#include "voos.h"
#include "passageiros.h"
#include "reservasCatalog.h"
#include "reservas.h"
#include "passageirosCatalog.h"



struct aeronaveVoos 
    {
        char* identifier; // Identificador da aeronave
        int n_voos; // Número de voos realizados
    };

struct dadosAeroportoPartidas {
    const char* start;
    const char* end;
    char* best_code;
    int best_count;
    Aeroportos* best_ap;
};

/* Query 4 structures - commented out
struct recolher_gastos_ctx {
    GHashTable* semana_gastos; // semana_id -> GArray de PassageiroGastoSemana 
    GHashTable* passageiro_count; // documento -> contagem de aparições no top 10 
    char* start_date;
    char* end_date;
    int has_filter;
    int start_days;
    int end_days;
};

// Estrutura para armazenar passageiro + gasto de uma semana 
struct PassageiroGastoSemana {
    int documento;
    double gasto;
};
*/


struct q6_ctx {
    PassageirosCatalog* passageirosCatalog;
    VoosCatalog* voosCatalog;
    ReservasCatalog* reservasCatalog;
    const char* nationality;
    GHashTable* airport_to_count; // key: gchar* iata (g_strdup), value: int* count
};

// Simple cache for query 6: key = "<nationality>|<mode>" where mode is 'S' or 'N'
static GHashTable* q6_cache = NULL;
// Lazy precomputed index for query 6: nationality -> (airport -> set(documents))
static GHashTable* q6_index = NULL;

// normalize a copied string: trims and removes surrounding quotes; returns newly allocated string
static char* normalize_copy(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char* tmp = (char*)malloc(len + 1);
    if (!tmp) return NULL;
    memcpy(tmp, s, len + 1);
    // trim left
    size_t start = 0;
    while (tmp[start] && isspace((unsigned char)tmp[start])) start++;
    // trim right
    size_t end = strlen(tmp);
    while (end > start && isspace((unsigned char)tmp[end-1])) end--;
    // remove surrounding quotes
    if (end - start >= 2 && ((tmp[start] == '"' && tmp[end-1] == '"') || (tmp[start] == '\'' && tmp[end-1] == '\''))) {
        start++;
        end--;
    }
    size_t newlen = end > start ? end - start : 0;
    // shift to front
    if (start > 0 && newlen > 0) memmove(tmp, tmp + start, newlen);
    tmp[newlen] = '\0';
    return tmp;
}

// destroy function for inner table values (the inner table itself)
static void destroy_inner_table(gpointer data) {
    GHashTable* inner = (GHashTable*)data;
    if (inner) g_hash_table_destroy(inner);
}

// Cleanup function to free q6_cache and q6_index
void q6_cleanup(void) {
    if (q6_cache) {
        g_hash_table_destroy(q6_cache);
        q6_cache = NULL;
    }
    if (q6_index) {
        g_hash_table_destroy(q6_index);
        q6_index = NULL;
    }
}


/* Converter YYYY-MM-DD em dias desde epoch; retorna -1 se inválido 
static int date_to_days(const char* date_str) {
    if (!date_str || strlen(date_str) < 10) return -1;
    struct tm tmv = {0};
    if (sscanf(date_str, "%4d-%2d-%2d", &tmv.tm_year, &tmv.tm_mon, &tmv.tm_mday) != 3) return -1;
    tmv.tm_year -= 1900;
    tmv.tm_mon -= 1;
    tmv.tm_isdst = -1;
    time_t t = mktime(&tmv);
    if (t == (time_t)-1) return -1;
    return (int)(t / 86400); //dias inteiros
}
*/
/* Converte semana_id (YYYYMMDD do domingo) em dias
static int week_start_days_from_id(int semana_id) {
    int year = semana_id / 10000;
    int month = (semana_id / 100) % 100;
    int day = semana_id % 100;
    char buf[20]; // espaço folgado para suprimir -Wformat-truncation
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
    return date_to_days(buf);
}
*/

// Chamar a query necessária
void run_query(Program* program, int query_number, char* arguments, int command_number, int s_existe) {
    switch(query_number){
        case 1: {
            listarResumoAeroporto(program, arguments, command_number, s_existe);
            break;
        }

        case 2: {
            listarTopAeronaves(program, arguments, command_number, s_existe);
            break;
        }

        case 3: {
            listarAeroportoMaisPartidas(program, arguments, command_number, s_existe);
            break;
        }

        /* Query 4 - commented out
        case 4: {
            listarPassageiroTopSemanas(program, arguments, command_number, s_existe);
            break;
        }
        */

        case 5:{
            
            listarCompanhiasAtrasoMedio(program, arguments, command_number, s_existe);
            break;
        }
        case 6:{

            listarDestinoMaisComumPorNacionalidade(program, arguments, command_number, s_existe);
            break;
        }    

        default:
            //printf("Número de query inválido: %d\n", query_number);
            save_output(command_number, "");
            break;
    }
}


//Query 1- listar resumo de um aeroporto
void listarResumoAeroporto(Program* program, char* arguments, int command_number, int s_existe) {
    if (!program) {
        save_output(command_number, "");
        return;
    }

    AeroportosCatalog* catalog = getAeroportosCatalog(program);
    if (!catalog) {
        save_output(command_number, "");
        return;
    }

    if (!arguments) {
        save_output(command_number, "");
        return;
    }
    
    // Extrair o codigo do aeroporto dos args e remover espaços em branco
    char* code = strdup(arguments); 
    
    // Remover espaços iniciais
    char* start = code;
    while (*start && isspace((unsigned char)*start)) start++;
    
    // Remover espaços finais
    char* end = start + strlen(start) - 1;
    while (end >= start && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    
    // Se ficou vazio, retornar
    if (*start == '\0') {
        free(code);
        save_output(command_number, "");
        return;
    }

    // Busca o aeroporto pelo código
    Aeroportos* aeroporto = buscarAeroporto(catalog, start);

    if(!aeroporto){
        // Aeroporto não encontrado
        free(code);
        save_output(command_number, "");
        return;
    }
    
    char* name = getName(aeroporto);
    char* city = getCity(aeroporto);
    char* country = getCountry(aeroporto);
    char* type = getType(aeroporto);
    int arrival_count = getArrivingPassengers(aeroporto);
    int departure_count = getDepartingPassengers(aeroporto);

    char* output = NULL; 
    if(!s_existe){
        output = g_strdup_printf("%s;%s;%s;%s;%s;%d;%d\n",
                                 start, name, city, country, type, arrival_count, departure_count);
    } else {
        output = g_strdup_printf("%s=%s=%s=%s=%s=%d=%d\n",
                                 start, name, city, country, type, arrival_count, departure_count);
    }

    save_output(command_number, output);

    g_free(output);
    free(name);
    free(city);
    free(country);
    free(type);
    free(code);
}



//Query 2- Top N aeronaves com mais voos realizados
void listarTopAeronaves(Program* program, char* arguments, int command_number, int s_existe) {
    if (!program) {
        save_output(command_number, "");
        return;
    }
    // Extrair o catálogo de aeronaves e voos do programa e os argumentos
    AeronavesCatalog* aeronaves_catalog = getAeronavesCatalog(program);
    
    if (!aeronaves_catalog || !arguments) {
        save_output(command_number, "");
        return;
    }

    int top_n = 0;
    char fabricante[50]; 
    int arg_fabricante; 

    if(sscanf(arguments, "%d %s", &top_n, fabricante) == 2){
        arg_fabricante = 1; 
    } else if (sscanf(arguments, "%d", &top_n) == 1){
        arg_fabricante = 0;
    }else {
        save_output(command_number, "");
        return;
    }

    if (top_n <= 0) {
        save_output(command_number, "");
        return;
    }

    // Iniciar o output 
    char* output = g_strdup("");

    GArray* naves_ordenadas_voos = getAeronavesArray(aeronaves_catalog);
    if (!naves_ordenadas_voos || naves_ordenadas_voos->len == 0) {
        save_output(command_number, "");
        g_free(output);
        return;
    }

    GString* out = g_string_new("");

    // Escrever as top N aeronaves no ficheiro de output
    int n_aeronaves_add = 0; 
    for(guint i = 0; i < naves_ordenadas_voos->len && n_aeronaves_add < top_n; i++){
        AeronaveRanking* nave_voos = g_array_index(naves_ordenadas_voos, AeronaveRanking*, i);
        
        if(!nave_voos) continue;

        Aeronaves* aeronave_obj = getAeronaveRank_Aeronave(nave_voos);
        if(!aeronave_obj) continue;

        // Fabricante e modelo 
        char* fabricante_atual = getManufacturer(aeronave_obj);
        char* modelo_atual = getModel(aeronave_obj);
        
        if(!fabricante_atual || !modelo_atual) {
            if(fabricante_atual) free(fabricante_atual);
            if(modelo_atual) free(modelo_atual);
            continue;
        }

        // Verificar se o fabricante é o mesmo, caso necessário 
        if(arg_fabricante){
            if (g_strcmp0(fabricante_atual, fabricante) != 0) {
                free(fabricante_atual);
                free(modelo_atual);
                continue; // não é o fabricante pedido
            }
        }
        const char* identifier = getAeronaveRank_Id(nave_voos);
        if(!identifier) {
            free(fabricante_atual);
            free(modelo_atual);
            continue;
        }
        
        int n_voos = getNVoos(aeronave_obj);
        
        if(!s_existe){
            if(out->len > 0) g_string_append_c(out, '\n');
            g_string_append_printf(out, "%s;%s;%s;%d", identifier, fabricante_atual, modelo_atual, n_voos);
        } else {
            if(out->len > 0) g_string_append_c(out, '\n');
            g_string_append_printf(out, "%s=%s=%s=%d", identifier, fabricante_atual, modelo_atual, n_voos);
        }


        free(fabricante_atual);
        free(modelo_atual);
        n_aeronaves_add++; 
    }

    g_string_append_c(out, '\n'); // Adicionar /n na linha final 
    save_output(command_number, out->str);
    g_string_free(out, TRUE);
}
 
// Guardar o output
void save_output(int command_number, const char* output){
    char* output_file = g_strdup_printf(OUTPUT_DIR_NAME "/" OUTPUT_FILE_NAME, command_number + 1);
    FILE* file = fopen(output_file, "w");
    
    if(file == NULL){
        perror("Erro ao abrir o ficheiro de output");
        g_free(output_file);
        return;
    }
    
    if(output == NULL || output[0] == '\0'){
        fprintf(file, "\n");
    } else {
        fprintf(file, "%s", output);
    }

    fclose(file);
    g_free(output_file);
}



// Query 3 - aeroporto com mais partidas entre 2 datas (inclusive); Formato  "<YYYY-MM-DD> <YYYY-MM-DD>"

void encontrarAeroportoMaisPartidas(gpointer key, gpointer value, gpointer user_data){
    const char* code = (const char*)key;
    Aeroportos* ap = (Aeroportos*)value;
    struct dadosAeroportoPartidas* data = (struct dadosAeroportoPartidas*)user_data;

    int count = getDeparturesBetweenDates(ap, data->start, data->end);
    if (count <= 0) return;

    if (data->best_code == NULL || count > data->best_count ||
        (count == data->best_count && g_strcmp0(code, data->best_code) < 0)) {
        if(data->best_code){
           free(data->best_code);
        }
        data->best_code = strdup(code);
        data->best_count = count;
        data->best_ap = ap; /*Guardar referencia ao aeroporto atual*/
    }
}

void listarAeroportoMaisPartidas(Program* program, char* arguments, int command_number, int s_existe) {
    if (!program || !arguments) {
        save_output(command_number, "");
        return;
    }

    AeroportosCatalog* aeroportos_catalog = getAeroportosCatalog(program);
    if (!aeroportos_catalog) {
        save_output(command_number, "");
        return;
    }

    char start[32], end[32];
    if (sscanf(arguments, "%31s %31s", start, end) != 2) {
        save_output(command_number, "");
        return;
    }

    //inicializar estrutura de dados (para callback)
    struct dadosAeroportoPartidas data;
    data.start = start;//guarda a data inicial
    data.end = end;//guarda a data final
    data.best_code = NULL;//ainda nao temos melhor codigo
    data.best_count = -1; //inicializar com -1 para garantir que qualquer contagem é maior
    data.best_ap = NULL; //ainda nao temos melhor aeroporto

    aeroportos_foreach(aeroportos_catalog, (GHFunc)encontrarAeroportoMaisPartidas, &data);

   if (!data.best_code || !data.best_ap) {
        if (data.best_code) free(data.best_code);
        save_output(command_number, "");
        return;
    }
    
    Aeroportos* ap = data.best_ap;  /* Ja temos o aeroporto do callback */


    char* name = getName(ap);
    char* city = getCity(ap);
    char* country = getCountry(ap);

    // char* output = g_strdup_printf("%s,%s,%s,%s,%d\n", data.best_code, name, city, country, data.best_count);
    
    char* output = NULL;
    if(!s_existe){
        output = g_strdup_printf("%s;%s;%s;%s;%d\n", data.best_code, name, city, country, data.best_count);
    } else {
        output = g_strdup_printf("%s=%s=%s=%s=%d\n", data.best_code, name, city, country, data.best_count);
    }
    
    save_output(command_number, output);

    free(data.best_code);
    g_free(output);
    free(name);
    free(city);
    free(country);
}

/*
//Query 4 - Passageiro que esteve mais tempo no top 10 durante um período

 //Comparador: gasto descendente, documento ascendente
gint comparar_gasto_desc(gconstpointer a, gconstpointer b) {
    const struct PassageiroGastoSemana* pa = (const struct PassageiroGastoSemana*)a;
    const struct PassageiroGastoSemana* pb = (const struct PassageiroGastoSemana*)b;
    
    if (pa->gasto > pb->gasto) return -1;
    if (pa->gasto < pb->gasto) return 1;
    return pa->documento - pb->documento; // Desempate por documento
}

// Callback para iterar passageiros e coletar gastos por semana

void coletar_gastos_callback(gpointer key, gpointer value, gpointer user_data) {
    (void)key;
    Passageiros* passageiro = (Passageiros*)value;
    struct recolher_gastos_ctx* ctx = (struct recolher_gastos_ctx*)user_data;
    
    int documento = getPassengerDocumentNumber(passageiro);
    
    // Usar _array para evitar GList memory leak
    int count = 0;
    int* semanas = obterSemanasComGasto_array(passageiro, &count);
    
    for (int i = 0; i < count; i++) {
        int semana_id = semanas[i];
        double gasto = obterGastoSemana(passageiro, semana_id);

        // Filtro de datas opcional: considerar semanas que INTERSETAM o intervalo
        if (ctx->has_filter && ctx->start_days >= 0 && ctx->end_days >= 0) {
            int week_start_days = week_start_days_from_id(semana_id);
            int week_end_days = week_start_days + 6; // semana completa
            if (week_end_days < ctx->start_days || week_start_days > ctx->end_days) {
                continue; // fora do intervalo
            }
        }
        
        // Adiciona ao hash de semanas
        GArray* array = g_hash_table_lookup(ctx->semana_gastos, GINT_TO_POINTER(semana_id));
        if (!array) {
            array = g_array_new(FALSE, FALSE, sizeof(struct PassageiroGastoSemana));
            g_hash_table_insert(ctx->semana_gastos, GINT_TO_POINTER(semana_id), array);
        }
        
        struct PassageiroGastoSemana entrada = {.documento = documento, .gasto = gasto};
        g_array_append_val(array, entrada);
    }
    
    if (semanas) free(semanas);
}

void listarPassageiroTopSemanas(Program* program, char* arguments, int command_number, int s_existe) {
    if (!program) {
        save_output(command_number, "");
        return;
    }

    PassageirosCatalog* passageiros_catalog = getPassageirosCatalog(program);
    if (!passageiros_catalog) {
        save_output(command_number, "");
        return;
    }

    char start_date[32] = "";
    char end_date[32] = "";
    int has_date_filter = 0;

    if (arguments && arguments[0] != '\0') {
        if (sscanf(arguments, "%31s %31s", start_date, end_date) == 2) {
            has_date_filter = 1;
        }
    }

    // Recolher gastos por semana
    GHashTable* semana_gastos = g_hash_table_new_full(
        g_direct_hash, g_direct_equal, NULL,
        (GDestroyNotify)g_array_unref
    );
    
    // Contador de passageiros no top 10
    GHashTable* passageiro_count = g_hash_table_new(g_direct_hash, g_direct_equal);
    
    struct recolher_gastos_ctx ctx = {
        .semana_gastos = semana_gastos,
        .passageiro_count = passageiro_count,
        .start_date = start_date,
        .end_date = end_date,
        .has_filter = has_date_filter,
        .start_days = has_date_filter ? date_to_days(start_date) : -1,
        .end_days = has_date_filter ? date_to_days(end_date) : -1
    };
    
    // Iterar todos os passageiros para coletar gastos por semana
    passageiros_foreach(passageiros_catalog, coletar_gastos_callback, &ctx);
    
    // Agora processar cada semana para extrair o top 10 e contar aparições
    GHashTableIter iter_semanas;
    gpointer key_semana, value_semana;
    g_hash_table_iter_init(&iter_semanas, semana_gastos);
    
    while (g_hash_table_iter_next(&iter_semanas, &key_semana, &value_semana)) {
        GArray* array = (GArray*)value_semana;
        
        // Ordenar por gasto descendente, desempate por documento ascendente
        g_array_sort(array, comparar_gasto_desc);
        
        // Considerar apenas os top 10 desta semana
        int top_limit = (array->len < 10) ? array->len : 10;
        for (int j = 0; j < top_limit; j++) {
            struct PassageiroGastoSemana* pg = &g_array_index(array, struct PassageiroGastoSemana, j);
            int doc = pg->documento;
            
            // Incrementar contador de aparições no top 10
            gpointer curr_count = g_hash_table_lookup(passageiro_count, GINT_TO_POINTER(doc));
            int new_count = GPOINTER_TO_INT(curr_count) + 1;
            g_hash_table_insert(passageiro_count, GINT_TO_POINTER(doc), GINT_TO_POINTER(new_count));
        }
    }
    
   // Encontrar passageiro com mais aparições no top 10 (desempate: documento menor)
    int max_count = 0;
    int best_documento = -1; // -1 indica que ainda não há vencedor
    
    GHashTableIter iter_resultado;
    gpointer key_resultado, value_resultado;
    g_hash_table_iter_init(&iter_resultado, passageiro_count);
    
while (g_hash_table_iter_next(&iter_resultado, &key_resultado, &value_resultado)) {
        int documento = GPOINTER_TO_INT(key_resultado);
        int count = GPOINTER_TO_INT(value_resultado);
        if (count > max_count || (count == max_count && (best_documento == -1 || documento < best_documento))) {
            max_count = count;
            best_documento = documento;
        }
    }

    // Construir resultado apenas para o vencedor
    GString* result = g_string_new("");
    if (max_count > 0 && best_documento != -1) {
        Passageiros* passageiro = buscarPassageiro(passageiros_catalog, best_documento);
        if (passageiro) {
            char* first_name = getFirstName(passageiro);
            char* last_name = getLastName(passageiro);
            char* dob = getDob(passageiro);
            char* nationality = getNationality(passageiro);
            if (first_name && last_name && dob && nationality) {
                const char* fmt = s_existe ? "%09d=%s=%s=%s=%s=%d" : "%09d;%s;%s;%s;%s;%d";
                g_string_append_printf(result, fmt,
                    best_documento, first_name, last_name, dob, nationality, max_count);
            }
            if (first_name) free(first_name);
            if (last_name) free(last_name);
            if (dob) free(dob);
            if (nationality) free(nationality);
        }
    }

    save_output(command_number, result->str);
    
    g_string_free(result, TRUE);
    g_hash_table_destroy(semana_gastos);
    g_hash_table_destroy(passageiro_count);
}
*/

// Query 5 - Top N companhias aéreas com maior atraso médio em voos
void listarCompanhiasAtrasoMedio(Program* program, char* arguments, int command_number, int s_existe) {
    int N = 0;
    if(arguments){
        N = atoi(arguments);
    } 

    if(N <= 0){
        save_output(command_number, "");
        return;
    }

   VoosCatalog* voos_catalog = getVoosCatalog(program);
   GPtrArray* airline_atraso_ar = getAirlineAtrasoArray(voos_catalog);
   
   GString* out = g_string_new("");
   for(int i = 0; i < N && i < (int)airline_atraso_ar->len; i++){
        AirlineAtraso* item = (AirlineAtraso*)g_ptr_array_index(airline_atraso_ar, i);
        char* airline_atual = get_AirlineAtraso_airline(item);
        double atraso_medio = get_AirlineAtraso_atrasoMedio(item);
        int n_voos = get_AirlineAtraso_contadorVoos(item);
        if(!s_existe){
            if(out->len > 0) g_string_append_c(out, '\n');
            g_string_append_printf(out, "%s;%d;%.3f", airline_atual, n_voos, atraso_medio);
        } else {
            if(out->len > 0) g_string_append_c(out, '\n');
            g_string_append_printf(out, "%s=%d=%.3f", airline_atual, n_voos, atraso_medio);
        }
   } 

   g_string_append_c(out, '\n');
   save_output(command_number, out->str);
   g_string_free(out, TRUE);
}
// Query 6 - Listar o aeroporto de destino mais comum para passageiros de uma nacionalidade

// q6_reserva_cb removed: index-building now uses `q6_index_build_cb` and lazy `q6_index`.

// Build index callback: iterate reservations and populate q6_index
static void q6_index_build_cb(gpointer key, gpointer value, gpointer user_data) {
    (void)key;
    Reservas* reserva = (Reservas*)value;
    Program* program = (Program*)user_data;

    PassageirosCatalog* pc = getPassageirosCatalog(program);
    VoosCatalog* vc = getVoosCatalog(program);

    const char* doc_str = getReservationDocumentNumber_internal(reserva);
    if (!doc_str) return;
    int doc = atoi(doc_str);

    Passageiros* p = buscarPassageiro(pc, doc);
    if (!p) return;

    char* nat = getNationality(p);
    if (!nat) return;
    char* norm_nat = normalize_copy(nat);
    free(nat);
    if (!norm_nat || norm_nat[0] == '\0') { if (norm_nat) free(norm_nat); return; }

    const char* flight_ids = getFlightIds_internal(reserva);
    if (!flight_ids || flight_ids[0] == '\0') { free(norm_nat); return; }

    // parse flight ids and count each non-cancelled flight destination (each arrival)
    const char *ptr = flight_ids;
    char flight_buf[64];
    int counted_any = 0;

    while (*ptr) {
        while (*ptr && (*ptr == '[' || *ptr == ']' || *ptr == ',' || *ptr == ';' || *ptr == '\'' || *ptr == '"' || *ptr == ' ')) ptr++;
        if (!*ptr) break;
        int i = 0;
        while (*ptr && *ptr != '[' && *ptr != ']' && *ptr != ',' && *ptr != ';' && *ptr != '\'' && *ptr != '"' && *ptr != ' ' && i < (int)sizeof(flight_buf)-1) {
            flight_buf[i++] = *ptr++;
        }
        flight_buf[i] = '\0';
        if (i == 0) continue;

        Voos* v = buscarVoo(vc, flight_buf);
        if (!v) continue;
        const char* status_internal = getStatus_internal(v);
        if (status_internal && strcmp(status_internal, "Cancelled") == 0) continue;
        const char *dest_internal = getDestination_internal(v);
        if (!dest_internal || dest_internal[0] == '\0') continue;

        char* norm_dest = normalize_copy(dest_internal);
        if (!norm_dest || norm_dest[0] == '\0') { if (norm_dest) free(norm_dest); continue; }

        // insert into q6_index: nationality -> (airport -> count)
        GHashTable* inner = g_hash_table_lookup(q6_index, norm_nat);
        if (!inner) {
            inner = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
            char* nat_key = g_strdup(norm_nat);
            g_hash_table_insert(q6_index, nat_key, inner);
        }

        int* cntp = (int*)g_hash_table_lookup(inner, norm_dest);
        if (!cntp) {
            cntp = g_new(int, 1);
            *cntp = 1;
            char* dest_key = g_strdup(norm_dest);
            g_hash_table_insert(inner, dest_key, cntp);
        } else {
            (*cntp)++;
        }

        free(norm_dest);
        counted_any = 1;
    }

    free(norm_nat);
    if (!counted_any) return;
}

void listarDestinoMaisComumPorNacionalidade(Program* program, char* arguments, int command_number, int s_existe) {
    if (!program || !arguments) { save_output(command_number, ""); return; }

    PassageirosCatalog* pc = getPassageirosCatalog(program);
    VoosCatalog* vc = getVoosCatalog(program);
    ReservasCatalog* rc = getReservasCatalog(program);
    if (!pc || !vc || !rc) { save_output(command_number, ""); return; }

    // trim and normalize argument (remove surrounding quotes and spaces)
    char* arg = strdup(arguments);
    char* st = arg; while (*st && isspace((unsigned char)*st)) st++;
    char* en = st + strlen(st) - 1; while (en >= st && isspace((unsigned char)*en)) { *en = '\0'; en--; }
    if (*st == '\0') { free(arg); save_output(command_number, ""); return; }

    // normalize argument into a newly allocated buffer (trim and remove surrounding quotes)
    char* normalized_arg = NULL;
    {
        const char* s = st;
        // remove surrounding quotes if present
        size_t len = strlen(s);
        size_t start = 0, end = len;
        while (start < len && isspace((unsigned char)s[start])) start++;
        while (end > start && isspace((unsigned char)s[end-1])) end--;
        if (end - start >= 2 && ((s[start] == '"' && s[end-1] == '"') || (s[start] == '\'' && s[end-1] == '\''))) {
            start++;
            end--;
        }
        size_t newlen = (end > start) ? (end - start) : 0;
        normalized_arg = (char*)malloc(newlen + 1);
        if (newlen > 0) memcpy(normalized_arg, s + start, newlen);
        normalized_arg[newlen] = '\0';
    }

        // initialize cache if needed
        if (!q6_cache) q6_cache = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);

        // check cache: key = normalized_arg + '|' + mode
        char cache_key_buf[512];
        snprintf(cache_key_buf, sizeof(cache_key_buf), "%s|%c", normalized_arg, s_existe ? 'S' : 'N');
        char *cached = g_hash_table_lookup(q6_cache, cache_key_buf);
        if (cached) {
            // return cached output
            save_output(command_number, cached);
            free(arg);
            if (normalized_arg) free(normalized_arg);
            return;
        }

    // Build lazy index on first use
    if (!q6_index) {
        q6_index = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, destroy_inner_table);
        reservas_foreach(rc, q6_index_build_cb, program);
    }

    // lookup nationality in precomputed index
    GHashTable* inner = g_hash_table_lookup(q6_index, normalized_arg);
    // if no entry, inner will be NULL and result is empty
    GHashTable* airport_to_set = NULL;
    if (inner) airport_to_set = inner;
    else airport_to_set = NULL;

    // find best (max count, tie-break: smallest lexicographic IATA)
    GHashTableIter iter;
    gpointer key, val;
    char* best_code = NULL;
    int best_count = 0;
    if (airport_to_set) {
        g_hash_table_iter_init(&iter, airport_to_set);
        while (g_hash_table_iter_next(&iter, &key, &val)) {
            char* iata = (char*)key;
            int cnt = val ? *((int*)val) : 0;
            if (cnt > best_count || (cnt == best_count && (best_code == NULL || strcmp(iata, best_code) < 0))) {
                if (best_code) free(best_code);
                best_code = strdup(iata);
                best_count = cnt;
            }
        }
    }

    char* output = NULL;
    if (!best_code) {
        output = g_strdup("\n");
    } else {
        if (!s_existe)
            output = g_strdup_printf("%s;%d\n", best_code, best_count);
        else{
            output = g_strdup_printf("%s=%d\n", best_code, best_count);
    }}

    save_output(command_number, output);

    // store in cache (duplicate key and output)
    char *store_key = g_strdup(cache_key_buf);
    char *store_val = g_strdup(output);
    g_hash_table_insert(q6_cache, store_key, store_val);

    if (best_code) free(best_code);
    g_free(output);
    free(arg);
    if (normalized_arg) free(normalized_arg);
}
