#include <assert.h>
#include <string.h>
#include "levenshtein.h"

int main()
{
  const char *word1 = "pieces";
  const char *word2 = "paces";
  assert(distance(word1, strlen(word1), word2, strlen(word2)) == 2);
}