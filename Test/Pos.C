#include "valid.H"

valid tst;

int main()
{
    string subject1("indiana");
    string subject2("alabama");
    string subject3("arkansas");

    // pattern w/ concatenation
    Pattern p1 = Pattern("a") & Pos(4) & Len(1) & Pattern("a");
    Pattern p2 = Pos(0U) & Pattern("a") & Len(1) & Pattern("a");

    tst.validate(p1, subject1, false);
    tst.validate(p1, subject2, false);
    tst.validate(p1, subject3, false);
    tst.validate(p2, subject1, false);
    tst.validate(p2, subject2, true);
    tst.validate(p1, subject3, false);

    return tst.state();
}
