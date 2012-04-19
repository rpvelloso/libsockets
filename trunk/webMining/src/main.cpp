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
#include <fstream>
#include <unistd.h>
#include "tnode.h"
#include "tdom.h"
#include "misc.h"


using namespace std;

class tCustomDOM : public tDOM {
public:
	tCustomDOM() : tDOM() {
		c = 0;
		filterStr = "";
		filterTag = "";
	};
	virtual ~tCustomDOM() {
		if (c) cerr << "Found " << c << " results." << endl;
	};
	virtual void onTagFound(tNode *n) {
		switch (n->getType()) {
		case 0:
			cout << "<" << n->getTagName();
			if (n->getText().size()>0) cout << " " << n->getText();
			cout << ">" <<endl;
			break;
		case 1:
			cout << "</" << n->getTagName();
			if (n->getText().size()>0) cout << " " << n->getText();
			cout << ">" << endl;
			break;
		case 2:
		case 3:
			cout << n->getTagName();
			if (n->getText().size()>0) cout << " " << n->getText();
			cout << endl;
			break;
		default:
			break;
		}
	};

	virtual void onPatternFound(tNode *n, tNode *p, float s) {
		cout << "<DIV class=\"pattern\"> " << ++c << " Similarity " << (s*100.0) << "%" << endl;
		printNode(n,1);
		cout << "</DIV>" << endl << endl;
	};

	virtual void onDataRecordFound(tDataRegion dr) {
		list<tNode *>::iterator i=dr.s;
		int j=0;

		for (;i!=dr.e;i++) {
			if (filter(*i)) {
				if (!j++) cout << "<DIV class=\"record\" length=\"" << dr.groupSize << "\"> " << ++c << endl;
				printNode(*i,1);
			}
		}
		if (j) cout << "</DIV>" << endl;
	};

	int filter(tNode *n) {
		int ret;

		if (filterStr == "" && filterTag == "") ret = 1;
		else ret = searchString(n,filterTag,filterStr,0);
		return ret && n->size>10;
	};

	int c;
	string filterStr,filterTag;
};

void printUsage(char *p)
{
	cout << "usage: "<<p<<" [-i input_file] [-p pattern file] [-s search_string] [-v] [-t ###.##] [-m] [-f str]"<<endl;
	cout << "-i input file (default stdin)"<<endl;
	cout << "-p pattern file to search for"<<endl;
	cout << "-s search string: a list of tags to search for (tag1,tag2,...)"<<endl;
	cout << "-t value. Similarity threshold. default 100%. Ex.: -t 90.7 (90.7%)" << endl;
	cout << "-v Verbose (do not abbreviate tags/text content." << endl;
	cout << "-m performs MDR" << endl;
	cout << "-f text filter string" << endl;
	exit(-1);
}

#define K 10

int main(int argc, char *argv[])
{
	int opt,mdr=0;
	float st=1.0; // similarity threshold
	string inp="",search="",pattern="",filterStr="";
	tCustomDOM *d = new tCustomDOM();
	tCustomDOM *p = new tCustomDOM();
	fstream patternFile,inputFile;

	while ((opt = getopt(argc, argv, "i:t:s:p:f:mhv")) != -1) {
		switch (opt) {
		case 'i':
			inp = optarg;
			break;
		case 's':
			search = optarg;
			break;
		case 'p':
			pattern = optarg;
			break;
		case 't':
			st = atof(optarg)/100;
			break;
		case 'v':
			d->setVerbose(1);
			break;
		case 'm':
			mdr = 1;
			break;
		case 'f':
			filterStr = optarg;
			break;
		case 'h':
		default:
	    	printUsage(argv[0]);
	    break;
		}
	}

	if (pattern != "") {
		patternFile.open(pattern.c_str());
		if (patternFile.is_open()) {
			p->scan(patternFile);
			patternFile.close();
		} else {
			perror("open");
			return -1;
		}
	}

	if (inp != "") {
		inputFile.open(inp.c_str());
		if (inputFile.is_open()) {
			d->scan(inputFile);
			inputFile.close();
		} else {
			perror("open");
			return -1;
		}
	} else {
		d->scan(cin);
	}

	if (!mdr && search != "") {
		string t;

		while ((t=stringTok(search,","))!="") {
			lowerCase(t);
			d->searchTag(t,filterStr);
		}
	}

	if (!mdr && pattern != "") {
		d->searchPattern(p,st);
	}

	if (search == "" && pattern == "" && !mdr) {
		d->printDOM();
	}

	if (mdr) {
		d->filterTag = search!=""?search:"#text";
		d->filterStr = filterStr;
		d->MDR(d->getRoot(),K,st,1);
		cerr << "Similarity threshold used: " << st << endl;
	}

	delete d;
	delete p;

	return 0;
}
