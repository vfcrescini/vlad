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

CFLAGS = 	\
	-O	\
	-Wall	\
	$(NULL)

PROGS =		\
	server	\
	$(NULL)

all : $(PROGS)

server : server.c
	$(CC) $(CFLAGS) -o $@ server.c

clean : 
	$(RM) *.o

distclean : clean
	$(RM) $(PROGS)
