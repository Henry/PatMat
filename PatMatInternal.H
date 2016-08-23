/// Copyright 2013-2016 Henry G. Weller
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
/// Title: Internal Data Structures
///  Description:
// -----------------------------------------------------------------------------

#ifndef PatMatInternal_H
#define PatMatInternal_H

#include "Pattern.H"

// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------
/// Forward declarations
// -----------------------------------------------------------------------------
class PatElmt_;

std::ostream& operator<<(std::ostream& os, const PatElmt_& pe);


// -----------------------------------------------------------------------------
/// Pattern_: internal pattern object
// -----------------------------------------------------------------------------
class Pattern_
{
public:

    // Stack index
    Natural stackIndex_;

    // The pattern element tree
    const PatElmt_* pe_;

    // Reference count
    Natural refs_;

    // Constructor
    Pattern_(const Natural stackIndex, const PatElmt_* p);

    // Destructor
    ~Pattern_();

    // Decrement reference count and delete if reference count -> 0
    static void free(Pattern_* p);
};


#define PATTERN_CODES                                                          \
    PATTERN_CODE(Abort, "Abort", 0),                                           \
    PATTERN_CODE(Arb_Y, "Arb", 0),                                             \
    PATTERN_CODE(Assign, "Assign", 0),                                         \
    PATTERN_CODE(Bal, "Bal", 0),                                               \
    PATTERN_CODE(BreakX_X, "BreakX", 0),                                       \
    PATTERN_CODE(EOP, "EOP", 0),                                               \
    PATTERN_CODE(Fail, "Fail", 0),                                             \
    PATTERN_CODE(Fence, "Fence", 0),                                           \
    PATTERN_CODE(Fence_X, "Fence", 0),                                         \
    PATTERN_CODE(Fence_Y, "Fence", 0),                                         \
    PATTERN_CODE(R_Enter, "Enter", 0),                                         \
    PATTERN_CODE(R_Remove, "Remove", 0),                                       \
    PATTERN_CODE(R_Restore, "Restore", 0),                                     \
    PATTERN_CODE(Rem, "Rem", 0),                                               \
    PATTERN_CODE(Succeed, "Succeed", 0),                                       \
    PATTERN_CODE(Unanchored, "Unanchored", 0),                                 \
                                                                               \
    PATTERN_CODE(Alt, " | ", 0),                                               \
    PATTERN_CODE(Arb_X, "Arb", 0),                                             \
    PATTERN_CODE(Arbno_S, "Arbno", 0),                                         \
    PATTERN_CODE(Arbno_X, "Arbno", 0),                                         \
                                                                               \
    PATTERN_CODE(Rpat, "Defer", 0),                                            \
                                                                               \
    PATTERN_CODE(Pred_Func, "Defer", 0),                                       \
                                                                               \
    PATTERN_CODE(Assign_Imm, " . ", 0),                                        \
    PATTERN_CODE(Assign_OnM, " $ ", 0),                                        \
    PATTERN_CODE(Any_SP, "Any", 1),                                            \
    PATTERN_CODE(Break_SP, "Break", 0),                                        \
    PATTERN_CODE(BreakX_SP, "BreakX", 0),                                      \
    PATTERN_CODE(NotAny_SP, "NotAny", 1),                                      \
    PATTERN_CODE(NSpan_SP, "NSpan", 0),                                        \
    PATTERN_CODE(Span_SP, "Span", 1),                                          \
    PATTERN_CODE(String_SP, "String", 0),                                      \
                                                                               \
    PATTERN_CODE(Call_Imm_SS, " * ", 0),                                       \
    PATTERN_CODE(Call_OnM_SS, " % ", 0),                                       \
                                                                               \
    PATTERN_CODE(Call_Imm_SV, " * ", 0),                                       \
    PATTERN_CODE(Call_OnM_SV, " % ", 0),                                       \
                                                                               \
    PATTERN_CODE(Null, "\"\"", 0),                                             \
                                                                               \
    PATTERN_CODE(String, "String", 1),                                         \
    PATTERN_CODE(String_2, "String", 1),                                       \
    PATTERN_CODE(String_3, "String", 1),                                       \
    PATTERN_CODE(String_4, "String", 1),                                       \
    PATTERN_CODE(String_5, "String", 1),                                       \
    PATTERN_CODE(String_6, "String", 1),                                       \
                                                                               \
    PATTERN_CODE(Setcur, "Setcur", 0),                                         \
    PATTERN_CODE(Setcur_Func, "Setcur", 0),                                    \
                                                                               \
    PATTERN_CODE(Any_CH, "Any", 1),                                            \
    PATTERN_CODE(Break_CH, "Break", 0),                                        \
    PATTERN_CODE(BreakX_CH, "BreakX", 0),                                      \
    PATTERN_CODE(Char, "Char", 1),                                             \
    PATTERN_CODE(NotAny_CH, "NotAny", 1),                                      \
    PATTERN_CODE(NSpan_CH, "NSpan", 0),                                        \
    PATTERN_CODE(Span_CH, "Span", 1),                                          \
                                                                               \
    PATTERN_CODE(Any_Set, "Any", 1),                                           \
    PATTERN_CODE(Break_Set, "Break", 0),                                       \
    PATTERN_CODE(BreakX_Set, "BreakX", 0),                                     \
    PATTERN_CODE(NotAny_Set, "NotAny", 1),                                     \
    PATTERN_CODE(NSpan_Set, "NSpan", 0),                                       \
    PATTERN_CODE(Span_Set, "Span", 1),                                         \
                                                                               \
    PATTERN_CODE(Arbno_Y, "Arbno", 0),                                         \
    PATTERN_CODE(Len_Nat, "Len", 1),                                           \
    PATTERN_CODE(Pos_Nat, "Pos", 0),                                           \
    PATTERN_CODE(RPos_Nat, "RPos", 0),                                         \
    PATTERN_CODE(RTab_Nat, "RTab", 0),                                         \
    PATTERN_CODE(Tab_Nat, "Tab", 0),                                           \
                                                                               \
    PATTERN_CODE(Pos_NG, "Pos", 0),                                            \
    PATTERN_CODE(Len_NG, "Len", 0),                                            \
    PATTERN_CODE(RPos_NG, "RPos", 0),                                          \
    PATTERN_CODE(RTab_NG, "RTab", 0),                                          \
    PATTERN_CODE(Tab_NG, "Tab", 0),                                            \
                                                                               \
    PATTERN_CODE(Pos_NP, "Pos", 0),                                            \
    PATTERN_CODE(Len_NP, "Len", 0),                                            \
    PATTERN_CODE(RPos_NP, "RPos", 0),                                          \
    PATTERN_CODE(RTab_NP, "RTab", 0),                                          \
    PATTERN_CODE(Tab_NP, "Tab", 0),                                            \
                                                                               \
    PATTERN_CODE(Any_SG, "Any", 1),                                            \
    PATTERN_CODE(Break_SG, "Break", 0),                                        \
    PATTERN_CODE(BreakX_SG, "BreakX", 0),                                      \
    PATTERN_CODE(NotAny_SG, "NotAny", 1),                                      \
    PATTERN_CODE(NSpan_SG, "NSpan", 0),                                        \
    PATTERN_CODE(Span_SG, "Span", 1),                                          \
    PATTERN_CODE(String_SG, "String", 0),

