#include "valid.H"

valid tst;

int main()
{
    string subject1("Hello World!");
    string subject2("Hello World!123");
    string subject3("Hello 123World!");
    string hello("Hello");

    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;

    Pattern p2 =
        hello_pattern
      & ' '
      & world_pattern
      & NSpan(string("0123456789"));

    Pattern p3 =
        hello_pattern
      & ' '
      & NSpan(string("0123456789"))
      & world_pattern;

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
