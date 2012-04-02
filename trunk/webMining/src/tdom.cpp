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
#include <iostream>
#include "tdom.h"
#include "misc.h"

#define TTL_STA 11
#define TTL_SYM 6

#define PARSE_ERROR(l,c,s) cerr << "parse error (" << state << ") at line " \
							<< line << " col " << col << endl; return -1;

#define MAX_TXT_SIZE 50

static string ltags[] = {
		"table","tbody","tfoot","thead","tr",
		"ul","ol","dl","frameset","colgroup",
		"dir","menu"};
set<string> listTags(ltags,ltags+12);

static string itags[] = {
		"tr","th","td","li","dt","dd","frame",
		"noframes","tbody","tfoot","thead","col",
		"colgroup","p"};
set<string> itemTags(itags,itags+13);

static string stags[] = {"img","br","meta","param","area"}; // tags without subtree
set<string> singleTags(stags,stags+5);

/* maps an item to it's containers */
static pair<string,string> limap[] = {
	pair<string,string>("frame","frameset"),
	pair<string,string>("noframes","frameset"),
	pair<string,string>("li","ul"),
	pair<string,string>("li","ol"),
	pair<string,string>("li","dl"),
	pair<string,string>("li","dir"),
	pair<string,string>("li","menu"),
	pair<string,string>("dt","dl"),
	pair<string,string>("dd","dl"),
	pair<string,string>("tr","table"),
	pair<string,string>("tr","tbody"),
	pair<string,string>("tr","tfoot"),
	pair<string,string>("tr","thead"),
	pair<string,string>("td","tr"),
	pair<string,string>("th","tr"),
	pair<string,string>("tbody","table"),
	pair<string,string>("tfoot","table"),
	pair<string,string>("thead","table"),
	pair<string,string>("col","colgroup"),
	pair<string,string>("colgroup","table")
};
multimap<string,string> listItemMap(limap,limap+20);

tDOM::tDOM() {
	root = current = new tNode(-1,"");
};

tDOM::~tDOM() {
	delete root;
};

void tDOM::onTagFound(tNode *n) {
	switch (n->type) {
	case 0:
		cout << "<" << n->tagName;
		if (n->text.size()>0) cout << " " << n->text;
		cout << ">" <<endl;
		break;
	case 1:
		cout << "</" << n->tagName;
		if (n->text.size()>0) cout << " " << n->text;
		cout << ">" << endl;
		break;
	case 2:
	case 3:
		cout << n->tagName;
		if (n->text.size()>0) cout << " " << n->text;
		cout << endl;
		break;
	default:
		break;
	}
}

void tDOM::searchTag(string tag) {
	search(root,tag);
}

void tDOM::addNode(int tp, string tx) {
	tNode *c, *n = new tNode(tp,tx);

	switch (n->type) {
		case 0: /* open */

			/* auto close tags when nesting occurs */
			if (itemTags.find(n->tagName) != itemTags.end()) {
				c = current;
				while (1) {
					if (current == root) {
						current = c;
						break;
					}
					if (!multimap_pair_search(listItemMap,n->tagName,current->tagName)) break;
					if (current->tagName == n->tagName) {
						current = current->parent;
						break;
					}
					current = current->parent;
				}
			}

			current->addNode(n);

			if (singleTags.find(n->tagName) == singleTags.end())
				if (n->text[n->text.size()-1] != '/') current = n;

			break;

		case 1: /* close */
			c = current;
			while ((current != root) && (current->tagName != n->tagName))
				current = current->parent;
			if (current->tagName == n->tagName) {
				if (current != root) current = current->parent;
				current->addNode(n); /* dont create nodes for close tags to minimize tree size */
			} else {
				current = c; /* ignores unmatched close tag */
			}
			//delete n;
			break;

		case 2: /* text */
			current->addNode(n);
			break;

		case 3: /* comment */
			current->addNode(n);
			break;
		default: break;
	}
}

void tDOM::printDOM() {
	printNode(root,0);
}

