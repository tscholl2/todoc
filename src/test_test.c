#include "tester.h"
#include <string.h>

BEGIN_TEST(foo)
{
    assert_int(1, <, 2);
    assert_int(1, >, 2);
}
END_TEST

BEGIN_TEST(bar)
{
    char *a = "1";
    char *b = "1";
    ASSERT(a, b, strcmp(a, b) == 0, "%s == %s");
}
END_TEST

int main()
{
    RUN_TEST(foo);
    RUN_TEST(bar);
}