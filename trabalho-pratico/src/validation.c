#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include "validation.h"
#include "aeronavesCatalog.h"
#include "voosCatalog.h"
#include "passageirosCatalog.h"


// Optional catalogs registered for cross-entity validation

VoosCatalog* g_voos_catalog_for_validation = NULL;
void setVoosCatalogForValidation(VoosCatalog* catalog) {
    g_voos_catalog_for_validation = catalog;
}

PassageirosCatalog* g_passageiros_catalog_for_validation = NULL;
void setPassageirosCatalogForValidation(PassageirosCatalog* catalog) {
    g_passageiros_catalog_for_validation = catalog;
}

AeronavesCatalog* g_aeronaves_catalog_for_validation = NULL;
void setAeronavesCatalogForValidation(AeronavesCatalog* catalog) {
    g_aeronaves_catalog_for_validation = catalog;
}

AeroportosCatalog* g_aeroportos_catalog_for_validation = NULL;
void setAeroportosCatalogForValidation(AeroportosCatalog* catalog) {
    g_aeroportos_catalog_for_validation = catalog;
}

int validate_voo(char **fields, int field_count) {
    if (field_count < 12) return 0;
    if (!validate_flight_id(fields[0])) return 0;
    if (!validate_datetime(fields[1] )) return 0;
    if (!validate_datetime(fields[2] )) return 0;
    if (!validate_datetime(fields[3] )) return 0;
    if (!validate_datetime(fields[4] )) return 0;
    if (strcmp(fields[7], fields[8]) == 0) return 0; // origem não pode ser igual ao destino
    if (strcmp(fields[6], "On Time") != 0 && strcmp(fields[6], "Delayed") != 0 && strcmp(fields[6], "Cancelled") != 0) {
        return 0;
    }

    // logica de voo:

    if (strcmp(fields[1], "N/A") != 0 && strcmp(fields[2], "N/A") != 0) {
        if (strcmp(fields[1], fields[2]) > 0) return 0;
    }
    // prevista < actual aterrar
    if (strcmp(fields[3], "N/A") != 0 && strcmp(fields[4], "N/A") != 0) {
        if (strcmp(fields[3], fields[4]) > 0) return 0;
    }
    // prevista aterrar >= prevista descolar
    if (strcmp(fields[1], "N/A") != 0 && strcmp(fields[3], "N/A") != 0) {
        if (strcmp(fields[3], fields[1]) < 0) return 0;
    }
    // actual aterrar >= actual descolar
    if (strcmp(fields[2], "N/A") != 0 && strcmp(fields[4], "N/A") != 0) {
        if (strcmp(fields[4], fields[2]) < 0) return 0;
    }

    if (fields[9] == NULL || strlen(fields[9]) == 0) return 0; // campo vazio
    if (g_aeronaves_catalog_for_validation != NULL) {
        if (buscarAeronave(g_aeronaves_catalog_for_validation, fields[9]) == NULL)
            return 0; // aeronave não existe, inválido
    }

    // verificar se os aeroportos de origem/destino existem no catálogo
    if (g_aeroportos_catalog_for_validation != NULL) {
        if (fields[7] == NULL || fields[8] == NULL) return 0;
        if (buscarAeroporto(g_aeroportos_catalog_for_validation, fields[7]) == NULL) return 0;
        if (buscarAeroporto(g_aeroportos_catalog_for_validation, fields[8]) == NULL) return 0;
    }

    // gate deve ser inteiro positivo
    for (const char *p = fields[5]; *p; ++p) {
        if (*p < '0' || *p > '9') return 0;
    }

    if (strcmp(fields[6], "Cancelled") == 0) {
        if (!(strcmp(fields[2], "N/A") == 0 && strcmp(fields[4], "N/A") == 0)) {
            return 0;
        }
    }

    return 1; // Válido
}

int validate_airports(char **fields, int field_count) {
    //if (field_count < 8 ) return 0;
    // Validate IATA + ICAO codes and coordinates using helper functions
    if (!validate_airport_codes(fields[0], fields[6])) return 0;
    if (!validate_coordinates_pair(fields[4], fields[5])) return 0;
    if (!validate_airport_type(fields[7])) return 0;
    return 1; // Válido
}




