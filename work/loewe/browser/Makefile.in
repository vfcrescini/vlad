# The contents of this file are subject to the Mozilla Public License
# Version 1.1 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License at
# http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS
# IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
# implied. See the License for the specific language governing
# rights and limitations under the License.
#
# The Original Code is Goanna.
#
# The Initial Developer of the Original Code is Tuxia
# Labs PTY LTD.  Portions created by Tuxia Labs 
# PTY LTD are Copyright (C) 2001 Tuxia Labs PTY LTD. All
# Rights Reserved.
#
# Contributor(s):
#   Vino Crescini <vino.crescini@tuxia.com>

DEPTH 		= goanna
topsrcdir 	= goanna
srcdir		= .
VPATH		= .

include $(DEPTH)/config/autoconf.mk

REQUIRES	= xpcom string dom

CPPSRCS		= \
		gtkEmbedBrowser.cpp \
		gtkEmbedIDManager.cpp \
		gtkEmbedSocketListener.cpp \
		gtkEmbedPref.cpp \
		gtkEmbedPrintManager.cpp \
		gtkEmbedSSLDialog.cpp \
		gtkEmbedPromptDialog.cpp \
		gtkEmbedHelperAppDialog.cpp \
		gtkEmbedDialogManager.cpp \
		gtkEmbedParamList.cpp \
		$(NULL) 

PROGRAM		= nanozilla 

LIBS		= \
		$(MOZ_COMPONENT_LIBS) \
		-lgtkembedmoz \
		-lgtksuperwin \
		$(TK_LIBS) \
		$(NULL)



include $(topsrcdir)/config/rules.mk

CXXFLAGS	+= $(MOZ_GTK_CFLAGS)
DEFINES		+= -D_BSD_SOURCE
DEFINES		+= -D_GNU_SOURCE
DEFINES		+= -D__STRICT_ANSI__
