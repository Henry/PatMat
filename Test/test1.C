#include <string>
#include <iostream>

#include "Pattern.H"

using namespace PatMat;
using namespace std;

class MyStringObj
:
    public StringGetter
{
    string value;

public:

    MyStringObj()
    {}

    ~MyStringObj()
    {}

    string get() const
    {
        // cout << "get: " << value << '\n';
        return value;
    }

    const char* get(unsigned& l) const
    {
        // cout << "get: " << value << '\n';
        l = value.length();
        return &value[0];
    }

    void set(const string& str)
    {
        value = str;
        // cout << "set: " << value << '\n';
    }
};


int main()
{
    // Simple match, constant subject, on the fly pattern
    if (Any("aeiuo")("Hello"))
    {
        cout << "matched!\n";
    }

    string subject("Hello World!");
    string hello("Hello");
    Pattern hello_pattern(hello);
    Pattern world_pattern("World");

    // Pattern w/ concatenation
    Pattern p1 = hello_pattern & ' ' & world_pattern;
    if (p1(subject))
    {
        cout << "matched!\n";
    }

    // Pattern with alternation
    string goodbye("Goodbye");
    //Pattern p2 = (goodbye | hello) & ' ' & world_pattern;
    Pattern p2 = goodbye;
    p2 |= hello;
    p2 &= ' ' & world_pattern;

    // Dump pattern as table on stdout
    p2.dump(cout);

    // Print as an expression
    cout << "Pattern p2: " << p2 << '\n';

    if (p2(subject))
    {
        cout << "matched!\n";
    }

    // Simple match and replace
    if ((hello_pattern(subject) = "Goodbye"))
    {
        cout << "replaced: " << subject << '\n';
    }

    // Assigment on match
    string vowel;
    if ((Any("aeiuo") * vowel)("Hello"))
    {
        cout << "First vowel: " << vowel << '\n';
    }

    // Output on match
    cout << "First vowel: ";
    (Any("aeiuo") * output)("Hello");

    // Immediate assigment
    string nonv;
    Natural pos;
    Pattern p3 = Setcur(pos) & 'l' % nonv & Abort();

    // Dump pattern as table on stdout
    p3.dump(cout);

    // Print as an expression
    cout << "Pattern p3: " << p3 << '\n';

    if (!(Setcur(pos) & 'l' % nonv & Abort())("Hello"))
    {
        cout << "l: " << nonv << " at pos " << pos << '\n';
    }

    // Assign on match & replace w/ value from match
    string sss;
    if (((goodbye * sss)(subject) = "<b>" + sss + "</b>"))
    {
        cout << "replaced: " << subject << '\n';
    }

    // Test "delayed evaluation" of string value
    MyStringObj s;
    Pattern p4 = "H" & vowel & +s;
    s.set("ll");  // After p4 creation
    subject = "Hello";
    if ((p4(subject) = ""))
    {
        cout << "remainder: " << subject << '\n';
    }

    string s2;
    Pattern p5('c');
    Pattern p6 = "H" & +p5 & +s2;
    subject = "Hello";
    p5 = vowel; // After p6 creation
    s2 = "ll";  // After p6 creation
    if ((p6(subject) = ""))
    {
        cout << "remainder: " << subject << '\n';
    }

    // Look at assignment...
    cout << "here1\n";
    {
        Pattern p3 = p4;
        p4 = Pattern("hi");
    }
    cout << "here2\n";

    return 0;
}
