#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap.h"

// Creates a new hashmap struct with a given number of buckets.
struct hashmap *hm_create(int num_buckets)
{
    int i;

    if (num_buckets <= 0)
    {
        return NULL;
    }

    // Allocate space for a new hashmap.
    struct hashmap *hm = (struct hashmap *)malloc(sizeof(struct hashmap));

    // Allocate zero-ed out space for each of the buckets.
    hm->map = calloc(num_buckets, sizeof(struct wordnode));

    // Initialize variables of hashmap
    hm->num_buckets = num_buckets;

    // Go through each bucket of hashmap and set to null
    for (i = 0; i < num_buckets; i++)
    {
        hm->map[i] = NULL;
    }
    // Return the newly created HashMap.
    return hm;
}

/*Return the number of documents that a specified word appears in.
Input: Hashmap to be operated on, word to count document occurences of.
Output: Number of documents that word appears in.*/
int hm_get_docs(struct hashmap *hm, char *word)
{
    if (hm == NULL || word == NULL)
    {
        return -1;
    }

    int curBucket = hash_code(hm, word);
    struct wordnode *curnode;
    curnode = hm->map[curBucket];

    while (curnode != NULL)
    {
        if (strcmp(curnode->word, word) == 0)
        {
            return curnode->doc_freq;
        }
        curnode = curnode->next;
    }
    return -1;
}

// Returns the term frequency of a given word/document pair, if it exists.
// Returns -1 for error cases, and 0 if the word/doc pair does not exist.
int hm_get(struct hashmap *hm, char *word, char *document_id)
{
    if (hm == NULL || word == NULL || document_id == NULL)
    {
        return -1;
    }

    int curBucket = hash_code(hm, word);
    struct wordnode *curnode;
    struct docnode *curdoc;
    curnode = hm->map[curBucket];

    if (hm_get_docs(hm, word) != -1)
    {
        while (curnode != NULL)
        {
            if (strcmp(curnode->word, word) == 0)
            {
                curdoc = curnode->doclist;
                while (curdoc != NULL)
                {
                    if (strcmp(curdoc->document_id, document_id) == 0)
                    {
                        return curdoc->term_freq;
                    }
                    curdoc = curdoc->next;
                }
            }
            curnode = curnode->next;
        }
    }
    return -1;
}

// For use in the insert method, does not use the document id parameter. Returns 1 if there is a matching word and 0 if there is not.
int hm_get2(struct hashmap *hm, char *word)
{
    if (hm == NULL || word == NULL)
    {
        return -1;
    }

    int curBucket = hash_code(hm, word);
    struct wordnode *curnode;
    curnode = hm->map[curBucket];

    // Check for a word match in the bucket. If we have one, return 1. If not, return 0.
    while (curnode != NULL)
    {
        if (strcmp(curnode->word, word) == 0)
        {
            return curnode->doc_freq;
        }
        curnode = curnode->next;
    }
    // Return 0 if there is no word match
    return -1;
}

