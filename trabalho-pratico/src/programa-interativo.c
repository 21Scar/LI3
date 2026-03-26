#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "main.h"
#include "querys.h"


// Remove o caractere newline no final de uma string
static void trim_newline(char *s) {
    if (!s) return;
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') s[len - 1] = '\0';
}

// Lê uma linha do stdin com tratamento de erros
static int read_line(char *buffer, size_t size, const char *prompt) {
    printf("%s", prompt);
    if (!fgets(buffer, size, stdin)) {
        fprintf(stderr, "Erro a ler input.\n");
        return 0;
    }
    trim_newline(buffer);
    return 1;
}

// Lê um número inteiro do utilizador
static int read_integer(const char *prompt) {
    char buffer[64];
    if (!read_line(buffer, sizeof(buffer), prompt)) {
        return -1;
    }
    if (buffer[0] == '\0') {
        printf("Entrada vazia.\n");
        return -1;
    }

    char *endptr = NULL;
    int num = (int)strtol(buffer, &endptr, 10);
    return num;
}

// Lê uma resposta sim/não do utilizador com validação
// Retorna 1 para sim (s/S/sim/SIM), 0 para não (n/N/não/NÃO), -1 para entrada inválida
static int read_yes_no(const char *prompt) {
    char buffer[64];
    
    while (1) {
        if (!read_line(buffer, sizeof(buffer), prompt)) {
            return -1;
        }
        
        if (buffer[0] == '\0') {
            printf("Entrada vazia. Por favor, responda com 's/sim' ou 'n/não'.\n");
            continue;
        }
        
        // Converter para minúsculas para simplificar validação
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] >= 'A' && buffer[i] <= 'Z') {
                buffer[i] += 32;  // Converter maiúscula para minúscula
            }
        }
        
        // Validar entrada
        if (strcmp(buffer, "s") == 0 || strcmp(buffer, "sim") == 0) {
            return 1;
        } else if (strcmp(buffer, "n") == 0 || strcmp(buffer, "não") == 0) {
            return 0;
        } else {
            printf("Entrada inválida. Por favor, responda com 's/sim' ou 'n/não'.\n");
            continue;
        }
    }
}

// Valida se um inteiro é positivo (> 0)
static int read_positive_integer(const char *prompt) {
    while (1) {
        int num = read_integer(prompt);
        if (num == -1) {
            printf("Erro ao ler número.\n");
            continue;
        }
        if (num <= 0) {
            printf("Número deve ser positivo (> 0). Tente novamente.\n");
            continue;
        }
        return num;
    }
}

// Valida um código de aeroporto (3 caracteres, letras maiúsculas)
static int read_airport_code(char *buffer, size_t size, const char *prompt) {
    while (1) {
        if (!read_line(buffer, size, prompt)) {
            return 0;
        }
        
        if (strlen(buffer) != 3) {
            printf("Código de aeroporto deve ter exatamente 3 caracteres. Tente novamente.\n");
            continue;
        }
        
        int valid = 1;
        for (int i = 0; i < 3; i++) {
            if (buffer[i] < 'A' || buffer[i] > 'Z') {
                if (buffer[i] < 'a' || buffer[i] > 'z') {
                    valid = 0;
                    break;
                }
            }
        }
        
        if (!valid) {
            printf("Código de aeroporto deve conter apenas letras. Tente novamente.\n");
            continue;
        }
        
        // Converter para maiúsculas
        for (int i = 0; i < 3; i++) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] -= 32;
            }
        }
        
        return 1;
    }
}

// Valida um formato de data (YYYY-MM-DD)
static int is_valid_date(const char *date_str) {
    if (strlen(date_str) != 10) return 0;
    if (date_str[4] != '-' || date_str[7] != '-') return 0;
    
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (date_str[i] < '0' || date_str[i] > '9') return 0;
    }
    
    int year = (date_str[0] - '0') * 1000 + (date_str[1] - '0') * 100 +
               (date_str[2] - '0') * 10 + (date_str[3] - '0');
    int month = (date_str[5] - '0') * 10 + (date_str[6] - '0');
    int day = (date_str[8] - '0') * 10 + (date_str[9] - '0');
    
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0;
    if (year < 1900 || year > 2100) return 0;
    
    return 1;
}

