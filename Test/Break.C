#include "valid.H"

valid tst;

int main()
{
    string subject1("Hello World");
    string subject2("Hello World123");
    string subject3("Hello123World");
    string subject4("HelloWorld");
    string subject5("HelloWorldWordBye");
    string hello("Hello");

    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;
    Pattern p2 = hello_pattern & ' ' & Break("ZP") & world_pattern;
    Pattern p3 = hello_pattern & Break("WZ") & world_pattern;
    Pattern p4 = hello_pattern & Break("WZ") & world_pattern & Pattern("Bye");;

    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, false);
    tst.validate(p2, subject2, false);
    tst.validate(p2, subject3, false);
    tst.validate(p3, subject1, true);
    tst.validate(p3, subject2, true);
    tst.validate(p3, subject4, true);
    tst.validate(p4, subject5, false);

    return tst.state();
}
