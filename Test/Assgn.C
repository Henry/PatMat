#include "valid.H"

valid tst;

int main()
{
    // assigment on match
    string vowel;
    Pattern p1 = Any("aeiou") * vowel;
    tst.validate(p1, "Hello", true);
    tst.validate_assign(p1, vowel, "e");

    // immediate assigment
    string nonv;
    unsigned int pos;
    Pattern p2 = Setcur(pos) & 'l' % nonv & Abort();
    tst.validate(p2, "Hello", false);
    tst.validate_assign(p2, nonv, "l");

    // assign on match & replace w/ value from match
    string sss;
    string subject1("hello");
    string subject2("goodbye");

    MatchRes match1(subject1);
    Pattern p3 = Pattern("good") * sss;
    tst.validate(match1, p3, false);

    MatchRes match2(subject2);
    tst.validate(match2, p3, true);
    match2 = "<b>" + sss + "</b>";
    tst.validate_assign(p3, match2, "<b>good</b>bye");

    // test "delayed evaluation" of string value
    MyStringObj s;
    Pattern p4 = "H" & vowel & +s;
    s.set("ll");  // AFTER p4 creation
    string subject("Hello");
    string r("");

    tst.validate(p4, subject, r, true);
    tst.validate_assign(p4, subject, "o");

    return tst.state();
}
