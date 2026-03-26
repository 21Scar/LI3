#include <stdio.h>
#include <glib.h>
#include "aeronaves.h"
#include "aeronavesCatalog.h"

struct aeronavesCatalog {
    GHashTable* aeronaves;
    GArray* aeronaves_list; // para query 2
};

struct aeronaveRanking{
    const char* identifier;
    Aeronaves* aeronave;
};


AeronavesCatalog* criarAeronavesCatalog() {
    AeronavesCatalog* catalog = (AeronavesCatalog*)malloc(sizeof(AeronavesCatalog));
    if (!catalog) {
        fprintf(stderr, "Erro ao alocar memoria para o catalogo de aeronaves\n");
        return NULL;
    }
    catalog->aeronaves = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, (GDestroyNotify)free_aeronave);
    catalog->aeronaves_list = NULL;
    return catalog;
}

void destruirAeronavesCatalog(AeronavesCatalog* catalog) {
    if(catalog->aeronaves_list){
        /* Libertar cada AeronaveRanking* alocado com g_new */
        for (guint i = 0; i < catalog->aeronaves_list->len; i++) {
            AeronaveRanking* ar = g_array_index(catalog->aeronaves_list, AeronaveRanking*, i);
            g_free(ar); 
        }
        g_array_free(catalog->aeronaves_list, TRUE);
    }
    g_hash_table_destroy(catalog->aeronaves);
    free(catalog);
}

void inserirAeronave(AeronavesCatalog* catalog, Aeronaves* aeronave) {
    char *tmp = getIdentifier(aeronave);
    char *key = strdup(tmp);
    free(tmp);
    g_hash_table_insert(catalog->aeronaves, key, aeronave);

}

Aeronaves* buscarAeronave(AeronavesCatalog* catalog, const char* identifier) {
    return g_hash_table_lookup(catalog->aeronaves, identifier);
}

void aeronaves_foreach(AeronavesCatalog* catalog, GHFunc func, gpointer user_data) {
    g_hash_table_foreach(catalog->aeronaves, func, user_data);
}

// Comparador para o GArray 
gint compararAeronaves(gconstpointer a, gconstpointer b){
    const AeronaveRanking* const* pa = (const AeronaveRanking* const*)a;
    const AeronaveRanking* const* pb = (const AeronaveRanking* const*)b;
    
    const AeronaveRanking* aeronave_a = *pa;
    const AeronaveRanking* aeronave_b = *pb;
    
    if(!aeronave_a || !aeronave_b || !aeronave_a->aeronave || !aeronave_b->aeronave) return 0;

    int nvoos_a = getNVoos(aeronave_a->aeronave);
    int nvoos_b = getNVoos(aeronave_b->aeronave);

    if(nvoos_a != nvoos_b) return nvoos_b - nvoos_a; // ordem decrescente

    return g_strcmp0(aeronave_a->identifier, aeronave_b->identifier); // ordem alfabética crescente
        
}

// Construir e ordenar o GArray
void arrayAeronaves (AeronavesCatalog* catalog){
    if(!catalog) return; 
    if(catalog->aeronaves_list){
       return; // já foi criado
    }

    GArray* aeronavesOrdem = g_array_new(FALSE, FALSE, sizeof(AeronaveRanking*));

    GHashTableIter iter;
    gpointer key, value;
    g_hash_table_iter_init(&iter, catalog->aeronaves);
    while (g_hash_table_iter_next(&iter, &key, &value)) {
        const char* id = (const char*)key;
        Aeronaves* aeronave = (Aeronaves*)value;

        AeronaveRanking* node = g_new(AeronaveRanking, 1);
        node->identifier = id;
        node->aeronave = aeronave;
        g_array_append_val(aeronavesOrdem, node);
    }

    g_array_sort(aeronavesOrdem, (GCompareFunc)compararAeronaves);
    catalog->aeronaves_list = aeronavesOrdem;
}

GArray* getAeronavesArray(AeronavesCatalog* catalog){
    return catalog->aeronaves_list;
}

const char* getAeronaveRank_Id(const AeronaveRanking* ar){
    return ar->identifier;
}

Aeronaves* getAeronaveRank_Aeronave(const AeronaveRanking* ar){
    return ar->aeronave;
}