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


// -----------------------------------------------------------------------------
/// Experimental Dynamic function
// -----------------------------------------------------------------------------

struct Dynamic
{
    enum
    {
        DY_VSTR,
        DY_BOOL,
        DY_PAT,
        DY_UNK
    } type;

    union Val
    {
        std::string *str;          // type == DY_VSTR
        bool pred;              // type == DY_BOOL
        struct
        {                       // type == DY_PAT
            Pattern_ *p;
            void (*release) (void *cookie);
            void *cookie;
        } pat;
    } val;
};

Pattern_ *dynamicFunc
(
    void (*func) (void *, void *, Dynamic *),
    void *cookie
);


// -----------------------------------------------------------------------------
/// Pattern_: internal pattern object
// -----------------------------------------------------------------------------
class Pattern_
{
public:

    // Stack index
    unsigned stackIndex_;

    // The pattern element tree
    const PatElmt_ *pe_;

    // Reference count
    unsigned refs_;

    // Constructor
    Pattern_(const unsigned stackIndex, const PatElmt_ *p);

    // Destructor
    ~Pattern_();

    // Decrement reference count and delete if reference count -> 0
    static void free(Pattern_ *p);
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
    PATTERN_CODE(Any_VP, "Any", 1),                                            \
    PATTERN_CODE(Break_VP, "Break", 0),                                        \
    PATTERN_CODE(BreakX_VP, "BreakX", 0),                                      \
    PATTERN_CODE(NotAny_VP, "NotAny", 1),                                      \
    PATTERN_CODE(NSpan_VP, "NSpan", 0),                                        \
    PATTERN_CODE(Span_VP, "Span", 1),                                          \
    PATTERN_CODE(String_VP, "String", 0),                                      \
                                                                               \
    PATTERN_CODE(Call_Imm, " * ", 0),                                          \
    PATTERN_CODE(Call_OnM, " % ", 0),                                          \
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
    PATTERN_CODE(Any_Set, "Any", 1),                                            \
    PATTERN_CODE(Break_Set, "Break", 0),                                        \
    PATTERN_CODE(BreakX_Set, "BreakX", 0),                                      \
    PATTERN_CODE(NotAny_Set, "NotAny", 1),                                      \
    PATTERN_CODE(NSpan_Set, "NSpan", 0),                                        \
    PATTERN_CODE(Span_Set, "Span", 1),                                          \
                                                                               \
    PATTERN_CODE(Arbno_Y, "Arbno", 0),                                         \
    PATTERN_CODE(Len_Nat, "Len", 1),                                           \
    PATTERN_CODE(Pos_Nat, "Pos", 0),                                           \
    PATTERN_CODE(RPos_Nat, "RPos", 0),                                         \
    PATTERN_CODE(RTab_Nat, "RTab", 0),                                         \
    PATTERN_CODE(Tab_Nat, "Tab", 0),                                           \
                                                                               \
    PATTERN_CODE(Pos_NF, "Pos", 0),                                            \
    PATTERN_CODE(Len_NF, "Len", 0),                                            \
    PATTERN_CODE(RPos_NF, "RPos", 0),                                          \
    PATTERN_CODE(RTab_NF, "RTab", 0),                                          \
    PATTERN_CODE(Tab_NF, "Tab", 0),                                            \
                                                                               \
    PATTERN_CODE(Pos_NP, "Pos", 0),                                            \
    PATTERN_CODE(Len_NP, "Len", 0),                                            \
    PATTERN_CODE(RPos_NP, "RPos", 0),                                          \
    PATTERN_CODE(RTab_NP, "RTab", 0),                                          \
    PATTERN_CODE(Tab_NP, "Tab", 0),                                            \
                                                                               \
    PATTERN_CODE(Any_VF, "Any", 1),                                            \
    PATTERN_CODE(Break_VF, "Break", 0),                                        \
    PATTERN_CODE(BreakX_VF, "BreakX", 0),                                      \
    PATTERN_CODE(NotAny_VF, "NotAny", 1),                                      \
    PATTERN_CODE(NSpan_VF, "NSpan", 0),                                        \
    PATTERN_CODE(Span_VF, "Span", 1),                                          \
    PATTERN_CODE(String_VF, "String", 0),                                      \
                                                                               \
    PATTERN_CODE(Dynamic_Func, "Dynamic", 0)

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
    const PatElmt_ *pNext_;

    union Val
    {
        // PC_Arb_Y | PC_Assign | PC_Bal | PC_BreakX_X | PC_Cancel | PC_EOP |
        // PC_Fail | PC_Fence | PC_Fence_X | PC_Fence_Y | PC_Null | PC_R_Enter
        // | PC_R_Remove | PC_R_Restore | PC_Rest | PC_Succeed | PC_Unanchored
        // => null;

