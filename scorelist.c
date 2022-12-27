#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <scorelist.h>
#include <math.h>

// Initialize a list of the scores of the docs
struct scorelist *sl_create(void)
{
    struct scorelist *sl = malloc(sizeof(struct scorelist));
    sl->size = 0;
    sl->first = NULL;
    return sl;
}

// Delete the list of document scores
void sl_delete(struct scorelist *sl)
{
    struct scorenode *curnode = sl->first;
    while (sl->first != NULL)
    {
        curnode = sl->first;
        sl->first = curnode->next;

        // free each node
        free(curnode->filename);
        free(curnode);

        // decrement the size variable for the total list
        sl->size--;
    }

    // Once we have freed each individual node, free the entire scores list.
    free(sl);
}

// Print out the list of scores (mostly for error checking)
void sl_print(struct scorelist *sl)
{
    struct scorenode *curnode = sl->first;
    while (curnode != NULL)
    {
        printf("file: %s\n", curnode->filename);
        curnode = curnode->next;
    }

    if (allzeroes(sl) == 1)
    {
        printf("No documents were found\n");
    }
}

int allzeroes(struct scorelist *sl)
{
    struct scorenode *curnode = sl->first;
    while (curnode != NULL)
    {
        if (curnode->score > 0)
        {
            return 0;
        }
        curnode = curnode->next;
    }
    return 1;
}

// Save the list of scores to search_scores.txt
void sl_toFile(struct scorelist *sl)
{
    FILE *search_scores = fopen("search_scores.txt", "a");
    struct scorenode *curnode = sl->first;
    while (curnode != NULL)
    {
        if (curnode->score > 0)
        {
            fprintf(search_scores, "%s:\t%f\n", curnode->filename, curnode->score);
        }
        curnode = curnode->next;
    }
    fclose(search_scores);
}

// Insert a score (make sure they are ranked highest to lowest!!!)
void sl_insert(struct scorelist *sl, char *filename, double score)
{
    struct scorenode *newnode = malloc(sizeof(struct scorenode));

    newnode->filename = strdup(filename);
    newnode->score = score;

    // Case 1: New node to be added is added to an empty list, so just insert at the head.
    if (sl->first == NULL)
    {
        sl->first = newnode;
        newnode->next = NULL;
    }
    // Case 2: Otherwise, need to figure out where in the list we should place the new node.
    else
    {
        struct scorenode *curnode = sl->first;
        struct scorenode *prevnode = NULL;

        // Traverse through list until we find the correct location to insert the new scorenode in.
        while ((curnode != NULL) && (curnode->score > score))
        {
            prevnode = curnode;
            curnode = curnode->next;
        }
        if (prevnode == NULL) // We need to insert at the head of the list, aka the score is higher than any of the current nodes in the list.
        {
            newnode->next = sl->first;
            sl->first = newnode;
        }
        else
        {
            newnode->next = curnode;
            prevnode->next = newnode;
        }
    }

    sl->size++;
}