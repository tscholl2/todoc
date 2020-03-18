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
  FILE* f = fmemopen(s,sizeof(s),"rb");
  // FILE* f = fopen("a.txt","rb");
  char* b;
  size_t size;
  // FILE* f = open_memstream(&b,&size);
  // fprintf(f,"%s",s);
  fseek(f,0,SEEK_SET);
  Item* a = Item_read(f);
  assert(fclose(f) == 0);
  f = open_memstream(&b,&size);
  Item_write(a,f);
  fclose(f);
  printf("%s\n",b);
  assert(memcmp(s,b,strlen(s)) == 0);

  // Test compare

  // assert(Item_compare(a,b) < 0);
}