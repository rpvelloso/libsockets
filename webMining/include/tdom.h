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

#ifndef TDOM_H_
#define TDOM_H_

#include <string>
#include <iostream>
#include <set>
#include <map>
#include "tnode.h"
#include "tdataregion.h"

using namespace std;

class tDOM {
public:
	tDOM();
	virtual ~tDOM();
	int searchTag(string, string);
	void searchPattern(tDOM *,float);
	void addNode(int, string);
	void printDOM();
	int scan(istream &);
	tNode *getRoot();
	tNode *findNext(tNode *, string);
    void setVerbose(int);
    list<tDataRegion> MDR(tNode *, int, float, int);
    void printTagPath(string, tNode *);
protected:
	int treeMatch(tNode *, tNode *);
	size_t STM(tNode *, tNode *);
	void searchTree(tNode *, tNode *, float);
	int searchString(tNode *, string, string, int);
	void printNode(tNode *, int);
	int treeSize(tNode *);
	int treeDepth(tNode *);

	virtual void onTagFound(tNode *) = 0;
	virtual void onPatternFound(tNode *, tNode *, float) = 0;
	virtual void onDataRecordFound(tDataRegion) = 0;
	virtual void onDataRegionFound(tDataRegion, float, int);

	tNode *root;
	tNode *current;
	size_t count;
	int verbose;
	string ignoring;
};

#endif /* TDOM_H_ */