int validate_aircrafts(char **fields, int field_count) {
    if (field_count < 6 ) return 0;
    if (!fields[3]) return 0;       
    if (strlen(fields[3]) != 4) return 0;
    for (int i = 0; i < 4; i++) if (fields[3][i] < '0' || fields[3][i] > '9') return 0;
    int year = atoi(fields[3]);
    if (year > 2025) return 0;
    return 1; // Válido
}

int validate_passengers(char **fields, int field_count){
    if (field_count < 10) return 0;
    if (!validate_email(fields[6])) return 0;
    if (!validate_document_number(fields[0])) return 0;
    if (!validate_gender(fields[5])) return 0;
    if (!validate_date_dob(fields[3])) return 0;

    return 1; // Válido
}

int validate_reservation(char **fields, int field_count){
    if (field_count < 8) return 0;
    if (!validate_reservation_id(fields[0])) return 0;
    if (!validate_flight_id_list(fields[1])) return 0;
    if (!validate_document_number(fields[2])) return 0;
    // A validação de formato, existência no catálogo e conexão cronológica
    // dos voos (1 ou 2) é feita por `validate_flight_id_list`.
    // verificar que o document number corresponde a um passageiro existente
    if (g_passageiros_catalog_for_validation != NULL) {
        int doc = atoi(fields[2]);
        if (buscarPassageiro(g_passageiros_catalog_for_validation, doc) == NULL)
            return 0; // passageiro não existe
    }
    

    return 1; // Válido
}

int validate_list_format(const char *list){
    if (list == NULL) return 0;
    size_t len = strlen(list);
    if (len > 1 && list[0] == '[' && list[len - 1] == ']'){
        return 1;
    }
    return 0;
}

int validate_flight_id_list(const char *list) {
    if (list == NULL) return 0;
    size_t len = strlen(list);
    // must start with '[' and end with ']'
    if (len < 3 || list[0] != '[' || list[len-1] != ']') return 0;
    // Flexible validation:
    // - allow 1 or 2 items separated by a comma
    // - each item may be surrounded by single quotes and whitespace
    // - each item must pass validate_flight_id (which accepts length 7 or 8)

    // copy inner content into a mutable buffer
    char buffer[1024];
    size_t inner_len = len - 2; // without '[' and ']'
    if (inner_len >= sizeof(buffer)) return 0;
    strncpy(buffer, list + 1, inner_len);
    buffer[inner_len] = '\0';

    int count = 0;
    Voos* found_voos[2] = {NULL, NULL};
    char *token = strtok(buffer, ",");
    while (token) {
        // trim leading spaces
        while (isspace((unsigned char)*token)) token++;
        // trim trailing spaces
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) *end-- = '\0';

        // remove surrounding single quotes if present
        if (token[0] == '\'' && *end == '\'' && end > token) {
            token++;
            *end = '\0';
        }

        if (!validate_flight_id(token)) return 0;

        // if a voos catalog is registered, check existence and keep pointer
        extern VoosCatalog* g_voos_catalog_for_validation;
        if (g_voos_catalog_for_validation != NULL) {
            Voos* v = buscarVoo(g_voos_catalog_for_validation, token);
            if (v == NULL) return 0; // not found
            if (count < 2) found_voos[count] = v;
        }

        count++;
        if (count > 2) return 0;
        token = strtok(NULL, ",");
    }

    if (count < 1 || count > 2) return 0;

    // if two flights and catalog available, ensure chronological connection
    extern VoosCatalog* g_voos_catalog_for_validation;
    if (count == 2 && g_voos_catalog_for_validation != NULL) {
        time_t dep1 = getDepartureTs(found_voos[0]);
        time_t dep2 = getDepartureTs(found_voos[1]);
        int ok = 0;
        if (dep1 <= dep2) {
            char *dest1 = getDestination(found_voos[0]);
            char *orig2 = getOrigin(found_voos[1]);
            if (dest1 != NULL && orig2 != NULL) {
                if (strcmp(dest1, orig2) == 0) ok = 1;
            }
            if (dest1) free(dest1);
            if (orig2) free(orig2);
        } else {
            char *dest2 = getDestination(found_voos[1]);
            char *orig1 = getOrigin(found_voos[0]);
            if (dest2 != NULL && orig1 != NULL) {
                if (strcmp(dest2, orig1) == 0) ok = 1;
            }
            if (dest2) free(dest2);
            if (orig1) free(orig1);
        }
        if (!ok) return 0;
    }

    return 1;
}

