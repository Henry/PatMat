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
// -----------------------------------------------------------------------------

#include "PatMatInternal.H"
#include "PatMatInternalI.H"

// -----------------------------------------------------------------------------

namespace PatMat
{

// ----------------------------------------------------------------------------
///  Constructors
// ----------------------------------------------------------------------------

Pattern_::Pattern_(const unsigned stackIndex, const PatElmt_ *p)
:
    stackIndex_(stackIndex),
    pe_(p),
    refs_(1)
{}


// ----------------------------------------------------------------------------
///  Destructor
// ----------------------------------------------------------------------------

void Pattern_::free(Pattern_ *p)
{
    // Check the pattern is no longer referenced
    if (p->refs_ == 0 || --p->refs_ == 0)
    {
        delete p;
    }
}


Pattern_::~Pattern_()
{
    // Otherwise we must free all elements
    int n = pe_->index_;
    PatElmt_ *refs[n];

    // References to elements in pattern to be finalized
    buildRefArray(pe_, refs);

    for (int j = 0; j < n; j++)
    {
        switch (refs[j]->pCode_)
        {
            case PC_String:
                delete refs[j]->val.Str;
                break;
            case PC_Any_CS:
            case PC_Break_CS:
            case PC_BreakX_CS:
            case PC_NotAny_CS:
            case PC_NSpan_CS:
            case PC_Span_CS:
                delete refs[j]->val.CS;
                break;
            case PC_Pred_Func:
                (void)refs[j]->val.BF.cookie;
                break;
            case PC_Call_Imm:
            case PC_Call_OnM:
                (void)refs[j]->val.MF.cookie;
                break;
            case PC_Pos_NF:
            case PC_Len_NF:
            case PC_RPos_NF:
            case PC_RTab_NF:
                (void)refs[j]->val.NF.cookie;
                break;
            case PC_Any_VF:
            case PC_Break_VF:
            case PC_BreakX_VF:
            case PC_NotAny_VF:
            case PC_NSpan_VF:
            case PC_Span_VF:
            case PC_String_VF:
                (void)refs[j]->val.VF.cookie;
                break;
            case PC_Dynamic_Func:
                (void)refs[j]->val.DF.cookie;
                break;
            default:
                break;
        }
        delete refs[j];
    }
    pe_ = NULL;
}


// -----------------------------------------------------------------------------
/// recordPE helper function for buildRefArray
// -----------------------------------------------------------------------------
// Record given pattern element if not already recorded in ra,
// and also record any referenced pattern elements recursively.
static void recordPE(const PatElmt_ *e, PatElmt_ **ra)
{
    IDOUT(cout<< "  recordPE called with const PatElmt_ *= " << e;)

    if (e == EOP || ra[e->index_ - 1] != NULL)
    {
        IDOUT(cout<< ", nothing to do\n";)
        return;
    }
    else
    {
        IDOUT(cout<< ", recording " << e->index_ - 1 << endl;)

        ra[e->index_ - 1] = const_cast<PatElmt_*>(e);
        recordPE(e->pNext_, ra);

        if (PCHasAlt(e->pCode_))
            recordPE(e->val.Alt, ra);
    }
}


// -----------------------------------------------------------------------------
///  buildRefArray
// -----------------------------------------------------------------------------
// Given a pattern element which is the leading element of a pattern
// structure, and a Ref_Array with bounds 1 .. E.Index, fills in the
// Ref_Array so that its N'th entry references the element of the referenced
// pattern whose Index value is N.
void buildRefArray(const PatElmt_ *e, PatElmt_ **ra)
{
    IDOUT(cout<< "Entering buildRefArray\n";)
    for (int i = 0; i < e->index_; i++)
    {
        ra[i] = NULL;
    }
    recordPE(e, ra);
    IDOUT(cout<< endl;)
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------
