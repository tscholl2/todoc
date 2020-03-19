// for strptime
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <regex.h>
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
    Item *a = Item_new();
    int ok,c,i,n;
    i = n = 0;
    while ((c = fgetc(in)) != EOF)
    {
        i++;
        n = c == '\n' ? n + 1 : 0;
        if (n == 3)
            break;
    }
    if (n != 3)
        return NULL;
    char *text = malloc(i + 1);
    text[i] = 0;
    rewind(in);
    ok = fread(text, sizeof(char), i, in);
    assert(ok == i);
    regex_t preg;
    ok = regcomp(&preg,
                 "================================================================================\n"
                 "   (.{24})          --->            (.{24})   \n"
                 "================================================================================\n"
                 "([^(?:\n\n\n)]*)\n\n\n",
                 REG_EXTENDED);
    assert(ok == 0);
    regmatch_t pmatch[4];
    ok = regexec(&preg, text, 4, pmatch, 0);
    assert(ok == 0);
    regfree(&preg);
    struct tm tm = {};
    tm.tm_isdst = -1;
    assert(strptime(&text[pmatch[1].rm_so], "%c", &tm) == text + pmatch[1].rm_eo);
    a->created = mktime(&tm);
    a->completed = 0;
    if (text[pmatch[2].rm_so] != '-')
    {
        assert(strptime(&text[pmatch[2].rm_so], "%c", &tm) == text + pmatch[2].rm_eo);
        a->completed = mktime(&tm);
    }
    n = pmatch[3].rm_eo - pmatch[3].rm_so;
    a->text = malloc(n + 1);
    memcpy(a->text, &text[pmatch[3].rm_so], n);
    a->text[n] = 0;
    a->text_length = n;
    free(text);
    return a;
}

int Item_write(Item *a, FILE *out)
{
    int ok;
    char *date1 = asctime(localtime(&a->created)); // not thread safe
    date1[24] = 0;                                 // kill new line at the end
    char *date2 = "------------------------";
    if (a->completed != 0)
    {
        date2 = asctime(localtime(&a->completed));
        date2[24] = 0;
    }
    ok = fprintf(out, "================================================================================\n");
    assert(ok == 81);
    ok = fprintf(out, "   %s          --->            %s   \n", date1, date2);
    assert(ok == 81);
    ok = fprintf(out, "================================================================================\n");
    assert(ok == 81);
    ok = fwrite(a->text, 1, a->text_length, out);
    assert(ok == a->text_length);
    ok = fprintf(out, "\n\n\n");
    assert(ok == 3);
    return 0;
}
