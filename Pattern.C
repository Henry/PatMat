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

#include "Pattern.H"

// Inline internal functions
#include "PatMatInternalI.H"

// -----------------------------------------------------------------------------

namespace PatMat
{

// ----------------------------------------------------------------------------
///  Constructors
// ----------------------------------------------------------------------------

Pattern::Pattern()
:
    pat_(NULL)
{}

Pattern::Pattern(unsigned stackIndex, const PatElmt_ *p)
:
    pat_(new Pattern_(stackIndex, p))
{}

Pattern::Pattern(const Pattern& p)
:
    pat_(p.pat_)
{
    debug("Pattern::Pattern(const Pattern& p) ");

    if (pat_)
    {
        debug("Pattern::Pattern(const Pattern& p): hold ");
        pat_->refs_++;
    }
}

Pattern::Pattern(const std::string& str)
:
    pat_(new Pattern_(0, new PatElmt_(str)))
{}

Pattern::Pattern(const Character *str)
:
    pat_(new Pattern_(0, new PatElmt_(str)))
{}

Pattern::Pattern(const Character *str, const unsigned l)
:
    pat_(new Pattern_(0, new PatElmt_(str, l)))
{}

Pattern::Pattern(const Character c)
:
    pat_(new Pattern_(0, new PatElmt_(c)))
{}

Pattern::Pattern(StringInterface* obj)
{
    unsigned l;
    const Character *p = obj->getString(&l);
    pat_ = new Pattern_(0, new PatElmt_(p, l));
}


// ----------------------------------------------------------------------------
///  Destructor
// ----------------------------------------------------------------------------

Pattern::~Pattern()
{
    debug("Pattern::~Pattern() ");
    if (pat_)
    {
        Pattern_::free(pat_);
    }
}


// ----------------------------------------------------------------------------
///  Assignment
// ----------------------------------------------------------------------------

Pattern& Pattern::operator=(const Pattern& p)
{
    debug("Pattern::operator= ");
    if (pat_)
    {
        debug("Pattern::operator= delete ");
        Pattern_::free(pat_);
    }

    pat_ = p.pat_;
    if (pat_)
    {
        debug("Pattern::operator= hold ");
        pat_->refs_++;
    }

    return *this;
}


// ----------------------------------------------------------------------------
///  Helper functions (passed as callbacks)
// ----------------------------------------------------------------------------

static std::string fetch_string_pointer(void *global_cookie, void *local_cookie)
{
    std::string s = *static_cast<std::string*>(local_cookie);
    (void)global_cookie;
    return std::string(s);
}

static std::string fetch_string_object(void *global_cookie, void *local_cookie)
{
    StringInterface *obj = static_cast<StringInterface*>(local_cookie);
    unsigned l;
    const Character *p = obj->getString(&l);
    (void)global_cookie;
    return std::string(p, l);
}

static unsigned fetch_unsigned_object(void *global_cookie, void *local_cookie)
{
    UnsignedInterface *obj = static_cast<UnsignedInterface*>(local_cookie);
    (void)global_cookie;
    return obj->getUnsigned();
}

static bool fetch_bool_object(void *global_cookie, void *local_cookie)
{
    BoolInterface *obj = static_cast<BoolInterface*>(local_cookie);
    (void)global_cookie;
    return obj->getBool();
}


// ----------------------------------------------------------------------------
///  Alternation
// ----------------------------------------------------------------------------

Pattern& Pattern::operator|=(const std::string& r)
{
    return *this = *this | r;
}

Pattern& Pattern::operator|=(const Character *r)
{
    return *this = *this | r;
}

Pattern& Pattern::operator|=(const Character r)
{
    return *this = *this | r;
}

Pattern& Pattern::operator|=(const Pattern& r)
{
    return *this = *this | r;
}

inline Pattern Pattern::orStrPat(const std::string& l, const Pattern& r)
{
    return Pattern
    (
        r.pat_->stackIndex_ + 1,
        alternate(new PatElmt_(l),
        copy(r.pat_->pe_))
    );
}

inline Pattern Pattern::orPatStr(const Pattern& l, const std::string& r)
{
    return Pattern
    (
        l.pat_->stackIndex_ + 1,
        alternate(copy(l.pat_->pe_),
        new PatElmt_(r))
    );
}

inline Pattern Pattern::orStrStr(const std::string& l, const std::string& r)
{
    return Pattern(1, alternate(new PatElmt_(l), new PatElmt_(r)));
}

Pattern operator|(const std::string& l, const Pattern& r)
{
    return Pattern::orStrPat(std::string(l), r);
}

Pattern operator|(const Character *l, const Pattern& r)
{
    return Pattern::orStrPat(std::string(l), r);
}

Pattern operator|(const Pattern& l, const std::string& r)
{
    return Pattern::orPatStr(l, std::string(r));
}

Pattern operator|(const Pattern& l, const Character *r)
{
    return Pattern::orPatStr(l, std::string(r));
}

Pattern operator|(const std::string& l, const std::string& r)
{
    return Pattern::orStrStr(std::string(l), std::string(r));
}

Pattern operator|(const Character *l, const std::string& r)
{
    return Pattern::orStrStr(std::string(l), std::string(r));
}

Pattern operator|(const std::string& l, const Character *r)
{
    return Pattern::orStrStr(std::string(l), std::string(r));
}

Pattern Or(const Character *l, const Character *r)
{
    return Pattern::orStrStr(std::string(l), std::string(r));
}

template<class T>
inline bool max(T a, T b)
{
    return (((a) > (b)) ? (a) : (b));
}

Pattern operator|(const Pattern& l, const Pattern& r)
{
    return Pattern
    (
        max(l.pat_->stackIndex_, r.pat_->stackIndex_) + 1,
        alternate(copy(l.pat_->pe_), copy(r.pat_->pe_))
    );
}

Pattern operator|(const Character l, const Pattern& r)
{
    return Pattern(1, alternate(new PatElmt_(l), copy(r.pat_->pe_)));
}

Pattern operator|(const Pattern& l, const Character r)
{
    return Pattern(1, alternate(copy(l.pat_->pe_), new PatElmt_(r)));
}

Pattern operator|(const std::string& l, const Character r)
{
    return Pattern(1, alternate(new PatElmt_(l), new PatElmt_(r)));
}

Pattern Or(const Character *l, const Character r)
{
    return Pattern(1, alternate(new PatElmt_(l), new PatElmt_(r)));
}

Pattern operator|(const Character l, const std::string& r)
{
    return Pattern(1, alternate(new PatElmt_(l), new PatElmt_(r)));
}

Pattern Or(const Character l, const Character *r)
{
    return Pattern(1, alternate(new PatElmt_(l), new PatElmt_(r)));
}

Pattern Or(const Character l, const Character r)
{
    return Pattern(1, alternate(new PatElmt_(l), new PatElmt_(r)));
}


// ----------------------------------------------------------------------------
///  Any
// ----------------------------------------------------------------------------

Pattern Any(const Character c)
{
    return Pattern(0, new PatElmt_(PC_Any_CH, 1, EOP, c));
}

Pattern Any(const CharacterSet& set)
{
    return Pattern(0, new PatElmt_(PC_Any_Set, 1, EOP, set));
}

Pattern Any(const std::string& str)
{
    return Pattern(0, new PatElmt_(PC_Any_Set, 1, EOP, str));
}

Pattern Any(std::string* str)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Any_VF, 1, EOP, fetch_string_pointer, str)
    );
}

