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

class tFormExtractDOM : public tDOM {
public:
	tFormExtractDOM() : tDOM() {};
	//virtual ~tCustomDOM() {};
	virtual void onTagFound(tNode *n) {
		if (n->getTagName() == "form") {
			cout << n->getTagName()
			<< ";" << getAttribute("action",n->getText())
			<< ";" << getAttribute("method",n->getText())
			<< ";" << getAttribute("name",n->getText())
			<< endl;
			cout << "tag;type;value;name;size" << endl;
			searchForm(n);
		}
		cout << endl;
	}
	virtual void onPatternFound(tNode *n, tNode *p, float s) {};
	virtual void onDataRecordFound(tDataRegion dr) {};
protected:
	void searchForm(tNode *n) {
		list<tNode *>::iterator i = n->getNodes().begin();

		for (;i!=n->getNodes().end();i++) {
			if (((*i)->getTagName() == "input") ||
				((*i)->getTagName() == "select") ||
				((*i)->getTagName() == "option") ||
				((*i)->getTagName() == "textarea"))
				cout << (*i)->getTagName()
				<< ";" << getAttribute("type",(*i)->getText())
				<< ";" << getAttribute("value",(*i)->getText())
				<< ";" << getAttribute("name",(*i)->getText())
				<< ";" << getAttribute("size",(*i)->getText())
				<< endl;
			searchForm(*i);
		}
	}

	string getAttribute(string attr, string s) {
		lowerCase(attr);
		while (s.size() > 1) {
			string a = stringTok(s," ");
			string b = stringTok(a,"=");
			lowerCase(b);
			if (b == attr) {
				return a;
			}
		}
		return "";
	}
};

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
		c++;
		if (verbose) {
			printNode(n,1);
			cout << endl;
		} else {
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
				if (!j++) cout << "<DIV class=\"region\" length=\"" << dr.DRLength << "\"> " << endl;
				cout << "<SPAN class=\"record\"> " << ++c << endl;
				printNode(*i,1);
				cout << "</SPAN>" << endl;
			}
		}
		if (j) cout << "</DIV>" << endl;
	};

	int filter(tNode *n) {
		int ret=1;

		if ((filterStr != "") || (filterTag != "")) {
			if (filterTag == "") filterTag = "#text";
			ret = searchString(n,filterTag,filterStr,0);
		}
		return ret && n->size>10;
	};

	int c;
	string filterStr,filterTag;
};

void printUsage(char *p)
{
	cout << "usage: "<<p<<" [-i input_file] [-p pattern file] [-s search_string] [-v] [-t ###.##] [-m] [-f str] [-x] [-d]"<<endl;
	cout << "-i input file (default stdin)"<<endl;
	cout << "-p pattern file to search for"<<endl;
	cout << "-s search string: a list of tags to search for (tag1,tag2,...)"<<endl;
	cout << "-t value. Similarity threshold. default 100%. Ex.: -t 90.7 (90.7%)" << endl;
	cout << "-v Verbose (do not abbreviate tags/text content." << endl;
	cout << "-m performs MDR" << endl;
	cout << "-f text filter string" << endl;
	cout << "-x display tag path of input" << endl;
	cout << "-d mine forms and fields" << endl;
	exit(-1);
}

#define K 10

int main(int argc, char *argv[])
{
	int opt,mdr=0,tp=0,mineForms=0;
	float st=1.0; // similarity threshold
	string inp="",search="",pattern="",filterStr="";
	tCustomDOM *d = new tCustomDOM();
	tCustomDOM *p = new tCustomDOM();
	tFormExtractDOM *fe = new tFormExtractDOM();
	fstream patternFile,inputFile;

	while ((opt = getopt(argc, argv, "i:t:s:p:f:mhvxd")) != -1) {
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
		case 'x':
			tp = 1;
			break;
		case 'd':
			mineForms = 1;
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

	if (mineForms) {
		fe->setRoot(d->getRoot());
		fe->searchTag("form","");
		return 0;
	}

	if (tp) {
		d->printTagPath("",d->findNext(NULL,"body"));
	}

	if (!tp && !mdr && search != "") {
		string t,s=search;

		while ((t=stringTok(s,","))!="") {
			lowerCase(t);
			d->searchTag(t,filterStr);
		}
	}

	if (!tp && !mdr && pattern != "") {
		d->searchPattern(p,st);
	}

	if (search == "" && pattern == "" && !mdr && !tp) {
		d->printDOM();
	}

	if (mdr) {
		d->filterTag = search;
		d->filterStr = filterStr;
		d->MDR(d->getRoot(),K,st,1);
		cerr << "Similarity threshold used: " << st << endl;
	}

	delete d;
	delete p;

	return 0;
}
