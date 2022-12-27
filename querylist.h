#ifndef QUERYLIST_H
#define QUERYLIST_H

struct querynode
{
    struct querynode *next;
    char *word;
};

struct querylist
{
    struct querynode *head;
    int size;
};

struct querylist *ql_create(void);
void ql_delete(struct querylist *ql);
void ql_insert(struct querylist *ql, char *w);

#endif