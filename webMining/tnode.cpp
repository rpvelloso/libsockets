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
#include <set>
#include "tnode.h"
#include "misc.h"

using namespace std;

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
	size = 1;
	depth = aligned = matches = 0;
}

tNode::~tNode() {
	//cout << this << " : " << nodes.size() << " : " << tagName << " : " << text << endl;
	for (auto i=nodes.begin();i!=nodes.end();i++) {
		//if ((*i)->parent == this) delete (*i);
	}
	nodes.clear();
	alignments.clear();
}

tNode *tNode::addNode(tNode *n) {
	if (n) {
		addNode(nodes.end(),n);
	}
	return n;
};

list<tNode *>::iterator tNode::addNode(list<tNode *>::iterator p, tNode *n) {
	if (n) {
		if (n->parent == NULL) n->parent = this;
		size += n->size;
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

string tNode::getAttribute(string attr) {
	string t = this->text;
	size_t pos;

	lowerCase(attr);
	pos = t.find(attr+"=");
	if (pos != string::npos) {
		string d;

		pos += attr.size()+1;
		t = t.substr(pos);
		d = t[0];
		if ((d=="\"" || d=="\'") && (t[0]!=t[1])) return stringTok(t,d);
		else return stringTok(t," ");
	}
	return "";
}

extern set<string> singleTags;
#define MAX_TXT_SIZE 50


void tNode::printNode(int lvl, int verbose) {
	for (int j=1;j<lvl;j++) cout << " ";
	switch (this->type) {
		case 0:
			cout << "<"; break;
		case 1:
			cout << "</"; break;
		case 2:
			cout << ""; break;
		case 3:
			cout << "<!--"; break;
		default: break;
	}

	if (this->type != -1) {
		if (this->type < 2) cout << this->tagName;
		if (this->text.size() > 0) {
			cout << " " << (verbose?this->text:this->text.substr(0,MAX_TXT_SIZE));
			if (!verbose && (this->text.size() > MAX_TXT_SIZE)) cout << " ...";
		}
		if (this->type == 3) cout << "--";
		if (this->type != 2) {
			if (!verbose) cout << " (" << this->depth << ", " << this->size << ")";
			cout << ">";
		}
		cout << endl;
	}

	for (auto i = this->nodes.begin();i!=this->nodes.end();i++) (*i)->printNode(lvl+1,verbose);

	// close current tag
	if ((this->type != -1) && (singleTags.find(this->tagName) == singleTags.end())) {
		if (this->type != 2) for (int j=1;j<lvl;j++) cout << " ";
		if (this->type == 3) cout << "-->" << endl;
		else if (this->type != 2) {
			cout << "</" << this->tagName << ">" << endl;
		}
	}
}

string tNode::toString(int lvl, int verbose) {
	string nstr="";

	for (int j=1;j<lvl;j++) nstr.append(" ");
	switch (this->type) {
		case 0:
			nstr.append("<"); break;
		case 1:
			nstr.append("</"); break;
		case 2:
			break;
		case 3:
			nstr.append("<!--"); break;
		default: break;
	}

	if (this->type != -1) {
		if (this->type < 2) nstr.append(this->tagName);
		if (this->text.size() > 0) {
			nstr.append(" "+(verbose?this->text:this->text.substr(0,MAX_TXT_SIZE)));
			if (!verbose && (this->text.size() > MAX_TXT_SIZE)) nstr.append(" ...");
		}
		if (this->type == 3) nstr.append("--");
		if (this->type != 2) {
			if (!verbose) nstr.append(" ("+to_string(this->depth)+", "+to_string(this->size)+")");
			nstr.append(">");
		}
		nstr.append(CRLF);
	}

	for (auto i = this->nodes.begin();i!=this->nodes.end();i++) nstr.append((*i)->toString(lvl+1,verbose));

	// close current tag
	if ((this->type != -1) && (singleTags.find(this->tagName) == singleTags.end())) {
		if (this->type != 2) for (int j=1;j<lvl;j++) nstr.append(" ");
		if (this->type == 3) nstr.append("-->" CRLF);
		else if (this->type != 2) {
			nstr.append("</"+this->tagName+">"+CRLF);
		}
	}
	return nstr;
}
