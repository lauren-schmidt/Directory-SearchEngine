#ifndef SEARCH_H
#define SEARCH_H

#include <glob.h>
#include "hashmap.h"
#include "scorelist.h"
#include "querylist.h"
#include <math.h>

struct hashmap *training(glob_t searchresult, int numBuckets);
struct querylist *read_query(char *query);
void rank(struct hashmap *hm, struct querylist *searchq, glob_t filelist);
void stop_word(struct hashmap *hm, int numfiles);

#endif