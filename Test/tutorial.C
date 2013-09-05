#include <string>
#include <iostream>

#include "Pattern.H"

using namespace PatMat;
using namespace std;

int main()
{
    {
        MatchRes m;
        string subject("Change brackets around character (a)");
        string c;
        Match(subject, '(' & Len(1) * c & ')', m);
        m = '[' + c + ']';
        cout << subject << endl;
    }
    {
        //const Pattern digs = Span("0123456789");
        const Pattern digs = Span(CharacterSets::digit);
        const Pattern lNum = Pos(0U) & digs & '.' & Span(' ');
        string line("258. Words etc.");
        Match(line, lNum, "");
        cout << line << endl;
    }
    {
        string num1, num2;
        const Pattern blank = NSpan(' ');
        const Pattern num = Span("0123456789");
        const Pattern nums = blank & num * num1 & Span(" ,") & num * num2;
        Match(" 124, 257 ", nums);
        cout << "num1 = " << num1 << "; num2 = " << num2 << endl;
    }

    {
        const Pattern Digs  = Span(CharacterSets::digit);
        const Pattern UDigs = Digs & Arbno('_' & Digs);

        const Pattern Edig  = Span(CharacterSets::xdigit);
        const Pattern UEdig = Edig & Arbno('_' & Edig);

        const Pattern Bnum  = UDigs & '#' & UEdig & '#';
    }

    return 0;
}
