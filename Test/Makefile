### Copyright 2013 Henry G. Weller
###-----------------------------------------------------------------------------
##  This file is part of
### ---     The PatMat Pattern Matcher
###-----------------------------------------------------------------------------
##
##  PatMat is free software: you can redistribute it and/or modify it under the
##  terms of the GNU General Public License version 2 as published by the Free
##  Software Foundation.
##
##  PatMat is distributed in the hope that it will be useful, but WITHOUT ANY
##  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
##  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
##  details.
##
##  You should have received a copy of the GNU General Public License along with
##  this program.  If not, see <http://www.gnu.org/licenses/>.
##
###-----------------------------------------------------------------------------
### Title: Documentation Makefile
###  Maintainer: Henry G. Weller
###   make
###    Build for the default target and run
###   make TARGET=opt
###    Build optimised and run
###   make TARGET=debug
###    Build debug and run
###   make TARGET=debug MEMTEST=valgrind
###    Build debug and run in valgrind
###-----------------------------------------------------------------------------
PM_DIR := ..
include $(PM_DIR)/Make/Makefile.config

# MEMTEST = valgrind

###-----------------------------------------------------------------------------
### Source files
###-----------------------------------------------------------------------------
TESTS=	Any Any2 Any3 AnyCS Arb Arbno Arbno2 Arbno3 Assgn \
	Bal Break Break2 BreakX BreakX2 \
	Defer Fence Len NotAny NSpan \
	Pos Rem Rpos Rtab Span Tab

OTHERS= test1

###-----------------------------------------------------------------------------
### Build and run
###-----------------------------------------------------------------------------
LDFLAGS=$(LIBSO)

OBJDIR=platforms/$(BUILDENV)/$(TARGET)
OBJECTS=$(TESTS:%=$(OBJDIR)/%)
OTHER_OBJECTS=$(OTHERS:%=$(OBJDIR)/%)

.PHONY: all
all: $(TESTS) $(OBJECTS)

.PHONY: others
others: $(OTHERS) $(OTHER_OBJECTS)

$(OBJDIR)/valid.o: valid.C valid.H ../Pattern.H $(OBJDIR)/dummy
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/%: %.C $(OBJDIR)/valid.o valid.H $(LIBSO) $(OBJDIR)/dummy
	@$(CXX) $(CXXFLAGS) $(DFLAGS_$(TARGET)) $(OBJDIR)/valid.o -o $@ $< $(LDFLAGS)

%: $(OBJDIR)/%
	@echo -n "$@:    	TARGET=$(TARGET) " && $(MEMTEST) $< || exit 1;

###-----------------------------------------------------------------------------
### Miscellaneous commands
###-----------------------------------------------------------------------------
$(OBJDIR)/dummy:
	$R mkdir -p $(OBJDIR)
	touch $(OBJDIR)/dummy

.PHONY: clean distclean
clean distclean:
	rm -rf platforms

###-----------------------------------------------------------------------------