// Lê e valida um intervalo de datas (data1 data2)
static int read_date_range(char *buffer, size_t size, const char *prompt) {
    while (1) {
        if (!read_line(buffer, size, prompt)) {
            return 0;
        }
        
        if (buffer[0] == '\0') {
            printf("Data inválida ou vazia. Tente novamente.\n");
            continue;
        }
        
        // Tentar fazer parse de duas datas separadas por espaço
        char date1[20], date2[20];
        if (sscanf(buffer, "%s %s", date1, date2) != 2) {
            printf("Formato inválido. Use: YYYY-MM-DD YYYY-MM-DD\n");
            continue;
        }
        
        if (!is_valid_date(date1) || !is_valid_date(date2)) {
            printf("Uma ou ambas as datas estão inválidas. Formato esperado: YYYY-MM-DD\n");
            continue;
        }
        
        return 1;
    }
}

// Lê e valida uma string não-vazia 
static int read_non_empty_string(char *buffer, size_t size, const char *prompt) {
    while (1) {
        if (!read_line(buffer, size, prompt)) {
            return 0;
        }
        
        if (buffer[0] == '\0') {
            printf("Campo não pode estar vazio. Tente novamente.\n");
            continue;
        }
        
        return 1;
    }
}

// Exibe o menu principal de queries
 
static void print_menu(void) {
    printf("\n=== MENU DE QUERIES ===\n");
    printf("0: Sair\n");
    printf("1: Listar o resumo de um aeroporto\n");
    printf("2: Top N aeronaves com mais voos realizados\n");
    printf("3: Aeroporto com mais partidas entre 2 datas\n");
    printf("4: Passageiro no top 10 com maior gasto durante um período\n");
    printf("5: Top N companhias aéreas com maior atraso médio\n");
    printf("6: Aeroporto de destino mais comum para uma nacionalidade\n");
    printf("7: Mudar caminho do dataset\n");
}

// Solicita argumentos específicos para cada query ao utilizador
 
static void get_query_arguments(int query_num, char *args, size_t args_size) {
    args[0] = '\0';

    switch (query_num) {
        case 1: {
            // Query 1: Código de aeroporto
            read_airport_code(args, args_size, 
                "Código do aeroporto (ex: 'KOJ'): ");
            break;
        }
        case 2: {
            // Query 2: Número N e opcionalmente fabricante
            int n = read_positive_integer("Número N (ex: '5'): ");
            char manufacturer[128];
            int has_manufacturer = read_yes_no("Filtrar por fabricante? (s/N): ");
            
            if (has_manufacturer == 1) {
                read_non_empty_string(manufacturer, sizeof(manufacturer), 
                    "Nome do fabricante (ex: 'Boeing'): ");
                snprintf(args, args_size, "%d %s", n, manufacturer);
            } else {
                snprintf(args, args_size, "%d", n);
            }
            break;
        }
        case 3:
            // Query 3: Intervalo de datas
            read_date_range(args, args_size, 
                "Data inicial e final (YYYY-MM-DD YYYY-MM-DD): ");
            break;
        case 4: {
            // Query 4 : Intervalo de datas e opcionalmente formato 'S'
            int has_dates = read_yes_no("Filtrar por datas? (s/N): ");
            if (has_dates == 1) {
                read_date_range(args, args_size,
                    "Data inicial e final (YYYY-MM-DD YYYY-MM-DD): ");
            }
            break;
        }
        case 5: {
            // Query 5: Número N
            int n5 = read_positive_integer("Número N (ex: '10'): ");
            snprintf(args, args_size, "%d", n5);
            break;
        }
        case 6:
            // Query 6: Nacionalidade
            read_non_empty_string(args, args_size, 
                "Nacionalidade (ex: 'France'): ");
            break;
        default:
            break;
    }
}