/*Finds the KPV of the word using hashing function and inserts it into its correct location in the hashmap. Also stores all information relating to the node, including the number of occurrences of the word and the ID of the document it is found in.
Input: Hashmap to be operated on, word to be inserted, document ID of word, number of occurrences of word in document.
Output: Updated hashmap with inserted value. */
void hash_table_insert(struct hashmap *hm, char *word, char *document_id, int num_occurrences)
{
    if (hm == NULL || word == NULL || document_id == NULL)
    {
        return;
    }

    unsigned int h = hash_code(hm, word);
    struct wordnode *headnode = hm->map[h];
    struct wordnode *curnode = headnode;
    struct docnode *curdoc;

    // If the word is not in the hashmap, add the word and doc node in.
    if (hm_get2(hm, word) == -1)
    {
        if (curnode != NULL)
        {
            while (curnode->next != NULL)
            {
                curnode = curnode->next;
            }
        }

        // Exited while loop
        struct wordnode *temp = calloc(sizeof(struct wordnode), 1);
        struct docnode *tempdoc = calloc(sizeof(struct docnode), 1);
        temp->word = strdup(word);

        if (curnode == NULL)
        {
            hm->map[h] = temp;
        }
        else
        {
            curnode->next = temp;
        }
        temp->next = NULL;
        temp->doc_freq = 1;
        temp->doclist = tempdoc;
        tempdoc->next = NULL;
        tempdoc->document_id = strdup(document_id);
        tempdoc->term_freq = 1;
        return;
    }
    else
    {
        if ((hm_get2(hm, word) != -1) && (hm_get(hm, word, document_id) != -1))
        {
            while (curnode != NULL)
            {
                if (strcmp(curnode->word, word) == 0)
                {
                    curdoc = curnode->doclist;
                    while (curdoc != NULL)
                    {
                        if (strcmp(curdoc->document_id, document_id) == 0)
                        {
                            curdoc->term_freq++;
                            return;
                        }
                        curdoc = curdoc->next;
                    }
                }
                curnode = curnode->next;
            }
        }
        else
        {
            struct docnode *doc = calloc(sizeof(struct docnode), 1);
            doc->document_id = strdup(document_id);
            doc->term_freq++;
            while (curnode != NULL)
            {
                if (strcmp(curnode->word, word) == 0)
                {
                    curdoc = curnode->doclist;
                    doc->next = curdoc;
                    curnode->doclist = doc;
                    curnode->doc_freq += num_occurrences;
                    return;
                }
                curnode = curnode->next;
            }
        }
        return;
    }
}

// Remove KVP in HashMap associated with given key
void hm_remove(struct hashmap *hm, char *word)
{

    // Find the bucket of the KVP associated with the key.
    int h = hash_code(hm, word);
    struct wordnode *curword = hm->map[h];
    struct wordnode *prevword = NULL;

    // Step into the bucket and parse through the linked-list. Look for the node that will store all occurrences of the word.
    while (curword != NULL)
    {
        if (strcmp(curword->word, word) == 0)
        {
            if (prevword == NULL)
            {
                hm->map[h] = curword->next;
            }
            else
            {
                prevword->next = curword->next;
            }
            // Free the word:
            free(curword->word);

            struct docnode *curdoc = NULL;
            while (curword->doclist)
            {
                curdoc = curword->doclist;
                curword->doclist = curword->doclist->next;
                free(curdoc->document_id);
                free(curdoc);
            }
            free(curword);
            return;
        }
        prevword = curword;
        curword = curword->next;
    }
    return;
}

// Deallocate (free) HashMap that is passed in and all of its elements
void hm_destroy(struct hashmap *hm)
{
    struct wordnode *curnode;
    struct wordnode *next;
    struct docnode *curword;
    struct docnode *nextword;
    int i;

    if (hm == NULL)
    {
        return;
    }

    for (i = 0; i < hm->num_buckets; i++)
    {
        curnode = hm->map[i];
        while (curnode != NULL)
        {
            curword = curnode->doclist;
            while (curword != NULL)
            {
                nextword = curword->next;
                free(curword->document_id);
                free(curword);
                curword = nextword;
            }
            next = curnode->next;
            free(curnode->word);
            // free(curnode->doclist);
            free(curnode);
            curnode = next;
        }
    }
    free(hm->map);
    free(hm);
}

/*Hashing algorithm: sum ASCII codes of characters in word,
compute mod # buckets to determine buckets in hashmap.
i.e. sum % buckets*/
int hash_code(struct hashmap *hm, char *word)
{
    int asciiSum = 0;
    int index, i;
    int len = strlen(word);
    for (i = 0; i < len; i++)
    {
        asciiSum += (int)(word[i]);
    }
    index = asciiSum % hm->num_buckets;
    return index;
}

void hm_print(struct hashmap *hm)
{
    int i;
    for (i = 0; i < hm->num_buckets; i++)
    {
        struct wordnode *curword = hm->map[i];
        while (curword)
        {
            printf("Bucket %d: %s (%d documents)\n", i, curword->word, curword->doc_freq);
            struct docnode *doc = curword->doclist;
            while (doc)
            {
                printf("\t%s: %d\n", doc->document_id, doc->term_freq);
                doc = doc->next;
            }
            curword = curword->next;
        }
    }
    return;
}