Pattern Any(StringInterface* obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Any_VF, 1, EOP, fetch_string_object, obj)
    );
}


// ----------------------------------------------------------------------------
///  Arb
// ----------------------------------------------------------------------------
//    +---+
//    | x |---->
//    +---+
//      .
//      .
//    +---+
//    | y |---->
//    +---+
//
//  The PC_Arb_X element is numbered 2, and the PC_Arb_Y element is 1
//
//      The following array is used to determine if a pattern used as an
//      argument for Arbno is eligible for treatment using the simple Arbno
//      structure (i.e. it is a pattern that is guaranteed to match at least
//      one character on success, and not to make any entries on the stack.

#define PATTERN_CODE(X, S, O) O
static const unsigned char OK_For_Simple_Arbno[] =
{
    PATTERN_CODES
};
#undef PATTERN_CODE

Pattern Arb()
{
    const PatElmt_ *y = new PatElmt_(PC_Arb_Y, 1, EOP);
    const PatElmt_ *x = new PatElmt_(PC_Arb_X, 2, EOP, y);
    return Pattern(1, x);
}


// ----------------------------------------------------------------------------
///  Arbno
// ----------------------------------------------------------------------------
// arbnoSimple
//
//      +-------------+
//      |             ^
//      V             |
//    +---+           |
//    | S |---->      |
//    +---+           |
//      .             |
//      .             |
//    +---+           |
//    | p |---------->+
//    +---+
//
//  The node numbering of the constituent pattern p is not affected.
//  The S node has a node number of P->index_ + 1.
//
//  Note that we know that p cannot be EOP, because a null pattern
//  does not meet the requirements for simple Arbno.

