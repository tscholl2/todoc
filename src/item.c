#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "item.h"
#include "levenshtein.h"

struct _Item
{
    char *text;
    size_t text_length;
    time_t created;
    time_t completed;
};

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
    return strncmp(a->text, b->text, a->text_length < b->text_length ? a->text_length : b->text_length);
}

int Item_fuzzy_search(Item *a, char *fuzzy_needle, int fuzzy_needle_length)
{
    int result = 0;
    char *haystack = a->text;
    int len1 = fuzzy_needle_length;
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

Item *Item_read(FILE *in)
{
    int c;
    // Tue May 26 21:51:03 2015
    char date[26];
    struct tm * dt;
    strptime(date,"%a ",dt);
    //fscanf(in,"",);
    while ((c = fgetc(in)) != EOF)
    {
    }
}

int Item_write(Item *a, FILE *out)
{
    int ok;
    char date[26] = {};
    char *t = calloc(sizeof(char), a->text_length + 1);
    memcpy(t, a->text, a->text_length);
    if (a->completed != NULL)
    {
        ok = ctime_s(date, sizeof(date), &a->created);
        assert(ok == 0);
        fprintf(out,
                "================================================================================"
                "   %s          --->          -------------------------   \n"
                "================================================================================"
                "%s\n\n\n",
                date, t);
    }
    else
    {
        fprintf(out, "");
    }
}
