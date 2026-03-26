#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <string.h>
#include <time.h>
#include "main.h"
#include "parserGenerico.h"
#include "validation.h"
#include "aeroportosCatalog.h"
#include "voosCatalog.h"
#include "aeronavesCatalog.h"
#include "passageirosCatalog.h"
#include "reservasCatalog.h"
#include "passageiros.h"
#include "querys.h"
 
 
struct program {
    PassageirosCatalog *passageirosCatalog;
    AeronavesCatalog *aeronavesCatalog;
    AeroportosCatalog *aeroportosCatalog;
    VoosCatalog *voosCatalog;
    ReservasCatalog *reservasCatalog;
};


int main(int argc,char** argv)
{
    // Verifica se o número de argumentos é válido
    if (argc != 3)
    {
        printf("Numero de argumentos inválido. Recebe <caminho-dataset> <ficheiro-input>\n");
        return 1;
    }

    // Cria pasta de output caso não exista
    g_mkdir_with_parents(OUTPUT_DIR_NAME, 0700);

    Program *program = initialize_program(argv);

    run_program(program, argv);

    free_program(program);

    return 0;
}

Program* initialize_program(char **arguments) {
    
    Program *program = (Program *)malloc(sizeof(Program));
    if (!program) return NULL;

    // Inicializa campos a NULL para limpeza segura em caso de erro
    program->passageirosCatalog = NULL;
    program->aeronavesCatalog = NULL;
    program->aeroportosCatalog = NULL;
    program->voosCatalog = NULL;
    program->reservasCatalog = NULL;

    // leitura e inserção dos dados das aeronaves no sistema
    char aircrafts_file_path[512] = "";
    snprintf(aircrafts_file_path, sizeof(aircrafts_file_path), "%s/aircrafts.csv", arguments[1]);
    
    FILE* aircrafts_file = fopen(aircrafts_file_path, "r");

    if (aircrafts_file ==  NULL)
    {
        perror("Erro ao abrir ficheiro.");
        free_program(program);
        return NULL;
    }

    AeronavesCatalog* aeronavesCatalog = criarAeronavesCatalog();
    if (!aeronavesCatalog) {
        fclose(aircrafts_file);
        free_program(program);
        return NULL;
    }
    program->aeronavesCatalog = aeronavesCatalog;

    const char *aircrafts_file_name = strrchr(aircrafts_file_path, '/');
    if (aircrafts_file_name != NULL)
        aircrafts_file_name = aircrafts_file_name + 1;
    else
        aircrafts_file_name = aircrafts_file_path;

    parse_csv_and_store_data(aircrafts_file, criarAeronave, aeronavesCatalog, aircrafts_file_name);
    
    //passar o catálogo para validação
    setAeronavesCatalogForValidation(aeronavesCatalog);
    fclose(aircrafts_file);

    // leitura e inserção dos dados dos aeroportos no sistema
    char airports_file_path[512] = "";
    snprintf(airports_file_path, sizeof(airports_file_path), "%s/airports.csv", arguments[1]);
    FILE* airports_file = fopen(airports_file_path, "r");

    if (airports_file ==  NULL)
    {
        perror("Erro ao abrir ficheiro.");
        free_program(program);
        return NULL;
    }

    AeroportosCatalog* aeroportosCatalog = criarAeroportosCatalog();
    if (!aeroportosCatalog) {
        fclose(airports_file);
        free_program(program);
        return NULL;
    }
    program->aeroportosCatalog = aeroportosCatalog;

    const char *airports_file_name = strrchr(airports_file_path, '/');
    if (airports_file_name != NULL)
        airports_file_name = airports_file_name + 1;
    else
        airports_file_name = airports_file_path;

    parse_csv_and_store_data(airports_file, criarAeroporto, aeroportosCatalog, airports_file_name);
    // tornar o catálogo de aeroportos disponível à validação
    setAeroportosCatalogForValidation(aeroportosCatalog);
    fclose(airports_file);

    // Leitura e inserção dos dados dos voos no sistema
    char flights_file_path[512] = "";
    snprintf(flights_file_path, sizeof(flights_file_path), "%s/flights.csv", arguments[1]);
    FILE* flights_file = fopen(flights_file_path, "r");

    if (flights_file ==  NULL)
    {
        perror("Erro ao abrir ficheiro.");
        free_program(program);
        return NULL;
    }

    VoosCatalog* voosCatalog = criarVoosCatalog();
    if (!voosCatalog) {
        fclose(flights_file);
        free_program(program);
        return NULL;
    }
    program->voosCatalog = voosCatalog;

    setAeronavesCatalog(program->voosCatalog, program->aeronavesCatalog); // ligar catálogos
    setAeroportosCatalog(program->voosCatalog, program->aeroportosCatalog); // ligar aeroportos para contadores de partidas

    const char *flights_file_name = strrchr(flights_file_path, '/');
    if (flights_file_name != NULL)
        flights_file_name = flights_file_name + 1;
    else
        flights_file_name = flights_file_path;

    parse_csv_and_store_data(flights_file, criarVoo, voosCatalog, flights_file_name);
    setVoosCatalogForValidation(voosCatalog);
    fclose(flights_file);
    arrayAeronaves(program->aeronavesCatalog); // cria o GArray ordenado
    construirAirlineAtrasoArray(program->voosCatalog); // construir array para Q5

    // leitura e inserção dos dados dos passageiros no sistema
    char passengers_file_path[512] = "";
    snprintf(passengers_file_path, sizeof(passengers_file_path), "%s/passengers.csv", arguments[1]);
    FILE* passengers_file = fopen(passengers_file_path, "r");   
    if (passengers_file ==  NULL)
    {
        perror("Erro ao abrir ficheiro.");
        free_program(program);
        return NULL;
    }

    PassageirosCatalog* passageirosCatalog = criarPassageirosCatalog();
    if (!passageirosCatalog) {
        fclose(passengers_file);
        free_program(program);
        return NULL;
    }
    program->passageirosCatalog = passageirosCatalog;
    const char *passengers_file_name = strrchr(passengers_file_path, '/');
    if (passengers_file_name != NULL)
        passengers_file_name = passengers_file_name + 1;
    else
        passengers_file_name = passengers_file_path;

    parse_csv_and_store_data(passengers_file, criarPassageiros, passageirosCatalog, passengers_file_name);
    setPassageirosCatalogForValidation(passageirosCatalog);
    fclose(passengers_file);

    // leitura e inserção dos dados das reservas no sistema
    char reservations_file_path[512] = "";
    snprintf(reservations_file_path, sizeof(reservations_file_path), "%s/reservations.csv", arguments[1]);
    FILE* reservations_file = fopen(reservations_file_path, "r");
    if (reservations_file ==  NULL)
    {
        perror("Erro ao abrir ficheiro.");
        free_program(program);
        return NULL;
    }
    ReservasCatalog* reservasCatalog = criarReservasCatalog();
    if (!reservasCatalog) {
        fclose(reservations_file);
        free_program(program);
        return NULL;
    }
    program->reservasCatalog = reservasCatalog;
    const char *reservations_file_name = strrchr(reservations_file_path, '/');
    if (reservations_file_name != NULL)
        reservations_file_name = reservations_file_name + 1;
    else
        reservations_file_name = reservations_file_path;
    
    parse_csv_and_store_data(reservations_file, criarReserva, reservasCatalog, reservations_file_name);
    fclose(reservations_file);

    /* Atualizar contadores de passageiros nos aeroportos baseado nas reservas */
    atualizarContagemPassageiros(program->reservasCatalog, program->voosCatalog, program->aeroportosCatalog);
    
    /* Atualizar gastos por semana em cada passageiro - Query 4 only - commented out */
    // atualizarGastoPassageiros(program->passageirosCatalog, program->reservasCatalog, program->voosCatalog);

    return program;
}

