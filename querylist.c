#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <querylist.h>


// Initialize list of query nodes
struct querylist *ql_create(void)
{
    struct querylist *ql = malloc(sizeof(struct querylist));
    ql->size = 0;
    ql->head = NULL;
    return ql;
}

// Delete list of query nodes
void ql_delete(struct querylist *ql)
{
    struct querynode *curnode = ql->head;
    while (ql->head != NULL)
    {
        curnode = ql->head;
        ql->head = curnode->next;

        // Free each node
        free(curnode->word);
        free(curnode);

        ql->size--;
    }

    free(ql);
}

// Insert node + related word string into list
void ql_insert(struct querylist *ql, char *w)
{
    struct querynode *newnode = malloc(sizeof(struct querynode));
    newnode->word = strdup(w);
    newnode->next = ql->head;
    ql->head = newnode;
    ql->size++;
}