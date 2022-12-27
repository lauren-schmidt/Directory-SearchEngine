#ifndef HASHMAP_H
#define HASHMAP_H

struct wordnode
{
        char *word;
        int doc_freq; // Total number of documents the word appears in
        struct wordnode *next;
        struct docnode *doclist;
};

struct docnode
{
        char *document_id;   // Document that this instance of the word appears in
        int term_freq; // Number of occurrences within the document
        struct docnode *next;
};

struct hashmap
{
        struct wordnode **map;
        int num_buckets;
};

struct hashmap *hm_create(int num_buckets);
int hm_get_docs(struct hashmap *hm, char *word);
int hm_get(struct hashmap *hm, char *word, char *document_id);
int hm_get2(struct hashmap *hm, char *word);
void hash_table_insert(struct hashmap *hm, char *word, char *document_id, int num_occurrences);
void hm_remove(struct hashmap *hm, char *word);
void hm_destroy(struct hashmap *hm);
int hash_code(struct hashmap *hm, char *word);
void hm_print(struct hashmap *hm);

#endif
