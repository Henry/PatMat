/// Copyright 2013-2016 Henry G. Weller
/// Copyright 2007-2010 Philip L. Budne
// -----------------------------------------------------------------------------
//  This file is part of
/// ---     The PatMat Pattern Matcher
// -----------------------------------------------------------------------------
//
//  PatMat is free software: you can redistribute it and/or modify it under the
//  terms of the GNU General Public License version 2 as published by the Free
//  Software Foundation.
//
//  Goofie is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
//  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
//  details.
//
//  You should have received a copy of the GNU General Public License along with
//  this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  As a special exception, if you link this file with other files to produce an
//  executable, this file does not by itself cause the resulting executable to
//  be covered by the GNU General Public License. This exception does not
//  however invalidate any other reasons why the executable file might be
//  covered by the GNU Public License.
//
//
//  PatMat was developed from the SPIPAT and GNAT.SPITBOL.PATTERNS package.
//  GNAT was originally developed by the GNAT team at New York University.
//  Extensive contributions were provided by Ada Core Technologies Inc.
//  SPIPAT was developed by Philip L. Budne.
// -----------------------------------------------------------------------------
/// Title: Main Pattern class and user interface functions
///  Description:
//    This package provides a complete set of operations for constructing
//    patterns that can be used in the pattern matching operations provided.
// -----------------------------------------------------------------------------

#ifndef Pattern_H
#define Pattern_H

#include "CharacterSet.H"

#include <iostream>
#include <string>

// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------
/// Forward declarations
// -----------------------------------------------------------------------------
class Pattern;
class Pattern_;
class PatElmt_;


// -----------------------------------------------------------------------------
/// Typedefs
// -----------------------------------------------------------------------------

typedef char Character;
typedef unsigned int Natural;
typedef int Flags;


// -----------------------------------------------------------------------------
/// Internal Debugging
// -----------------------------------------------------------------------------
// Define to activate some built-in debugging traceback
//#define IDOUT(X) X
#define IDOUT(X)


// -----------------------------------------------------------------------------
/// StringGetter: string getter
// -----------------------------------------------------------------------------

//- String interface which returns a std::string.  When this class is used as a
//  formal parameter type in this package, it indicates a deferred pattern.  The
//  get function will be called when the pattern element is matched to
//  obtain the currently referenced string value.
class StringGetter
{
public:

    virtual ~StringGetter()
    {}

    virtual std::string get() const
    {
        Natural l;
        const Character* p = get(l);
        return std::string(p, l);
    }

    virtual const Character* get(Natural&) const = 0;
};


// -----------------------------------------------------------------------------
/// StringSetter: string setter
// -----------------------------------------------------------------------------

//- String interface which sets a std::string.
class StringSetter
{
public:

    virtual ~StringSetter()
    {}

    virtual void set(const std::string&) = 0;
};


// -----------------------------------------------------------------------------
/// UnsignedGetter: Natural getter
// -----------------------------------------------------------------------------

//- Unsigned interface which returns an Natural.
//  When this type is used as a formal parameter type in this package, it
//  indicates a deferred pattern.  The function will be called when the pattern
//  element is matched to obtain the currently referenced Natural value.
class UnsignedGetter
{
public:

    virtual ~UnsignedGetter()
    {}

    virtual Natural get() const = 0;
};


// -----------------------------------------------------------------------------
/// UnsignedSetter: Natural setter
// -----------------------------------------------------------------------------

//- Unsigned interface which sets an Natural.
class UnsignedSetter
{
public:

    virtual ~UnsignedSetter()
    {}

    virtual void set(const Natural) = 0;
};


// -----------------------------------------------------------------------------
/// BoolGetter: boolean getter
// -----------------------------------------------------------------------------

//- Bool interface which returns a bool. When this class is used as a formal
//  parameter type in this package, it indicates a deferred predicate
//  pattern. The function will be called when the pattern element is matched and
//  failure signalled if false is returned.
class BoolGetter
{
public:

    virtual ~BoolGetter()
    {}

    virtual bool get() const = 0;
};


// -----------------------------------------------------------------------------
/// Match data
// -----------------------------------------------------------------------------

enum MatchRet
{
    MATCH_UNITITIALIZED_PATTERN,
    MATCH_LOGIC_ERROR,
    MATCH_FAILURE,
    MATCH_SUCCESS
};

