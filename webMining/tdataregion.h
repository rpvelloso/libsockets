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

#ifndef TDATAREGION_H_
#define TDATAREGION_H_

#include <list>
#include "tnode.h"

class tDataRegion {
public:

	void clear() {
		groupSize=0; // group size
		DRLength=0;
		start=0;
		end=0;
		p = NULL;
	};

	tNode *p;
	list<tNode *>::iterator s,e;
	size_t groupSize; // group size
	size_t DRLength;
	size_t start;
	int end;
};

#endif /* TDATAREGION_H_ */
