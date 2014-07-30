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
#include <sstream>
#include <iostream>
#include <set>
#include <map>
#include <vector>
#include "tnode.h"
#include "tdataregion.h"
#include "tMDR.h"
#include "tTPSFilter.h"

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
	tNode *getBody();
	void setRoot(tNode *);
	tNode *findNext(tNode *, string);
    void setVerbose(int);
	//void LZExtraction();

	tMDR mdr;
	tTPSFilter tpsf;

protected:
	int treeMatch(tNode *, tNode *);
	void searchTree(tNode *, tNode *, float);
	int searchString(tNode *, string, string, int);
	int treeSize(tNode *);
	int treeDepth(tNode *);

	virtual void onTagFound(tNode *) {};//= 0;
	virtual void onPatternFound(tNode *, tNode *, float) {};//= 0;

	tNode *root,*body;
	tNode *current;
	int count;
	int verbose;
	string ignoring;
	int formOpen;
};

#endif /* TDOM_H_ */