Pattern Arbno(const Character c)
{
    return Pattern(0, arbnoSimple(new PatElmt_(c)));
}

Pattern Arbno(const std::string& str)
{
    if (str.length() == 0)
    {
        return Pattern(0, EOP);
    }
    else
    {
        return Pattern(0, arbnoSimple(new PatElmt_(str)));
    }
}

Pattern Arbno(const Character *s)
{
    return Arbno(std::string(s));
}

Pattern Arbno(const Pattern& p)
{
    PatElmt_ *pe = copy(p.pat_->pe_);

    if (p.pat_->stackIndex_ == 0 && OK_For_Simple_Arbno[pe->pCode_])
    {
        return Pattern(0, arbnoSimple(pe));
    }
    else
    {
        // This is the complex case, either the pattern makes stack entries
        // or it is possible for the pattern to match the null string (more
        // accurately, we don't know that this is not the case).

        // +--------------------------+
        // | ^
        // V |
        // +---+ |
        // | x |----> |
        // +---+ |
        // .  |
        // .  |
        // +---+ +---+ +---+ |
        // | e |---->| p |---->| y |--->+
        // +---+ +---+ +---+

        // The node numbering of the constituent pattern p is not affected.
        // Where n is the number of nodes in p, the y node is numbered n + 1,
        // the e node is n + 2, and the x node is n + 3.
        PatElmt_ *e = new PatElmt_(PC_R_Enter, 0, EOP);
        PatElmt_ *x = new PatElmt_(PC_Arbno_X, 0, EOP, e);
        PatElmt_ *y = new PatElmt_(PC_Arbno_Y, 0, x, p.pat_->stackIndex_ + 3);
        PatElmt_ *epy = bracket(e, pe, y);

        x->val.Alt = epy;
        x->index_ = epy->index_ + 1;
        return Pattern(p.pat_->stackIndex_ + 3, x);
    }
}


// ----------------------------------------------------------------------------
///  Assignment
// ----------------------------------------------------------------------------

static void put_string_pointer
(
    const std::string& str,
    void *global_cookie,
    void *local_cookie
)
{
    *static_cast<std::string*>(local_cookie) = str;
    (void)global_cookie;
}

static void put_string_object
(
    const std::string& str,
    void *global_cookie,
    void *local_cookie
)
{
    StringInterface *obj = static_cast<StringInterface*>(local_cookie);
    (void)global_cookie;
    obj->putString(str);
}

static void output_string
(
    const std::string& str,
    void *global_cookie,
    void *local_cookie
)
{
    std::ostream *stream = static_cast<std::ostream*>(local_cookie);
    *stream
        << str
        << '\n'; // SNOBOL doesn't, Ada does
    (void)global_cookie;
}


