#include "valid.H"

valid tst;

int main()
{
    string subject1("indiana");
    string subject2("alabama");
    string subject3("arkansas");

    // pattern w/ concatenation
    Pattern p1 = Rtab(2) & Len(1) & Pattern("a");

    tst.validate(p1, subject1, true);
    tst.validate(p1, subject2, true);
    tst.validate(p1, subject3, false);

    return tst.state();
}
