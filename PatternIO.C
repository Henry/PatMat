/// Copyright 2013 Henry G. Weller
/// Copyright 2007-2010 Philip L. Budne
/// Copyright 1998-2005 AdaCore
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
/// Title: Pattern output
///  Description:
///   dump
//      dump a representation of the internal structure representing the
//      pattern.
///   operator<<(ostream&, const Pattern&)
//      Write a string representation of the pattern.
// -----------------------------------------------------------------------------

#include "Pattern.H"
#include "PatMatInternal.H"

#include <iostream>
#include <iomanip>
using std::endl;

// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------
/// Pattern code names
// -----------------------------------------------------------------------------
#define PATTERN_CODE(X, S, O) #X
static const char *patternCodeSymbols[] =
{
    PATTERN_CODES
};
#undef PATTERN_CODE

#define PATTERN_CODE(X, S, O) S
const char *patternCodeNames[] =
{
    PATTERN_CODES
};
#undef PATTERN_CODE


// -----------------------------------------------------------------------------
/// Forward declarations
// -----------------------------------------------------------------------------
static void writePatternSequence
(
    std::ostream& os,
    const PatElmt_ *e,
    const PatElmt_ *succ,
    PatElmt_ *refs[],
    bool paren
);


// -----------------------------------------------------------------------------
/// writeNodeId
// -----------------------------------------------------------------------------
//  Writes out a string identifying the given pattern element
static void writeNodeId(std::ostream& os, const PatElmt_ *e)
{
    if (!e)
    {
        os  << "NULL  ";
    }
    else if (e == EOP)
    {
        os  << "EOP   ";
    }
    else
    {
        os  << "#" << std::left << std::setw(5) << e->index_;
    }
}


// -----------------------------------------------------------------------------
/// Dump
// -----------------------------------------------------------------------------