        // PC_Alt | PC_Arb_X | PC_Arbno_S | PC_Arbno_X
        const PatElmt_ *Alt;

        // PC_Rpat
        Pattern_ **PP;

        // PC_Pred_Func
        struct
        {
            bool(*func) (void *, void *);
            void *cookie;
        } BF;

        // PC_Assign_Imm_VP | PC_Assign_OnM_VP | PC_Any_VP | PC_Break_VP |
        // PC_BreakX_VP | PC_NotAny_VP | PC_NSpan_VP | PC_Span_VP |
        // PC_String_VP
        std::string *VP;

        // PC_Call_Imm PC_Call_OnM
        struct
        {
            void (*func) (const std::string&, void *, void *);
            void *cookie;
        } MF;

        // PC_String
        std::string *Str;

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
        unsigned *Var;

        // PC_Setcur_Func
        struct
        {
            void (*func) (unsigned, void *, void *);
            void *cookie;
        } CF;

        // PC_Any_CH | PC_Break_CH | PC_BreakX_CH | PC_Char | PC_NotAny_CH |
        // PC_NSpan_CH | PC_Span_CH
        Character Char;

        // PC_Any_Set | PC_Break_Set | PC_BreakX_set | PC_NotAny_Set | PC_NSpan_Set
        // | PC_Span_Set
        CharacterSet *set;

        // PC_Arbno_Y | PC_Len_Nat | PC_Pos_Nat | PC_RPos_Nat | PC_RTab_Nat |
        // PC_Tab_Nat
        unsigned Nat;

        // PC_Pos_NF | PC_Len_NF | PC_RPos_NF | PC_RTab_NF |
        struct
        {
            unsigned (*func) (void *, void *);
            void *cookie;
        } NF;

        // PC_Pos_NP | PC_Len_NP | PC_RPos_NP | PC_RTab_NP | PC_Tab_NP
        const unsigned *NP;

        // PC_Any_VF | PC_Break_VF | PC_BreakX_VF | PC_NotAny_VF | PC_NSpan_VF
        // | PC_Span_VF | PC_String_VF
        struct
        {
            std::string (*func) (void *, void *);
            void *cookie;
        } VF;

        // PC_Dynamic_Func
        struct
        {
            void (*func) (void *, void *, Dynamic *);
            void *cookie;
        } DF;
    } val;

    // Constructors

        PatElmt_()
        {}


        PatElmt_(Character c);
        PatElmt_(const std::string& str);
        void setStr(const Character* str, const unsigned l);
        PatElmt_(const Character* str, const unsigned l);
        PatElmt_(const Character* str);

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            const Character Char
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            const PatElmt_ *alt
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            Pattern_ **pat
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            bool(*func)(void *, void *),
            void *cookie
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            std::string *vp
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            void(*func)(const std::string&, void *, void *),
            void *cookie
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            unsigned *Var
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            void(*func)(unsigned, void *, void *),
            void *cookie
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            const CharacterSet& set
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            const std::string& s
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            const unsigned Nat
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            unsigned(*func)(void *, void *),
            void *cookie
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            const unsigned *NP
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            std::string (*func)(void *, void *),
            void *cookie
        );

        inline PatElmt_
        (
            const PatternCode pc,
            const IndexT index,
            const PatElmt_ *pNext,
            void(*func)(void *, void *, Dynamic *),
            void *cookie
        );
};


// -----------------------------------------------------------------------------
/// Match data and functions
// -----------------------------------------------------------------------------

struct MatchState
{
    int flags;
    std::string subject;
    Pattern_ *pattern;
    void *matchCookie;
    unsigned start, stop;  // Output from match
    const char *exception;
};

enum MatchRet
{
    MATCH_EXCEPTION,
    MATCH_FAILURE,
    MATCH_SUCCESS
};

MatchRet match(MatchState&);


// -----------------------------------------------------------------------------
/// PatElmt function declarations
// -----------------------------------------------------------------------------
PatElmt_ *copy(const PatElmt_ *P);
PatElmt_ *alternate(const PatElmt_ *L, const PatElmt_ *R);
PatElmt_ *arbnoSimple(const PatElmt_ *P);
PatElmt_ *bracket(PatElmt_ *E, PatElmt_ *P, PatElmt_ *A);
const PatElmt_ *concat(const PatElmt_ *L, const PatElmt_ *R, unsigned Incr);
void setSuccessor(const PatElmt_ *Pat, const PatElmt_ *Succ);
void buildRefArray(const PatElmt_ *E, PatElmt_** RA);


// -----------------------------------------------------------------------------
/// EOP: End of pattern indicator
// -----------------------------------------------------------------------------
extern const PatElmt_ EOP_Element;
extern const PatElmt_ *EOP;


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