#define PATTERN_CODE(X, S, O) PC_##X
enum PatternCode
{
    PATTERN_CODES
};
#undef PATTERN_CODE

typedef uint16_t IndexT;

class PatElmt_
{
    public:

    PatternCode pCode_;

    //- Serial index number of pattern element within pattern
    IndexT index_;

    //- Successor element), to be matched after this one
    const PatElmt_* pNext_;

    union Val
    {
        // PC_Arb_Y | PC_Assign | PC_BreakX_X | PC_Cancel | PC_EOP |
        // PC_Fail | PC_Fence | PC_Fence_X | PC_Fence_Y | PC_Null | PC_R_Enter
        // | PC_R_Remove | PC_R_Restore | PC_Rest | PC_Succeed | PC_Unanchored
        // => null;

        // PC_Alt | PC_Arb_X | PC_Arbno_S | PC_Arbno_X
        const PatElmt_* Alt;

        // PC_Rpat
        Pattern_*const* PP;

        // PC_Pred_Func
        const BoolGetter* BG;

        // PC_Assign_Imm | PC_Assign_OnM | PC_Call_Imm_SV | PC_Call_OnM_SV
        std::string* SV;

        // PC_Any_SP | PC_Break_SP | PC_BreakX_SP | PC_NotAny_SP | PC_NSpan_SP |
        // PC_Span_SP | PC_String_SP
        const std::string* SP;

