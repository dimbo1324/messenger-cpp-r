#include "Auth.h"

#include <cassert>

int main()
{
    assert(!auth::validatePassword("").ok);
    assert(!auth::validatePassword("short1").ok);
    assert(!auth::validatePassword("NoDigitsHere").ok);
    assert(auth::validatePassword("Correct123").ok);

    assert(!auth::isValidLogin("ab"));
    assert(!auth::isValidLogin("bad login"));
    assert(auth::isValidLogin("alice_1"));

    return 0;
}