int tDOM::scan(istream &htmlInput) {
	char c;
	int oldsta = 0, state = 0, line = 0, col = 0, close = 0, oc = 0; // oc = open comment
	string tagName = "", text = "", comment = "";
	int ct; // char type

	int transitionTable[TTL_SYM][TTL_STA] = {
		{ 0,10, 2, 2, 0, 2, 9, 9, 2, 9, 2},
		{-1, 5,-1,-1, 0,-1,-1,-1,-1,-1,-1},
		{-1, 3,-1,-1, 0,-1,-1,-1,-1,-1,-1},
		{-1,-1, 4,-1, 0,-1,-1, 4,-1,-1, 4},
		{ 1,-1,-1,-1, 0,-1,-1,-1,-1,-1,-1},
		{-1,-1,-1,-1, 0, 8, 7,-1, 9, 6,-1},
	};

	if (root) {
		delete root;
		root = current = new tNode(-1,"");
	}

	c = htmlInput.get();
	while ((!htmlInput.eof()) && (state != -1)) {

		if (c == '\n') {
			line++;
			col = 0;
		} else col++;

		switch (state) {
		case 0:
			if (c == '<') ct = 4;
			else ct = 0;
			break;
		case 1:
			if (c == '!') ct = 1;
			else if (c == '/') ct = 2;
			else ct = 0;
			break;
		case 2:
		case 10:
			if (c == '>') ct = 3;
			else ct = 0;
			break;
		case 3:
		case 4:
			ct = 0;
			break;
		case 5:
		case 6:
		case 8:
		case 9:
			if (c == '-') ct = 5;
			else ct = 0;
			break;
		case 7:
			if (c == '>') ct = 3;
			else ct = 0;
			break;
		default:
			PARSE_ERROR(line,col,state);
			break;
		}

		if (state == 3) close = 1;
		if (state == 10) close = 0;

		oldsta = state;
		state = transitionTable[ct][state];

		if ((state == 2) || (state == 10)) tagName += c;
		if ((state == 9) && (state == oldsta)) oc = 1;
		if ((oldsta == 9) && (state != 9)) oc = 0;
		if (!state && !oldsta) text += c;
		if (oc) comment += c;

		if (state == 4) {
			trim(tagName);
			trim(text);
			trim(comment);
			if (text != "") addNode(2, text);
			if (comment != "") addNode(3, comment);
			if (tagName != "") addNode(close == 1, tagName);

			tagName = "";
			text = "";
			comment = "";
		} else c = htmlInput.get();
	}

	trim(tagName);
	trim(text);
	trim(comment);
	if (text != "") addNode(2, text);
	if (comment != "") addNode(3, comment);
	if (tagName != "") addNode(close == 1, tagName);

	if (!((state >= 0) && (state < TTL_STA))) {
			PARSE_ERROR(line,col,state);
	}
	return 0;
}

int tDOM::treeMatch(tNode *t, tNode *p) {
	list<tNode *>::iterator i,j;

	if (t->tagName == p->tagName) {
		int r=1;

		i = t->nodes.begin();
		for (;i!=t->nodes.end();i++) {
			j = p->nodes.begin();
			for (;j!=p->nodes.end();j++) {
				if ((*i)->tagName == (*j)->tagName) r += treeMatch(*i,*j);
			}
		}
		return r;
	}
	return 0;
}

void tDOM::search(tNode *n, string t) {
	list<tNode *>::iterator i;

	if (n) {
		i = n->nodes.begin();
		for (;i!=n->nodes.end();i++) {
			if ((*i)->tagName == t) onTagFound(*i);
			search(*i,t);
		}
	}
}

void tDOM::printNode(tNode *n, int lvl) {
	list<tNode *>::iterator i;

	if (n) {
		for (int j=1;j<lvl;j++) cout << " ";
		switch (n->type) {
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

		if (n->type != -1) {
			if (n->tagName != "") cout << n->tagName;
			if (n->text.size() > 0) {
				cout << " " << n->text.substr(0,MAX_TXT_SIZE);
				if (n->text.size() > MAX_TXT_SIZE) cout << " ...";
			}
			if (n->type == 3) cout << "--";
			if (n->type != 2) cout << ">";
			cout << endl;
		}

		i = n->nodes.begin();
		for (;i!=n->nodes.end();i++) printNode(*i,lvl+1);
	}
}
