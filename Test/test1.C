#include <string>
#include <iostream>

#include "Pattern.H"

using namespace PatMat;
using namespace std;

class MyStringObj
:
    public StringInterface
{
    string value;

public:

    MyStringObj()
    {}

    string getString()
    {
        // cout << "getString: " << value << '\n';
        return value;
    }

    const char *getString(unsigned *l)
    {
        // cout << "getString: " << value << '\n';
        *l = value.length();
        return &value[0];
    }

    void putString(const string& str)
    {
        value = str;
        // cout << "putString: " << value << '\n';
    }

    ~MyStringObj()
    {}
};

int main()
{
    // simple match, constant subject, on the fly pattern
    if (Match("Hello", Any("aeiuo")))
        cout << "matched!\n";

    string subject("Hello World!");
    string hello("Hello");
    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;
    if (Match(subject, p1))
        cout << "matched!\n";

    // pattern with alternation
    string goodbye("Goodbye");
    //Pattern p2 = (goodbye | hello) & ' ' & world_pattern;
    Pattern p2 = goodbye;
    p2 |= hello;
    p2 &= ' ' & world_pattern;

    // dump pattern as table on stdout
    p2.dump(cout);

    // print as an expression
    cout << "Pattern p2: " << p2 << '\n';

    if (Match(subject, p2))
        cout << "matched!\n";

    // simple match and replace
    if (Match(subject, hello_pattern, "Goodbye"))
        cout << "replaced: " << subject << '\n';

    // assigment on match
    string vowel;
    if (Match("Hello", Any("aeiuo") * vowel))
        cout << "First vowel: " << vowel << '\n';

    // output on match
    cout << "First vowel: ";
    Match("Hello", Any("aeiuo") * cout);

    // immediate assigment
    string nonv;
    unsigned pos;
    Pattern p3 = Setcur(pos) & 'l' % nonv & Abort();

    // dump pattern as table on stdout
    p3.dump(cout);

    // print as an expression
    cout << "Pattern p3: " << p3 << '\n';

    if (!Match("Hello", Setcur(pos) & 'l' % nonv & Abort()))
        cout << "l: " << nonv << " at pos " << pos << '\n';

    // assign on match & replace w/ value from match
    string sss;
    MatchRes match(subject);
    if (Match(match, goodbye * sss))
    {
        match = "<b>" + sss + "</b>";
        cout << "replaced: " << match << '\n';
    }

    // test "delayed evaluation" of string value
    MyStringObj s;
    Pattern p4 = "H" & vowel & +s;
    s.putString("ll");  // AFTER p4 creation
    subject = "Hello";
    if (Match(subject, p4, ""))
        cout << "remainder: " << subject << '\n';

    string s2;
    Pattern p5('c');
    Pattern p6 = "H" & +p5 & +s2;
    subject = "Hello";
    p5 = vowel; // AFTER p6 creation
    s2 = "ll";  // AFTER p6 creation
    if (Match(subject, p6, ""))
        cout << "remainder: " << subject << '\n';

    // look at assignment...
    cout << "here1\n";
    {
        Pattern p3 = p4;
        p4 = Pattern("hi");
    }
    cout << "here2\n";

    return 0;
}