// ----------------------------------------------------------------------------
///  Assign on match
// ----------------------------------------------------------------------------
//    +---+     +---+     +---+
//    | e |---->| p |---->| a |---->
//    +---+     +---+     +---+
//
//  The node numbering of the constituent pattern p is not affected.
//  Where n is the number of nodes in p, the a node is numbered n + 1,
//  and the e node is n + 2.

inline Pattern assignOnmatch(const Pattern& p, std::string *var)
{
    PatElmt_ *pe = copy(p.pat_->pe_);
    PatElmt_ *e = new PatElmt_(PC_R_Enter, 0, EOP);
    PatElmt_ *a = new PatElmt_(PC_Assign_OnM, 0, EOP, var);
    return Pattern(p.pat_->stackIndex_ + 3, bracket(e, pe, a));
}


// ----------------------------------------------------------------------------
///  Call on match
// ----------------------------------------------------------------------------
//    +---+     +---+     +---+
//    | e |---->| p |---->| c |---->
//    +---+     +---+     +---+
//
//  The node numbering of the constituent pattern p is not affected.
//  Where n is the number of nodes in p, the W node is numbered n + 1,
//  and the e node is n + 2.

inline Pattern Pattern::callOnmatch
(
    const Pattern& p,
    void (*func) (const std::string&, void *, void *),
    void *cookie
)
{
    PatElmt_ *pe = copy(p.pat_->pe_);
    PatElmt_ *e = new PatElmt_(PC_R_Enter, 0, EOP);
    PatElmt_ *c = new PatElmt_(PC_Call_OnM, 0, EOP, func, cookie);
    return Pattern(p.pat_->stackIndex_ + 3, bracket(e, pe, c));
}

Pattern operator*(const Pattern& p, std::string& str)
{
    return Pattern::callOnmatch(p, put_string_pointer, &str);
}

Pattern operator*(const Pattern& p, StringInterface * obj)
{
    return Pattern::callOnmatch(p, put_string_object, obj);
}

// Could use a StringObject that does output on "putString"!
Pattern operator*(const Pattern& p, std::ostream& stream)
{
    return Pattern::callOnmatch(p, output_string, &stream);
}


// ----------------------------------------------------------------------------
///  Assign immediate
// ----------------------------------------------------------------------------
//    +---+     +---+     +---+
//    | e |---->| p |---->| a |---->
//    +---+     +---+     +---+
//
//  The node numbering of the constituent pattern p is not affected.
//  Where n is the number of nodes in p, the a node is numbered n + 1,
//  and the e node is n + 2.

inline Pattern assignImmed(const Pattern& p, std::string *var)
{
    PatElmt_ *pe = copy(p.pat_->pe_);
    PatElmt_ *e = new PatElmt_(PC_R_Enter, 0, EOP);
    PatElmt_ *a = new PatElmt_(PC_Assign_Imm, 0, EOP, var);
    return Pattern(p.pat_->stackIndex_ + 3, bracket(e, pe, a));
}


// ----------------------------------------------------------------------------
///  Call immediate
// ----------------------------------------------------------------------------
//    +---+     +---+     +---+
//    | e |---->| p |---->| c |---->
//    +---+     +---+     +---+
//
//  The node numbering of the constituent pattern p is not affected.
//  Where n is the number of nodes in p, the W node is numbered n + 1,
//  and the e node is n + 2.

inline Pattern Pattern::callImmed
(
    const Pattern& p,
    void (*func) (const std::string&, void *, void *),
    void *cookie
)
{
    PatElmt_ *Pat = copy(p.pat_->pe_);
    PatElmt_ *e = new PatElmt_(PC_R_Enter, 0, EOP);
    PatElmt_ *c = new PatElmt_(PC_Call_Imm, 0, EOP, func, cookie);
    return Pattern(3, bracket(e, Pat, c));
}

Pattern operator%(const Pattern& p, std::string& var)
{
    return Pattern::callImmed(p, put_string_pointer, &var);
}

Pattern operator%(const Pattern& p, StringInterface * obj)
{
    return Pattern::callImmed(p, put_string_object, obj);
}

