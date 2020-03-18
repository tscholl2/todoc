#include <assert.h>
#include <string.h>
#include "item.h"

int main()
{
  Item* a = Item_new();
  Item* b = Item_new();
  a->text = "hello";
  b->text = "world";
  assert(Item_compare(a,b) < 0);
}