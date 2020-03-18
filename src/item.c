#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "item.h"
#include "levenshtein.h"

Item *Item_new()
{
    Item *a = malloc(sizeof(Item));
    a->created = time(NULL);
    return a;
}

void Item_free(Item *a)
{
    return free(a);
}

Item *Item_complete(Item *a)
{
    a->completed = time(NULL);
    return a;
}

int Item_compare(Item *a, Item *b)
{
    time_t d;
    if ((d = a->completed - b->completed) != 0)
        return d;
    if ((d = a->created - b->created) != 0)
        return d;
    return strcmp(a->text, b->text);
}

int Item_fuzzy_search(Item *a, char *fuzzy_needle)
{
    int result = 0;
    char *haystack = a->text;
    int len1 = strlen(fuzzy_needle);
    char c = haystack[0];
    while (c != 0)
    {
        int len2 = 0;
        while (c = haystack[len2], c != 0 && isspace(c))
            len2++;
        int d = -distance(fuzzy_needle, len1, haystack, len2);
        result = result > d ? result : d;
        haystack += len2;
    }
    return result;
}
