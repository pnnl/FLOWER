###############################################################################
# 
#
# THIS FILE INITIALLY CREATED WITH:  
#     TEMPLATE NAME:  make_file 
#     COMMAND NAME:   genmake -comp ensip -type exe -out flower 
#     CREATED FOR:     
#
# REPOSITORY INFORMATION: 
#  $Revision$ 
#  $Author$ 
#  $Date$ 
#
# _____________________________________________________________________________
#
#  You should never have to alter this file in any way other than the key-value
#  pairs below.  If you want to  override/add definitions or rules, you need to
#  use an auxiliary Makefile.  The definitions/rules files use the variables in
#  the auxiliary file.  You are free to add new variables to this auxiliary
#  Makefile, but keep in mind that these variables should be stuffed into
#  existing variables so that the templates can incorporate them.  See the
#  documentation for auxiliary makefiles by looking at one of them.
###############################################################################

# These variables are needed by the templates ALWAYS
MAKE_TYPE    := exe
COMP_NAME    := ensip
OUT_NAME      = flower


# This is the main makefile template
MAIN_MF := $(shell getsetup -make $(COMP_NAME))
include $(MAIN_MF)