void run_program(Program *program, char **arguments) {

    // Leitura do ficheiro de queries
    FILE *input_file = fopen(arguments[2], "r");

    if (input_file == NULL)
    {
        perror("Erro ao abrir ficheiro.");
        return;
    }


    char *line_buffer;
    size_t buffer_size = BUFFER_SIZE;
    int read = 0;

    line_buffer = (char *)malloc(buffer_size * sizeof(char));
    if (!line_buffer) {
        fclose(input_file);
        return;
    }

    int command_number = 0;
    /* Optional per-query timing: if environment variable MEASURE_PER_QUERY is set,
       create a timings file under resultados/query_times.txt and log per-command durations. */
    char *measure_env = getenv("MEASURE_PER_QUERY");
    FILE *timings_fp = NULL;
    struct timespec tq0, tq1;
    if (measure_env) {
        char timings_path[512];
        snprintf(timings_path, sizeof(timings_path), "%s/query_times.txt", OUTPUT_DIR_NAME);
        timings_fp = fopen(timings_path, "w");
        if (timings_fp) fprintf(timings_fp, "#command_number query_number seconds\n");
    }

    // Para cada comando
    while ((read = getline(&line_buffer, &buffer_size, input_file)) != -1) 
   {
        // Ignora newline
        line_buffer[read - 1] = '\0';

        // Obter número da query
        int query_number = line_buffer[0] - '0';

        // Verificar se há o "S"
        int s_existe = (line_buffer[1] == 'S');

        // Apontar para os args 
        char* args = line_buffer + (s_existe ? 3 : 2);  
        
        if (timings_fp) clock_gettime(CLOCK_MONOTONIC, &tq0);

        run_query(program, query_number, args, command_number, s_existe); 

        if (timings_fp) {
            clock_gettime(CLOCK_MONOTONIC, &tq1);
            double secs = (tq1.tv_sec - tq0.tv_sec) + (tq1.tv_nsec - tq0.tv_nsec) / 1e9;
            fprintf(timings_fp, "%d %d %.9f\n", command_number, query_number, secs);
            fflush(timings_fp);
        }

        command_number++;
    }

    free(line_buffer);
    fclose(input_file);
    
}

void free_program(Program *program) {
    if(!program) return;

    // Destruir catálogos (verifica nomes reais das funçoes)
    if(program->aeroportosCatalog){
        destruirAeroportosCatalog(program->aeroportosCatalog);
        program->aeroportosCatalog = NULL;
    }
    if(program->aeronavesCatalog){
        destruirAeronavesCatalog(program->aeronavesCatalog);
         program->aeronavesCatalog = NULL;
    }
    if(program->voosCatalog){
        destruirVoosCatalog(program->voosCatalog);
        program->voosCatalog = NULL;
    }
    if(program->passageirosCatalog){
        destruirPassageirosCatalog(program->passageirosCatalog);
        program->passageirosCatalog = NULL;
    }
    if(program->reservasCatalog){
        destruirReservasCatalog(program->reservasCatalog);
        program->reservasCatalog = NULL;
    }

    // cleanup query caches
    q6_cleanup();

    free(program);
}


PassageirosCatalog* getPassageirosCatalog(Program* program) {
    return program->passageirosCatalog;
}
AeronavesCatalog* getAeronavesCatalog(Program* program) {
    return program->aeronavesCatalog;
}
AeroportosCatalog* getAeroportosCatalog(Program* program) {
    return program->aeroportosCatalog;
}
VoosCatalog* getVoosCatalog(Program* program) {
    return program->voosCatalog;
}
ReservasCatalog* getReservasCatalog(Program* program) {
    return program->reservasCatalog;
}