const char* const MatchRetMessages[] =
{
    "Uninitialized pattern",
    "Internal logic error patterns",
    "Match failure",
    "Match success"
};

class MatchState
{
public:

    // Output from match
    Natural start_, stop_;
    MatchRet ret_;

    MatchState()
    :
        start_(0),
        stop_(0),
        ret_(MATCH_FAILURE)
    {}

    inline bool matched() const
    {
        return ret_ == MATCH_SUCCESS;
    }

    inline operator bool() const
    {
        return matched();
    }

    inline Natural start() const
    {
        return start_;
    }

    inline Natural stop() const
    {
        return stop_;
    }
};

class MutableMatchState
:
    public MatchState
{
public:

    std::string& subject_;

    MutableMatchState(const MatchState& ms, std::string& subject)
    :
        MatchState(ms),
        subject_(subject)
    {}

    bool operator=(const std::string& replacement)
    {
        if (matched())
        {
            subject_.replace(start_, stop_ - start_, replacement);
            return true;
        }
        return false;
    }

    bool operator=(const Character* replacement)
    {
        if (matched())
        {
            subject_.replace(start_, stop_ - start_, replacement);
            return true;
        }
        return false;
    }
};


// -----------------------------------------------------------------------------
/// Pattern: pattern object
// -----------------------------------------------------------------------------

class Pattern
{
    // Private data

        Pattern_* pat_;

    // Private static functions

        static Pattern inline orStrPat(const std::string&, const Pattern&);
        static Pattern inline orPatStr(const Pattern&, const std::string&);
        static Pattern inline orStrStr(const std::string&, const std::string&);

        Pattern(Natural stackIndex, const PatElmt_* P);


public:

    // Match flags
    static const int debug = 1;
    static const int anchor = 2;
    static const int trace = 4;

    // Constructors

        Pattern();
        Pattern(const Pattern&);
        Pattern(const Character);
        Pattern(const std::string&);
        Pattern(const Character*);
        Pattern(const Character* ptr, const Natural len);
        Pattern(const StringGetter&);

    // Destructor
    ~Pattern();

    // Member operators

        Pattern& operator=(const Pattern&);

    // Alternation member operators

        Pattern& operator|=(const std::string&);
        Pattern& operator|=(const Character*);
        Pattern& operator|=(const Character);
        Pattern& operator|=(const Pattern&);

    // Concatenation member operators

        Pattern& operator&=(const std::string&);
        Pattern& operator&=(const Character*);
        Pattern& operator&=(const Character);
        Pattern& operator&=(const Pattern&);

    // Pattern construction functions not declared as methods
    // to keep usage SNOBOL-like.

        friend Pattern Abort();

    // Alternation

        friend Pattern operator|(const std::string& l, const Pattern& r);
        friend Pattern operator|(const Character* l, const Pattern& r);
        friend Pattern operator|(const Pattern& l, const std::string& r);
        friend Pattern operator|(const Pattern& l, const Character* r);
        friend Pattern operator|(const Pattern& l, const Pattern& r);
        friend Pattern operator|(const Character l, const Pattern& r);
        friend Pattern operator|(const Pattern& l, const Character r);
        friend Pattern operator|(const std::string& l, const std::string& r);
        friend Pattern operator|(const Character* l, const std::string& r);
        friend Pattern operator|(const std::string& l, const Character* r);
        friend Pattern Or(const Character* l, const Character* r);
        friend Pattern operator|(const std::string& l, const Character r);
        friend Pattern Or(const Character* l, const Character r);
        friend Pattern operator|(const Character l, const std::string& r);
        friend Pattern Or(const Character l, const Character* r);
        friend Pattern Or(const Character l, const Character r);

        friend Pattern Any(const Character);
        friend Pattern Any(const CharacterSet&);
        friend Pattern Any(const std::string&);
        friend Pattern Any(const std::string*);
        friend Pattern Any(const StringGetter&);

        friend Pattern Arb();

        friend Pattern Arbno(const Pattern&);
        friend Pattern Arbno(const Character*);
        friend Pattern Arbno(const std::string&);
        friend Pattern Arbno(const Character);

    // Assignment

