#include "valid.H"

valid tst;

int main()
{
    string subject1("Hello World!");
    string subject2("Hello World!abc");
    string subject3("Hello abcWorld!");
    string hello("Hello");

    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;
    Pattern p2 = hello_pattern & ' ' & world_pattern & Arb();
    Pattern p3 = hello_pattern & Arb() & world_pattern;

    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, true);
    tst.validate(p2, subject2, true);
    tst.validate(p2, subject3, false);
    tst.validate(p3, subject1, true);
    tst.validate(p3, subject2, true);
    tst.validate(p3, subject3, true);

    return tst.state();
}