// Verifica se uma query requer argumentos
static int query_requires_args(int query_num) {
    return (query_num == 1 || query_num == 2 || query_num == 3 || 
            query_num == 5 || query_num == 6);
}

// Lê o ficheiro de output gerado pela query e apresenta no terminal
static void display_query_output(int command_number) {
    char output_file[512];
    snprintf(output_file, sizeof(output_file), OUTPUT_DIR_NAME "/" OUTPUT_FILE_NAME, 
             command_number);
    
    FILE *f = fopen(output_file, "r");
    if (!f) {
        printf("Output: (sem conteúdo)\n");
        return;
    }

    char buffer[4096];
    size_t n = fread(buffer, 1, sizeof(buffer) - 1, f);
    buffer[n] = '\0';
    fclose(f);

    printf("\n--- OUTPUT ---\n%s", buffer);
}

// Menu para executar queries de um dataset carregado
static int query_menu(Program *program) {
    int command_number = 1;

    while (1) {
        print_menu();
        
        int query_num = read_integer("Escolha: ");
        if (query_num == -1) {
            printf("Opção inválida.\n");
            continue;
        }

        // Opção 0: Sair completamente
        if (query_num == 0) {
            printf("A sair...\n");
            return 0;
        }

        // Opção 7: Mudar dataset
        if (query_num == 7) {
            printf("A descarregar dataset atual...\n");
            return 1;
        }

        // Validar query
        if (query_num < 1 || query_num > 6) {
            printf("Query inválida. Escolha entre 1 e 6.\n");
            continue;
        }

        // Questionar sobre o formato 'S'
        int has_S = read_yes_no("Formato 'S'? (s/N): ");
        if (has_S == -1) {
            printf("Erro ao ler input.\n");
            continue;
        }

        // Obter argumentos da query
        char args[256];
        get_query_arguments(query_num, args, sizeof(args));

        // Validar argumentos
        if (query_requires_args(query_num) && args[0] == '\0') {
            printf("Argumentos inválidos ou vazios.\n");
            continue;
        }

        
        run_query(program, query_num, args, command_number - 1, has_S);

        
        display_query_output(command_number);
        command_number++;

        // Perguntar se quer executar outra query
        int continuar = read_yes_no("\nDeseja executar outra query? (s/N): ");
        if (continuar == -1) {
            printf("Erro ao ler input.\n");
            continue;
        }
        if (continuar == 0) {
            printf("A regressar ao menu de dataset...\n");
            return 1;
        }
    }
}

// Menu para carregar um dataset
static int dataset_menu(void) {
    char dataset_path[512];

    while (1) {
        if (!read_line(dataset_path, sizeof(dataset_path), 
                       "Introduza o caminho dos ficheiros de dados: ")) {
            return -1;
        }

        if (dataset_path[0] == '\0') {
            printf("Caminho vazio. Tente novamente.\n");
            continue;
        }

        // Inicializar programa com o dataset
        char *arguments[3] = { "programa-interativo", dataset_path, NULL };
        Program *program = initialize_program(arguments);

        if (program == NULL) {
            printf("Falha a carregar dataset. Verifique o caminho e ficheiros.\n");
            continue;
        }

        printf("Dataset carregado com sucesso!\n");

        // Menu de queries
        int result = query_menu(program);

        
        free_program(program);
        printf("Dataset descarregado.\n");

        
        return result;
    }
}

int main(void) {
    g_mkdir_with_parents(OUTPUT_DIR_NAME, 0700);

    printf("\n\n");
    printf("========================================\n");
    printf("   PROGRAMA INTERATIVO DE QUERIES\n");
    printf("========================================\n\n");
    printf("Bem-vindo! Carregue um dataset para começar.\n\n\n");

    while (1) {
        int result = dataset_menu();
        if (result == -1 || result == 0) {
            break;  // Sair do programa
        }
    }

    printf("\nPrograma terminado.\n");
    return 0;
}