        friend Pattern operator*(const Pattern& p, std::string&);
        friend Pattern operator*(const Pattern& p, StringSetter&);
        friend Pattern operator%(const Pattern& p, std::string&);
        friend Pattern operator%(const Pattern& p, StringSetter&);

        friend Pattern Bal(const Character open, const Character close);

        friend Pattern Break(const Character);
        friend Pattern Break(const CharacterSet&);
        friend Pattern Break(const std::string&);
        friend Pattern Break(const std::string*);
        friend Pattern Break(const StringGetter&);

        friend Pattern BreakXMake(PatElmt_*);

        friend Pattern BreakX(const Character);
        friend Pattern BreakX(const CharacterSet&);
        friend Pattern BreakX(const std::string&);
        friend Pattern BreakX(const std::string*);
        friend Pattern BreakX(const StringGetter&);

    // Concatenation

        friend Pattern operator&(const std::string& l, const Pattern& r);
        friend Pattern operator&(const Character* l, const Pattern& r);
        friend Pattern operator&(const Pattern& l, const std::string& r);
        friend Pattern operator&(const Pattern& l, const Character* r);
        friend Pattern operator&(const Pattern& l, const Pattern& r);
        friend Pattern operator&(const Character l, const Pattern& r);
        friend Pattern operator&(const Pattern& l, const Character r);

        friend Pattern Defer(const Pattern&);
        friend Pattern Defer(const std::string&);
        friend Pattern Defer(const StringGetter&);
        friend Pattern Defer(const BoolGetter&);

        friend Pattern Fail();

        friend Pattern Fence();
        friend Pattern Fence(const Pattern&);

        friend Pattern Len(const Natural);
        friend Pattern Len(const UnsignedGetter&);
        friend Pattern Len(const Natural*);

        friend Pattern NotAny(const Character);
        friend Pattern NotAny(const CharacterSet&);
        friend Pattern NotAny(const std::string&);
        friend Pattern NotAny(const std::string*);
        friend Pattern NotAny(const StringGetter&);

        friend Pattern NSpan(const Character);
        friend Pattern NSpan(const CharacterSet&);
        friend Pattern NSpan(const std::string&);
        friend Pattern NSpan(const std::string*);
        friend Pattern NSpan(const StringGetter&);

        friend Pattern Pos(const Natural);
        friend Pattern Pos(const UnsignedGetter&);
        friend Pattern Pos(const Natural*);

        friend Pattern Rem();

        friend Pattern Rpos(const Natural);
        friend Pattern Rpos(const UnsignedGetter&);
        friend Pattern Rpos(const Natural*);

        friend Pattern Rtab(const Natural);
        friend Pattern Rtab(const UnsignedGetter&);
        friend Pattern Rtab(const Natural*);

        friend Pattern Setcur(Natural&);

        friend Pattern Span(const Character);
        friend Pattern Span(const CharacterSet&);
        friend Pattern Span(const std::string&);
        friend Pattern Span(const std::string*);
        friend Pattern Span(const StringGetter&);

        friend Pattern Succeed();

        friend Pattern Tab(const Natural);
        friend Pattern Tab(const UnsignedGetter&);
        friend Pattern Tab(const Natural*);

        friend Pattern assignOnmatch(const Pattern& p, std::string& var);
        friend Pattern assignImmed(const Pattern& p, std::string& var);

    // Matching

        bool operator()
        (
            const Character* subject,
            const Flags flags = 0
        ) const;

        bool operator()
        (
            const std::string& subject,
            const Flags flags = 0
        ) const;

        MutableMatchState operator()
        (
            std::string& subject,
            const Flags flags = 0
        ) const;

    // Output

        inline void debugMsg(const Character* fmt) const;

        void dump(std::ostream&) const;
        friend std::ostream& operator<<(std::ostream&, const Pattern&);
};


// -----------------------------------------------------------------------------
/// Sout: string setter which outputs the string
// -----------------------------------------------------------------------------

class Output
:
    public StringSetter
{
public:

    void set(const std::string& str)
    {
        std::cout << str;
    }
};

extern Output output;


// ----------------------------------------------------------------------------
/// Pattern functions and operators
// ----------------------------------------------------------------------------
#include "PatternOperations.H"

// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif // Pattern_H
// -----------------------------------------------------------------------------