        // PC_Call_Imm_SS PC_Call_OnM_SS
        StringSetter* SS;

        // PC_String
        std::string* Str;

        // PC_String_2
        Character Str2[2];

        // PC_String_3
        Character Str3[3];

        // PC_String_4
        Character Str4[4];

        // PC_String_5
        Character Str5[5];

        // PC_String_6
        Character Str6[6];

        // PC_Setcur
        Natural* NV;

        // PC_Setcur_Func
        UnsignedSetter* NS;

        // PC_Any_CH | PC_Break_CH | PC_BreakX_CH | PC_Char | PC_NotAny_CH |
        // PC_NSpan_CH | PC_Span_CH
        Character Char;

        // PC_Bal
        struct
        {
            Character open;
            Character close;
        };

        // PC_Any_Set | PC_Break_Set | PC_BreakX_set | PC_NotAny_Set
        // | PC_NSpan_Set | PC_Span_Set
        CharacterSet* set;

        // PC_Arbno_Y | PC_Len_Nat | PC_Pos_Nat | PC_RPos_Nat | PC_RTab_Nat |
        // PC_Tab_Nat
        Natural Nat;

        // PC_Pos_NG | PC_Len_NG | PC_RPos_NG | PC_RTab_NG |
        const UnsignedGetter* NG;

        // PC_Pos_NP | PC_Len_NP | PC_RPos_NP | PC_RTab_NP | PC_Tab_NP
        const Natural* NP;

        // PC_Any_SG | PC_Break_SG | PC_BreakX_SG | PC_NotAny_SG | PC_NSpan_SG
        // | PC_Span_SG | PC_String_SG
        const StringGetter* SG;

    } val;

    // Constructors

        PatElmt_()
        {}


        PatElmt_(Character c);
        PatElmt_(const std::string& str);
        void setStr(const Character* str, const Natural l);
        PatElmt_(const Character* str, const Natural l);
        PatElmt_(const Character* str);

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const Character c
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const Character open,
            const Character close
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const PatElmt_* alt
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            Pattern_*const* pat
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const BoolGetter* iPtr
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            std::string* sp
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const std::string* vp
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            StringSetter* ss
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            Natural* var
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            UnsignedSetter* iPtr
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const CharacterSet& set
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const std::string& s
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const Natural n
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const UnsignedGetter* iPtr
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const Natural* nPtr
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_* pNext,
            const StringGetter* iPtr
        );


    // Output

        friend std::ostream& operator<<(std::ostream &, const PatElmt_&);
};


// -----------------------------------------------------------------------------
/// Match data and functions
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

    const std::string& subject;

    // Output from match
    Natural start, stop;
    MatchRet ret;

    MatchState(const std::string& str)
    :
        subject(str),
        ret(MATCH_FAILURE)
    {}

    operator bool() const
    {
        return ret == MATCH_SUCCESS;
    }
};

MatchRet match(MatchState&, const Pattern_* pattern, const Flags flags);


// -----------------------------------------------------------------------------
/// PatElmt function declarations
// -----------------------------------------------------------------------------
PatElmt_* copy(const PatElmt_* P);
PatElmt_* alternate(const PatElmt_* L, const PatElmt_* R);
PatElmt_* arbnoSimple(const PatElmt_* P);
PatElmt_* bracket(PatElmt_* E, PatElmt_* P, PatElmt_* A);
const PatElmt_* concat(const PatElmt_* L, const PatElmt_* R, Natural Incr);
void setSuccessor(const PatElmt_* Pat, const PatElmt_* Succ);
void buildRefArray(const PatElmt_* E, PatElmt_** RA);


// -----------------------------------------------------------------------------
/// EOP: End of pattern indicator
// -----------------------------------------------------------------------------
extern const PatElmt_ EOP_Element;
extern const PatElmt_* EOP;


// -----------------------------------------------------------------------------
/// Exceptions
// -----------------------------------------------------------------------------

#ifndef __dead
#ifdef __GNUC__
#define __dead __attribute__((__noreturn__))
#else
#define __dead
#endif
#endif


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
#endif // PatMatInternal_H
// -----------------------------------------------------------------------------