void PatMat::Pattern::dump(std::ostream& os) const
{
    const Pattern_ *pat = pat_;
    const PatElmt_ *p = pat->pe_;

    os  << endl
        << "Pattern Dump Output (pattern at "
        << pat << " stack index = " << pat->stackIndex_ << ")\n";

    // If uninitialized pattern, dump line and we are done
    if (p == NULL)
    {
        os  << "Uninitialized pattern value" << endl;
        return;
    }

    // If (null pattern, just dump it and we are all done
    if (p->pCode_ == PC_EOP)
    {
        os  << "EOP (null pattern)" << endl;
        return;
    }

    // We build a reference array whose N'th element points to the
    // pattern element whose index_ value is N.
    PatElmt_ *refs[p->index_];
    buildRefArray(p, refs);

    // Now dump the nodes in reverse sequence. We output them in reverse
    // sequence since this corresponds to the natural order used to
    // construct the patterns.
    for (int j = p->index_ - 1; j >= 0; j--)
    {
        const PatElmt_ *ePtr = refs[j];
        const PatElmt_& e = *ePtr;
        writeNodeId(os, ePtr);
        os  << ePtr << "\t"
            << std::left << std::setw(10) << patternCodeSymbols[e.pCode_];
        writeNodeId(os, e.pNext_);

        switch (e.pCode_)
        {
            case PC_Alt:
            case PC_Arb_X:
            case PC_Arbno_S:
            case PC_Arbno_X:
                writeNodeId(os, e.val.Alt);
                break;

            case PC_Rpat:
                os  << e.val.PP;
                break;

            case PC_Pred_Func:
                os  << e.val.BF.func << '(' << e.val.BF.cookie << ')';
                break;

            case PC_Assign_Imm:
            case PC_Assign_OnM:
            case PC_Any_VP:
            case PC_Break_VP:
            case PC_BreakX_VP:
            case PC_NotAny_VP:
            case PC_NSpan_VP:
            case PC_Span_VP:
            case PC_String_VP:
                os  << e.val.VP;
                break;

            case PC_Call_Imm:
            case PC_Call_OnM:
                os  << e.val.MF.func << '(' << e.val.MF.cookie << ')';
                break;

            case PC_String:
                os  << "\"" << std::setw(e.val.Str->length())
                    << *(e.val.Str)
                    << "\"";
                break;

            case PC_String_2:
                os  << "\"" << std::setw(2);
                os.write(e.val.Str2, 2);
                os  << "\"";
                break;

            case PC_String_3:
                os  << "\"" << std::setw(3);
                os.write(e.val.Str3, 3);
                os  << "\"";
                break;

            case PC_String_4:
                os  << "\"" << std::setw(4);
                os.write(e.val.Str4, 4);
                os  << "\"";
                break;

            case PC_String_5:
                os  << "\"" << std::setw(5);
                os.write(e.val.Str5, 5);
                os  << "\"";
                break;

            case PC_String_6:
                os  << "\"" << std::setw(6);
                os.write(e.val.Str6, 6);
                os  << "\"";
                break;

            case PC_Setcur:
                os  << e.val.NP;
                break;

            case PC_Any_CH:
            case PC_Break_CH:
            case PC_BreakX_CH:
            case PC_Char:
            case PC_NotAny_CH:
            case PC_NSpan_CH:
            case PC_Span_CH:
                os  << '\'' << e.val.Char << '\'';
                break;

            case PC_Any_Set:
            case PC_Break_Set:
            case PC_BreakX_Set:
            case PC_NotAny_Set:
            case PC_NSpan_Set:
            case PC_Span_Set:
                os  << "\"" << e.val.set << "\"";
                break;

            case PC_Arbno_Y:
            case PC_Len_Nat:
            case PC_Pos_Nat:
            case PC_RPos_Nat:
            case PC_RTab_Nat:
            case PC_Tab_Nat:
                os  << e.val.Nat;
                break;

            case PC_Pos_NF:
            case PC_Len_NF:
            case PC_RPos_NF:
            case PC_RTab_NF:
            case PC_Tab_NF:
                os  << e.val.NF.func << '(' << e.val.NF.cookie << ')';
                break;

            case PC_Pos_NP:
            case PC_Len_NP:
            case PC_RPos_NP:
            case PC_RTab_NP:
            case PC_Tab_NP:
                os  << e.val.NP;
                break;

            case PC_Any_VF:
            case PC_Break_VF:
            case PC_BreakX_VF:
            case PC_NotAny_VF:
            case PC_NSpan_VF:
            case PC_Span_VF:
            case PC_String_VF:
                os  << e.val.VF.func << '(' << e.val.VF.cookie << ')';
                break;

            default:
                break;
        }
        os  << endl;
    }
    os  << endl;
}


