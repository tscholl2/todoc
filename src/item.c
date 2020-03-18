// for strptime
#define _XOPEN_SOURCE

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
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
    int ok;
    // skip header
    ok = fseek(in, 81, SEEK_CUR);
    assert(ok == 0);
    struct tm tm = {};
    tm.tm_isdst = -1;
    char buf[26] = {};
    // skip left space
    ok = fseek(in, 3, SEEK_CUR);
    assert(ok == 0);
    // read created time
    ok = fread(buf, sizeof(char), sizeof(buf) - 1, in);
    assert(ok == 25);
    strptime(buf, "%c", &tm);
    a->created = mktime(&tm);
    // skip inbetween
    ok = fseek(in, 24, SEEK_CUR);
    assert(ok == 0);
    // read completed time
    ok = fread(buf, sizeof(char), sizeof(buf) - 1, in);
    assert(ok == 25);
    strptime(buf, "%c", &tm);
    a->completed = ok == 25 ? mktime(&tm) : 0;
    // skip after space and next line
    ok = fseek(in, 3 + 1 + 80 + 1, SEEK_CUR);
    assert(ok == 0);
    // read text until \n\n\n
    char *text = malloc(10000);
    int c;
    int i = 0;
    int n = 0;
    while ((c = fgetc(in)) != EOF)
    {
        n = c == '\n' ? n + 1 : 0;
        if (n == 3)
        {
            text[--i] = 0;
            break;
        }
        text[i++] = c;
    }
    a->text = text;
    a->text_length = i;
    return a;
}

int Item_write(Item *a, FILE *out)
{
    int ok;
    char *date1 = asctime(localtime(&a->created)); // not thread safe
    date1[24] = 0;
    char *date2 = "-------------------------";
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
    ok = fprintf(out, "%s\n\n\n", a->text);
    assert(ok == a->text_length + 3);
    return 0;
}
