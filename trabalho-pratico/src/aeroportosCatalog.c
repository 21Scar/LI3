#include <stdio.h>
#include <glib.h>
#include "aeroportos.h"
#include "aeroportosCatalog.h"

struct aeroportosCatalog {
    GHashTable* aeroportos;
};

AeroportosCatalog* criarAeroportosCatalog() {
    AeroportosCatalog* catalog = (AeroportosCatalog*)malloc(sizeof(AeroportosCatalog));
    if (!catalog) {
        fprintf(stderr, "Erro ao alocar memoria para o catalogo de aeroportos\n");
        return NULL;
    }
    catalog->aeroportos = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_aeroporto);
    return catalog;
}

void destruirAeroportosCatalog(AeroportosCatalog* catalog) {
    g_hash_table_destroy(catalog->aeroportos);
    free(catalog);
}

void inserirAeroporto(AeroportosCatalog* catalog, Aeroportos* aeroporto) {
    char *tmp = getCode(aeroporto);
    char *key = strdup(tmp);
    free(tmp);
    g_hash_table_insert(catalog->aeroportos, key, aeroporto);

}

Aeroportos* buscarAeroporto(AeroportosCatalog* catalog, const char* code) {
    return g_hash_table_lookup(catalog->aeroportos, code);
}

void aeroportos_foreach(AeroportosCatalog* catalog, GHFunc func, gpointer user_data) {
    if (!catalog || !func) return;
    g_hash_table_foreach(catalog->aeroportos, func, user_data);
}