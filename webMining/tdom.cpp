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

#include <string.h>
#include <string>
#include <iostream>
#include <vector>
#include <set>
#include <cmath>
#include <queue>
#include <utility>
#include <cmath>
#include "tdom.h"
#include "misc.h"
#include "tMDR.h"

#define TTL_STA 11
#define TTL_SYM 6

#define PARSE_ERROR(l,c,s) cerr << "parse error (" << state << ") at line " \
							<< line << " col " << col << endl; return -1;

static string ltags[] = {
		"table","tbody","tfoot","thead","tr",
		"ul","ol","dl","frameset","colgroup",
		"dir","menu","select","p"};
set<string> listTags(ltags,ltags+14);

static string itags[] = {
		"tr","th","td","li","dt","dd","frame",
		"noframes","tbody","tfoot","thead","col",
		"colgroup","option","p"};
set<string> itemTags(itags,itags+15);

static string stags[] = {"hr","img","br","meta","param","area","link","doctype"}; // tags without subtree
set<string> singleTags(stags,stags+8);

static string igtags[] = {"script","noscript","head","doctype"};
set<string> ignoreTags(igtags,igtags+4);

/* maps an item to it's containers */
static pair<string,string> limap[] = {
	pair<string,string>("frame","frameset"),
	pair<string,string>("noframes","frameset"),
	pair<string,string>("option","select"),
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
multimap<string,string> listItemMap(limap,limap+21);

tDOM::tDOM() {
	root = current = body = new tNode(-1,"");
	count = 0;
	verbose = 0;
	ignoring = "";
	formOpen = 0;
};

tDOM::~tDOM() {
	if (root)
		delete root;
};

int tDOM::searchTag(string tag, string text) {
	return searchString(root,tag,text,1);
}

void tDOM::searchPattern(tDOM *p, float st) {
	if (st > 0) {
		if (st > 1) st = 1;
		searchTree(root,p->getRoot()->nodes.front(),st);
	}
}

void tDOM::addNode(int tp, string tx) {
	tNode *c, *n = new tNode(tp,tx);

	if (ignoreTags.find(n->tagName) != ignoreTags.end()) {
		if ((ignoring == "") && (n->type == 0)) ignoring = n->tagName;
		else if ((ignoring == n->tagName) && (n->type == 1)) ignoring = "";
	}

	if (ignoring == "") {
		switch (n->type) {
			case 0: /* open */

				if (n->tagName=="form") {
					if (formOpen) break;
					else formOpen=1;
				}

				if (n->tagName == "body") body = n;

				/* auto close tags when nesting occurs */
				if (itemTags.find(n->tagName) != itemTags.end()) {
					c = current;
					while (1) {
						if (current == root) {
							current = c;
							break;
						}
						if (current->tagName == "form" || !multimap_pair_search(listItemMap,n->tagName,current->tagName))
							break;
						if (current->tagName == n->tagName) {
							current = current->parent;
							break;
						}
						current = current->parent;
					}
				}

				current->addNode(n);

				if (singleTags.find(n->tagName) == singleTags.end()) {
					if (n->text[n->text.size()-1] != '/') current = n;
				}

				if (n->text[n->text.size()-1] == '/') n->text.erase(n->text.size()-1,1);

				count ++;
				break;

			case 1: /* close */
				if (n->tagName == "form") formOpen=0;
				c = current;
				while ((current != root) && (current->tagName != n->tagName))
					current = current->parent;
				if (current->tagName == n->tagName) {
					if (current != root) current = current->parent;
					//current->addNode(n); /* don't create close tags nodes to minimize tree size */
				} else {
					current = c; /* ignores unmatched close tag */
				}
				delete n;
				break;

			case 2: /* text */
				current->addNode(n);
				count ++;
				break;

			/*case 3: // comment
				current->addNode(n);
				count ++;
				break;*/
			default: break;
		}
	} else
		if (singleTags.find(ignoring) != singleTags.end()) ignoring = "";
}

void tDOM::printDOM() {
	if (root) root->printNode(0,verbose);
	cout << "Node count: " << count << endl;
}

bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}
int tDOM::scan(istream &htmlInput) {
	char c;
	int oldsta = 0, state = 0, line = 0, col = 0, close = 0, oc = 0; // oc = open comment
	string tagName = "", text = "", comment = "";
	int ct; // char type
	stringstream filteredHtmlInput("");

	int transitionTable[TTL_SYM][TTL_STA] = {
		{ 0,10, 2, 2, 0, 2, 9, 9, 2, 9, 2},
		{-1, 5,-1,-1, 0,-1,-1,-1,-1,-1,-1},
		{-1, 3,-1,-1, 0,-1,-1,-1,-1,-1,-1},
		{-1,-1, 4,-1, 0,-1,-1, 4,-1,-1, 4},
		{ 1,-1, 1,-1, 0,-1,-1,-1,-1,-1, 1},
		{-1,-1,-1,-1, 0, 8, 7,-1, 9, 6,-1},
	};

	if (root) {
		delete root;
		root = current = new tNode(-1,"");
		//root->parent = root;
		count = 0;
	}

	// filter out scripts and comments
	c = htmlInput.get();
	while (!htmlInput.eof()) {
		tagName = tagName + c;
		if (hasEnding(tagName,"<!--")) {
			filteredHtmlInput.write(tagName.c_str(),tagName.size()-4);
			tagName = "";
		} else if (hasEnding(tagName,"<script")) {
			filteredHtmlInput.write(tagName.c_str(),tagName.size()-7);
			tagName = "";
		} else if (
			hasEnding(tagName,"</script>") ||
			hasEnding(tagName,"-->")) {

			tagName = "";
		}
		c=htmlInput.get();
	}
	filteredHtmlInput << tagName;
	tagName = "";

	c = filteredHtmlInput.get();
	while (filteredHtmlInput.rdbuf()->in_avail() && (state != -1)) {

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
			else if (c == '<') ct = 4;
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
		if (state == 1) tagName = "";

		if (state == 4) {
			trim(tagName);
			lowerCase(tagName);
			trim(text);
			trim(comment);
			if (text != "") addNode(2, text);
			if (comment != "") addNode(3, comment);
			if (tagName != "") addNode(close == 1, tagName);

			tagName = "";
			text = "";
			comment = "";
		} else c = filteredHtmlInput.get();
	}

	trim(tagName);
	lowerCase(tagName);
	trim(text);
	trim(comment);
	if (text != "") addNode(2, text);
	if (comment != "") addNode(3, comment);
	if (tagName != "") addNode(close == 1, tagName);

	if (!((state >= 0) && (state < TTL_STA))) {
			PARSE_ERROR(line,col,state);
	}
	treeSize(root);
	treeDepth(root);
	current = root;
	return 0;
}

