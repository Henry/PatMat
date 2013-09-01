#include "valid.H"


valid tst;

int main()
{
    string subject1("Hello World");
    string subject2("Hello World123");
    string subject3("HelloWorld");
    string subject4("Hello 123World");
    string subject5("HelloWorldWordBye");
    string hello("Hello");
    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;
    Pattern p2 = hello_pattern & ' ' & Break('Z') & world_pattern;
    Pattern p3 = hello_pattern & Break('W') & world_pattern;
    Pattern p4 = hello_pattern & Break('W') & world_pattern & Pattern("Bye");;
    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, false);
    tst.validate(p2, subject2, false);
    tst.validate(p2, subject3, false);
    tst.validate(p3, subject1, true);
    tst.validate(p3, subject2, true);
    tst.validate(p3, subject3, true);
    tst.validate(p3, subject4, true);
    tst.validate(p3, subject5, true);
    if (tst.passed())
        return 0;
    else
        return 1;
}