// could use a StringObject that does output on "putString"!
Pattern operator%(const Pattern& p, std::ostream& stream)
{
    return Pattern::callImmed(p, output_string, &stream);
}


// ----------------------------------------------------------------------------
///  Bal
// ----------------------------------------------------------------------------

Pattern Bal()
{
    return Pattern(1, new PatElmt_(PC_Bal, 1, EOP));
}


// ----------------------------------------------------------------------------
///  Break
// ----------------------------------------------------------------------------

Pattern Break(const Character c)
{
    return Pattern(0, new PatElmt_(PC_Break_CH, 1, EOP, c));
}

Pattern Break(const CharacterSet& set)
{
    return Pattern(0, new PatElmt_(PC_Break_Set, 1, EOP, set));
}

Pattern Break(const std::string& str)
{
    return Pattern(0, new PatElmt_(PC_Break_Set, 1, EOP, str));
}

Pattern Break(std::string *str)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Break_VF, 1, EOP, fetch_string_pointer, str)
    );
}

Pattern Break(StringInterface* obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Break_VF, 1, EOP, fetch_string_object, obj)
    );
}


// ----------------------------------------------------------------------------
///  BreakX
// ----------------------------------------------------------------------------
//    +---+     +---+
//    | b |---->| a |---->
//    +---+     +---+
//      ^         .
//      |         .
//      |       +---+
//      +<------| x |
//              +---+
//
//  The b node is numbered 3, the alternative node is 1, and the x
//  node is 2.

inline Pattern BreakXMake(PatElmt_ *b)
{
    PatElmt_ *x = new PatElmt_(PC_BreakX_X, 2, b);
    PatElmt_ *a = new PatElmt_(PC_Alt, 1, EOP, x);
    b->pNext_ = a;
    return Pattern(2, b);
}

Pattern BreakX(const Character c)
{
    return BreakXMake(new PatElmt_(PC_BreakX_CH, 3, NULL, c));
}

Pattern BreakX(const CharacterSet& set)
{
    return BreakXMake(new PatElmt_(PC_BreakX_Set, 3, NULL, set));
}

Pattern BreakX(const std::string& str)
{
    return BreakXMake(new PatElmt_(PC_BreakX_Set, 3, NULL, str));
}

Pattern BreakX(std::string *str)
{
    return BreakXMake
    (
        new PatElmt_(PC_BreakX_VF, 3, NULL, fetch_string_pointer, str)
    );
}

Pattern BreakX(StringInterface* obj)
{
    return BreakXMake
    (
        new PatElmt_(PC_BreakX_VF, 3, NULL, fetch_string_object, obj)
    );
}


// ----------------------------------------------------------------------------
///  Abort (Cancel)
// ----------------------------------------------------------------------------

Pattern Abort()
{
    return Pattern(0, new PatElmt_(PC_Abort, 1, EOP));
}

// ----------------------------------------------------------------------------
///  Deferred evaluation
// ----------------------------------------------------------------------------

// DANGEROUS if Pattern lifetime longer than referenced Pattern variable!!
Pattern Defer(Pattern& p)
{
    // NOT const! must be a variable!
    return Pattern(3, new PatElmt_(PC_Rpat, 1, EOP, &p.pat_));
}

Pattern Defer(std::string& str)
{
    // NOT const! must be a variable!
    return Pattern
    (
        0,
        new PatElmt_(PC_String_VF, 1, EOP, fetch_string_pointer, &str)
    );
}

Pattern Defer(StringInterface& obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_String_VF, 1, EOP, fetch_string_object, &obj)
    );
}

Pattern Defer(BoolInterface& obj)
{
    return Pattern
    (
        3,
        new PatElmt_(PC_Pred_Func, 1, EOP, fetch_bool_object, &obj)
    );
}


// ----------------------------------------------------------------------------
///  Fail
// ----------------------------------------------------------------------------

Pattern Fail()
{
    return Pattern(0, new PatElmt_(PC_Fail, 1, EOP));
}


// ----------------------------------------------------------------------------
///  Fence
// ----------------------------------------------------------------------------

