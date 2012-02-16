/*
    Copyright 2011 Roberto Panerai Velloso.

    This file is part of libsockets.

    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "utils.h"

string stringtok(string *s, string d) {
	string tok="",s1=*s;
	unsigned int i=0;

	while ((d.find(s1[i++])!=string::npos) && (i<s1.size()));
	for (--i;i<s1.size();i++) {
		if (d.find(s1[i])==string::npos) tok = tok + s1[i];
		else break;
	}
	while ((d.find(s1[i++])!=string::npos) && (i<s1.size()));
	s1.erase(0,--i);
	*s = s1;
	return tok;
}
