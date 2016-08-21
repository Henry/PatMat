// ----------------------------------------------------------------------------
/// Pattern functions and operators
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
///  Abort
// ----------------------------------------------------------------------------
// Constructs a pattern that immediately aborts the entire match
Pattern Abort();

// ----------------------------------------------------------------------------
///  Alternation
// ----------------------------------------------------------------------------
// Creates a pattern that will first try to match l and then on a subsequent
// failure, attempts to match r instead.
// Matches l followed by r
// SPITBOL: binary "|"
// C++: binary "|"

Pattern operator|(const std::string& l, const Pattern& r);
Pattern operator|(const Character *l, const Pattern& r);
Pattern operator|(const Pattern& l, const std::string& r);
Pattern operator|(const Pattern& l, const Character *r);
Pattern operator|(const std::string& l, const std::string& r);
Pattern operator|(const Character *l, const std::string& r);
Pattern operator|(const std::string& l, const Character *r);
Pattern Or(const Character *l, const Character *r);
Pattern operator|(const Pattern& l, const Pattern& r);
Pattern operator|(Character l, const Pattern& r);
Pattern operator|(const Pattern& l, Character r);
Pattern operator|(const std::string& l, Character r);
Pattern Or(const Character *l, Character r);
Pattern operator|(Character l, const std::string& r);
Pattern Or(Character l, const Character *r);

// ----------------------------------------------------------------------------
/// Any
// ----------------------------------------------------------------------------
// Constructs a pattern that matches a single character that is one of the
// characters in the given argument. The pattern fails if the current character
// is not in str.

Pattern Any(Character c);
Pattern Any(const CharacterSet& set);
Pattern Any(const std::string& str);
Pattern Any(const std::string *str);
Pattern Any(StringInterface&);

// ----------------------------------------------------------------------------
/// Arb
// ----------------------------------------------------------------------------
//
// Constructs a pattern that will match any string. On the first attempt, the
// pattern matches a null string, then on each successive failure, it matches
// one more character, and only fails if matching the entire rest of the string.

Pattern Arb();

// ----------------------------------------------------------------------------
///  Arbno
// ----------------------------------------------------------------------------
//
// Pattern repetition. First matches null, then on a subsequent failure attempts
// to match an additional instance of the given pattern.  Equivalent to (but
// more efficient than) P & ("" | (P & ("" | ...

Pattern Arbno(Character c);
Pattern Arbno(const std::string& str);
Pattern Arbno(const Character *str);
Pattern Arbno(const Pattern& p);

// ----------------------------------------------------------------------------
///  Assignment immediately
// ----------------------------------------------------------------------------
//
// Matches P, and if the match succeeds, assigns the matched sub-string to the
// given std::string variable S. This assignment happens as soon as the
// sub-string is matched, and if the pattern P1 is matched more than once during
// the course of the match, then the assignment will occur more than once.
//
// SPITBOL: binary "$"
// C++: binary "%"

Pattern operator%(const Pattern& p, std::string& var);
Pattern operator%(const Pattern& p, StringInterface& obj);

// ----------------------------------------------------------------------------
///  Assignment on match
// ----------------------------------------------------------------------------
//
// Like "%" above, except that the assignment happens at most once after the
// entire match is completed successfully. If the match fails, then no
// assignment takes place.
//
// SPITBOL: binary "."
// C++: binary "*"

Pattern operator*(const Pattern& p, std::string& var);
Pattern operator*(const Pattern& p, StringInterface& obj);

// ----------------------------------------------------------------------------
///  Bal
// ----------------------------------------------------------------------------
//
// Constructs a pattern that will match any non-empty string that is parentheses
// balanced with respect to the parentheses characters open and close.
// Attempts to extend the string if a subsequent failure occurs.
Pattern Bal(const Character open, const Character close);

// ----------------------------------------------------------------------------
///  Break
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches a (possibly null) string which is
// immediately followed by a character in the given argument. This character is
// not part of the matched string. The pattern fails if the remaining characters
// to be matched do not include any of the characters in str.
Pattern Break(Character str);
Pattern Break(const CharacterSet& set);
Pattern Break(const std::string& str);
Pattern Break(const std::string *str);
Pattern Break(StringInterface&);

// ----------------------------------------------------------------------------
///  BreakX
// ----------------------------------------------------------------------------
//
// Like Break, but the pattern attempts to extend on a failure to find the next
// occurrence of a character in str, and only fails when the last such instance
// causes a failure.
Pattern BreakX(Character str);
Pattern BreakX(const CharacterSet& set);
Pattern BreakX(const std::string& str);
Pattern BreakX(const std::string *str);
Pattern BreakX(StringInterface&);

// ----------------------------------------------------------------------------
///  Concatenation operators
// ----------------------------------------------------------------------------
//
// Matches L followed by r
// SPITBOL: binary " " operator
// C++: binary "&" operator

