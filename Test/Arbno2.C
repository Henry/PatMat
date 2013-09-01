#include "valid.H"


valid tst;

int main()
{
    string subject1("HelloHelloHello World!");
    string subject2("Hello World!aa");
    string subject3("Hello aaWorld!");
    string hello("Hello");
    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = Arbno(hello_pattern) & ' ' & world_pattern;
    Pattern p2 = hello_pattern & ' ' & world_pattern & Arbno('a');
    Pattern p3 = hello_pattern & ' ' & Arbno('a') & world_pattern;
    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, true);
    tst.validate(p2, subject2, true);
    tst.validate(p2, subject3, false);
    tst.validate(p3, subject1, true);
    tst.validate(p3, subject2, true);
    tst.validate(p3, subject3, true);
    if (tst.passed())
    {
        return 0;
    }
    else
        return 1;
}
