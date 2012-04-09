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
	tCustomDOM() : tDOM() { c = 0; };
	virtual ~tCustomDOM() {};
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

	virtual void onPatternFound(tNode *n, tNode *p) {
		cout << ++c << " * --- Pattern found: " << endl;
		printNode(n,1);
		cout << STM(n,p) << endl;
		cout << "* --- Pattern end --- *" << endl << endl;
	};

	int c;
};

void printUsage(char *p)
{
	cout << "usage: "<<p<<" [-i input_file] [-p pattern file] [-s search_string]"<<endl;
	cout << "-i input file (default stdin)"<<endl;
	cout << "-p pattern file to search for"<<endl;
	cout << "-s search string: a list of tags to search for (tag1,tag2,...)"<<endl;
	cout << "-t value. Similarity threshold for pattern search. default 100%. Ex.: -t 90.7 (90.7%)";
	exit(-1);
}

int main(int argc, char *argv[])
{
	int opt;
	string inp="",search="",pattern="";
	tCustomDOM *d = new tCustomDOM();
	tCustomDOM *p = new tCustomDOM();
	fstream patternFile,inputFile;
	float st=100;

	while ((opt = getopt(argc, argv, "i:t:s:p:h")) != -1) {
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
			st = atof(optarg);
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

	if (search != "") {
		string t;

		while ((t=stringTok(search,","))!="") {
			lowerCase(t);
			d->searchTag(t);
		}
	}

	if (pattern != "") {
		d->searchPattern(p,st);
	}

	if (search == "" && pattern == "") {
		d->printDOM();
	}

	delete d;
	delete p;

	return 0;
}