// Simple case
Pattern Fence()
{
    return Pattern(1, new PatElmt_(PC_Fence, 1, EOP));
}

// Function case
//
//    +---+     +---+     +---+
//    | e |---->| p |---->| x |---->
//    +---+     +---+     +---+
//
//  The node numbering of the constituent pattern p is not affected.
//  Where n is the number of nodes in p, the x node is numbered n + 1,
//  and the e node is n + 2.
Pattern Fence(const Pattern& p)
{
    PatElmt_ *pe = copy(p.pat_->pe_);
    PatElmt_ *e = new PatElmt_(PC_R_Enter, 0, EOP);
    PatElmt_ *x = new PatElmt_(PC_Fence_X, 0, EOP);
    return Pattern(p.pat_->stackIndex_ + 1, bracket(e, pe, x));
}


// ----------------------------------------------------------------------------
///  Len
// ----------------------------------------------------------------------------
Pattern Len(unsigned count)
{
    // Note, the following is not just an optimization, it is needed
    // to ensure that Arbno (Len (0)) does not generate an infinite
    // matching loop (since PC_Len_Nat is OK_For_Simple_Arbno).
    if (count == 0)
    {
        return Pattern(0, new PatElmt_(PC_Null, 1, EOP));
    }
    else
    {
        return Pattern(0, new PatElmt_(PC_Len_Nat, 1, EOP, count));
    }
}

Pattern Len(UnsignedInterface* obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Len_NF, 1, EOP, fetch_unsigned_object, obj)
    );
}

Pattern Len(const unsigned *count)
{
    return Pattern(0, new PatElmt_(PC_Len_NP, 1, EOP, count));
}


// ----------------------------------------------------------------------------
///  NotAny
// ----------------------------------------------------------------------------

Pattern NotAny(const Character c)
{
    return Pattern(0, new PatElmt_(PC_NotAny_CH, 1, EOP, c));
}

Pattern NotAny(const CharacterSet& set)
{
    return Pattern(0, new PatElmt_(PC_NotAny_Set, 1, EOP, set));
}

Pattern NotAny(const std::string& str)
{
    return Pattern(0, new PatElmt_(PC_NotAny_Set, 1, EOP, str));
}

Pattern NotAny(std::string* str)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_NotAny_VF, 1, EOP, fetch_string_pointer, str)
    );
}

Pattern NotAny(StringInterface * obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_NotAny_VF, 1, EOP, fetch_string_object, obj)
    );
}


// ----------------------------------------------------------------------------
///  NSpan
// ----------------------------------------------------------------------------

Pattern NSpan(const Character c)
{
    return Pattern(0, new PatElmt_(PC_NSpan_CH, 1, EOP, c));
}

Pattern NSpan(const CharacterSet& set)
{
    return Pattern(0, new PatElmt_(PC_NSpan_Set, 1, EOP, set));
}

Pattern NSpan(const std::string& str)
{
    return Pattern(0, new PatElmt_(PC_NSpan_Set, 1, EOP, str));
}

Pattern NSpan(std::string *str)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_NSpan_VF, 1, EOP, fetch_string_pointer, str)
    );
}

Pattern NSpan(StringInterface * obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_NSpan_VF, 1, EOP, fetch_string_object, obj)
    );
}


// ----------------------------------------------------------------------------
///  Pos
// ----------------------------------------------------------------------------

Pattern Pos(unsigned count)
{
    return Pattern(0, new PatElmt_(PC_Pos_Nat, 1, EOP, count));
}

Pattern Pos(UnsignedInterface *obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Pos_NF, 1, EOP, fetch_unsigned_object, obj)
    );
}

Pattern Pos(const unsigned *Ptr)
{
    return Pattern(0, new PatElmt_(PC_Pos_NP, 1, EOP, Ptr));
}


// ----------------------------------------------------------------------------
///  Rem
// ----------------------------------------------------------------------------

Pattern Rem()
{
    return Pattern(0, new PatElmt_(PC_Rem, 1, EOP));
}


