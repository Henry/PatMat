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
### Title: Makefile
###  Maintainer: Henry G. Weller
###   make
###    Build for the default target
###   make TARGET=opt
###    Build optimised
###   make TARGET=debug
###    Build debug
###-----------------------------------------------------------------------------
PM_DIR := .
include $(PM_DIR)/Make/Makefile.config

###-----------------------------------------------------------------------------
### Source files
###-----------------------------------------------------------------------------
SOURCES= CharacterSet.C Pattern.C PatternIO.C \
    PatMatInternal.C PatElmt.C xmatch.C

INCLUDES= CharacterSet.H Pattern.H PatternOperations.H \
    PatMatInternal.H PatMatInternalI.H

###-----------------------------------------------------------------------------
### Build PatMat library
###-----------------------------------------------------------------------------
$(LIBSO): $(SOURCES) $(INCLUDES) $(LIBDIR) Makefile Make/Makefile.config
	$V $(CXX) $(CXXFLAGS) $(DFLAGS_$(TARGET)) $(SOURCES) -fPIC -shared -o $@

###-----------------------------------------------------------------------------
### Test commands
###-----------------------------------------------------------------------------
.PHONY: test
test: $(LIBSO)
	$V $(MAKE) -C Test

.PHONY: test1
test1: $(LIBSO)
	$V $(MAKE) -C Test test1

.PHONY: valgrind
valgrind: $(LIBSO)
	$V $(MAKE) MEMTEST=valgrind -C Test

###-----------------------------------------------------------------------------
### Miscellaneous commands
###-----------------------------------------------------------------------------
$(LIBDIR):
	$R mkdir -p $(LIBDIR)

README.org: index.org
	@sed 's%file:%http://henry.github.com/PatMat/%' $< > $@

.PHONY: doc
doc: index.html README.org
	@$(MAKE) -C Doc

.PHONY: clean distclean
clean distclean:
	$H $(MAKE) -C Test clean
	$H rm -rf platforms

###-----------------------------------------------------------------------------
