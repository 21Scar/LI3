#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parserGenerico.h"

#define BUFFER_SIZE 200
#define DELIMITER ","

static FILE *g_error_file_handle = NULL;
static char *g_error_file_path = NULL;
static char *g_error_csv_name = NULL;

static void close_error_log_file(void) {
    if (g_error_file_handle) {
        fflush(g_error_file_handle);
        fclose(g_error_file_handle);
        g_error_file_handle = NULL;
    }
    free(g_error_file_path);
    g_error_file_path = NULL;
    free(g_error_csv_name);
    g_error_csv_name = NULL;
}

static FILE *get_error_log_file(const char *csv_name) {
    if (csv_name == NULL) return NULL;

    if (g_error_csv_name != NULL && strcmp(g_error_csv_name, csv_name) == 0 && g_error_file_handle != NULL) {
        return g_error_file_handle;
    }

    close_error_log_file();

    const char *output_dir = "resultados/";
    size_t error_file_size = strlen(output_dir) + strlen(csv_name) + 20;  // enough for "_errors.csv" + \0
    char *error_file = (char *)malloc(error_file_size);
    if (!error_file) return NULL;
    snprintf(error_file, error_file_size, "%s%s", output_dir, csv_name);

    char *dot_position = strrchr(error_file, '.');
    if (dot_position != NULL && strcmp(dot_position, ".csv") == 0){
        snprintf(dot_position, error_file_size - (dot_position - error_file), "_errors.csv");
    } else {
        snprintf(error_file + strlen(error_file), error_file_size - strlen(error_file), "_errors.csv");
    }

    FILE *file = fopen(error_file, "a");
    if (!file) {
        free(error_file);
        return NULL;
    }
    // Buffer writes heavily to avoid per-line I/O overhead.
    setvbuf(file, NULL, _IOFBF, 1 << 20);

    g_error_file_handle = file;
    g_error_file_path = error_file;
    g_error_csv_name = strdup(csv_name);

    static int atexit_registered = 0;
    if (!atexit_registered) {
        atexit(close_error_log_file);
        atexit_registered = 1;
    }

    return g_error_file_handle;
}

int parse_csv_and_store_data(FILE* csv, StoreData* store_data, void* data_catalog, const char *csv_name)
{
    char *line_buffer = NULL;
    size_t buffer_size = 0;
    ssize_t read = 0;
    int header_written = 0;

    // Lê a primeira linha (cabeçalho)
    read = getline(&line_buffer, &buffer_size, csv);
    if (read != -1) {
        if (read > 0 && line_buffer[read - 1] == '\n') line_buffer[read - 1] = '\0';
        log_invalid_entry(NULL, 0, csv_name, line_buffer, &header_written, NULL);  // Escreve o cabeçalho no arquivo de erros
    }

    // Lê as linhas subsequentes
    while ((read = getline(&line_buffer, &buffer_size, csv)) != -1) {
        if (read > 0 && line_buffer[read - 1] == '\n') line_buffer[read - 1] = '\0';

        char **fields = NULL;
        int field_count = 0;

        size_t len = strlen(line_buffer);
        char *cur = (char *)malloc(len + 1);
        size_t cur_len = 0;
        int in_quotes = 0;

        for (size_t i = 0; i <= len; i++) {
            char c = line_buffer[i];

            if (c == '"') {
                // handle escaped quotes ""
                if (in_quotes && line_buffer[i + 1] == '"') {
                    cur[cur_len++] = '"';
                    i++; // skip next quote
                    continue;
                }
                if (!in_quotes && cur_len == 0) {
                    // starting a quoted field
                }
                in_quotes = !in_quotes;
                continue;
            }

            if (!in_quotes && (c == ',' || c == '\0')) {
                cur[cur_len] = '\0';

                // Preserve the exact content of the field (no trimming).
                fields = (char **)realloc(fields, (field_count + 1) * sizeof(char *));
                fields[field_count] = strdup(cur);
                field_count++;

                cur_len = 0; // reset buffer
                continue;
            }

            // normal character
            cur[cur_len++] = c;
        }

        // If the line ended while still in quotes (malformed CSV), salvage the last field
        if (in_quotes) {
            // ensure cur is null-terminated and preserve content as-is
            cur[cur_len] = '\0';
            fields = (char **)realloc(fields, (field_count + 1) * sizeof(char *));
            fields[field_count] = strdup(cur);
            field_count++;
        }

        free(cur);

        int create_result = store_data(fields, data_catalog);

        if (create_result != 0) {
            // write the original raw line to the errors file so we preserve exact formatting
            log_invalid_entry(NULL, 0, csv_name, NULL, &header_written, line_buffer);
        }

        for (int i = 0; i < field_count; i++) free(fields[i]);
        free(fields);
    }

    free(line_buffer);
    return 0;
}



void write_field_with_quotes(FILE *file, const char *field){
    fprintf(file, "\"%s\"", field);
}



char* remove_quotes(char* field){
    if (field == NULL) return NULL;
    size_t len = strlen(field);

    if (len >= 2 && field[0] == '"' && field[len - 1] == '"') {
        field[len - 1] = '\0';
        return field + 1;
    }
    return field;
}

void log_invalid_entry(char **fields, int field_count, const char *csv_name, const char *header, int *header_written, const char *raw_line){
    FILE *file = get_error_log_file(csv_name);
    if (!file) return;

    if (!*header_written && header != NULL){
        fprintf(file, "%s\n", header);
        *header_written = 1;
    }

    // If raw_line is provided, write it as-is (preserves spaces/quotes exactly as input)
    if (raw_line != NULL) {
        fprintf(file, "%s\n", raw_line);
    } else if (fields != NULL) {
        for (int i = 0; i < field_count; i++){
            write_field_with_quotes(file, fields[i]);
            if (i < field_count - 1) fprintf(file, ",");
        }
        fprintf(file, "\n");
    }
}


