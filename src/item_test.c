#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "item.h"

int main()
{

  // Test read/write

  char s[] =
      "================================================================================\n"
      "   Mon Aug 13 08:23:14 2012          --->            Mon Aug 13 08:23:14 2012   \n"
      "================================================================================\n"
      "This is a sample todo.\n\n\n"
      "================================================================================\n"
      "   Tue Aug 14 08:23:14 2012          --->            Wed Aug 15 08:23:14 2012   \n"
      "================================================================================\n"
      "This is another todo.\n\n\n"
      "================================================================================\n"
      "   Mon Aug 13 08:23:14 2012          --->            Thu Aug 16 08:23:14 2012   \n"
      "================================================================================\n"
      "This is another todo.\n\n\n"
      "";
  FILE *f;
  f = fmemopen(s, sizeof s, "rb+");
  Item *a = Item_read(f);
  Item *b = Item_read(f);
  Item *c = Item_read(f);
  fclose(f);
  char buf[5000] = {};
  f = fmemopen(buf, 5000, "rb+");
  Item_write(a, f);
  Item_write(b, f);
  Item_write(c,f);
  fclose(f);
  assert(memcmp(s, buf, strlen(s)) == 0);

  // Test compare

  // assert(Item_compare(a,b) < 0);
}