// ----------------------------------------------------------------------------
///  Rpos
// ----------------------------------------------------------------------------

Pattern Rpos(unsigned count)
{
    return Pattern(0, new PatElmt_(PC_RPos_Nat, 1, EOP, count));
}

Pattern Rpos(UnsignedInterface *obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_RPos_NF, 1, EOP, fetch_unsigned_object, obj)
    );
}

Pattern Rpos(const unsigned *count)
{
    return Pattern(0, new PatElmt_(PC_RPos_NP, 1, EOP, count));
}


// ----------------------------------------------------------------------------
///  Rtab
// ----------------------------------------------------------------------------

Pattern Rtab(unsigned count)
{
    return Pattern(0, new PatElmt_(PC_RTab_Nat, 1, EOP, count));
}

Pattern Rtab(UnsignedInterface *obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_RTab_NF, 1, EOP, fetch_unsigned_object, obj)
    );
}

Pattern Rtab(const unsigned *count)
{
    return Pattern(0, new PatElmt_(PC_RTab_NP, 1, EOP, count));
}


// ----------------------------------------------------------------------------
///  Setcur
// ----------------------------------------------------------------------------

Pattern Setcur(unsigned &var)
{
    // not const; must be a variable!
    return Pattern(0, new PatElmt_(PC_Setcur, 1, EOP, &var));
}


// ----------------------------------------------------------------------------
///  Span
// ----------------------------------------------------------------------------

Pattern Span(const Character c)
{
    return Pattern(0, new PatElmt_(PC_Span_CH, 1, EOP, c));
}

Pattern Span(const CharacterSet& set)
{
    return Pattern(0, new PatElmt_(PC_Span_Set, 1, EOP, set));
}

Pattern Span(const std::string& str)
{
    return Pattern(0, new PatElmt_(PC_Span_Set, 1, EOP, str));
}

Pattern Span(std::string *str)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Span_VF, 1, EOP, fetch_string_pointer, str)
    );
}

Pattern Span(StringInterface *obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Span_VF, 1, EOP, fetch_string_object, obj)
    );
}


// ----------------------------------------------------------------------------
///  Succeed
// ----------------------------------------------------------------------------

Pattern Succeed()
{
    return Pattern(1, new PatElmt_(PC_Succeed, 1, EOP));
}


// ----------------------------------------------------------------------------
///  Tab
// ----------------------------------------------------------------------------

Pattern Tab(unsigned count)
{
    return Pattern(0, new PatElmt_(PC_Tab_Nat, 1, EOP, count));
}

Pattern Tab(UnsignedInterface *obj)
{
    return Pattern
    (
        0,
        new PatElmt_(PC_Tab_NF, 1, EOP, fetch_unsigned_object, obj)
    );
}

Pattern Tab(const unsigned *count)
{
    return Pattern(0, new PatElmt_(PC_Tab_NP, 1, EOP, count));
}


// ----------------------------------------------------------------------------
///  Concatenation
// ----------------------------------------------------------------------------
// NOTE: {string,char} + {string,char} omitted-- use string concatenation!
//
//  Concat needs to traverse the left operand performing the following
//  set of fixups:
//
//    a) Any successor pointers (pNext_ fields) that are set to EOP are
//       reset to point to the second operand.
//
//    b) Any PC_Arbno_Y node has its stack count field incremented
//       by the parameter Incr provided for this purpose.
//
//    d) Num fields of all pattern elements in the left operand are
//       adjusted to include the elements of the right operand.
//
//  Note: we do not use setSuccessor in the processing for Concat, since
//  there is no point in doing two traversals, we may as well do everything
//  at the same time.

Pattern& Pattern::operator&=(const Character r)
{
    return *this = *this & r;
}

Pattern& Pattern::operator&=(const std::string& r)
{
    return *this = *this & r;
}

Pattern& Pattern::operator&=(const Character *r)
{
    return *this = *this & r;
}

Pattern& Pattern::operator&=(const Pattern& r)
{
    return *this = *this & r;
}

