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
/// Title: XMatch the match engine
///  Description:
//    This is the main match function translated from the original MINIMAL
//    sources for SPITBOL.  The code is not a direct translation, but the
//    approach is followed closely. In particular, we use the one stack approach
//    developed in the SPITBOL implementation.
//
///   Description of Algorithm and Data Structures
//
//    A pattern structure is represented as a linked graph of nodes
//    with the following structure:
//
//      +------------------------------------+
//      I                pCode_              I
//      +------------------------------------+
//      I                Index               I
//      +------------------------------------+
//      I                pNext_              I
//      +------------------------------------+
//      I             parameter(s)           I
//      +------------------------------------+
//
//    pCode_ is a code value indicating the type of the patterm node. This code
//    is used both as the discriminant value for the record, and as the case
//    index in the main match routine that branches to the proper match code for
//    the given element.
//
//    Index is a serial index number. The use of these serial index numbers is
//    described in a separate section.
//
//    pNext_ is a pointer to the successor node, i.e the node to be matched if
//    the attempt to match the node succeeds. If this is the last node of the
//    pattern to be matched, then pNext_ points to a dummy node of kind PC_EOP
//    (end of pattern), which initiales pattern exit.
//
//    The parameter or parameters are present for certain node types, and the
//    type varies with the pattern code.
//
//    Range of pattern codes that has an Alt field. This is used in the
//    recursive traversals, since these links must be followed.
//
//    EOP_Element is the end of pattern element, and is thus the representation
//    of a null pattern. It has a zero index element since it is never placed
//    inside a pattern. Furthermore it does not need a successor, since it marks
//    the end of the pattern, so that no more successors are needed.
//
//    EOP is the end of pattern pointer, that is used in the pNext_ pointer of
//    other nodes to signal end of pattern.
//
///   The Pattern History Stack
//
//    The pattern history stack is used for controlling backtracking when a
//    match fails. The idea is to stack entries that give a cursor value to be
//    restored, and a node to be reestablished as the current node to attempt an
//    appropriate rematch operation. The processing for a pattern element that
//    has rematch alternatives pushes an appropriate entry or entry on to the
//    stack, and the proceeds. If a match fails at any point, the top element of
//    the stack is popped off, resetting the cursor and the match continues by
//    accessing the node stored with this entry.
//
//    StackEntry is the type used for a history stack. The actual instance of
//    the stack is declared as a local variable in the Match routine, to
//    properly handle recursive calls to Match. All stack pointer values are
//    negative to distinguish them from normal cursor values.
//
//    Note: the pattern matching stack is used only to handle backtracking.  If
//    no backtracking occurs, its entries are never accessed, and never popped
//    off, and in particular it is normal for a successful match to terminate
//    with entries on the stack that are simply discarded.
//
//    Note: in subsequent diagrams of the stack, we always place element zero
//    (the deepest element) at the top of the page, then build the stack down on
//    the page with the most recent (top of stack) element being the bottom-most
//    entry on the page.
//
//    Stack checking is handled by labeling every pattern with the maximum
//    number of stack entries that are required, so a single check at the start
//    of matching the pattern suffices. There are two exceptions.
//
//    First, the count does not include entries for recursive pattern
//    references. Such recursions must therefore perform a specific stack check
//    with respect to the number of stack entries required by the recursive
//    pattern that is accessed and the amount of stack that remains unused.
//
//  Second, the count includes only one iteration of an Arbno pattern,
//  so a specific check must be made on subsequent iterations that there
//  is still enough stack space left. The Arbno node has a field that
//  records the number of stack entries required by its argument for
//  this purpose.
//
///   Use of Serial Index Field in Pattern Elements
//
//    The serial index numbers for the pattern elements are assigned as a
//    pattern is consructed from its constituent elements. Note that there is
//    never any sharing of pattern elements between patterns (copies are always
//    made), so the serial index numbers are unique to a particular pattern as
//    referenced from the P field of a value of type Pattern.
//
//    The index numbers meet three separate invariants, which are used for
//    various purposes as described in this section.
//
//    First, the numbers uniquely identify the pattern elements within a
//    pattern. If Num is the number of elements in a given pattern, then the
//    serial index numbers for the elements of this pattern will range from
//    1 .. Num, so that each element has a separate value.
//
//    The purpose of this assignment is to provide a convenient auxiliary data
//    structure mechanism during operations which must traverse a pattern
//    (e.g. copy and finalization processing). Once constructed patterns are
//    strictly read only. This is necessary to allow sharing of patterns between
//    tasks. This means that we cannot go marking the pattern (e.g. with a
//    visited bit). Instead we cosntuct a separate vector that contains the
//    necessary information indexed by the Index values in the pattern
//    elements. For this purpose the only requirement is that they be uniquely
//    assigned.
//
//    Second, the pattern element referenced directly, i.e. the leading pattern
//    element, is always the maximum numbered element and therefore indicates
//    the total number of elements in the pattern. More precisely, the element
//    referenced by the P field of a pattern value, or the element returned by
//    any of the internal pattern construction routines in the body (that return
//    a value of type const PatElmt_ *) always is this maximum element,
//
//    The purpose of this requirement is to allow an immediate determination of
//    the number of pattern elements within a pattern. This is used to properly
//    size the vectors used to contain auxiliary information for traversal as
//    described above.
//
//    Third, as compound pattern structures are constructed, the way in which
//    constituent parts of the pattern are constructed is stylized. This is an
//    automatic consequence of the way that these compounjd structures are
//    constructed, and basically what we are doing is simply documenting and
//    specifying the natural result of the pattern construction. The section
//    describing compound pattern structures gives details of the numbering of
//    each compound pattern structure.
//
//    The purpose of specifying the stylized numbering structures for the
//    compound patterns is to help simplify the processing in the Image
//    function, since it eases the task of retrieving the original recursive
//    structure of the pattern from the flat graph structure of elements.  This
//    use in the Image function is the only point at which the code makes use of
//    the stylized structures.
//
///   Recursive Pattern Matches
//
//    The pattern primitive (+P) where P is a Pattern_Ptr or Pattern_Func causes
//    a recursive pattern match. This cannot be handled by an actual recursive
//    call to the outer level Match routine, since this would not allow for
//    possible backtracking into the region matched by the inner pattern. Indeed
//    this is the classical clash between recursion and backtracking, and a
//    simple recursive stack structure does not suffice.
//
//    This section describes how this recursion and the possible associated
//    backtracking is handled. We still use a single stack, but we establish the
//    concept of nested regions on this stack, each of which has a stack base
//    value pointing to the deepest stack entry of the region. The base value
//    for the outer level is zero.
//
//    When a recursive match is established, two special stack entries are
//    made. The first entry is used to save the original node that starts the
//    recursive match. This is saved so that the successor field of this node is
//    accessible at the end of the match, but it is never popped and executed.
//
//    The second entry corresponds to a standard new region action. A
//    PC_R_Remove node is stacked, whose cursor field is used to store the outer
//    stack base, and the stack base is reset to point to this PC_R_Remove
//    node. Then the recursive pattern is matched and it can make history stack
//    entries in the normal matter, so now the stack looks like:
//
//      (stack entries made by outer level)
//
//      (Special entry, node is (+P) successor
//       cursor entry is not used)
//
//      (PC_R_Remove entry, "cursor" value is (negative)     <-- Stack base
//       saved base value for the enclosing region)
//
//      (stack entries made by inner level)
//
//    If a subsequent failure occurs and pops the PC_R_Remove node, it removes
//    itself and the special entry immediately underneath it, restores the stack
//    base value for the enclosing region, and then again signals failure to
//    look for alternatives that were stacked before the recursion was
//    initiated.
//
//    Now we need to consider what happens if the inner pattern succeeds, as
//    signalled by accessing the special PC_EOP pattern primitive. First we
//    recognize the nested case by looking at the Base value. If this Base value
//    is stack.first, then the entire match has succeeded, but if the base value
//    is greater than stack.first, then we have successfully matched an inner
//    pattern, and processing continues at the outer level.
//
//    There are two cases. The simple case is when the inner pattern has made no
//    stack entries, as recognized by the fact that the current stack pointer is
//    equal to the current base value. In this case it is fine to remove all
//    trace of the recursion by restoring the outer base value and using the
//    special entry to find the appropriate successor node.
//
//    The more complex case arises when the inner match does make stack
//    entries. In this case, the PC_EOP processing stacks a special entry whose
//    cursor value saves the saved inner base value (the one that references the
//    corresponding PC_R_Remove value), and whose node pointer references a
//    PC_R_Restore node, so the stack looks like:
//
//      (stack entries made by outer level)
//
//      (Special entry, node is (+P) successor,
//       cursor entry is not used)
//
//      (PC_R_Remove entry, "cursor" value is (negative)
//       saved base value for the enclosing region)
//
//      (stack entries made by inner level)
//
//      (PC_Region_Replace entry, "cursor" value is (negative)
//       stack pointer value referencing the PC_R_Remove entry).
//
//    If the entire match succeeds, then these stack entries are, as usual,
//    ignored and abandoned. If on the other hand a subsequent failure causes
//    the PC_Region_Replace entry to be popped, it restores the inner base value
//    from its saved "cursor" value and then fails again.  Note that it is OK
//    that the cursor is temporarily clobbered by this pop, since the second
//    failure will reestablish a proper cursor value.
//
///   Compound Pattern Structures
//
//    This section discusses the compound structures used to represent
//    constructed patterns. It shows the graph structures of pattern elements
//    that are constructed, and in the case of patterns that provide
//    backtracking possibilities, describes how the history stack is used to
//    control the backtracking. Finally, it notes the way in which the Index
//    numbers are assigned to the structure.
//
//    In all diagrams, solid lines (built witth minus signs or vertical bars,
//    represent successor pointers (pNext_ fields) with > or V used to indicate
//    the direction of the pointer. The initial node of the structure is in the
//    upper left of the diagram. A dotted line is an alternative pointer from
//    the element above it to the element below it. See individual sections for
//    details on how alternatives are used.
//
///   Concatenation
//
//    In the pattern structures listed in this section, a line that looks
//    lile ----> with nothing to the right indicates an end of pattern
//    (EOP) pointer that represents the end of the match.
//
//    When a pattern concatenation (L & R) occurs, the resulting structure
//    is obtained by finding all such EOP pointers in L, and replacing
//    them to point to R. This is the most important flattening that
//    occurs in constructing a pattern, and it means that the pattern
//    matching circuitry does not have to keep track of the structure
//    of a pattern with respect to concatenation, since the appropriate
//    succesor is always at hand.
//
//    Concatenation itself generates no additional possibilities for
//    backtracking, but the constituent patterns of the concatenated
//    structure will make stack entries as usual. The maximum amount
//    of stack required by the structure is thus simply the sum of the
//    maximums required by L and R.
//
//    The index numbering of a concatenation structure works by leaving the
//    numbering of the right hand pattern, R, unchanged and adjusting the
//    numbers in the left hand pattern, L up by the count of elements in R. This
//    ensures that the maximum numbered element is the leading element as
//    required (given that it was the leading element in L).
//
///   Alternation
//
//    A pattern (L or R) constructs the structure:
//
//       +---+     +---+
//    A  |   +---->| L |---->
//       +---+     +---+
//         .
//         .
//       +---+
//       | R |---->
//       +---+
//
//    The A element here is a PC_Alt node, and the dotted line represents the
//    contents of the Alt field. When the PC_Alt element is matched, it stacks a
//    pointer to the leading element of R on the history stack so that on
//    subsequent failure, a match of R is attempted.
//
//    The A node is the higest numbered element in the pattern. The original
//    index numbers of R are unchanged, but the index numbers of the L pattern
//    are adjusted up by the count of elements in R.
//
//    Note that the difference between the index of the L leading element the
//    index of the R leading element (after building the alt structure)
//    indicates the number of nodes in L, and this is true even after the
//    structure is incorporated into some larger structure. For example, if the
//    A node has index 16, and L has index 15 and R has index 5, then we know
//    that L has 10 (15-5) elements in it.
//
//    Suppose that we now concatenate this structure to another pattern with 9
//    elements in it. We will now have the A node with an index of 25, L with an
//    index of 24 and R with an index of 14. We still know that L has 10 (24-14)
//    elements in it, numbered 15-24, and consequently the successor of the
//    alternation structure has an index with a value less than 15. This is used
//    in Image to figure out the original recursive structure of a pattern.
//
//    To clarify the interaction of the alternation and concatenation
//    structures, here is a more complex example of the structure built for the
//    pattern:
//
//        (V or W or X) (Y or Z)
//
//    where A,B,C,D,E are all single element patterns:
//
//      +---+     +---+       +---+     +---+
//      I A I---->I V I---+-->I A I---->I Y I---->
//      +---+     +---+   I   +---+     +---+
//        .               I     .
//        .               I     .
//      +---+     +---+   I   +---+
//      I A I---->I W I-->I   I Z I---->
//      +---+     +---+   I   +---+
//        .               I
//        .               I
//      +---+             I
//      I X I------------>+
//      +---+
//
//    The numbering of the nodes would be as follows:
//
//      +---+     +---+       +---+     +---+
//      I 8 I---->I 7 I---+-->I 3 I---->I 2 I---->
//      +---+     +---+   I   +---+     +---+
//        .               I     .
//        .               I     .
//      +---+     +---+   I   +---+
//      I 6 I---->I 5 I-->I   I 1 I---->
//      +---+     +---+   I   +---+
//        .               I
//        .               I
//      +---+             I
//      I 4 I------------>+
//      +---+
//
//    Note: The above structure actually corresponds to
//
//      (A or (B or C)) (D or E)
//
//    rather than
//
//      ((A or B) or C) (D or E)
//
//    which is the more natural interpretation, but in fact alternation is
//    associative, and the construction of an alternative changes the left
//    grouped pattern to the right grouped pattern in any case, so that the
//    Image function produces a more natural looking output.
//
///   Arb
//
//
//    An Arb pattern builds the structure
//
//      +---+
//      | X |---->
//      +---+
//        .
//        .
//      +---+
//      | Y |---->
//      +---+
//
//    The X node is a PC_Arb_X node, which matches null, and stacks a pointer to
//    Y node, which is the PC_Arb_Y node that matches one extra character and
//    restacks itself.
//
//    The PC_Arb_X node is numbered 2, and the PC_Arb_Y node is 1
//
///   Arbno (simple case)
//
//    The simple form of Arbno can be used where the pattern always matches at
//    least one character if it succeeds, and it is known not to make any
//    history stack entries. In this case, Arbno (P) can construct the following
//    structure:
//
//        +-------------+
//        |             ^
//        V             |
//      +---+           |
//      | S |---->      |
//      +---+           |
//        .             |
//        .             |
//      +---+           |
//      | P |---------->+
//      +---+
//
//    The S (PC_Arbno_S) node matches null stacking a pointer to the pattern
//    P. If a subsequent failure causes P to be matched and this match succeeds,
//    then node A gets restacked to try another instance if needed by a
//    subsequent failure.
//
//    The node numbering of the constituent pattern P is not affected.  The S
//    node has a node number of P.Index + 1.
//
///   Arbno (complex case)
//
//    A call to Arbno (P), where P can match null (or at least is not known to
//    require a non-null string) and/or P requires pattern stack entries,
//    constructs the following structure:
//
//        +--------------------------+
//        |                          ^
//        V                          |
//      +---+                        |
//      | X |---->                   |
//      +---+                        |
//        .                          |
//        .                          |
//      +---+     +---+     +---+    |
//      | E |---->| P |---->| Y |--->+
//      +---+     +---+     +---+
//
//    The node X (PC_Arbno_X) matches null, stacking a pointer to the E-P-X
//    structure used to match one Arbno instance.
//
//    Here E is the PC_R_Enter node which matches null and creates two stack
//    entries. The first is a special entry whose node field is not used at all,
//    and whose cursor field has the initial cursor.
//
//    The second entry corresponds to a standard new region action. A
//    PC_R_Remove node is stacked, whose cursor field is used to store the outer
//    stack base, and the stack base is reset to point to this PC_R_Remove
//    node. Then the pattern P is matched, and it can make history stack entries
//    in the normal manner, so now the stack looks like:
//
//       (stack entries made before assign pattern)
//
//       (Special entry, node field not used,
//        used only to save initial cursor)
//
//       (PC_R_Remove entry, "cursor" value is (negative)  <-- Stack Base
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//    If the match of P fails, then the PC_R_Remove entry is popped and it
//    removes both itself and the special entry underneath it, restores the
//    outer stack base, and signals failure.
//
//    If the match of P succeeds, then node Y, the PC_Arbno_Y node, pops the
//    inner region. There are two possibilities. If matching P left no stack
//    entries, then all traces of the inner region can be removed.  If there are
//    stack entries, then we push an PC_Region_Replace stack entry whose
//    "cursor" value is the inner stack base value, and then restore the outer
//    stack base value, so the stack looks like:
//
//       (stack entries made before assign pattern)
//
//       (Special entry, node field not used,
//        used only to save initial cursor)
//
//       (PC_R_Remove entry, "cursor" value is (negative)
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//       (PC_Region_Replace entry, "cursor" value is (negative)
//        stack pointer value referencing the PC_R_Remove entry).
//
//    Now that we have matched another instance of the Arbno pattern, we need to
//    move to the successor. There are two cases. If the Arbno pattern matched
//    null, then there is no point in seeking alternatives, since we would just
//    match a whole bunch of nulls.  In this case we look through the
//    alternative node, and move directly to its successor (i.e. the successor
//    of the Arbno pattern). If on the other hand a non-null string was matched,
//    we simply follow the successor to the alternative node, which sets up for
//    another possible match of the Arbno pattern.
//
//    As noted in the section on stack checking, the stack count (and hence the
//    stack check) for a pattern includes only one iteration of the Arbno
//    pattern. To make sure that multiple iterations do not overflow the stack,
//    the Arbno node saves the stack count required by a single iteration, and
//    the Concat function increments this to include stack entries required by
//    any successor. The PC_Arbno_Y node uses this count to ensure that
//    sufficient stack remains before proceeding after matching each new
//    instance.
//
//    The node numbering of the constituent pattern P is not affected.
//    Where N is the number of nodes in P, the Y node is numbered N + 1,
//    the E node is N + 2, and the X node is N + 3.
//
///   Assign Immediate
//
//    Immediate assignment (P * V) constructs the following structure
//
//      +---+     +---+     +---+
//      | E |---->| P |---->| A |---->
//      +---+     +---+     +---+
//
//    Here E is the PC_R_Enter node which matches null and creates two stack
//    entries. The first is a special entry whose node field is not used at all,
//    and whose cursor field has the initial cursor.
//
//    The second entry corresponds to a standard new region action. A
//    PC_R_Remove node is stacked, whose cursor field is used to store the outer
//    stack base, and the stack base is reset to point to this PC_R_Remove
//    node. Then the pattern P is matched, and it can make history stack entries
//    in the normal manner, so now the stack looks like:
//
//       (stack entries made before assign pattern)
//
//       (Special entry, node field not used,
//        used only to save initial cursor)
//
//       (PC_R_Remove entry, "cursor" value is (negative)  <-- Stack Base
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//    If the match of P fails, then the PC_R_Remove entry is popped and it
//    removes both itself and the special entry underneath it, restores the
//    outer stack base, and signals failure.
//
//    If the match of P succeeds, then node A, which is the actual PC_Assign_Imm
//    node, executes the assignment (using the stack base to locate the entry
//    with the saved starting cursor value), and the pops the inner
//    region. There are two possibilities, if matching P left no stack entries,
//    then all traces of the inner region can be removed. If there are stack
//    entries, then we push an PC_Region_Replace stack entry whose "cursor"
//    value is the inner stack base value, and then restore the outer stack base
//    value, so the stack looks like:
//
//       (stack entries made before assign pattern)
//
//       (Special entry, node field not used,
//        used only to save initial cursor)
//
//       (PC_R_Remove entry, "cursor" value is (negative)
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//       (PC_Region_Replace entry, "cursor" value is the (negative)
//        stack pointer value referencing the PC_R_Remove entry).
//
//    If a subsequent failure occurs, the PC_Region_Replace node restores the
//    inner stack base value and signals failure to explore rematches of the
//    pattern P.
//
//    The node numbering of the constituent pattern P is not affected.
//    Where N is the number of nodes in P, the A node is numbered N + 1,
//    and the E node is N + 2.
//
///   Assign On Match
//
//    The assign on match (**) pattern is quite similar to the assign immediate
//    pattern, except that the actual assignment has to be delayed. The
//    following structure is constructed:
//
//      +---+     +---+     +---+
//      | E |---->| P |---->| A |---->
//      +---+     +---+     +---+
//
//    The operation of this pattern is identical to that described above for
//    deferred assignment, up to the point where P has been matched.
//
//    The A node, which is the PC_Assign_OnM node first pushes a PC_Assign node
//    onto the history stack. This node saves the ending cursor and acts as a
//    flag for the final assignment, as further described below.
//
//    It then stores a pointer to itself in the special entry node field.  This
//    was otherwise unused, and is now used to retrive the address of the
//    variable to be assigned at the end of the pattern.
//
//    After that the inner region is terminated in the usual manner, by stacking
//    a PC_R_Restore entry as described for the assign immediate case. Note that
//    the optimization of completely removing the inner region does not happen
//    in this case, since we have at least one stack entry (the PC_Assign one we
//    just made).  The stack now looks like:
//
//       (stack entries made before assign pattern)
//
//       (Special entry, node points to copy of
//        the PC_Assign_OnM node, and the
//        cursor field saves the initial cursor).
//
//       (PC_R_Remove entry, "cursor" value is (negative)
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//       (PC_Assign entry, saves final cursor)
//
//       (PC_Region_Replace entry, "cursor" value is (negative)
//        stack pointer value referencing the PC_R_Remove entry).
//
//    If a subsequent failure causes the PC_Assign node to execute it simply
//    removes itself and propagates the failure.
//
//    If the match succeeds, then the history stack is scanned for PC_Assign
//    nodes, and the assignments are executed (examination of the above diagram
//    will show that all the necessary data is at hand for the assignment).
//
//    To optimize the common case where no assign-on-match operations are
//    present, a global flag Assign_OnM is maintained which is initialize to
//    false, and gets set true as part of the execution of the PC_Assign_OnM
//    node. The scan of the history stack for PC_Assign entries is done only if
//    this flag is set.
//
//    The node numbering of the constituent pattern P is not affected.
//    Where N is the number of nodes in P, the A node is numbered N + 1,
//    and the E node is N + 2.
//
///   Bal
//
//    Bal builds a single node:
//
//      +---+
//      | B |---->
//      +---+
//
//    The node B is the PC_Bal node which matches a parentheses balanced string,
//    starting at the current cursor position. It then updates the cursor past
//    this matched string, and stacks a pointer to itself with this updated
//    cursor value on the history stack, to extend the matched string on a
//    subequent failure.
//
//    Since this is a single node it is numbered 1 (the reason we include it in
//    the compound patterns section is that it backtracks).
//
///   BreakX
//
//    BreakX builds the structure
//
//      +---+     +---+
//      | B |---->| A |---->
//      +---+     +---+
//        ^         .
//        |         .
//        |       +---+
//        +<------| X |
//                +---+
//
//    Here the B node is the BreakX_xx node that performs a normal Break
//    function. The A node is an alternative (PC_Alt) node that matches null,
//    but stacks a pointer to node X (the PC_BreakX_X node) which extends the
//    match one character (to eat up the previously detected break character),
//    and then rematches the break.
//
//    The B node is numbered 3, the alternative node is 1, and the X node is 2.
//
///   Fence
//
//    Fence builds a single node:
//
//      +---+
//      | F |---->
//      +---+
//
//    The element F, PC_Fence, matches null, and stacks a pointer to a PC_Abort
//    element which will abort the match on a subsequent failure.
//
//    Since this is a single element it is numbered 1 (the reason we include it
//    in the compound patterns section is that it backtracks).
//
///   Fence Function
//
//    A call to the Fence function builds the structure:
//
//      +---+     +---+     +---+
//      | E |---->| P |---->| X |---->
//      +---+     +---+     +---+
//
//    Here E is the PC_R_Enter node which matches null and creates two stack
//    entries. The first is a special entry which is not used at all in the
//    fence case (it is present merely for uniformity with other cases of region
//    enter operations).
//
//    The second entry corresponds to a standard new region action. A
//    PC_R_Remove node is stacked, whose cursor field is used to store the outer
//    stack base, and the stack base is reset to point to this PC_R_Remove
//    node. Then the pattern P is matched, and it can make history stack entries
//    in the normal manner, so now the stack looks like:
//
//       (stack entries made before fence pattern)
//
//       (Special entry, not used at all)
//
//       (PC_R_Remove entry, "cursor" value is (negative)  <-- Stack Base
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//    If the match of P fails, then the PC_R_Remove entry is popped and it
//    removes both itself and the special entry underneath it, restores the
//    outer stack base, and signals failure.
//
//    If the match of P succeeds, then node X, the PC_Fence_X node, gets
//    control. One might be tempted to think that at this point, the history
//    stack entries made by matching P can just be removed since they certainly
//    are not going to be used for rematching (that is whole point of Fence
//    after all!) However, this is wrong, because it would result in the loss of
//    possible assign-on-match entries for deferred pattern assignments.
//
//    Instead what we do is to make a special entry whose node references
//    PC_Fence_Y, and whose cursor saves the inner stack base value, i.e.  the
//    pointer to the PC_R_Remove entry. Then the outer stack base pointer is
//    restored, so the stack looks like:
//
//       (stack entries made before assign pattern)
//
//       (Special entry, not used at all)
//
//       (PC_R_Remove entry, "cursor" value is (negative)
//        saved base value for the enclosing region)
//
//       (stack entries made by matching P)
//
//       (PC_Fence_Y entry, "cursor" value is (negative) stack
//        pointer value referencing the PC_R_Remove entry).
//
//    If a subsequent failure occurs, then the PC_Fence_Y entry removes the
//    entire inner region, including all entries made by matching P, and
//    alternatives prior to the Fence pattern are sought.
//
//    The node numbering of the constituent pattern P is not affected.
//    Where N is the number of nodes in P, the X node is numbered N + 1,
//    and the E node is N + 2.
//
///   Succeed
//
//    Succeed builds a single node:
//
//      +---+
//      | S |---->
//      +---+
//
//    The node S is the PC_Succeed node which matches null, and stacks a pointer
//    to itself on the history stack, so that a subsequent failure repeats the
//    same match.
//
//    Since this is a single node it is numbered 1 (the reason we include it in
//    the compound patterns section is that it backtracks).
//
///   Call Immediate
//
//    The structure built for a write immediate operation:
//
//      +---+     +---+     +---+
//      | E |---->| P |---->| C |---->
//      +---+     +---+     +---+
//
//    Here E is the PC_R_Enter node and C is the PC_Call_Imm node. The handling
//    is identical to that described above for Assign Immediate, except that at
//    the point where a successful match occurs, the matched substring is
//    written to the referenced file.
//
//    The node numbering of the constituent pattern P is not affected.
//    Where N is the number of nodes in P, the W node is numbered N + 1,
//    and the E node is N + 2.
//
///   Call On Match
//
//    The structure built for call write on match operation:
//
//      +---+     +---+     +---+
//      | E |---->| P |---->| C |---->
//      +---+     +---+     +---+
//
//    Here E is the PC_R_Enter node and C is the PC_Call_OnM node. The handling
//    is identical to that described above for Assign On Match, except that at
//    the point where a successful match has completed, the matched substring is
//    written to the referenced file.
//
//    The node numbering of the constituent pattern P is not affected.
//    Where N is the number of nodes in P, the W node is numbered N + 1,
//    and the E node is N + 2.
//
///   Constant Patterns
//
//    The pattern elements:
//
//    CP_Assign, CP_Abort, CP_Fence_Y, CP_R_Remove, CP_R_Restore
//
//    are referenced only from the pattern history stack. In each
//    case the processing for the pattern element results in pattern match
//    abort, or futher failure, so there is no need for a successor and no need
//    for a node number
//
///   XMatch
//
//    the common pattern match routine. It is passed the MatchState ms
//    containing a string and a pattern, and it indicates success or failure,
//    and on success the section of the string matched. It does not perform any
//    assignments to the subject string, so pattern replacement is for the
//    caller.
//
//    ms.subject
//        The subject string. The lower bound is always one. In the
//        Match procedures, it is fine to use strings whose lower bound
//        is not one, but we perform a one time conversion before the
//        call to XMatch, so that XMatch does not have to be bothered
//        with strange lower bounds.
//
//    ms.pattern
//        Points to initial pattern element of pattern to be matched
//
//    ms.start
//        If match is successful, starting index of matched section.
//        This value is always non-zero. A value of zero is used to
//        indicate a failed match.
//
//    ms.stop
//        If match is successful, ending index of matched section.
//        This can be zero if we match the null string at the start,
//        in which case Start is set to zero, and Stop to one. If the
//        Match fails, then the contents of Stop is undefined.
//
// -----------------------------------------------------------------------------

