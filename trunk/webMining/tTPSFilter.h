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

#ifndef TTPSFILTER_H_
#define TTPSFILTER_H_

#include <map>
#include <vector>
#include <string>
#include "tnode.h"
#include "tdom.h"

class tTPSFilter {
public:
	tTPSFilter(tDOM *d);
	virtual ~tTPSFilter();

    void buildTagPath(string, tNode *, bool, bool, bool);
	map<long int, long int> tagPathSequenceFilter(bool);
	void DRDE(bool, float);
protected:
	long int searchRegion(wstring);
	bool prune(tNode *);
	vector<unsigned int> locateRecords(wstring, float);

	virtual void onDataRecordFound(vector<wstring> &, vector<unsigned int> &) = 0;

	map<string, int> tagPathMap;
	wstring tagPathSequence;
	vector<tNode *> nodeSequence;
	int count,pathCount;
	tDOM *dom;
};

#endif /* TTPSFILTER_H_ */
