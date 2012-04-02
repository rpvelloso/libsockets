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
#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <string>
#ifdef DEVEL_ENV
	#include "xmlParser.h"
#else
	#include <libsockets/xmlParser.h>
#endif



using namespace std;

#define upperCase(s) std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::toupper)
#define lowerCase(s) std::transform(s.begin(), s.end(), s.begin(), (int(*)(int))std::tolower)

extern string stringTok(string &, string);

#ifdef WIN32

extern int socketpair(int, int, int, int[2]);

#endif
#endif /* UTILS_H_ */