#include "PatMatInternal.H"
#include "PatMatInternalI.H"

#include <iostream>
#include <iomanip>
#include <cstring>

using std::cout;
using std::endl;

// -----------------------------------------------------------------------------

namespace PatMat
{

// -----------------------------------------------------------------------------
/// Constant pattern elements
// -----------------------------------------------------------------------------
const PatElmt_ EOP_Element(PC_EOP, 0, NULL);
const PatElmt_* EOP = &EOP_Element;

static const PatElmt_ CP_Assign(PC_Assign, 0, NULL);
static const PatElmt_ CP_Abort(PC_Abort, 0, NULL);
static const PatElmt_ CP_Fence_Y(PC_Fence_Y, 0, NULL);
static const PatElmt_ CP_R_Remove(PC_R_Remove, 0, NULL);
static const PatElmt_ CP_R_Restore(PC_R_Restore, 0, NULL);

// -----------------------------------------------------------------------------
/// ostream manipulator to handle indentation
// -----------------------------------------------------------------------------
class IndentManip
{
    const int nspaces_;

public:

    IndentManip(const int ns)
    :
        nspaces_(ns)
    {}

    inline friend std::ostream& operator<<(std::ostream&, const IndentManip);
};

inline std::ostream& operator<<(std::ostream& os, IndentManip im)
{
    int i = im.nspaces_;
    while (i-- >= 1)
    {
        os<< "| ";
    }
    return os;
}

inline IndentManip indent(const int ns)
{
    return IndentManip(ns);
}


// -----------------------------------------------------------------------------
/// LogicError
// -----------------------------------------------------------------------------

static const char* logicError()
{
    return "Internal logic error in PatMat patterns";
}


// -----------------------------------------------------------------------------
/// matchTrace
// -----------------------------------------------------------------------------
static void matchTrace
(
    const PatElmt_* n,
    const std::string subject,
    const int cursor
)
{
    if (cursor < 0)
    {
        return;
    }

    cout<< "Pattern: " << *n << "\n"
        << "Subject: " << subject << endl
        << "         ";

    // Display caret under cursor location
    for (int i = 0; i < cursor; i++)
    {
        if (subject[i] == '\t')
        {
            cout<< '\t';
        }
        else
        {
            cout<< ' ';
        }
    }
    cout<< '^' << endl;
}


// -----------------------------------------------------------------------------
/// General match function
// -----------------------------------------------------------------------------
template<int Debug>
static MatchRet XMatch(MatchState& ms)
{
    class StackEntry
    {
        public:

        //- Saved cursor value that is restored when this entry is popped
        //  from the stack if a match attempt fails. Occasionally, this
        //  field is used to store a history stack pointer instead of a
        //  cursor. Such cases are noted in the documentation and the value
        //  stored is negative since stack pointer values are always negative.
        union
        {
            unsigned int cursor;
            int stackPtr;
        };

        //- This pattern element reference is reestablished as the current
        //  node to be matched (which will attempt an appropriate rematch).
        PatElmt_ const* node;

        //- Null constructor for initialisation
        StackEntry()
        :
            cursor(0),
            node(NULL)
        {}
    };

    // Size used for internal pattern matching stack.
    const int stackSize = 100;

    class Stack
    {
    public:

        //- Current size (maximum number of entries on stack)
        int size;

        StackEntry staticEntries_[stackSize];
        StackEntry* entries_;

        //- Start of stack in the negative addressing used (-1)
        const int first;

        // This is the initial value of the ptr and stack.base. The initial
        // (first) element of the stack is not used so that when we pop the last
        // element off, ptr is still in range.
        const int init;

        //- Current stack pointer. This points to the top element of the stack
        //  that is currently in use. At the outer level this is the special
        //  entry placed on the stack according to the anchor mode.
        int ptr;

        //- This value is the stack base value, i.e. the stack pointer for the
        //  first history stack entry in the current stack region. See separate
        //  section on handling of recursive pattern matches.
        int base;

        Stack(unsigned int s)
        :
            size(s > stackSize ? s : stackSize),
            entries_(staticEntries_),
            first(-1),
            init(first -1),
            ptr(init),
            base(init)
        {
            // If the requested stack size is larger than the
            // statically allocated stack create one on the heap
            if (size > stackSize)
            {
                entries_ = new StackEntry[size];
            }
        }

        ~Stack()
        {
            if (size > stackSize)
            {
                delete[] entries_;
            }
        }

        void resize()
        {
            int oldSize = size;
            StackEntry* oldEntries = entries_;

            size *= 2;
            entries_ = new StackEntry[size];
            std::memcpy(entries_, oldEntries, sizeof(StackEntry)*oldSize);

            if (oldSize > stackSize)
            {
                delete[] oldEntries;
            }
        }

        //- Hide the fact that stack is indexed -1 .. -size ..
        inline StackEntry& operator()(const int i)
        {
            return entries_[-1 - i];
        }

        //- Push an entry onto the pattern matching stack
        //  with current cursor value
        inline void push(unsigned int cursor, const PatElmt_* node)
        {
            if (ptr < 1 - size)
            {
                resize();
            }

            ptr--;
            operator()(ptr).cursor = cursor;
            operator()(ptr).node = node;
        }

        //- Push an entry onto the pattern matching stack
        //  with current stackPtr value
        inline void push(int stackPtr, const PatElmt_* node)
        {
            if (ptr < 1 - size)
            {
                resize();
            }

            ptr--;
            operator()(ptr).stackPtr = stackPtr;
            operator()(ptr).node = node;
        }

        //- Pop an entry from the pattern matching stack
        inline void pop(unsigned& cursor, PatElmt_ const *& node)
        {
            cursor = operator()(ptr).cursor;
            node = operator()(ptr).node;
            ptr++;
        }

        //- Remove an entry from the pattern matching stack
        inline void remove()
        {
            ptr++;
        }

        //- Reset the stack pointer
        inline void reset(int p)
        {
            ptr = p;
        }

        //- This procedure makes a new region on the history stack. The caller
        //  first establishes the special entry on the stack, but does not
        //  push the stack pointer. Then this call stacks a PC_Remove_Region
        //  node, on top of this entry, using the cursor field of the
        //  PC_Remove_Region entry to save the outer level stack base value, and
        //  resets the stack base to point to this PC_Remove_Region node.
        inline void pushRegion()
        {
            if (ptr < 2 - size)
            {
                resize();
            }

            ptr -= 2;
            operator()(ptr).cursor = base;
            operator()(ptr).node = &CP_R_Remove;
            base = ptr;
        }

        //- Used at the end of processing of an inner region.  If the inner
        //  region left no stack entries, then all trace of it is removed.
        //  Otherwise a PC_Restore_Region entry is pushed to ensure proper
        //  handling of alternatives in the inner region.
        inline void popRegion()
        {
            if (ptr == base)
            {
                ptr += 2;
                base = operator()(ptr - 2).cursor;
            }
            else
            {
                if (ptr < 1 - size)
                {
                    resize();
                }

                ptr--;
                operator()(ptr).cursor = base;
                operator()(ptr).node = &CP_R_Restore;
                base = operator()(base).cursor;
            }
        }
    };

    // Pointer to current pattern node.
    // updated as the match proceeds through its constituent elements.
    const PatElmt_* node;

    // Subject string
    const Character* subject = ms.subject.c_str();

    // Length of subject string
    const unsigned int len = ms.subject.length();

    // If the value is non-negative, then this value is the index showing
    // the current position of the match in the subject string. The next
    // character to be matched is at subject[cursor]. Note that since
    // our view of the subject string in XMatch always has a lower bound
    // of one, regardless of original bounds, that this definition exactly
    // corresponds to the cursor value as referenced by functions like Pos.
    //
    // If the value is negative, then this is a saved stack pointer,
    // typically a base pointer of an inner or outer region. cursor
    // temporarily holds such a value when it is popped from the stack
    // by Fail. In all cases, cursor is reset to a proper non-negative
    // cursor value before the match proceeds (e.g. by propagating the
    // failure and popping a "real" cursor value from the stack.
    // int cursor;
    union
    {
        unsigned int cursor;
        int stackPtr;
    };

    // Dummy pattern element used in the unanchored case
    const PatElmt_ PE_Unanchored(PC_Unanchored, 0, ms.pattern->pe_);

    // Keeps track of recursive region level. This is used only for
    // debugging, it is the number of saved history stack base values.
    unsigned int regionLevel = 0;

    // The pattern matching failure stack for this call to Match
    // Check we have enough stack for this pattern. This check deals with
    // every possibility except a match of a recursive pattern, where we
    // make a check at each recursion level.
    Stack stack(ms.pattern->stackIndex_ + 2);      // accessed thru stack()

    // Set true if (assign-on-match or call-on-match operations may be
    // present in the history stack, which must then be scanned on a
    // successful match.
    bool assignOnM = false;

    // Start of processing for XMatch

    if (Debug || ms.flags & Pattern::trace)
    {
        cout<< endl;
    }

    if (Debug)
    {
        cout<< indent(regionLevel) << "Initiating pattern match\n";
        cout<< indent(regionLevel) << "subject = \"" << subject << "\"\n";
        cout<< indent(regionLevel) << "length = " << len << endl;
    }

    if (ms.pattern->pe_ == NULL)
    {
        ms.exception = "Uninitialized pattern";
        goto Match_Exception;
    }

    // In anchored mode, the bottom entry on the stack is an abort entry
    if (ms.flags & Pattern::anchor)
    {
        stack(stack.init).node = &CP_Abort;
        stack(stack.init).cursor = 0;
    }
    else
    {
        // In unanchored more, the bottom entry on the stack references
        // the special pattern element PE_Unanchored, whose pNext_ field
        // points to the initial pattern element. The cursor value in this
        // entry is the number of anchor moves so far.
        stack(stack.init).node = &PE_Unanchored;
        stack(stack.init).cursor = 0;
    }

    cursor = 0;
    node = ms.pattern->pe_;
    goto Match;

    // -----------------------------------
    // Main Pattern Matching State Control
    // -----------------------------------

    // This is a state machine which uses gotos to change state. The
    // initial state is Match, to initiate the matching of the first
    // element, so the goto Match above starts the match. In the
    // following descriptions, we indicate the global values that
    // are relevant for the state transition.

Match_Fail:
    // Come here if entire match fails
    if (Debug) cout<< indent(regionLevel) << "match fails\n";
    ms.start = 0;
    ms.stop = 0;
    return MATCH_FAILURE;

Match_Exception:
    // Come here if entire match fails
    if (Debug) cout<< indent(regionLevel) << "match fails\n";
    ms.start = 0;
    ms.stop = 0;
    return MATCH_EXCEPTION;

Match_Succeed:
    // Come here if entire match succeeds
    // cursor current position in subject string
    if (Debug) cout<< indent(regionLevel) << "match succeeds\n";
    ms.start = stack(stack.init).cursor + 1;
    ms.stop = cursor;
    if (Debug)
    {
        cout<< indent(regionLevel) << "matched positions "
            << ms.start << " .. " << ms.stop << endl
            << indent(regionLevel) << "matched substring = \""
            << slice(subject, ms.start, ms.stop) << "\"\n";
    }

    // Scan history stack for deferred assignments or writes
    if (assignOnM)
    {
        for (int s = stack.first; s >= stack.ptr; s--)
        {
            if (stack(s).node == &CP_Assign)
            {
                int innerBase = stack(s - 1).cursor;
                int specialEntry = innerBase + 1;
                const PatElmt_* nodeOnM = stack(specialEntry).node;
                unsigned int start = stack(specialEntry).cursor + 1;
                unsigned int stop = stack(s).cursor;
                std::string str = slice(subject, start, stop);

                switch (nodeOnM->pCode_)
                {
                    case PC_Assign_OnM:
                        *nodeOnM->val.SV = str;
                        if (Debug)
                        {
                            cout<< indent(regionLevel) << stack(s).node
                                << " deferred assignment of \""
                                << str << "\"\n";
                        }
                        break;
                    case PC_Call_OnM_SS:
                        nodeOnM->val.SS->set(str);
                        if (Debug)
                        {
                            cout<< indent(regionLevel) << stack(s).node
                                << " deferred call of \"" << str << "\"\n";
                        }
                        break;
                    case PC_Call_OnM_SV:
                        *nodeOnM->val.SV = str;
                        if (Debug)
                        {
                            cout<< indent(regionLevel) << stack(s).node
                                << " deferred call of \"" << str << "\"\n";
                        }
                        break;
                    default:
                        ms.exception = logicError();
                        goto Match_Exception;
                }   // switch nodeOnM->pCode_
            }   // CP_Assign
        }   // for each stack entry
    }   // assignOnM

    if (Debug) cout<< endl;
    return MATCH_SUCCESS;

Fail:
    // Come here if attempt to match current element fails
    stack.pop(cursor, node);

    if (Debug && stackPtr >= 0)
    {
        cout<< indent(regionLevel)
            << "failure, cursor reset to " << cursor << endl;
    }
    goto Match;

Succeed:
    // Come here if attempt to match current element succeeds
    //
    // cursor current position in subject string
    // node pointer to node successfully matched

    if (Debug)
    {
        cout<< indent(regionLevel) << "success, cursor = " << cursor << endl;
    }
    node = node->pNext_;

Match:
    // --------------------------------------------
    // Main Pattern Match Element Matching Routines
    // --------------------------------------------

    // Come here to match the next pattern element
    //
    // cursor current position in subject string
    // node pointer to node to be matched

    // Here is the case statement that processes the current node. The
    // processing for each element does one of five things:

    // goto Succeed to move to the successor
    // goto Match_Succeed if (the entire match succeeds
    // goto Match_Fail if (the entire match fails
    // goto Fail to signal failure of current match

    // Processing is NOT allowed to fall through

    if (ms.flags & Pattern::trace)
    {
        matchTrace(node, subject, cursor);
    }

    switch (node->pCode_)
    {
        case PC_Abort:
            // Abort
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Abort\n";
            }
            goto Match_Fail;

        case PC_Alt:
            // Alternation
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " setting up alternative "
                    << node->val.Alt << endl;
            }
            stack.push(cursor, node->val.Alt);
            node = node->pNext_;
            goto Match;

