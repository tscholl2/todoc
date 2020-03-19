#ifndef ITEM_HEADER_H
#define ITEM_HEADER_H
#include <time.h>
#include <stdio.h>

/**
 * A todo item consists of three things:
 *  - A description (arbitrary* text)
 *  - A (non-null) timestamp for when it was created
 *  - A (possibly null) timestamp for when it was completed
 * 
 * The arbitrary text must be trimmed and not contain double new lines.
 **/
typedef struct _Item Item;

/**
 * Allocates and returns a new Item.
 **/
Item *Item_new();

/**
 * Reads an item from an input stream.
 * Returns NULL if error occurs or if EOF.
 **/
Item* Item_read(FILE* in);

/**
 * Writes this item to an output stream.
 * Returns 0 if successful.
 **/
int Item_write(Item* a, FILE* out);

/**
 * Frees memory used by Item.
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
int Item_fuzzy_search(Item *a, char *fuzzy_needle, int fuzzy_needle_length);

/**
 * Comparisons are done so that the "first/minimal/smallest" item is the
 * newest one that is not completed.
 **/
int Item_compare(Item *a, Item *b);

#endif
