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

#include "PatMatInternal.H"

// -----------------------------------------------------------------------------

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const PatElmt_* alt
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Alt = alt;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    Pattern_*const* pat
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.PP = pat;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const BoolGetter* iPtr
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.BG = iPtr;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    std::string* sp
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.SP = sp;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const std::string* vp
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.SP = vp;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    StringSetter* ss
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.SS = ss;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    Natural* nv
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.NV = nv;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    UnsignedSetter* iPtr
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.NS = iPtr;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const Character c
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Char = c;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const Character open,
    const Character close
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.open = open;
    val.close = close;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const CharacterSet& set
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.set = new CharacterSet(set);
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const std::string& s
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.set = new CharacterSet(s);
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const Natural Nat
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Nat = Nat;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const UnsignedGetter* iPtr
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.NG = iPtr;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const Natural* NP
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.NP = NP;
}

inline PatMat::PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_* pNext,
    const StringGetter* iPtr
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.SG = iPtr;
}


// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------

inline bool PCHasAlt(PatternCode CODE)
{
    return ((CODE) >= PC_Alt &&  (CODE) <= PC_Arbno_X);
}


// -----------------------------------------------------------------------------
/// std::string
// -----------------------------------------------------------------------------

inline bool match(const Character* S1, const std::string& S2)
{
    return S2.compare(0, S2.length(), S1, S2.length()) == 0;
}

inline bool isInStr(Character c, const std::string& S)
{
    return S.find(c) != std::string::npos;
}

inline std::string slice
(
    const std::string& str,
    Natural start,
    unsigned stop
)
{
    return str.substr(start - 1, stop - start + 1);
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------