        case PC_Any_CH:
            // Any (one character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Any '" << node->val.Char << "'\n";
            }
            if (cursor < len && subject[cursor] == node->val.Char)
            {
                cursor++;
                goto Succeed;
            }
            goto Fail;

        case PC_Any_Set:
            // Any (character set case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Any '" << *(node->val.set) << "'\n";
            }
            if (cursor < len && isIn(subject[cursor], *(node->val.set)))
            {
                cursor++;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_Any_SG:
            // Any (string function case)
            {
                std::string str(node->val.SG->get());
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Any '" << str << "'\n";
                }
                if (cursor < len && isInStr(subject[cursor], str))
                {
                    cursor++;
                    goto Succeed;
                }
                goto Fail;
            }

        case PC_Any_SP:
            // Any (string pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Any '" << str << "'\n";
                }
                if (cursor < len && isInStr(subject[cursor], str))
                {
                    cursor++;
                    goto Succeed;
                }
                goto Fail;
            }

        case PC_Arb_X:
            // Arb (initial match)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Arb\n";
            }
            stack.push(cursor, node->val.Alt);
            node = node->pNext_;
            goto Match;

        case PC_Arb_Y:
            // Arb (extension)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " extending Arb\n";
            }
            if (cursor < len)
            {
                cursor++;
                stack.push(cursor, node);
                goto Succeed;
            }
            goto Fail;

        case PC_Arbno_S:
            // Arbno_S (simple Arbno initialize)
            // This is the node that initiates
            // the match of a simple Arbno structure.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    <<" setting up Arbno alternative " << node->val.Alt << endl;
            }
            stack.push(cursor, node->val.Alt);
            node = node->pNext_;
            goto Match;

        case PC_Arbno_X:
            // Arbno_X (Arbno initialize).
            // This is the node that initiates
            // the match of a complex Arbno structure.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " setting up Arbno alternative "
                    << node->val.Alt << endl;
            }
            stack.push(cursor, node->val.Alt);
            node = node->pNext_;
            goto Match;

        case PC_Arbno_Y:
            // Arbno_Y (Arbno rematch).
            // This is the node that is executed following successful
            // matching of one instance of a complex Arbno pattern.
            {
                bool Null_Match = (cursor == stack(stack.base + 1).cursor);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node << " extending Arbno\n";
                }
                stack.popRegion();
                regionLevel--;
                // If (arbno extension matched null, then immediately fail
                if (Null_Match)
                {
                    if (Debug)
                    {
                        cout<< indent(regionLevel)
                            << "Arbno extension matched null, so fails\n";
                    }
                    goto Fail;
                }

                goto Succeed;
            }

        case PC_Assign:
            // Assign. If (this node is executed, it means the assign-on-match
            // or call-on-match operation will not happen after all, so we
            // is propagate the failure, removing the PC_Assign node.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " deferred assign/call cancelled\n";
            }
            goto Fail;

        case PC_Assign_Imm:
            // Assign immediate. This node performs the actual assignment
            {
                std::string str
                (
                    slice(ms.subject, stack(stack.base + 1).cursor + 1, cursor)
                );
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        <<" executing immediate assignment of \""
                        << str << "\"\n";
                }
                *node->val.SV = str;
                stack.popRegion();
                regionLevel--;
                goto Succeed;
            }

        case PC_Assign_OnM:
            // Assign on match. This node sets up for the eventual assignment
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " registering deferred assignment\n";
            }
            stack(stack.base + 1).node = node;
            stack.push(cursor, &CP_Assign);
            stack.popRegion();
            regionLevel--;
            assignOnM = true;
            goto Succeed;

        case PC_Bal:
            // Bal
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching or extending Bal\n";
            }
            if (cursor >= len || subject[cursor] == node->val.close)
            {
                goto Fail;
            }
            if (subject[cursor] == node->val.open)
            {
                unsigned int Paren_Count = 1;
                for (;;)
                {
                    cursor++;
                    if (cursor >= len)
                    {
                        goto Fail;
                    }
                    else if (subject[cursor] == node->val.open)
                    {
                        Paren_Count++;
                    }
                    else if (subject[cursor] == node->val.close)
                    {
                        if (--Paren_Count == 0)
                        {
                            break;
                        }
                    }
                }
            }
            cursor++;
            stack.push(cursor, node);
            goto Succeed;

        case PC_Break_CH:
            // Break (one character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Break '"
                    << node->val.Char << "'\n";
            }
            while (cursor < len)
            {
                if (subject[cursor] == node->val.Char)
                {
                    goto Succeed;
                }
                cursor++;
            }
            goto Fail;

        case PC_Break_Set:
            // Break (character set case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Break '"
                    << node->val.set << "'\n";
            }
            while (cursor < len)
            {
                if (isIn(subject[cursor], *(node->val.set)))
                {
                    goto Succeed;
                }
                cursor++;
            }
            goto Fail;

        case PC_Break_SG:
            // Break (string function case)
            {
                std::string str(node->val.SG->get());
                if (Debug)
                {
                    cout<< indent(regionLevel) << node << " matching Break '"
                        << str << "'\n";
                }
                while (cursor < len)
                {
                    if (isInStr(subject[cursor], str))
                    {
                        goto Succeed;
                    }
                    cursor++;
                }
                goto Fail;
            }

        case PC_Break_SP:
            // Break (string pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node << " matching Break '"
                        << str << "'\n";
                }
                while (cursor  < len)
                {
                    if (isInStr(subject[cursor], str))
                    {
                        goto Succeed;
                    }
                    cursor++;
                }
                goto Fail;
            }

        case PC_BreakX_CH:
            // BreakX (one character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching BreakX '"
                    << node->val.Char << "'\n";
            }
            while (cursor < len)
            {
                if (subject[cursor] == node->val.Char)
                {
                    goto Succeed;
                }
                cursor++;
            }
            goto Fail;

        case PC_BreakX_Set:
            // BreakX (character set case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching BreakX '"
                    << *(node->val.set) << "'\n";
            }
            while (cursor < len)
            {
                if (isIn(subject[cursor], *(node->val.set)))
                {
                    goto Succeed;
                }
                cursor++;
            }
            goto Fail;

        case PC_BreakX_SG:
            // BreakX (string function case)
            {
                std::string str(node->val.SG->get());
                if (Debug)
                {
                    cout<< indent(regionLevel) << node << " matching BreakX '"
                       << str << "'\n";
                }
                while (cursor < len)
                {
                    if (isInStr(subject[cursor], str))
                    {
                        goto Succeed;
                    }
                    cursor++;
                }
                goto Fail;
            }


        case PC_BreakX_SP:
            // BreakX (string pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node << " matching BreakX '"
                       << str << "'\n";
                }
                while (cursor < len)
                {
                    if (isInStr(subject[cursor], str))
                    {
                        goto Succeed;
                    }
                    cursor++;
                }
                goto Fail;
            }

        case PC_BreakX_X:
            // BreakX_X (BreakX extension). See section on "Compound Pattern
            // Structures". This node is the alternative that is stacked
            // to skip past the break character and extend the break.
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " extending BreakX\n";
            }
            cursor++;
            goto Succeed;

        case PC_Char:
            // Character (one character string)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching '" << node->val.Char << "'\n";
            }
            if (cursor < len && subject[cursor] == node->val.Char)
            {
                cursor++;
                goto Succeed;
            }
            goto Fail;

        case PC_EOP:
            // End of Pattern
            if (stack.base == stack.init)
            {
                if (Debug) cout<< indent(regionLevel) << "end of pattern\n";
                goto Match_Succeed;
            }
            else
            {
                // End of recursive inner match. See separate section on
                // handing of recursive pattern matches for details.
                //
                // XXX release held pattern object here?
                if (Debug)
                {
                    cout<< indent(regionLevel)
                        << "terminating recursive match\n";
                }
                node = stack(stack.base + 1).node;
                stack.popRegion();
                regionLevel--;
                goto Match;
            }

        case PC_Fail:
            // Fail
            if (Debug) cout<< indent(regionLevel) << node << " matching Fail\n";
            goto Fail;

        case PC_Fence:
            // Fence (built in pattern)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Fence\n";
            }
            stack.push(cursor, &CP_Abort);
            goto Succeed;

        case PC_Fence_X:
            // Fence function node X.
            //
            // This is the node that gets control
            // after a successful match of the fenced pattern.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Fence function\n";
            }
            stack.push(stack.base, &CP_Fence_Y);
            stack.base = stack(stack.base).cursor;
            regionLevel--;
            goto Succeed;

        case PC_Fence_Y:
            // Fence function node Y.
            //
            // This is the node that gets control on
            // a failure that occurs after the fenced pattern has matched.
            //
            // Note: the cursor at this stage is actually the inner stack
            // base value. We don't reset this, but we do use it to strip
            // off all the entries made by the fenced pattern.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " pattern matched by Fence caused failure\n";
            }
            stack.reset(cursor + 2);
            goto Fail;

        case PC_Len_Nat:
            // Len (integer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Len " << node->val.Nat << endl;
            }
            if (cursor + node->val.Nat > len)
            {
                goto Fail;
            }
            cursor += node->val.Nat;
            goto Succeed;

        case PC_Len_NG:
            // Len (Integer function case)
            {
                const unsigned int n = node->val.NG->get();
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                       << " matching Len " << n << endl;
                }
                if (cursor + n > len)
                {
                    goto Fail;
                }
                cursor += n;
                goto Succeed;
            }

        case PC_Len_NP:
            // Len (integer pointer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Len " << *node->val.NP << endl;
            }
            if (cursor + *node->val.NP > len)
            {
                goto Fail;
            }
            cursor += *node->val.NP;
            goto Succeed;

        case PC_NotAny_CH:
            // NotAny (one character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching NotAny '" << node->val.Char << "'\n";
            }
            if (cursor < len && subject[cursor] != node->val.Char)
            {
                cursor++;
                goto Succeed;
            }
            goto Fail;

        case PC_NotAny_Set:
            // NotAny (character set case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching NotAny " << *(node->val.set) << endl;
            }
            if (cursor < len && !isIn(subject[cursor], *(node->val.set)))
            {
                cursor++;
                goto Succeed;
            }
            goto Fail;

        case PC_NotAny_SG:
            // NotAny (string function case)
            {
                std::string str(node->val.SG->get());
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching NotAny \"" << str << "\"\n";
                }
                if (cursor < len && !isInStr(subject[cursor], str))
                {
                    cursor++;
                    goto Succeed;
                }
                goto Fail;
            }

        case PC_NotAny_SP:
            // NotAny (string pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching NotAny \"" << str << "\"\n";
                }
                if (cursor < len && !isInStr(subject[cursor], str))
                {
                    cursor++;
                    goto Succeed;
                }
                goto Fail;
            }

        case PC_NSpan_CH:
            // NSpan (one character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching NSpan '" << node->val.Char << "'\n";
            }
            while (cursor < len && subject[cursor] == node->val.Char)
            {
                cursor++;
            }
            goto Succeed;

        case PC_NSpan_Set:
            // NSpan (character set case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching NSpan " << *(node->val.set) << endl;
            }
            while (cursor < len && isIn(subject[cursor], *(node->val.set)))
            {
                cursor++;
            }
            goto Succeed;

        case PC_NSpan_SG:
            // NSpan (string function case)
            {
                std::string str(node->val.SG->get());
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching NSpan \"" << str << "\"\n";
                }
                while (cursor < len && isInStr(subject[cursor], str))
                {
                    cursor++;
                }
                goto Succeed;
            }

        case PC_NSpan_SP:
            // NSpan (string pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching NSpan \"" << str << "\"\n";
                }
                while (cursor < len && isInStr(subject[cursor], str))
                {
                    cursor++;
                }
                goto Succeed;
            }

        case PC_Null:
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching null" << endl;
            }
            goto Succeed;

        case PC_Pos_Nat:
            // Pos (integer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Pos " << node->val.Nat << endl;
            }
            if (cursor == node->val.Nat)
            {
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_Pos_NG:
            // Pos (Integer function case)
            {
                const unsigned int n = node->val.NG->get();
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Pos " << n << endl;
                }
                if (cursor == n)
                {
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_Pos_NP:
            // Pos (integer pointer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Pos " << *node->val.NP << endl;
            }
            if (cursor == *node->val.NP)
            {
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_Pred_Func:
            // Predicate function
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching predicate function\n";
            }
            if (node->val.BG->get())
            {
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_R_Enter:
            // Region Enter. Initiate new pattern history stack region
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " starting match of nested pattern\n";
            }
            stack(stack.ptr - 1).cursor = cursor;
            stack.pushRegion();
            regionLevel++;
            goto Succeed;

        case PC_R_Remove:
            // Region Remove node. This is the node stacked by an
            // R_Enter.  It removes the special format stack entry right
            // underneath, and then restores the outer level stack base
            // and signals failure.

            // Note: the cursor value at this stage is actually the
            // (negative) stack base value for the outer level.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " failure, match of nested pattern terminated\n";
            }
            stack.base = cursor;
            regionLevel--;
            stack.remove();
            goto Fail;

        case PC_R_Restore:
            // Region restore node. This is the node stacked at the
            // end of an inner level match. Its function is to restore
            // the inner level region, so that alternatives in this
            // region can be sought.

            // Note: the cursor at this stage is actually the negative
            // of the inner stack base value, which we use to restore
            // the inner region.
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " failure, search for alternatives in"
                "nested pattern\n";
            }
            regionLevel++;
            stack.base = cursor;
            goto Fail;

        case PC_Rem:
            // Rem
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Rem\n";
            }
            cursor = len;
            goto Succeed;

        case PC_Rpat:
            // Initiate recursive match (pattern pointer case)
            stack(stack.ptr - 1).node = node->pNext_;
            stack.pushRegion();
            regionLevel++;
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " initiating recursive match\n";
            }
            node = (*node->val.PP)->pe_;
            goto Match;

        case PC_RPos_Nat:
            // RPos (integer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching RPos " << node->val.Nat << endl;
            }
            if (cursor == (len - node->val.Nat))
            {
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_RPos_NG:
            // RPos (integer function case)
            {
                const unsigned int n = node->val.NG->get();
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching RPos " << n << endl;
                }
                if (len == cursor + n)
                {
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_RPos_NP:
            // RPos (integer pointer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching RPos " << *node->val.NP << endl;
            }
            if (cursor == (len - *node->val.NP))
            {
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_RTab_Nat:
            // RTab (integer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching RTab " << node->val.Nat << endl;
            }
            if (cursor <= (len - node->val.Nat))
            {
                cursor = len - node->val.Nat;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_RTab_NG:
            // RTab (integer function case)
            {
                const unsigned int n = node->val.NG->get();
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching RTab " << n << endl;
                }
                if (len >= cursor + n)
                {
                    cursor = len - n;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_RTab_NP:
            // RTab (integer pointer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching RTab " << *node->val.NP << endl;
            }
            if (cursor <= (len - *node->val.NP))
            {
                cursor = len - *node->val.NP;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_Setcur:
            // Cursor assignment
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Setcur\n";
            }
            *node->val.NV = cursor;
            goto Succeed;

        case PC_Setcur_Func:
            // Cursor assignment
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching Setcur_Func\n";
            }
            node->val.NS->set(cursor);
            goto Succeed;

        case PC_Span_CH:
            // Span (one character case)
            {
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Span '" << node->val.Char << "'\n";
                }
                unsigned int cur = cursor;
                while (cur < len && subject[cur] == node->val.Char)
                {
                    cur++;
                }
                if (cur != cursor)
                {
                    cursor = cur;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_Span_Set:
            // Span (character set case)
            {
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Span " << *(node->val.set) << endl;
                }
                unsigned int cur = cursor;
                while (cur < len && isIn(subject[cur], *(node->val.set)))
                {
                    cur++;
                }
                if (cur != cursor)
                {
                    cursor = int(cur);
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_Span_SG:
            // Span (string function case)
            {
                std::string str(node->val.SG->get());
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Span \"" << str << "\"\n";
                }
                unsigned int cur = cursor;
                while (cur < len && isInStr(subject[cur], str))
                {
                    cur++;
                }

                if (cur != cursor)
                {
                    cursor = cur;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_Span_SP:
            // Span (string pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Span \"" << str << "\"\n";
                }
                unsigned int cur = cursor;
                while (cur < len && isInStr(subject[cur], str))
                    cur++;

                if (cur != cursor)
                {
                    cursor = cur;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_String_2:
            // String (two character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching \""
                    << node->val.Str2[0]
                    << node->val.Str2[1]
                    << "\"\n";
            }
            if
            (
                len >= cursor + 2
             && subject[cursor] == node->val.Str2[0]
             && subject[cursor + 1] == node->val.Str2[1]
            )
            {
                cursor += 2;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_String_3:
            // String (three character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching \""
                    << node->val.Str3[0]
                    << node->val.Str3[1]
                    << node->val.Str3[2]
                    << "\"\n";
            }
            if
            (
                len >= cursor + 3
             && subject[cursor] == node->val.Str3[0]
             && subject[cursor + 1] == node->val.Str3[1]
             && subject[cursor + 2] == node->val.Str3[2]
            )
            {
                cursor += 3;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_String_4:
            // String (four character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching \""
                    << node->val.Str4[0]
                    << node->val.Str4[1]
                    << node->val.Str4[2]
                    << node->val.Str4[3]
                    << "\"\n";
            }
            if
            (
                len >= cursor + 4
             && subject[cursor] == node->val.Str4[0]
             && subject[cursor + 1] == node->val.Str4[1]
             && subject[cursor + 2] == node->val.Str4[2]
             && subject[cursor + 3] == node->val.Str4[3]
            )
            {
                cursor += 4;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_String_5:
            // String (five character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching \""
                    << node->val.Str5[0]
                    << node->val.Str5[1]
                    << node->val.Str5[2]
                    << node->val.Str5[3]
                    << node->val.Str5[4]
                    << "\"\n";
            }
            if
            (
                len >= cursor + 5
             && subject[cursor] == node->val.Str5[0]
             && subject[cursor + 1] == node->val.Str5[1]
             && subject[cursor + 2] == node->val.Str5[2]
             && subject[cursor + 3] == node->val.Str5[3]
             && subject[cursor + 4] == node->val.Str5[4]
            )
            {
                cursor += 5;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_String_6:
            // String (six character case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching \""
                    << node->val.Str6[0]
                    << node->val.Str6[1]
                    << node->val.Str6[2]
                    << node->val.Str6[3]
                    << node->val.Str6[4]
                    << node->val.Str6[5]
                    << "\"\n";
            }
            if
            (
                len >= cursor + 6
             && subject[cursor] == node->val.Str6[0]
             && subject[cursor + 1] == node->val.Str6[1]
             && subject[cursor + 2] == node->val.Str6[2]
             && subject[cursor + 3] == node->val.Str6[3]
             && subject[cursor + 4] == node->val.Str6[4]
             && subject[cursor + 5] == node->val.Str6[5]
            )
            {
                cursor += 6;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_String:
            // String (case of more than six characters)
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " matching \"" << *(node->val.Str) << "\"\n";
            }
            if
            (
                len >= cursor + node->val.Str->length()
             && match(&subject[cursor], *(node->val.Str))
            )
            {
                cursor += node->val.Str->length();
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_String_SG:
            // String (function case)
            {
                std::string str(node->val.SG->get());
                unsigned int l = str.length();

                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching \"" << str << "\"\n";
                }
                if (len >= cursor + l && match(&subject[cursor], str))
                {
                    cursor += l;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_String_SP:
            // String (vstring pointer case)
            {
                std::string str(*node->val.SP);
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching \"" << str << "\"\n";
                }
                unsigned int l = str.length();
                if (len >= cursor + l && match(&subject[cursor], str))
                {
                    cursor += l;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_Succeed:
            // Succeed
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Succeed\n";
            }
            stack.push(cursor, node);
            goto Succeed;

        case PC_Tab_Nat:
            // Tab (integer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Tab "
                    << node->val.Nat << endl;
            }
            if (cursor <= node->val.Nat)
            {
                cursor = node->val.Nat;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_Tab_NG:
            // Tab (integer function case)
            {
                const unsigned int n = node->val.NG->get();
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " matching Tab " << n << endl;
                }
                if (cursor <= n)
                {
                    cursor = n;
                    goto Succeed;
                }
                else
                {
                    goto Fail;
                }
            }

        case PC_Tab_NP:
            // Tab (integer pointer case)
            if (Debug)
            {
                cout<< indent(regionLevel) << node << " matching Tab "
                    << *node->val.NP << endl;
            }
            if (cursor <= *node->val.NP)
            {
                cursor = *node->val.NP;
                goto Succeed;
            }
            else
            {
                goto Fail;
            }

        case PC_Unanchored:
            // Unanchored movement
            if (Debug)
            {
                // no node addr
                cout<< indent(regionLevel)
                    << "attempting to move anchor point\n";
            }
            if (cursor > len)
            {
                goto Match_Fail;        // All done if we tried every position
            }

            // Otherwise extend the anchor point, and restack ourself
            cursor++;
            stack.push(cursor, node);
            goto Succeed;

        case PC_Call_Imm_SS:
            // Call immediate. This node performs the call
            {
                std::string str
                (
                    slice(ms.subject, stack(stack.base + 1).cursor + 1, cursor)
                );
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " executing immediate write of \"" << str << "\"\n";
                }
                node->val.SS->set(str);
                stack.popRegion();
                regionLevel--;
                goto Succeed;
            }

        case PC_Call_Imm_SV:
            // Call immediate. This node performs the call
            {
                std::string str
                (
                    slice(ms.subject, stack(stack.base + 1).cursor + 1, cursor)
                );
                if (Debug)
                {
                    cout<< indent(regionLevel) << node
                        << " executing immediate write of \"" << str << "\"\n";
                }
                *node->val.SV = str;
                stack.popRegion();
                regionLevel--;
                goto Succeed;
            }

        case PC_Call_OnM_SS:
            // Write on match. This node sets up for the eventual write
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " registering deferred call" << endl;
            }
            stack(stack.base + 1).node = node;
            stack.push(cursor, &CP_Assign);
            stack.popRegion();
            regionLevel--;
            assignOnM = true;
            goto Succeed;

        case PC_Call_OnM_SV:
            // Write on match. This node sets up for the eventual write
            if (Debug)
            {
                cout<< indent(regionLevel) << node
                    << " registering deferred call" << endl;
            }
            stack(stack.base + 1).node = node;
            stack.push(cursor, &CP_Assign);
            stack.popRegion();
            regionLevel--;
            assignOnM = true;
            goto Succeed;

    }   // switch (node->pCode_)

    // We are NOT allowed to fall though this case statement, since every
    // match routine must end by executing a goto to the appropriate point
    // in the finite state machine model.
    ms.exception = logicError();

    // Fall-through if logicError does not throw an exception
    goto Match_Exception;
}


// -----------------------------------------------------------------------------
} // End namespace PatMat
// -----------------------------------------------------------------------------

PatMat::MatchRet PatMat::match(MatchState& ms)
{
    if (ms.flags & Pattern::debug)
    {
        return XMatch<1>(ms);
    }
    else
    {
        return XMatch<0>(ms);
    }
}


// -----------------------------------------------------------------------------
