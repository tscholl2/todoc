#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "item.h"

int main()
{

  // Test read/write

  char*s = "\
================================================================================\n\
   Mon Aug 13 08:23:14 2012          --->            Mon Aug 13 08:23:14 2012   \n\
================================================================================\n\
This is a sample todo.\n\n\n\
";
  FILE* f = fmemopen(NULL,5000,"rb+");
  fprintf(f,"%s",s);
  fseek(f,0,SEEK_SET);
  Item* a = Item_read(f);
  fclose(f);
  char* b;
  size_t size;
  f = open_memstream(&b,&size);
  Item_write(a,f);
  fclose(f);
  assert(memcmp(s,b,strlen(s)) == 0);

  // Test compare

  // assert(Item_compare(a,b) < 0);
}