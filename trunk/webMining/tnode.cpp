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

#include <cstdlib>
#include <iostream>
#include <string>
#include <list>
#include "tnode.h"
#include "misc.h"

using namespace std;

tNode::tNode(int tp, string tx) {
	type = tp;
	text = tx;
	parent = seed = NULL;
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
	size = 1;
	depth = aligned = matches = 0;
}

tNode::~tNode() {
	list<tNode *>::iterator i;

	if (seed != NULL) seed->nodes.remove(this);

	for (i=nodes.begin();i!=nodes.end();i++) {
		if ((*i)->parent == this) delete (*i);
		else if ((*i)->seed == this) (*i)->seed = NULL;
	}
	nodes.clear();
	alignments.clear();
}

tNode *tNode::addNode(tNode *n) {
	if (n) {
		addNode(nodes.end(),n);
		n->seed = NULL;
	}
	return n;
};

list<tNode *>::iterator tNode::addNode(list<tNode *>::iterator p, tNode *n) {
	if (n) {
		if (!n->parent) n->parent = this;
		size += n->size;
		n->seed = this;
		return nodes.insert(p,n);
	}
	return p;
}

int tNode::getType() {
	return type;
}

string& tNode::getText() {
	return text;
}

string& tNode::getTagName() {
	return tagName;
}

int tNode::compare(string s)
{
	if (s == "*") {
		return 1;
	} else if ((s[0] == '*') && (s[s.size()-1] == '*')) { // search in the middle
		s.erase(s.size()-1,1);
		s.erase(0,1);
		return tagName.find(s) != string::npos;
	} else if (s[0] == '*') { // search for suffix
		size_t p;

		s.erase(0,1);
		p = tagName.rfind(s);
		return ((p != string::npos) &&
				(p == (tagName.size() - s.size())));
	} else if (s[s.size()-1] == '*') { // search for prefix
		s.erase(s.size()-1,1);
		return (tagName.find(s) == 0);
	}
	return tagName == s; // exact match
}

int tNode::compare(tNode *n)
{
	return (compare(n->tagName) && (type == n->type));
}

void tNode::clear()
{
	nodes.clear();
	size = 1;
}

int tNode::getSize()
{
	return size;
}

int tNode::getDepth()
{
	return depth;
}

void tNode::setDepth(int d) {
	depth = d;
}

list<tNode*> &tNode::getNodes() {
	return nodes;
}

void tNode::align(tNode* n, tNode* r) {
	if (!n->aligned && alignments.find(r)==alignments.end()) {
		alignments[r]=n;
		n->aligned = 1;
		matches++;
	}
}