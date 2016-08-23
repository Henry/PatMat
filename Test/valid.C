#include "valid.H"

#include <string>
#include <iostream>
using namespace std;

valid::valid()
:
    tests(0),
    successes(0)
{}

bool valid::validate
(
    const Pattern& p,
    const std::string& s,
    const bool expected_result
)
{
    tests++;

    if (p(s) == expected_result)
    {
        successes++;
        return true;
    }
    else
    {
        cout<< "test " << tests << " *** FAILED! ***\n"
            << "pattern = " << p << "\n"
            << "string = " << s << endl;
        return false;
    }
}

bool valid::validate
(
    const Pattern& p,
    string& s,
    const string& replace,
    const bool expected_result
)
{
    tests++;
    if ((p(s) = replace) == expected_result)
    {
        successes++;
        return true;
    }
    else
    {
        cout<< "test " << tests << " *** FAILED! ***\n"
            << "pattern = " << p << "\n"
            << "string = " << s << "\n"
            << "replace = " << replace << endl;
        return false;
    }
}

bool valid::validate_assign
(
    const Pattern& p,
    const string& s1,
    const string& s2
)
{
    tests++;
    if (s1 == s2)
    {
        successes++;
        return true;
    }
    else
    {
        cout<< "test " << tests << " *** FAILED! ***\n"
            << "pattern = " << p << "\n"
            << "string1 = " << s1 << "\n"
            << "string2 = " << s2 << endl;
        return false;
    }
}

bool valid::passed() const
{
    if (tests == successes)
    {
        cout << successes << '/' << tests << " tests succeded\n";
        return true;
    }
    else
    {
        cout<< successes << '/' << tests << " tests succeded\n"
            << tests - successes << " tests failed" << endl;
        return false;
    }
}

int valid::state() const
{
    if (passed())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
