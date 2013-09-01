#include "CharacterSet.H"

using namespace PatMat;
using namespace std;

int main()
{
    cout<< (CharacterSets::alpha | CharacterSets::digit) << endl;
    cout<< (CharacterSets::alpha | string("12")) << endl;
    cout<< ((CharacterSets::alpha & "ab") | '3') << endl;
    cout<< (CharacterSet("ab") | "cd" | "12" | CharacterSet("abcde") | '3')
        << endl;

    CharacterSet cs("ab");
    cs |= "cd";
    cs |= "12";
    cs |= CharacterSet("abcde");
    cs |= '3';
    cs &= "ad12345";
    cout<< cs << endl;

    return 0;
}
