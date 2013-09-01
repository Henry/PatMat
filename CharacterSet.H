/// Copyright 2013 Henry G. Weller
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
//  PatMat was developed from the SPIPAT and GNAT.SPITBOL.PATTERNS package.
//  GNAT was originally developed by the GNAT team at New York University.
//  Extensive contributions were provided by Ada Core Technologies Inc.
//  SPIPAT was developed by Philip L. Budne.
// -----------------------------------------------------------------------------
/// Title: CharacterSet class
///  Description:
//    C++ implementation of the Ada Character_Set type
///  Standard CharacterSets:
//    CharacterSet representations of the standard C character type functions:
//
//       #include <ctype.h>
//
//       int isalnum(int c);
//       int isalpha(int c);
//       int isascii(int c);
//       int isblank(int c);
//       int iscntrl(int c);
//       int isdigit(int c);
//       int isgraph(int c);
//       int islower(int c);
//       int isprint(int c);
//       int ispunct(int c);
//       int isspace(int c);
//       int isupper(int c);
//       int isxdigit(int c);
// -----------------------------------------------------------------------------

#ifndef CharacterSet_H
#define CharacterSet_H

#include <iostream>
#include <stdint.h>

// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------
/// CharacterSet
// -----------------------------------------------------------------------------

class CharacterSet
{

private:

    // Private data

        static const int charSetSize_ = 256;
        static const int nWords_ = charSetSize_/32;
        uint32_t bitMap_[nWords_];

public:

    // Constructors

        //- Construct empty set
        inline CharacterSet();

        //- Construct character c in the set
        inline CharacterSet(const char c);

        //- Construct with all the characters in string s in the set
        inline CharacterSet(const std::string& s);

        //- Construct with all the characters corresponding to the function isa
        //  in the set
        inline CharacterSet(int (*isa)(int));

    // Member functions

        //- Empty the set
        void clear();

        //- Is character c in the set
        inline bool isIn(const char c) const;
        //isSubset

    // Or member operators

        inline void operator|=(const char c);
        inline void operator|=(const CharacterSet& cs);
        inline void operator|=(const std::string& s);

    // Xor member operators

        inline void operator^=(const char c);
        inline void operator^=(const CharacterSet& cs);
        inline void operator^=(const std::string& s);

    // And member operators

        inline void operator&=(const char c);
        inline void operator&=(const CharacterSet& cs);
        inline void operator&=(const std::string& s);

    // Friend declarations

        //- Return the inverse set
        friend CharacterSet operator~(const CharacterSet& cs);

        friend std::ostream& operator<<
        (
            std::ostream& os,
            const CharacterSet& cs
        );
};


// ----------------------------------------------------------------------------
///  Constructors
// ----------------------------------------------------------------------------
inline CharacterSet::CharacterSet()
{
    clear();
}

inline CharacterSet::CharacterSet(const char c)
{
    clear();
    operator|=(c);
}

inline CharacterSet::CharacterSet(const std::string& s)
{
    clear();
    for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter)
    {
       operator|=(*iter);
    }
}

inline CharacterSet::CharacterSet(int (*isa)(int))
{
    clear();

    for (char c=0; c<127; c++)
    {
        if (isa(c))
        {
            operator|=(c);
        }
    }
}


// ----------------------------------------------------------------------------
///  Empty set
// ----------------------------------------------------------------------------
inline void CharacterSet::clear()
{
    for (int i=0; i<nWords_; i++)
    {
        bitMap_[i] = 0;
    }
}


// ----------------------------------------------------------------------------
///  isIn set
// ----------------------------------------------------------------------------
inline bool CharacterSet::isIn(const char c) const
{
    return (bitMap_[c/32] & (1<<(c%32)));
}


// ----------------------------------------------------------------------------
///  Or
// ----------------------------------------------------------------------------
inline void CharacterSet::operator|=(const char c)
{
     bitMap_[c/32] |= (1<<(c%32));
}

inline void CharacterSet::operator|=(const CharacterSet& cs)
{
    for (int i=0; i<nWords_; i++)
    {
        bitMap_[i] |= cs.bitMap_[i];
    }
}

inline void CharacterSet::operator|=(const std::string& s)
{
    for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter)
    {
       operator|=(*iter);
    }
}

inline CharacterSet operator|(const CharacterSet& cs, char c)
{
    CharacterSet res(cs);
    res |= c;
    return res;
}

