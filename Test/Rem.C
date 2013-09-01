#include "valid.H"

valid tst;

int main()
{
    // assigment on match
    string vowel;
    string r;

    Pattern p5 = Any("aeiou") * vowel & Rem() * r;

    tst.validate(p5, "Hello", true);
    tst.validate_assign(p5, vowel, "e");
    tst.validate_assign(p5, r, "llo");

    return tst.state();
}
