#include "valid.H"

valid tst;

int main()
{
    string subject1("Hello World!");

    Pattern p1 = Any(CharacterSets::alpha) & "ello World!";
    Pattern p2 =
        "Hello"
      & Any(CharacterSets::space)
      & "World"
      & Any(CharacterSets::punct);

    tst.validate(p1, subject1, true);
    tst.validate(p2, subject1, true);

    return tst.state();
}