// -----------------------------------------------------------------------------
/// Write a pattern to ostream
// -----------------------------------------------------------------------------
static const PatElmt_ *writePattern
(
    std::ostream& os,
    const PatElmt_& e,
    PatElmt_ *refs[]
)
{
    // Successor set as result in e unless reset
    const PatElmt_ *eNext = e.pNext_;

    switch (e.pCode_)
    {
        case PC_Alt:
            {
                // Number of elements in left pattern of alternation
                IndexT elmtsInL = e.pNext_->index_ - e.val.Alt->index_;

                // Number of lowest index in elements of left pattern
                IndexT lowestInL = e.index_ - elmtsInL;

                // The successor of the alternation node must have a lower
                // index than any node that is in the left pattern or a
                // higher index than the alternation node itself.
                while
                (
                    eNext != EOP
                 && eNext->index_ >= lowestInL
                 && eNext->index_ < e.index_
                )
                {
                    eNext = eNext->pNext_;
                }

                os  << '(';
                const PatElmt_ *e1 = &e;
                do
                {
                    writePatternSequence(os, e1->pNext_, eNext, refs, false);
                    os  << patternCodeNames[PC_Alt];
                    e1 = e1->val.Alt;
                } while (e1->pCode_ == PC_Alt);

                writePatternSequence(os, e1, eNext, refs, false);
                os  << ')';
                break;
            }

        case PC_Abort:
        case PC_Arb_X:
        case PC_Bal:
        case PC_Fail:
        case PC_Fence:
        case PC_Rem:
        case PC_Succeed:
            os  << patternCodeNames[e.pCode_] << "()";
            break;

        case PC_Any_Set:
        case PC_Break_Set:
        case PC_BreakX_Set:
        case PC_NotAny_Set:
        case PC_NSpan_Set:
        case PC_Span_Set:
            os  << patternCodeNames[e.pCode_]
                << "(\"" << *e.val.set << "\")";
            break;

        case PC_Any_VF:
        case PC_Break_VF:
        case PC_BreakX_VF:
        case PC_NotAny_VF:
        case PC_NSpan_VF:
        case PC_Span_VF:
        case PC_String_VF:
            os  << patternCodeNames[e.pCode_]
                << '(' << e.val.VF.func << '(' << e.val.VF.cookie << "))";
            break;

        case PC_Any_VP:
        case PC_Break_VP:
        case PC_BreakX_VP:
        case PC_NotAny_VP:
        case PC_NSpan_VP:
        case PC_Span_VP:
        case PC_String_VP:
            os  << patternCodeNames[e.pCode_] << '(' << e.val.VP << ')';
            break;

        case PC_Arbno_S:
            os  << patternCodeNames[PC_Arbno_S] << '(';
            writePatternSequence(os, e.val.Alt, &e, refs, false);
            os  << ')';
            break;

        case PC_Arbno_X:
            os  << patternCodeNames[PC_Arbno_X] << '(';
            writePatternSequence
            (
                os,
                e.val.Alt->pNext_,
                refs[e.index_ - 3],
                refs,
                false
            );
            os  << ')';
            break;

        case PC_Assign_Imm:
        case PC_Assign_OnM:
            os  << ' ' << patternCodeNames[e.pCode_] << refs[e.index_]->val.VP;
            break;

        case PC_Any_CH:
        case PC_Break_CH:
        case PC_BreakX_CH:
        case PC_NotAny_CH:
        case PC_NSpan_CH:
        case PC_Span_CH:
            os  << patternCodeNames[e.pCode_] << "('" << e.val.Char << "')";
            break;

        case PC_Char:
            os  << '\'' << e.val.Char << '\'';
            break;

        case PC_Fence_X:
            // PC_R_Enter at refs[e.index_]
            os  << patternCodeNames[PC_Fence_X] << '(';
            writePatternSequence(os, refs[e.index_]->pNext_, &e, refs, false);
            os  << ')';
            break;

        case PC_Len_Nat:
        case PC_Pos_Nat:
        case PC_RPos_Nat:
        case PC_RTab_Nat:
        case PC_Tab_Nat:
            os  << patternCodeNames[e.pCode_] << '(' << e.val.Nat << ')';
            break;

        case PC_Len_NF:
        case PC_Pos_NF:
        case PC_RPos_NF:
        case PC_RTab_NF:
        case PC_Tab_NF:
            os  << patternCodeNames[e.pCode_]
                << '('<<  e.val.NF.func << '(' << e.val.NF.cookie << "))";
            break;

        case PC_Len_NP:
        case PC_Pos_NP:
        case PC_RPos_NP:
        case PC_RTab_NP:
        case PC_Setcur:
        case PC_Tab_NP:
            os  << patternCodeNames[e.pCode_] << '(' <<  e.val.NP << ')';
            break;

        case PC_Null:
            os  << "\"\"";
            break;

        case PC_R_Enter:
            // allows correct processing of PC_Fence_X & PC_Call_*
            //sp.Kill_Concat = true;
            eNext = refs[e.index_ - 2];
            break;

        case PC_Rpat:
            os  << patternCodeNames[e.pCode_]
                << '(' << e.val.PP << ')';
            break;

        case PC_Pred_Func:
            os  << patternCodeNames[e.pCode_]
                << '(' << e.val.BF.func << '(' << e.val.BF.cookie << "))";
            break;

        case PC_Dynamic_Func:
            os  << patternCodeNames[e.pCode_]
                << '(' << e.val.DF.func << '(' << e.val.DF.cookie << "))";
            break;

        case PC_Setcur_Func:
            // XXX not virtualized
            os  << patternCodeNames[e.pCode_]
                << '(' << e.val.CF.func << '(' << e.val.CF.cookie << "))";
            break;

        case PC_String:
            os<< "\"" << *e.val.Str << "\"";
            break;

        case PC_String_2:
            os<< "\"";
            os.write(e.val.Str2, 2);
            os<< "\"";
            break;

        case PC_String_3:
            os<< "\"";
            os.write(e.val.Str3, 3);
            os<< "\"";
            break;

        case PC_String_4:
            os<< "\"";
            os.write(e.val.Str4, 4);
            os<< "\"";
            break;

        case PC_String_5:
            os<< "\"";
            os.write(e.val.Str5, 5);
            os<< "\"";
            break;

        case PC_String_6:
            os<< "\"";
            os.write(e.val.Str6, 6);
            os<< "\"";
            break;

        case PC_Call_Imm:
        case PC_Call_OnM:
            os  << '(';
            writePatternSequence(os, refs[e.index_]->pNext_, &e, refs, true);
            os  << patternCodeNames[e.pCode_];
            os  << e.val.MF.func << '(' << e.val.MF.cookie << "))";
            break;

        case PC_Arb_Y:
        case PC_Arbno_Y:
        case PC_Assign:
        case PC_BreakX_X:
        case PC_EOP:
        case PC_Fence_Y:
        case PC_R_Remove:
        case PC_R_Restore:
        case PC_Unanchored:
            // Other pattern codes should not appear as leading elements
            os  << "???";
            break;
    }

    return eNext;
}


