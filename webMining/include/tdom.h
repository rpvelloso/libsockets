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

using namespace std;

class tDOM {
public:
	tDOM();
	virtual ~tDOM();
	virtual void onTagFound(tNode *) = 0;
	virtual void onPatternFound(tNode *, tNode *, float) = 0;
	virtual void onDataRegionFound(tNode *,list<tNode *>::iterator,list<tNode *>::iterator, int) = 0;
	int searchTag(string);
	void searchPattern(tDOM *,float);
	void addNode(int, string);
	void printDOM();
	int scan(istream &);
	tNode *getRoot();
    void setVerbose(int);
    void MDR(tNode *, int, float);

protected:
	int treeMatch(tNode *, tNode *);
	size_t STM(tNode *, tNode *);
	void searchTree(tNode *, tNode *, float);
	int searchString(tNode *, string);
	void printNode(tNode *, int);
	int treeSize(tNode *);

	tNode *root;
	tNode *current;
	size_t count;
	int verbose;
};

#endif /* TDOM_H_ */
