#ifndef SCORELIST_H
#define SCORELIST_H

struct scorenode
{
    double score;
    char *filename;
    struct scorenode *next;
};

struct scorelist
{
    int size;
    struct scorenode *first;
};

struct scorelist *sl_create(void);
void sl_delete(struct scorelist *sl);
void sl_print(struct scorelist *sl);
void sl_toFile(struct scorelist *sl);
void sl_insert(struct scorelist *sl, char *filename, double score);
int allzeroes(struct scorelist *sl); 

#endif