int tDOM::treeMatch(tNode *t, tNode *p) {
	if (t->compare(p)) {
		if (t->nodes.size() >= p->nodes.size()) {
			size_t m=0;

			auto i = t->nodes.begin();
			auto j = p->nodes.begin();
			for (; j!=p->nodes.end(); i++, j++) {
				if ((*i)->compare(*j)) m += treeMatch(*i,*j);
			}
			return (m == p->nodes.size());
		}
	}
	return 0;
}

void tDOM::searchTree(tNode *n, tNode *t, float st) {
	if (n) {
		if (st == 1) { // threshold = 100%, exact match
			if (treeMatch(n,t)) onPatternFound(n,t,1);
		} else {
			float sim=(float)tMDR::STM(n,t,NULL) / (float)t->size;
			if (sim >= st) onPatternFound(n,t,sim);
		}
		for (auto i = n->nodes.begin();i!=n->nodes.end();i++)
			searchTree(*i,t,st);
	}
}

int tDOM::searchString(tNode *n, string t, string tt, int callEvent) {
	int r=0;

	if (n) {
		for (auto i = n->nodes.begin();i!=n->nodes.end();i++) {
			if ((*i)->compare(t) && (tt=="" || (*i)->text.find(tt)!=string::npos)) {
				if (callEvent) onTagFound(*i);
				r++;
			}
			r += searchString(*i,t,tt,callEvent);
		}
	}
	return r;
}

tNode *tDOM::getRoot() {
	return root;
}

tNode *tDOM::getBody() {
	return body;
}

void tDOM::setVerbose(int v)
{
    verbose = v;
}

tNode *tDOM::findNext(tNode *n, string t) {
	tNode *r=NULL;

	if (!n) n = current;
	if (n) {
		for (auto i = n->nodes.begin();i!=n->nodes.end();i++) {
			if ((*i)->compare(t)) return (*i);
			if ((r=findNext(*i,t))) return r;
		}
	}
	return r;
}

void tDOM::setRoot(tNode *r)
{
	root = r;
}

int tDOM::treeSize(tNode* n) {
	int s=0;

	for (auto i=n->nodes.begin();i!=n->nodes.end();i++) {
		s += treeSize(*i);
	}
	n->size = s + 1;
	return n->size;
}

int tDOM::treeDepth(tNode* n) {
	int d=0,d1;

	for (auto i=n->nodes.begin();i!=n->nodes.end();i++) {
		d1 = treeDepth(*i);
		if (d1>d) d = d1;
	}
	n->depth = d+1;
	return n->depth;
}

/*class LZBlock {
public:
	LZBlock(int p, int l, int r) {
		position = p;
		length = l;
		reference = r;
	};

	int position;
	int length;
	int reference;
};

void tDOM::LZExtraction() {
	wstring inp = tagPathSequence;
	size_t len = inp.size(),blk=0;
	list<LZBlock *> blks;

	cout << endl << "LZ decomposition: " << endl;
	for (size_t i=0;i<len;i++) {
		wstring p,s;
		size_t k,l;

		s=inp.substr(i,len);
		p=inp.substr(0,i);

		k=l=0;
		for (size_t j=0;j<=s.size();j++) {
			size_t pos;

			pos=p.find(s.substr(0,j));
			if (pos != wstring::npos) {
				k=pos;
				l=j;
			}
		}

		++blk;
		if (l>=0) {
			cout << blk << "\t";
			if (k) {
				//if (l>1)
					blks.push_back(new LZBlock(i,l,k));
				cout << i << ":" << k << "\t" << l << "\t";
				for (size_t m=0;m<l;m++) cout << s[m] << ((m!=l-1)?",":"");
				i+=l-1;
			} else cout << i << "\t" << 1 << "\t" << inp[i];
			cout << endl;
		}
	}

	int start=0,end=0;

	for (auto i=blks.begin();i!=blks.end();i++) {
		if ((*i)->length > 1) {
			start = min(start,(*i)->reference);
			end = (*i)->position + (*i)->length - 1;
		} else {
			if (end != 0) {
				cout << start << ":" << end  << ":" << (end-start) << endl;
				for (int m=start;m<=end;m++) cout << tagPathSequence[m] << ","; cout << endl;
			}
			end = 0;
			start = (*i)->position + (*i)->length;
		}
	}
	if (end != 0) {
		cout << start << ":" << end  << ":" << (end-start) << endl;
		for (int m=start;m<=end;m++) cout << tagPathSequence[m] << ","; cout << endl;
	}
}
*/
