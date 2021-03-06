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
        const bool expected_result
    );

    bool validate
    (
        const Pattern& p,
        std::string& s,
        const std::string& replace,
        const bool expected_result
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


class MyStringObj
:
    public StringGetter
{
    std::string value;

public:

    MyStringObj()
    {}

    virtual ~MyStringObj()
    {}

    virtual std::string get() const
    {
        return value;
    }

    virtual const char* get(unsigned& l) const
    {
        l = value.length();
        return &value[0];
    }

    void set(const std::string& str)
    {
        value = str;
    }
};
