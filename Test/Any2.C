#include "valid.H"

valid tst;

int main()
{
    string subject1("Hello World!");
    string subject2("Hello World!abc");
    string subject3("Hello abcWorld!");
    string hello("Hello");

    Pattern hello_pattern(hello);
    Pattern world_pattern("World!");

    // pattern w/ concatenation
    string s1, s2, s3, s4;

    Pattern p1 = hello_pattern & ' ' & world_pattern;

    Pattern p2 =
        hello_pattern * s2
      & ' '
      & world_pattern
      & Any('a') * s1
      & Pattern("bc");

    Pattern p3 =
        hello_pattern
      & ' '
      & Any('a') * s3
      & Pattern("bc")
      & world_pattern * s4;

    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, false);
    tst.validate(p2, subject2, true);
    tst.validate_assign(p2, s1, "a");
    tst.validate_assign(p2, s2, "Hello");
    tst.validate(p2, subject3, false);
    tst.validate(p3, subject1, false);
    tst.validate(p3, subject2, false);
    tst.validate(p3, subject3, true);
    tst.validate_assign(p3, s3, "a");
    tst.validate_assign(p3, s4, "World!");

    return tst.state();
}
