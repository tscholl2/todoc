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
    time_t created;
    time_t completed;
};

Item *Item_new()
{
    Item *a = calloc(1, sizeof(Item));
    a->created = time(NULL);
    return a;
}

void Item_free(Item *a)
{
    free(a->text);
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
    int ok, c, i, n;
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
    fseek(in, -i, SEEK_CUR);
    ok = fread(text, sizeof(char), i, in);
    assert(ok == i);
    regex_t preg;
    ok = regcomp(
        &preg,
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
    a->text = text;
    memcpy(a->text, &text[pmatch[3].rm_so], n);
    a->text[n] = 0;
    return a;
}

int Item_write(Item *a, FILE *out)
{
    int ok;
    struct tm tm;
    char date1[26] = {};
    asctime_r(localtime_r(&a->created, &tm), date1);
    date1[24] = 0; // kill new line at the end
    char date2[] = "------------------------";
    if (a->completed != 0)
    {
        asctime_r(localtime_r(&a->completed, &tm), date2);
        date2[24] = 0;
    }
    ok = fprintf(
        out,
        "================================================================================\n"
        "   %s          --->            %s   \n"
        "================================================================================\n"
        "%s"
        "\n\n\n",
        date1, date2, a->text);
    assert(ok == 81 * 3 + strlen(a->text) + 3);
    return 0;
}
