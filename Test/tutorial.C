#include <string>
#include <iostream>

#include "Pattern.H"

using namespace PatMat;
using namespace std;

class MaxLen
:
    public BoolInterface
{
    const string& cur_;
    const string& max_;

    public:

        MaxLen(const string& cur, const string& max)
        :
            cur_(cur),
            max_(max)
        {}

        bool getBool()
        {
            return cur_.size() > max_.size();
        }
};


int main()
{
    {
        MatchRes m("Change brackets around character (a)");
        string c;
        Match(m, '(' & Len(1) % c & ')');
        m = '[' + c + ']';
        cout << m << endl;
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
        const Pattern nums = blank & num % num1 & Span(" ,") & num % num2;
        Match(" 124, 257 ", nums);
        cout << "num1 = " << num1 << "; num2 = " << num2 << endl;
    }
    {
        const Pattern digs  = Span(CharacterSets::digit);
        const Pattern uDigs = digs & Arbno('_' & digs);
        const Pattern eDig  = Span(CharacterSets::xdigit);
        const Pattern ueDig = eDig & Arbno('_' & eDig);
        const Pattern bChar = Any("#:");
        string temp;
        const Pattern bNum = uDigs & bChar % temp & ueDig & (+temp);
        const string subject("16#123_abc#");
        if (Match(subject, bNum)) cout << "Matched " << subject << endl;
    }
    {
        Pattern balancedString;

        Pattern element =
            NotAny("[]{}")
          | ('[' & (+balancedString) & ']')
          | ('{' & (+balancedString) & '}');

        balancedString = element & Arbno(element);

        cout << Match("xy[ab{cd}]", balancedString % cout & Fail()) << endl;
    }
    {
        string cur, max;
        unsigned loc;

        MaxLen GtS(cur, max);

        const CharacterSet& digit = CharacterSets::digit;
        const Pattern digits = Span(digit);

        const Pattern find =
            "" % max & Fence()         & // initialize max to null
            BreakX(digit)              & // scan looking for digits
            ((digits % cur             & // assign next string to cur
             (+GtS)                    & // check cur.size() > max.size()
             Setcur(loc))                // if so, save location
                     % max)            & // and assign to max
            Fail();                      // seek all alternatives

        Match("ab123cd4657ef23", find);
        cout<< "max = " << max << "; loc = " << loc << endl;
    }

    return 0;
}