Pattern operator&(const std::string& l, const Pattern& r)
{
    return Pattern
    (
        r.pat_->stackIndex_,
        concat(new PatElmt_(l), copy(r.pat_->pe_), r.pat_->stackIndex_)
    );
}

Pattern operator&(const Character *l, const Pattern& r)
{
    return Pattern
    (
        r.pat_->stackIndex_,
        concat(new PatElmt_(l), copy(r.pat_->pe_), r.pat_->stackIndex_)
    );
}

Pattern operator&(const Pattern& l, const std::string& r)
{
    return Pattern
    (
        l.pat_->stackIndex_,
        concat(copy(l.pat_->pe_), new PatElmt_(r), 0)
    );
}

Pattern operator&(const Pattern& l, const Character *r)
{
    return Pattern
    (
        l.pat_->stackIndex_,
        concat(copy(l.pat_->pe_), new PatElmt_(r), 0)
    );
}

Pattern operator&(const Pattern& l, const Pattern& r)
{
    return Pattern
    (
        l.pat_->stackIndex_ + r.pat_->stackIndex_,
        concat(copy(l.pat_->pe_), copy(r.pat_->pe_), r.pat_->stackIndex_)
    );
}

Pattern operator&(const Character l, const Pattern& r)
{
    return Pattern
    (
        r.pat_->stackIndex_,
        concat(new PatElmt_(l), copy(r.pat_->pe_), r.pat_->stackIndex_)
    );
}

Pattern operator&(const Pattern& l, const Character r)
{
    return Pattern
    (
        l.pat_->stackIndex_,
        concat(copy(l.pat_->pe_), new PatElmt_(r), 0)
    );
}


// ----------------------------------------------------------------------------
///  Match
// ----------------------------------------------------------------------------

bool Match(const Character *subject, const Pattern& p, int flags)
{
    MatchState ma;
    ma.flags = flags;
    ma.subject = subject;
    ma.pattern = p.pat_;
    ma.matchCookie = 0;
    // XXX check for MATCH_EXCEPTION, throw exception!?
    return match(ma) == MATCH_SUCCESS;
}

bool Match(const std::string& subject, const Pattern& p, int flags)
{
    MatchState ma;
    ma.flags = flags;
    ma.subject = subject;
    ma.pattern = p.pat_;
    ma.matchCookie = 0;
    // XXX check for MATCH_EXCEPTION, throw exception!?
    return match(ma) == MATCH_SUCCESS;
}


// ----------------------------------------------------------------------------
///  Match & Replace
// ----------------------------------------------------------------------------

bool Match
(
    std::string& subject,
    const Pattern& p,
    const std::string& replacement,
    int flags
)
{
    MatchState ma;
    ma.flags = flags;
    ma.subject = subject;
    ma.pattern = p.pat_;
    ma.matchCookie = 0;

    // XXX check for MATCH_EXCEPTION, throw exception!?
    if (match(ma) != MATCH_SUCCESS)
    {
        return false;
    }

    subject.replace(ma.start - 1, ma.stop - ma.start + 1, replacement);

    return true;
}

bool Match
(
    std::string& subject,
    const Pattern& p,
    const Character *replacement,
    int flags
)
{
    MatchState ma;
    ma.flags = flags;
    ma.subject = subject;
    ma.pattern = p.pat_;
    ma.matchCookie = 0;

    // XXX check for MATCH_EXCEPTION, throw exception!?
    if (match(ma) != MATCH_SUCCESS)
    {
        return false;
    }

    subject.replace(ma.start - 1, ma.stop - ma.start + 1, replacement);

    return true;
}

bool Match
(
    MatchRes& result,
    const Pattern& p,
    int flags
)
{
    MatchState ma;
    ma.flags = flags;
    ma.subject = result;
    ma.pattern = p.pat_;
    ma.matchCookie = 0;

    // XXX check for MATCH_EXCEPTION, throw exception!?
    if (match(ma) == MATCH_SUCCESS)
    {
        result.start_ = ma.start - 1;
        result.stop_ = ma.stop;
        return true;
    }

    return false;
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------