Pattern operator&(const std::string& l, const Pattern& r);
Pattern operator&(const Pattern& l, const std::string& r);
Pattern operator&(const Pattern& l, const Pattern& r);
Pattern operator&(Character l, const Pattern& r);
Pattern operator&(const Pattern& l, Character r);

// ----------------------------------------------------------------------------
///  Deferred Matching
// ----------------------------------------------------------------------------
// SPITBOL: unary "*"
// C++: unary "+"

//- This function constructs a pattern which at pattern matching time will
//  access the current value of this variable, and match against the pattern
//  value.
//
//  Here p must be a Pattern variable.
//
//  DANGEROUS if Pattern lifetime longer than referenced variable!!
Pattern Defer(Pattern& p);

//- This function constructs a pattern which at pattern matching time will
//  access the current value of this variable, and match against these
//  characters.
//
//  Here str must be a std::string variable.
Pattern Defer(const std::string& str);

//- Constructs a pattern which at pattern matching time calls the given
//  function, and then matches against the string or character value that is
//  returned by the call.
Pattern Defer(const StringInterface& obj);

//- Constructs a predicate pattern function that at pattern matching time calls
//  the given function. If True is returned, then the pattern matches.  If False
//  is returned, then failure is signalled.
Pattern Defer(const BoolInterface& obj);

inline Pattern operator+(Pattern& p)
{
    return Defer(p);
}

inline Pattern operator+(const std::string& s)
{
    return Defer(s);
}

inline Pattern operator+(const StringInterface& obj)
{
    return Defer(obj);
}

inline Pattern operator+(const BoolInterface& obj)
{
    return Defer(obj);
}

// ----------------------------------------------------------------------------
///  Fail
// ----------------------------------------------------------------------------
// Constructs a pattern that always fails
Pattern Fail();

// ----------------------------------------------------------------------------
///  Fence
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches null on the first attempt, and then causes
// the entire match to be aborted if a subsequent failure occurs.
Pattern Fence();

// Constructs a pattern that first matches P. if P fails, then the constructed
// pattern fails. If P succeeds, then the match proceeds, but if subsequent
// failure occurs, alternatives in P are not sought.  The idea of Fence is that
// each time the pattern is matched, just one attempt is made to match P,
// without trying alternatives.
Pattern Fence(const Pattern& p);

// ----------------------------------------------------------------------------
///  Len
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches exactly the given number of characters. The
// pattern fails if fewer than this number of characters remain to be matched in
// the string.
Pattern Len(unsigned int count);
Pattern Len(UnsignedInterface *count);
Pattern Len(const unsigned int *count);

// ----------------------------------------------------------------------------
///  NotAny
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches a single character that is not one of the
// characters in the given argument. The pattern Fails if the current character
// is in str.

Pattern NotAny(Character c);
Pattern NotAny(const CharacterSet& set);
Pattern NotAny(const std::string& str);
Pattern NotAny(std::string* str);
Pattern NotAny(StringInterface&);

// ----------------------------------------------------------------------------
///  NSpan
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches the longest possible string consisting
// entirely of characters from the given argument. The string may be empty, so
// this pattern always succeeds.

// Null or Span (always succeeds)
// [NOT in SPITBOL]

Pattern NSpan(Character c);
Pattern NSpan(const CharacterSet& set);
Pattern NSpan(const std::string& str);
Pattern NSpan(std::string *str);
Pattern NSpan(StringInterface&);

// ----------------------------------------------------------------------------
///  Pos
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches the null string if exactly count characters
// have already been matched, and otherwise fails.

Pattern Pos(unsigned int count);
Pattern Pos(UnsignedInterface *);
Pattern Pos(const unsigned int *ptr);

// ----------------------------------------------------------------------------
///  Rem
// ----------------------------------------------------------------------------
//
// Constructs a pattern that always succeeds, matching the remaining unmatched
// characters in the pattern.

// SPITBOL: REM
// C++: Rem

Pattern Rem();

// ----------------------------------------------------------------------------
///  Rpos
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches the null string if exactly count characters
// remain to be matched in the string, and otherwise fails.

Pattern Rpos(unsigned int count);
Pattern Rpos(UnsignedInterface *);
Pattern Rpos(const unsigned int *ptr);

// ----------------------------------------------------------------------------
///  Rtab
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches from the current location until exactly
// count characters remain to be matched in the string. The pattern fails if
// fewer than count characters remain to be matched.

Pattern Rtab(unsigned int count);
Pattern Rtab(UnsignedInterface *);
Pattern Rtab(const unsigned int *ptr);

// ----------------------------------------------------------------------------
///  Setcur
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches the null string, and assigns the current
// cursor position in the string to var. This value is the number of
// characters matched so far. So it is zero at the start of the match.
//
// SPITBOL: unary "@"
// C++: Setcur

Pattern Setcur(unsigned int &var);

// ----------------------------------------------------------------------------
///  Span
// ----------------------------------------------------------------------------
//
// Constructs a pattern that matches the longest possible string consisting
// entirely of characters from the given argument. The string cannot be empty ,
// so the pattern fails if the current character is not one of the characters in
// str.

