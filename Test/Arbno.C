#include "valid.H"

valid tst;

int main()
{
    string subject1("HelloHelloHello World!");
    string subject2("Hello World!abcabc");
    string subject3("Hello abcabcWorld!");
    string hello("Hello");

    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = Arbno(hello_pattern) & ' ' & world_pattern;
    Pattern p2 = hello_pattern & ' ' & world_pattern & Arbno(string("abc"));
    Pattern p3 = hello_pattern & ' ' & Arbno(string("abc")) & world_pattern;

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
