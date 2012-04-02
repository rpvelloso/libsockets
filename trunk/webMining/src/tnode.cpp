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

#include <string>
#include <list>
#include "tnode.h"
#include "misc.h"

tNode::tNode(int tp, string tx) {
	type = tp;
	text = tx;
	parent = NULL;
	if (type == 0 || type == 1) {
		tagName = text.substr(0,text.find_first_of(" \r\n\t"));
		lowerCase(tagName);
		text = text.substr(tagName.size());
		trim(text);
	} else {
		switch (type) {
			case -1: tagName = "#document"; break;
			case 2: tagName = "#text"; break;
			case 3: tagName = "#comment"; break;
			default: tagName = "#"; break;
		}
	}
}

tNode::~tNode() {
	list<tNode *>::iterator i;

	i=nodes.begin();
	for (;i!=nodes.end();i++) {
		delete (*i);
	}
	nodes.clear();
}

void tNode::addNode(tNode *n) {
	n->parent = this;
	nodes.push_back(n);
};

