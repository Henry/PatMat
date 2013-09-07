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
/// Title: Pattern class
///  Description:
// -----------------------------------------------------------------------------

#include "PatMatInternal.H"
#include "PatMatInternalI.H"

#include <string.h>
#include <cstdlib>

// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------
/// Exceptions
// -----------------------------------------------------------------------------

void __dead patMatException(const char *msg)
{
    std::cout<< "PatMat exception: " << msg<< std::endl;
    std::exit(1);
}

void __dead uninitializedPattern()
{
    patMatException("uninitializedPattern");
}


// ----------------------------------------------------------------------------
/// PatElmt: Pattern Element functions
// ----------------------------------------------------------------------------

PatElmt_::PatElmt_(Character c)
:
    pCode_(PC_Char),
    index_(1),
    pNext_(EOP)
{
    val.Char = c;
}

PatElmt_::PatElmt_(const std::string& str)
:
    pCode_(PC_Null),
    index_(1),
    pNext_(EOP)
{
    switch (str.length())
    {
        case 0:
            pCode_ = PC_Null;
            break;
        case 1:
            pCode_ = PC_Char;
            val.Char = str[0];
            break;
        case 2:
            pCode_ = PC_String_2;
            str.copy(val.Str2, 2);
            break;
        case 3:
            pCode_ = PC_String_3;
            str.copy(val.Str3, 3);
            break;
        case 4:
            pCode_ = PC_String_4;
            str.copy(val.Str4, 4);
            break;
        case 5:
            pCode_ = PC_String_5;
            str.copy(val.Str5, 5);
            break;
        case 6:
            pCode_ = PC_String_6;
            str.copy(val.Str6, 6);
            break;
        default:
            pCode_ = PC_String;
            val.Str = new std::string(str);
            break;
    }
}

void PatElmt_::setStr(const Character* str, const unsigned l)
{
    switch (l)
    {
        case 0:
            pCode_ = PC_Null;
            break;
        case 1:
            pCode_ = PC_Char;
            val.Char = str[0];
            break;
        case 2:
            pCode_ = PC_String_2;
            for (int i=0; i<2; i++) val.Str2[i] = str[i];
            break;
        case 3:
            pCode_ = PC_String_3;
            for (int i=0; i<3; i++) val.Str3[i] = str[i];
            break;
        case 4:
            pCode_ = PC_String_4;
            for (int i=0; i<4; i++) val.Str4[i] = str[i];
            break;
        case 5:
            pCode_ = PC_String_5;
            for (int i=0; i<5; i++) val.Str5[i] = str[i];
            break;
        case 6:
            pCode_ = PC_String_6;
            for (int i=0; i<6; i++) val.Str6[i] = str[i];
            break;
        default:
            pCode_ = PC_String;
            val.Str = new std::string(str, l);
            break;
    }
}

PatElmt_::PatElmt_(const Character* str, const unsigned l)
:
    pCode_(PC_Null),
    index_(1),
    pNext_(EOP)
{
    setStr(str, l);
}

PatElmt_::PatElmt_(const Character* str)
:
    pCode_(PC_Null),
    index_(1),
    pNext_(EOP)
{
    setStr(str, strlen(str));
}


// ----------------------------------------------------------------------------
///  Copy
// ----------------------------------------------------------------------------

PatElmt_ *copy(const PatElmt_ *P)
{
    if (P == NULL)
    {
        uninitializedPattern();
        return NULL;
    }
    else
    {
        // References to elements in P, indexed by index_ field
        PatElmt_ *E;
        PatElmt_ *Refs[P->index_];

        // Holds copies of elements of P, indexed by index_ field
        PatElmt_ *Copies[P->index_];

        buildRefArray(P, Refs);

        // Now copy all nodes
        for (int j = 0; j < P->index_; j++)
        {
            Copies[j] = new PatElmt_(*Refs[j]);
        }

        // Adjust all internal references
        for (int j = 0; j < P->index_; j++)
        {
            E = Copies[j];

            // Adjust successor pointer to point to copy
            if (E->pNext_ != EOP)
                E->pNext_ = Copies[E->pNext_->index_ - 1];

            // Adjust Alt pointer if there is one to point to copy
            if (PCHasAlt(E->pCode_) && E->val.Alt != EOP)
                E->val.Alt = Copies[E->val.Alt->index_ - 1];

            // Copy referenced string or CharacterSet
            switch (E->pCode_)
            {
                case PC_String:
                    E->val.Str = new std::string(*(E->val.Str));
                    break;
                case PC_Any_Set:
                case PC_Break_Set:
                case PC_BreakX_Set:
                case PC_NotAny_Set:
                case PC_NSpan_Set:
                case PC_Span_Set:
                    E->val.set = new CharacterSet(*(E->val.set));
                    break;
                default:
                    break;
            }

        } // for j
        return Copies[P->index_ - 1];
    } // else
} // Copy


