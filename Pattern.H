/// Copyright 2013 Henry G. Weller
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
class Pattern_;
class PatElmt_;
class MatchRes;
typedef char Character;


// -----------------------------------------------------------------------------
/// Internal Debugging
// -----------------------------------------------------------------------------
// Define to activate some built-in debugging traceback
//#define IDOUT(X) X
#define IDOUT(X)


// -----------------------------------------------------------------------------
/// StringInterface: string object interface
// -----------------------------------------------------------------------------

//- String interface which returns a std::string.  When this class is used as a
//  formal parameter type in this package, it indicates a deferred pattern.  The
//  get function will be called when the pattern element is matched to
//  obtain the currently referenced string value.
class StringInterface
{
public:

    virtual ~StringInterface()
    {}

    virtual std::string get() const = 0;

    virtual const Character* get(unsigned int&) const = 0;

    virtual void set(const std::string&) = 0;
};


// -----------------------------------------------------------------------------
/// UnsignedInterface: unsigned int object interface
// -----------------------------------------------------------------------------

//- Unsigned interface which returns an unsigned.  When this type is used as a
//  formal parameter type in this package, it indicates a deferred pattern.  The
//  function will be called when the pattern element is matched to obtain the
//  currently referenced unsigned int value.
class UnsignedInterface
{
public:

    virtual ~UnsignedInterface()
    {}

    virtual unsigned int get() const = 0;
};


// -----------------------------------------------------------------------------
/// BoolInterface: boolean object interface
// -----------------------------------------------------------------------------

//- Bool interface which returns a bool. When this class is used as a formal
//  parameter type in this package, it indicates a deferred predicate
//  pattern. The function will be called when the pattern element is matched and
//  failure signalled if false is returned.
class BoolInterface
{
public:

    virtual ~BoolInterface()
    {}

    virtual bool get() const = 0;
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

        static Pattern inline callOnmatch
        (
            const Pattern&,
            void (*)(const std::string&, void*),
            void*
        );

        static Pattern inline callImmed
        (
            const Pattern&,
            void (*)(const std::string&, void*),
            void*
        );

        Pattern(unsigned int stackIndex, const PatElmt_* P);


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
        Pattern(const Character* ptr, const unsigned int len);
        Pattern(const StringInterface&);

    // Destructor
    ~Pattern();

    inline void debugMsg(const Character* fmt)
    {
        IDOUT(std::cerr<< fmt << long(pat_) << std::endl;);
    }

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
        friend Pattern Any(const StringInterface&);

        friend Pattern Arb();

        friend Pattern Arbno(const Pattern&);
        friend Pattern Arbno(const Character*);
        friend Pattern Arbno(const std::string&);
        friend Pattern Arbno(const Character);

    // Assignment

        friend Pattern operator*(const Pattern& p, std::string&);
        friend Pattern operator*(const Pattern& p, StringInterface&);
        friend Pattern operator*(const Pattern& p, std::ostream&);
        friend Pattern operator%(const Pattern& p, std::string&);
        friend Pattern operator%(const Pattern& p, StringInterface&);
        friend Pattern operator%(const Pattern& p, std::ostream&);

        friend Pattern Bal(const Character open, const Character close);

        friend Pattern Break(const Character);
        friend Pattern Break(const CharacterSet&);
        friend Pattern Break(const std::string&);
        friend Pattern Break(const std::string*);
        friend Pattern Break(const StringInterface&);

        friend Pattern BreakXMake(PatElmt_*);

        friend Pattern BreakX(const Character);
        friend Pattern BreakX(const CharacterSet&);
        friend Pattern BreakX(const std::string&);
        friend Pattern BreakX(const std::string*);
        friend Pattern BreakX(const StringInterface&);

    // Concatenation