// Função auxiliar para converter "YYYY-MM-DD HH:MM" em time_t
// Implementação otimizada: evita sscanf/mktime (muito caros) e faz parse manual.
// Nota: isto calcula um timestamp em segundos relativo a 1970-01-01 00:00 (UTC-like).
// Para comparações e diffs internas do projeto, mantém ordem e diferenças consistentes.
static inline int parse_2digits(const char *p) {
    return (p[0] - '0') * 10 + (p[1] - '0');
}

static inline int parse_4digits(const char *p) {
    return (p[0] - '0') * 1000 + (p[1] - '0') * 100 + (p[2] - '0') * 10 + (p[3] - '0');
}

// days since 1970-01-01, algorithm by Howard Hinnant (public domain style)
static inline long long days_from_civil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return (long long)era * 146097LL + (long long)doe - 719468LL;
}

time_t datetime_to_time(const char *datetime) {
    if (datetime == NULL) return 0;
    if (strcmp(datetime, "N/A") == 0) return 0;

    // expected format: YYYY-MM-DD HH:MM (len 16)
    if (strlen(datetime) < 16) return 0;
    if (datetime[4] != '-' || datetime[7] != '-' || datetime[10] != ' ' || datetime[13] != ':') return 0;

    // digits validation (avoid undefined behavior if input is malformed)
    for (int i = 0; i < 16; i++) {
        if (i == 4 || i == 7 || i == 10 || i == 13) continue;
        if (datetime[i] < '0' || datetime[i] > '9') return 0;
    }

    int year = parse_4digits(datetime);
    unsigned month = (unsigned)parse_2digits(datetime + 5);
    unsigned day = (unsigned)parse_2digits(datetime + 8);
    unsigned hour = (unsigned)parse_2digits(datetime + 11);
    unsigned minute = (unsigned)parse_2digits(datetime + 14);

    long long days = days_from_civil(year, month, day);
    long long secs = days * 86400LL + (long long)hour * 3600LL + (long long)minute * 60LL;
    return (time_t)secs;
}



int validate_date(const char *date){

    if (strcmp(date,"N/A") == 0) return 1;

    if (strlen(date) != 16 || date[4] != '-' || date[7] != '-' || date[10] != ' ' || date[13] != ':') {
        return 0;  
    }
    
    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);
    int hour = atoi(date + 11);
    int minute = atoi(date + 14);
    
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;

    if (year>2025) return 0;
    if (year==2025 && month>9) return 0;
    if (year==2025 && month==9 && day>30) return 0;

    if (hour < 0 || hour > 23) return 0;
    if (minute < 0 || minute > 59) return 0;

    return 1;
}

// Backwards-compatible name: validate_datetime delegates to validate_date
int validate_datetime(const char *datetime) {
    return validate_date(datetime);
}

int validate_document_number(const char *doc_num) {
    //docnumber
    if (strlen(doc_num) != 9) return 0;
    
    for(int i=0; i<9; i++){
        if (doc_num[i] < '0' || doc_num[i] > '9'){
            return 0;
        }
    }
    return 1;
}

int validate_flight_id(const char *flight_id) {
    if (flight_id == NULL) return 0;
    // Accept either 7 or 8 characters: two letters followed by 5 or 6 digits
    size_t len = strlen(flight_id);
    if (len != 7 && len != 8) return 0;

    // First two must be uppercase letters
    for (int i = 0; i < 2; i++) {
        if (!isalpha((unsigned char)flight_id[i]) || !isupper((unsigned char)flight_id[i]))
            return 0;
    }

    // Remaining characters must all be digits
    for (size_t i = 2; i < len; i++) {
        if (!isdigit((unsigned char)flight_id[i])) return 0;
    }

    return 1; // válido
}

