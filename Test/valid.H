#include "Pattern.H"

using namespace PatMat;
using namespace std;

class valid
{
    int tests;
    int successes;

public:

    valid();

    bool validate
    (
        const Pattern& p,
        const std::string& s,
        const int expected_result
    );

    bool validate
    (
        const Pattern& p,
        std::string& s,
        MatchRes& result,
        const int expected_result
    );

    bool validate
    (
        const Pattern& p,
        std::string& s,
        const std::string& replace,
        const int expected_result
    );

    bool validate_assign
    (
        const Pattern& p,
        const std::string& s1,
        const std::string& s2
    );

    bool passed() const;

    int state() const;
};


class MyStringObj:public StringInterface
{
    std::string value;

public:

    MyStringObj()
    {}

    ~MyStringObj()
    {}

    std::string getString()
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

    void putString(const std::string& str)
    {
        value = str;
        // cout << "putString: " << value << '\n';
    }
};
