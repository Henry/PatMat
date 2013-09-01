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

#include "CharacterSet.H"
#include <ctype.h>

namespace PatMat
{

// ----------------------------------------------------------------------------
///  Write to ostream
// ----------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const CharacterSet& cs)
{
    for (char c=0; c<127; c++)
    {
        if (cs.isIn(c))
        {
            os<< c;
        }
    }
    return os;
}


// -----------------------------------------------------------------------------
/// CharacterSets
// -----------------------------------------------------------------------------

namespace CharacterSets
{
    const CharacterSet alnum(isalnum);
    const CharacterSet alpha(isalpha);
    const CharacterSet ascii(isascii);
    const CharacterSet blank(isblank);
    const CharacterSet cntrl(iscntrl);
    const CharacterSet digit(isdigit);
    const CharacterSet graph(isgraph);
    const CharacterSet lower(islower);
    const CharacterSet print(isprint);
    const CharacterSet punct(ispunct);
    const CharacterSet space(isspace);
    const CharacterSet upper(isupper);
    const CharacterSet xdigit(isxdigit);
}

// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
