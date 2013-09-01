#include "valid.H"

valid tst;

int main()
{
    string subject("Hello");

    string s;
    Pattern p1('c');
    Pattern p2 = "H" & +p1 & +s;
    p1 = Any("aeiuo"); // AFTER p2 creation
    s = "ll";          // AFTER p2 creation

    tst.validate(p2, subject, "", true);
    tst.validate_assign(p2, subject, "o");

    return tst.state();
}
