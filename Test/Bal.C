#include "valid.H"


valid tst;

int main()
{
    string subject1("HelloHelloHello World!");
    string subject2("()(())(pp())");
    string subject3("())");
    string subject4("())(");
    string subject5("((())");

    // Pattern p1 = Bal();
    Pattern p1 = Pos(0U) & Bal() & Rpos(0U);

    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);
    tst.validate(p1, subject4, false);
    tst.validate(p1, subject5, false);

    return tst.state();
}