        friend Pattern operator&(const std::string& l, const Pattern& r);
        friend Pattern operator&(const Character* l, const Pattern& r);
        friend Pattern operator&(const Pattern& l, const std::string& r);
        friend Pattern operator&(const Pattern& l, const Character* r);
        friend Pattern operator&(const Pattern& l, const Pattern& r);
        friend Pattern operator&(const Character l, const Pattern& r);
        friend Pattern operator&(const Pattern& l, const Character r);

        friend Pattern Defer(Pattern&);
        friend Pattern Defer(const std::string&);
        friend Pattern Defer(const StringInterface&);
        friend Pattern Defer(const BoolInterface&);

        friend Pattern Fail();

        friend Pattern Fence();
        friend Pattern Fence(const Pattern&);

        friend Pattern Len(const unsigned int);
        friend Pattern Len(const UnsignedInterface&);
        friend Pattern Len(const unsigned int*);

        friend Pattern NotAny(const Character);
        friend Pattern NotAny(const CharacterSet&);
        friend Pattern NotAny(const std::string&);
        friend Pattern NotAny(const std::string*);
        friend Pattern NotAny(const StringInterface&);

        friend Pattern NSpan(const Character);
        friend Pattern NSpan(const CharacterSet&);
        friend Pattern NSpan(const std::string&);
        friend Pattern NSpan(const std::string*);
        friend Pattern NSpan(const StringInterface&);

        friend Pattern Pos(const unsigned int);
        friend Pattern Pos(const UnsignedInterface&);
        friend Pattern Pos(const unsigned int*);

        friend Pattern Rem();

        friend Pattern Rpos(const unsigned int);
        friend Pattern Rpos(const UnsignedInterface&);
        friend Pattern Rpos(const unsigned int*);

        friend Pattern Rtab(const unsigned int);
        friend Pattern Rtab(const UnsignedInterface&);
        friend Pattern Rtab(const unsigned int*);

        friend Pattern Setcur(unsigned int&);

        friend Pattern Span(const Character);
        friend Pattern Span(const CharacterSet&);
        friend Pattern Span(const std::string&);
        friend Pattern Span(const std::string*);
        friend Pattern Span(const StringInterface&);

        friend Pattern Succeed();

        friend Pattern Tab(const unsigned int);
        friend Pattern Tab(const UnsignedInterface&);
        friend Pattern Tab(const unsigned int*);

        friend Pattern assignOnmatch(const Pattern& p, std::string& var);
        friend Pattern assignImmed(const Pattern& p, std::string& var);

    // Output

        void dump(std::ostream&) const;
        friend std::ostream& operator<<(std::ostream &, const Pattern&);

    // Matching

        friend bool Match
        (
            const Character* subject,
            const Pattern& p,
            int flags
        );

        friend bool Match
        (
            const std::string& subject,
            const Pattern& p,
            int flags
        );

        friend bool Match
        (
            std::string& subject,
            const Pattern& p,
            const std::string& replacement,
            int flags
        );

        friend bool Match
        (
            std::string& subject,
            const Pattern& p,
            const Character* replacement,
            int flags
        );

        friend bool Match
        (
            MatchRes& result,
            const Pattern& p,
            int flags
        );
};


// -----------------------------------------------------------------------------
/// MatchRes: match result object
// -----------------------------------------------------------------------------
class MatchRes
:
    public std::string
{
    int start_;
    int stop_;

    // Subject can be replaced: must not be literal
    friend bool Match
    (
        MatchRes& res,
        const Pattern& p,
        int flags
    );

public:

    MatchRes(const std::string& str)
    :
        std::string(str),
        start_(0),
        stop_(0)
    {}

    inline int start()
    {
        return start_;
    }

    inline int stop()
    {
        return stop_;
    }

    inline bool matched()
    {
        return stop_ > 0;
    }

    bool operator=(const std::string& replacement)
    {
        // debug output??
        if (matched())
        {
            replace(start_, stop_ - start_, replacement);
            return true;
        }
        return false;
    }

    bool operator=(const Character* replacement)
    {
        // debug output??
        if (matched())
        {
            replace(start_, stop_ - start_, replacement);
            return true;
        }
        return false;
    }
};


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