Pattern Span(Character c);
Pattern Span(const CharacterSet& set);
Pattern Span(const std::string& str);
Pattern Span(const std::string *str);
Pattern Span(StringInterface&);

// ----------------------------------------------------------------------------
///  Succeed
// ----------------------------------------------------------------------------
//
// Constructs a pattern that succeeds matching null, both on the first attempt,
// and on any rematch attempt, i.e. it is equivalent to an infinite alternation
// of null strings.

Pattern Succeed();

// ----------------------------------------------------------------------------
///  Tab
// ----------------------------------------------------------------------------
//
// Constructs a pattern that from the current location until count characters
// have been matched. The pattern fails if more than count characters have
// already been matched.

Pattern Tab(unsigned int count);
Pattern Tab(UnsignedInterface *);
Pattern Tab(const unsigned int *ptr);

// ----------------------------------------------------------------------------
/// Pattern Matching Operations
// ----------------------------------------------------------------------------
//
// The Match function performs an actual pattern matching operation.  The
// versions with two parameters perform a match without modifying the subject
// string and return a bool result indicating if the match is successful or
// not.
//
// Note that pattern assignment functions in the pattern may generate side
// effects, so these functions are not necessarily pure.
//
// Pattern::anchor
//
//   This flag can be set to cause all subsequent pattern matches to operate in
//   anchored mode. In anchored mode, no attempt is made to move the anchor
//   point, so that if the match succeeds it must succeed starting at the first
//   character. Note that the effect of anchored mode may be achieved in
//   individual pattern matches by using Fence or Pos(0) at the start of the
//   pattern.
//
//   In an unanchored match, which is the default, successive attempts are made
//   to match the given pattern at each character of the subject string until a
//   match succeeds, or until all possibilities have failed.

// ----------------------------------------------------------------------------
///  Match
// ----------------------------------------------------------------------------
//
// Simple match functions. The subject is matched against the pattern.  Any
// immediate or deferred assignments or writes are executed, and the returned
// value indicates whether or not the match succeeded.

bool Match(const Character *subj, const Pattern& p, int flags = 0);
bool Match(const std::string& subj, const Pattern& p, int flags = 0);

// ----------------------------------------------------------------------------
///  Match & Replacement
// ----------------------------------------------------------------------------
//
// The subject is matched against the pattern.  Any immediate or deferred
// assignments or writes are executed, and the returned value indicates whether
// or not the match succeeded.  If the match succeeds, then the matched part of
// the subject string is replaced by the given Replace string.

bool Match
(
    std::string& subj,
    const Pattern& p,
    const std::string& replacement,
    int flags = 0
);

bool Match
(
    std::string& subj,
    const Pattern& p,
    const Character *replacement,
    int flags = 0
);

// ----------------------------------------------------------------------------
/// Debugging Routines
// ----------------------------------------------------------------------------
//
// Debugging pattern matching operations can often be quite complex, since there
// is no obvious way to trace the progress of the match.  The declarations in
// this section provide some debugging assistance.

// ----------------------------------------------------------------------------
///  Pattern::debug flag
// ----------------------------------------------------------------------------
//
// The Pattern::debug flag can be provided to the Match functions to generate
// debugging information. The debugging output is a full trace of the actions of
// the pattern matcher, written to cout. The level of this information is
// intended to be comprehensible at the abstract level of this package
// declaration. However, note that the use of this switch often generates large
// amounts of output.

// ----------------------------------------------------------------------------
///  Debug assignment
// ----------------------------------------------------------------------------
//
// These are similar to the corresponding pattern assignment operations except
// that instead of setting the value of a variable, the matched sub-string is
// written to the appropriate file. This can be useful in following the progress
// of a match without generating the full amount of information obtained by
// setting Debug_Mode to True.
Pattern operator%(const Pattern& p, std::ostream &);
Pattern operator*(const Pattern& p, std::ostream &);

// ----------------------------------------------------------------------------
///  Dump pattern (as table) to std::ostream
// ----------------------------------------------------------------------------
//
// This procedure writes information about the pattern to the std::ostream
// provided.  The format of this information is keyed to the internal data
// structures used to implement patterns. The information provided by Dump is
// thus more precise than that yielded by Image, but is also a bit more obscure
// (i.e. it cannot be interpreted solely in terms of this spec, you have to know
// something about the data structures).
//
// void Pattern::dump(std::ostream &) const;

// ----------------------------------------------------------------------------
///  Write pattern to std::ostream
// ----------------------------------------------------------------------------
//
// This output operator writes a string representation of the pattern that
// corresponds to the syntax needed to create the given pattern using the
// functions in this package. The form of this string is such that it could
// actually be compiled and evaluated to yield the required pattern except for
// references to variables and functions.
std::ostream& operator<<(std::ostream &, const Pattern& p);

// -----------------------------------------------------------------------------