inline CharacterSet operator|(const char c, const CharacterSet& cs)
{
    return cs | c;
}

inline CharacterSet operator|(const CharacterSet& cs1, const CharacterSet& cs2)
{
    CharacterSet res(cs1);
    res |= cs2;
    return res;
}

inline CharacterSet operator|(const CharacterSet& cs, const std::string& s)
{
    CharacterSet res(cs);
    res |= s;
    return res;
}

inline CharacterSet operator|(const std::string& s, const CharacterSet& cs)
{
    CharacterSet res(cs);
    res |= s;
    return res;
}


// ----------------------------------------------------------------------------
///  Xor
// ----------------------------------------------------------------------------
inline void CharacterSet::operator^=(const char c)
{
     bitMap_[c/32] ^= (1<<(c%32));
}

inline void CharacterSet::operator^=(const CharacterSet& cs)
{
    for (int i=0; i<nWords_; i++)
    {
        bitMap_[i] ^= cs.bitMap_[i];
    }
}

inline void CharacterSet::operator^=(const std::string& s)
{
    for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter)
    {
       operator^=(*iter);
    }
}

inline CharacterSet operator^(const CharacterSet& cs, char c)
{
    CharacterSet res(cs);
    res ^= c;
    return res;
}

inline CharacterSet operator^(const char c, const CharacterSet& cs)
{
    return cs ^ c;
}

inline CharacterSet operator^(const CharacterSet& cs1, const CharacterSet& cs2)
{
    CharacterSet res(cs1);
    res ^= cs2;
    return res;
}

inline CharacterSet operator^(const CharacterSet& cs, const std::string& s)
{
    CharacterSet res(cs);
    res ^= s;
    return res;
}

inline CharacterSet operator^(const std::string& s, const CharacterSet& cs)
{
    CharacterSet res(cs);
    res ^= s;
    return res;
}

// ----------------------------------------------------------------------------
///  And
// ----------------------------------------------------------------------------
inline void CharacterSet::operator&=(const char c)
{
    operator&=(CharacterSet(c));
}

inline void CharacterSet::operator&=(const CharacterSet& cs)
{
    for (int i=0; i<nWords_; i++)
    {
        bitMap_[i] &= cs.bitMap_[i];
    }
}

inline void CharacterSet::operator&=(const std::string& s)
{
    operator&=(CharacterSet(s));
}

inline CharacterSet operator&(const CharacterSet& cs, char c)
{
    CharacterSet res(cs);
    res &= c;
    return res;
}

inline CharacterSet operator&(const char c, const CharacterSet& cs)
{
    return cs & c;
}

inline CharacterSet operator&(const CharacterSet& cs1, const CharacterSet& cs2)
{
    CharacterSet res(cs1);
    res &= cs2;
    return res;
}

inline CharacterSet operator&(const CharacterSet& cs, const std::string& s)
{
    CharacterSet res(cs);
    res &= s;
    return res;
}

inline CharacterSet operator&(const std::string& s, const CharacterSet& cs)
{
    CharacterSet res(cs);
    res &= s;
    return res;
}


// ----------------------------------------------------------------------------
///  Invert set
// ----------------------------------------------------------------------------
inline CharacterSet operator~(const CharacterSet& cs)
{
    CharacterSet res;

    for (int i=0; i<CharacterSet::nWords_; i++)
    {
        res.bitMap_[i] = ~cs.bitMap_[i];
    }

    return res;
}


// ----------------------------------------------------------------------------
///  inIn
// ----------------------------------------------------------------------------
inline bool isIn(char c, const CharacterSet& set)
{
    return set.isIn(c);
}


// -----------------------------------------------------------------------------
/// CharacterSets
// -----------------------------------------------------------------------------

namespace CharacterSets
{
    extern const CharacterSet alnum;
    extern const CharacterSet alpha;
    extern const CharacterSet ascii;
    extern const CharacterSet blank;
    extern const CharacterSet cntrl;
    extern const CharacterSet digit;
    extern const CharacterSet graph;
    extern const CharacterSet lower;
    extern const CharacterSet print;
    extern const CharacterSet punct;
    extern const CharacterSet space;
    extern const CharacterSet upper;
    extern const CharacterSet xdigit;
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif // CharacterSet_H
// -----------------------------------------------------------------------------
