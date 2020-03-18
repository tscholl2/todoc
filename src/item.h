#ifndef ITEM_HEADER_H
#define ITEM_HEADER_H
#include <time.h>

/**
 * A todo item consists of three things:
 *  - A description (arbitrary* text)
 *  - A (non-null) timestamp for when it was created
 *  - A (possibly null) timestamp for when it was completed
 * 
 * The arbitrary text must be trimmed and not contain double new lines.
 **/
typedef struct _Item
{
    char *text;
    time_t created;
    time_t completed;
} Item;

/**
 * Allocates and returns a new Item.
 **/
Item *Item_new();

/**
 * Frees memory used by Item, but NOT by the text.
 **/
void Item_free(Item *a);

/**
 * Sets the completed time.
 * The adjusted item is returned.
 **/
Item *Item_complete(Item *a);

/**
 * Given a search term, this returns a "score" for
 * how likely that term or something like it appears
 * in the Item. A higher score is more likely to appear.
 **/
int Item_fuzzy_search(Item *a, char *fuzzy_needle);

/**
 * Comparisons are done so that the "first/minimal/smallest" item is the
 * newest one that is not completed.
 **/
int Item_compare(Item *a, Item *b);

#endif
