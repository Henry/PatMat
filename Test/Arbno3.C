#include "valid.H"

valid tst;

int main()
{
    string subject1("HelloHelloHello World!");
    string subject2("Hello World!abcabc");
    string subject3("Hello abcabcWorld!");
    string subject4("HelloHelloHello World!HelloHelloHello World!");
    string subject5("Hello World!abcabcHello World!abcabc");
    string subject6("Hello abcabcWorld!Hello abcabcWorld!");
    string hello("Hello");

    Pattern hello_pattern(hello);
    Pattern world_pattern("World!");

    // pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;
    Pattern p2 = hello_pattern & ' ' & world_pattern & Arbno(string("abc"));
    Pattern p3 = hello_pattern & ' ' & Arbno(string("abc")) & world_pattern;
    Pattern p4 = Arbno(p1);
    Pattern p5 = Arbno(p2);
    Pattern p6 = Pattern("Never!") & Arbno(p3);

    tst.validate(p4, subject4, true);
    tst.validate(p4, subject5, true);
    tst.validate(p4, subject6, true);
    tst.validate(p5, subject4, true);
    tst.validate(p5, subject5, true);
    tst.validate(p5, subject6, true);
    tst.validate(p6, subject4, false);
    tst.validate(p6, subject5, false);
    tst.validate(p6, subject6, false);

    return tst.state();
}