// -----------------------------------------------------------------------------
/// Write a pattern sequence to ostream
// -----------------------------------------------------------------------------
//  e refers to a pattern structure whose successor is given by succ.
//  This function writes a representation of this pattern to the osteeam.
//  The paren parameter indicates whether parentheses are required if
//  the output is more than one element.

static void writePatternSequence
(
    std::ostream& os,
    const PatElmt_ *e,
    const PatElmt_ *succ,
    PatElmt_ *refs[],
    bool paren
)
{
    // The name of EOP is "" (the null string)
    if (e == EOP)
    {
        os  << "\"\"";
    }
    else
    {
        const PatElmt_ *e1 = e;
        bool mult = false;

        // Generate appropriate concatenation sequence
        if (e1 != succ && e1 != EOP)
        {
            mult = true;
            if (paren)
            {
                os  << '(';
            }
        }

        for (;;)
        {
            e1 = writePattern(os, *e1, refs);
            if (e1 == succ || e1 == EOP)
            {
                break;
            }
            // if (sp.Kill_Concat)
            // {
            //     sp.Kill_Concat = false;
            // }
            else
            {
                os  << " & ";
            }
        }

        if (mult && paren)
        {
            os  << ')';
        }
    }
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
/// std::ostream& operator<<(std::ostream& os, const Pattern& p)
// ----------------------------------------------------------------------------

std::ostream& PatMat::operator<<(std::ostream& os, const Pattern& p)
{
    // Build a reference array whose n'th element points to the
    // pattern element whose index_ value is n.
    PatElmt_ *refs[p.pat_->pe_->index_];
    buildRefArray(p.pat_->pe_, refs);

    writePatternSequence(os, p.pat_->pe_, &EOP_Element, refs, false);

    return os;
}


// ----------------------------------------------------------------------------
