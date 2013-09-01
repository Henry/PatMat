#include "valid.H"

valid tst;

int main()
{
    string subject1("Hello World");
    string subject2("Hello World123");
    string subject3("Hello123World");
    string hello("Hello");
    string ello("ello");

    Pattern hello_pattern(hello);
    Pattern ello_pattern(ello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = Fence() & hello_pattern & ' ' & world_pattern;
    Pattern p2 =
    Fence() & hello_pattern & ' ' & world_pattern &
    Span(string("0123456789"));
    Pattern p3 = Fence() & ello_pattern & Span("0123456789") & world_pattern;
    Pattern p4 = Fence() & ello_pattern & ' ' & world_pattern;
    Pattern p5 =
    Fence() & ello_pattern & ' ' & world_pattern &
    Span(string("0123456789"));
    Pattern p6 = Fence() & ello_pattern & Span("0123456789") & world_pattern;

    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, false);
    tst.validate(p2, subject2, true);
    tst.validate(p2, subject3, false);
    tst.validate(p3, subject1, false);
    tst.validate(p3, subject2, false);
    tst.validate(p3, subject3, false);
    tst.validate(p4, subject1, false);
    tst.validate(p4, subject2, false);
    tst.validate(p4, subject3, false);
    tst.validate(p5, subject1, false);
    tst.validate(p5, subject2, false);
    tst.validate(p5, subject3, false);
    tst.validate(p6, subject1, false);
    tst.validate(p6, subject2, false);
    tst.validate(p6, subject3, false);

    return tst.state();
}
