#include "PatMatInternal.H"

namespace PatMat
{

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    const PatElmt_ *alt
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Alt = alt;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    Pattern_ **pat
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.PP = pat;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    bool(*func)(void *, void *),
    void *cookie
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.BF.func = func;
    val.BF.cookie = cookie;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    PMString *vp
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.VP = vp;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    void(*func)(const PMString&, void *, void *),
    void *cookie
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.MF.func = func;
    val.MF.cookie = cookie;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    unsigned *Var
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Var = Var;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    void(*func)(unsigned, void *, void *),
    void *cookie
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.CF.func = func;
    val.CF.cookie = cookie;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    const Character c
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Char = c;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    const CharacterSet& CS
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.CS = new CharacterSet(CS);
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    const std::string& s
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.CS = new CharacterSet(s);
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    const unsigned Nat
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.Nat = Nat;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    unsigned(*func)(void *, void *),
    void *cookie
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.NF.func = func;
    val.NF.cookie = cookie;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    const unsigned *NP
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.NP = NP;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    PMString (*func)(void *, void *),
    void *cookie
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.VF.func = func;
    val.VF.cookie = cookie;
}

inline PatElmt_::PatElmt_
(
    const PatternCode pc,
    const IndexT index,
    const PatElmt_ *pNext,
    void(*func)(void *, void *, Dynamic *),
    void *cookie
)
:
    pCode_(pc),
    index_(index),
    pNext_(pNext)
{
    val.DF.func = func;
    val.DF.cookie = cookie;
}

inline bool match(const Character* S1, const PMString& S2)
{
    return S2.compare(0, S2.length(), S1, S2.length()) == 0;
}

inline PMString slice(const PMString& str, unsigned start, unsigned stop)
{
    return str.substr(start - 1, stop - start + 1);
}

inline bool PCHasAlt(PatternCode CODE)
{
    return ((CODE) >= PC_Alt &&  (CODE) <= PC_Arbno_X);
}

}
