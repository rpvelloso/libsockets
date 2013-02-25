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

#ifndef TNODE_H_
#define TNODE_H_

#include <list>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

class tDOM;

class tNode {
friend class tDOM;
friend class tCustomDOM;
public:
	tNode(int, string);
	virtual ~tNode();
	void addNode(tNode *);
	int getType();
	string &getText();
	string &getTagName();
	int compare(string);
	int compare(tNode *);
	void clear();
	int getSize();
	int getDepth();
	void setDepth(int);
	list<tNode *> &getNodes();
protected:
	void align(tNode *, tNode *);
	list<tNode *> nodes;
	map<tNode *,tNode *> alignments;
	tNode *parent;
	int type,size,depth,matches,aligned;
	string text, tagName;
};

#endif /* TNODE_H_ */
