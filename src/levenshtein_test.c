#include <assert.h>
#include <string.h>
#include "levenshtein.h"
#include "tester.h"

BEGIN_TEST(pieces_paces)
{
  const char *word1 = "pieces";
  const char *word2 = "paces";
  assert(distance(word1, strlen(word1), word2, strlen(word2)) == 2);
}
END_TEST

BEGIN_TEST(doggy_dog)
{
  const char *word1 = "doggy";
  const char *word2 = "dog";
  assert_int(distance(word1, strlen(word1), word2, strlen(word2)), ==, 2);
}
END_TEST

int main()
{
  RUN_TEST(pieces_paces);
  RUN_TEST(doggy_dog);
}