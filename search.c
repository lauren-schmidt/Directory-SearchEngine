#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <glob.h>
#include <math.h>
#include "hashmap.h"
#include "scorelist.h"
#include "querylist.h"
#include "search.h"

#define MAX 1024

int main(void)
{
    char word[MAX];

    // Prompt user for num_buckets
    int num_buckets = 0;
    printf("How many buckets?\n");
    fgets(word, MAX, stdin);

    // Take in user input at stdin and store in word
    sscanf(word, "%d", &num_buckets);

    // Prompt for what directory the user wants to search (glob)
    char searchquery[MAX];
    glob_t searchresult;
    printf("Enter a filename search string: \n");

    // Actually take in the user input at stdin and store in searchquery
    fgets(searchquery, MAX, stdin);
    searchquery[strlen(searchquery) - 1] = '\0';

    int return_value = glob(searchquery, 0, NULL, &searchresult);
    if (return_value != 0)
    {
        if (return_value == GLOB_NOMATCH)
        {
            fprintf(stderr, "No filename matches\n");
        }
        else
        {
            fprintf(stderr, "Glob error\n");
        }
    }

    // Run training algorithm
    struct hashmap *hm;
    hm = training(searchresult, num_buckets);

    // Get search query from user
    while (1)
    {
        char usr_qry[MAX];
        printf("Enter a search query (X to exit):\n");
        fgets(usr_qry, MAX, stdin);
        if (usr_qry[0] == 'X')
        {
            break;
        }

        usr_qry[strlen(usr_qry) - 1] = '\0';
        struct querylist *querylist = read_query(usr_qry);
        rank(hm, querylist, searchresult);
        ql_delete(querylist);
    }

    // Free all allocated memory for glob, hashmap, etc.
    hm_destroy(hm);
    globfree(&searchresult);
}

struct hashmap *training(glob_t searchresult, int numBuckets)
{
    unsigned int i;
    struct hashmap *hm = hm_create(numBuckets);

    // Go through all files, adding each word to the hashmap using hash_table_insert
    for (i = 0; i < searchresult.gl_pathc; i++)
    {
        // Open the current file at index [i] of our glob search result in READ-ONLY mode
        FILE *curfile = fopen(searchresult.gl_pathv[i], "r");

        // Send an error messages if there is a null file
        if (curfile == NULL)
        {
            fprintf(stderr, "Error opening file %s", searchresult.gl_pathv[i]);
            // Exit on error condition
            exit(1);
        }

        // Go through each word of the file and add that word to the hashmap.
        char word[21]; // max word length of 20 characters (given in specs) + a null terminator
        while (fscanf(curfile, "%s", word) != EOF)
        {
            hash_table_insert(hm, word, searchresult.gl_pathv[i], 1);
        }
    }

    // Remove stopwords
    stop_word(hm, searchresult.gl_pathc);
    // Return a hashmap with all words entered and all stop words removed.
    return hm;
}

// Stores the search query in a linked list.
struct querylist *read_query(char *query)
{
    struct querylist *ql = ql_create();

    // strtok breaks a String into a sequence of nonempty tokens separated by a given delimiter (a space here)
    char *toks = strtok(query, " ");
    while (toks != NULL)
    {
        ql_insert(ql, toks);
        toks = strtok(NULL, " ");
    }

    return ql;
}

// Rank list of scores according to tf-idf algorithm
void rank(struct hashmap *hm, struct querylist *searchq, glob_t filelist)
{
    // Initialize our list for scores
    struct scorelist *scorelist = sl_create();
    unsigned int i;
    int num_valid_scores = 0;

    // Go through each document, and then each word in the document. Get the score and add the document to its place in the list.
    for (i = 0; i < filelist.gl_pathc; i++)
    {
        double word_score = 0;
        struct querynode *curnode = searchq->head;
        while (curnode != NULL)
        {
            int tf = hm_get(hm, curnode->word, filelist.gl_pathv[i]);
            if (tf == -1)
            {
                tf = 0;
            }

            int df = hm_get_docs(hm, curnode->word);
            if (df <= 0)
            {
                df = 1;
            }

            double idf = log((double)filelist.gl_pathc / (double)df);

            // tf - idf
            word_score += tf * idf;
            curnode = curnode->next;
        }
        sl_insert(scorelist, filelist.gl_pathv[i], word_score);
        if (word_score > 0)
        {
            num_valid_scores++;
        }
    }
    if (num_valid_scores == 0)
    {
        // Couldn't find any matching docs
        printf("No documents found\n");
    }
    else
    {
        sl_print(scorelist);
    }
    sl_toFile(scorelist);
    sl_delete(scorelist);
}

// Identifies stopwords and removes them accordingly
void stop_word(struct hashmap *hm, int numfiles)
{
    int i;
    // Go through all the buckets of the hashmap, checking for words.
    for (i = 0; i < hm->num_buckets; i++)
    {
        struct wordnode *curword = hm->map[i];

        // Go through all of the words in each bucket.
        while (curword != NULL)
        {
            double df = (double)curword->doc_freq;
            double idf = log(numfiles / df);
            // If the word appears in all of the documents, remove it.
            if (idf == 0.00000)
            {
                hm_remove(hm, curword->word);
            }
            curword = curword->next;
        }
    }
}