// Validate both latitude and longitude strings together using the rules:
// - both must contain a '.'
// - at most 8 decimal digits after the '.'
// - only digits, one optional leading '-' and a single '.' are allowed
// - numeric value must be inside [-90,90] for latitude and [-180,180] for longitude
int validate_coordinates_pair(const char *lat_str, const char *lon_str) {
    if (lat_str == NULL || lon_str == NULL) return 0;

    double lat = atof(lat_str);
    double lon = atof(lon_str);
    if (lat < -90.0 || lat > 90.0) return 0;
    if (lon < -180.0 || lon > 180.0) return 0;

    const char *dot = strchr(lat_str, '.');
    if (!dot) return 0;
    if (strlen(dot + 1) > 8) return 0;

    const char *dot2 = strchr(lon_str, '.');
    if (!dot2) return 0;
    if (strlen(dot2 + 1) > 8) return 0;

    // validate characters in lat
    for (int i = 0; lat_str[i]; i++){
        char c = lat_str[i];
        if (!((c >= '0' && c <= '9') || c == '.' || (c == '-' && i == 0))) return 0;
    }

    // validate characters in lon
    for (int i = 0; lon_str[i]; i++){
        char c = lon_str[i];
        if (!((c >= '0' && c <= '9') || c == '.' || (c == '-' && i == 0))) return 0;
    }

    return 1;
}

// Validate airport codes: IATA (3 uppercase letters) and ICAO (4 uppercase letters)
int validate_airport_codes(const char *iata, const char *icao) {
    if (iata == NULL || icao == NULL) return 0;

    if (strlen(iata) != 3) return 0;
    for (int i = 0; i < 3; i++) {
        if (iata[i] < 'A' || iata[i] > 'Z') return 0;
    }

    if (strlen(icao) != 4) return 0;
    for (int i = 0; i < 4; i++) {
        if (icao[i] < 'A' || icao[i] > 'Z') return 0;
    }

    return 1;
}

int validate_aircraft_year(const char *year_str) {
    if (year_str == NULL) return 0;

    int len = strlen(year_str);

    // Tem de ter exatamente 4 dígitos
    if (len != 4) return 0;

    // Verifica que é tudo dígitos e não há espaços
    for (int i = 0; i < len; i++) {
        if (!isdigit((unsigned char)year_str[i])) return 0;
    }

    int year = atoi(year_str);

    if (year > 2025) return 0;

    return 1;
}


int validate_airport_type(const char *type) {

    if (type == NULL) return 0;

    return
        strcmp(type, "small_airport")   == 0 ||
        strcmp(type, "medium_airport")  == 0 ||
        strcmp(type, "large_airport")   == 0 ||
        strcmp(type, "heliport")        == 0 ||
        strcmp(type, "seaplane_base")   == 0;

}

int validate_email(const char *email) {
    if (email == NULL) return 0;
    const char *at = strchr(email, '@');
    if (!at) return 0;  

    const char *dot = strchr(at + 1, '.');
    if (!dot || dot == at + 1) return 0; 

    
    if (strlen(dot + 1) < 2 || strlen(dot + 1) > 3) return 0;

    for (const char *p = email; p < at; p++) {
        if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9') || *p == '.' )){
            return 0;
        }
    }

    for (const char *p = at + 1; p < dot; p++) {
        if (!((*p >= 'a' && *p <= 'z') || (*p >= '0' && *p <= '9'))){
            return 0;
        }
    }

    return 1;
}


int validate_reservation_id(const char *res_id) {
    if (res_id == NULL) return 0;
    // Verifica comprimento total
    if (strlen(res_id) != 10)
        return 0;

    // Verifica se começa com 'R'
    if (res_id[0] != 'R')
        return 0;

    // Verifica os 9 restantes caracteres (devem ser dígitos)
    for (int i = 1; i < 10; i++) {
        if (!isdigit((unsigned char)res_id[i]))
            return 0;
    }

    return 1; // válido
}

int validate_gender(const char *gender) {
    // Verifica se o campo existe e tem exatamente 1 carácter
    if (gender == NULL || strlen(gender) != 1)
        return 0;

    // Verifica se o carácter é M, F ou O
    char g = gender[0];
    if (g == 'M' || g == 'F' || g == 'O')
        return 1;

    return 0; // inválido
}

// Valida datas apenas com ano-mês-dia: "YYYY-MM-DD
int validate_date_dob(const char *date) {
    if (!date) return 0;
    if (strcmp(date, "N/A") == 0) return 0;
    // formato esperado: 10 chars: YYYY-MM-DD
    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') return 0;
    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    if (year > 2025) return 0;
    if (year == 2025 && month > 9) return 0;
    if (year == 2025 && month == 9 && day > 30) return 0;
    return 1;
}