// ----------------------------------------------------------------------------
///  Set-successor
// ----------------------------------------------------------------------------
// Note: this procedure is not used by the normal concatenation circuit,
// since other fixups are required on the left operand in this case, and
// they might as well be done all together.

void setSuccessor(const PatElmt_ *pe, const PatElmt_ *succ)
{
    if (pe == NULL)
    {
        uninitializedPattern();
    }
    else if (pe == EOP)
    {
        patMatException("Error in setSuccessor");
    }
    else
    {
        PatElmt_ *Refs[pe->index_];
        // We build a reference array for L whose N'th element points to
        // the pattern element of L whose original index_ value is N.

        buildRefArray(pe, Refs);

        for (int j = 0; j < pe->index_; j++)
        {
            PatElmt_ *p = Refs[j];
            if (p->pNext_ == EOP)
            {
                p->pNext_ = succ;
            }
            if (PCHasAlt(p->pCode_) && p->val.Alt == EOP)
            {
                p->val.Alt = succ;
            }
        }
    }
}


// ----------------------------------------------------------------------------
///  Alternation
// ----------------------------------------------------------------------------

PatElmt_ *alternate(const PatElmt_ *l, const PatElmt_ *r)
{
    // If the left pattern is null, then we just add the alternation
    // node with an index one greater than the right hand pattern.
    if (l == EOP)
    {
        return new PatElmt_(PC_Alt, r->index_ + 1, EOP, r);
    }
    else
    {
        // If the left pattern is non-null, then build a reference vector
        // for its elements, and adjust their index values to acccomodate
        // the right hand elements. Then add the alternation node.
        int n = l->index_;
        PatElmt_ *Refs[n];

        buildRefArray(l, Refs);

        for (int j = 0; j < n; j++)
        {
            Refs[j]->index_ += r->index_;
        }
        return new PatElmt_(PC_Alt, l->index_ + 1, l, r);
    }
}


// ----------------------------------------------------------------------------
///  Arbno
// ----------------------------------------------------------------------------
PatElmt_ *arbnoSimple(const PatElmt_ *p)
{
    PatElmt_ *s = new PatElmt_(PC_Arbno_S, p->index_ + 1, EOP, p);
    setSuccessor(p, s);
    return s;
}


// ----------------------------------------------------------------------------
///  Bracket
// ----------------------------------------------------------------------------
PatElmt_ *bracket(PatElmt_ *e, PatElmt_ *p, PatElmt_ *a)
{
    if (p == EOP)
    {
        e->pNext_ = a;
        e->index_ = 2;
        a->index_ = 1;
    }
    else
    {
        e->pNext_ = p;
        setSuccessor(p, a);
        e->index_ = p->index_ + 2;
        a->index_ = p->index_ + 1;
    }

    return e;
}


// ----------------------------------------------------------------------------
///  Concatenation
// ----------------------------------------------------------------------------
const PatElmt_ *concat(const PatElmt_ *l, const PatElmt_ *r, unsigned incr)
{
    if (l == EOP)
    {
        return r;
    }

    if (r == EOP)
    {
        return l;
    }

    // We build a reference array for l whose N'th element points to
    // the pattern element of l whose original index_ value is N.
    int n = l->index_;
    PatElmt_ *Refs[n];

    buildRefArray(l, Refs);

    for (int j = 0; j < n; j++)
    {
        PatElmt_ *p = Refs[j];

        p->index_ += r->index_;

        if (p->pCode_ == PC_Arbno_Y)
            p->val.Nat += incr;

        if (p->pNext_ == EOP)
            p->pNext_ = r;

        if (PCHasAlt(p->pCode_) && p->val.Alt == EOP)
            p->val.Alt = r;
    }   // for

    return l;
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------
