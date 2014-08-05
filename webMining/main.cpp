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
#include <cstdio>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <cmath>
#include <cerrno>
#include "tnode.h"
#include "tdom.h"
#include "misc.h"
#include "tMDR.h"
#include "tTPSFilter.h"
#include "tlua.h"


using namespace std;

class tFormExtractDOM : public tDOM {
public:
	tFormExtractDOM() : tDOM() {};
	//virtual ~tCustomDOM() {};
	virtual void onTagFound(tNode *n) {
		if (n->getTagName() == "form") {
			cout << n->getTagName()
			<< ";" << n->getAttribute("action")
			<< ";" << n->getAttribute("method")
			<< ";" << n->getAttribute("name")
			<< endl;
			cout << "tag;type;value;name;size" << endl;
			searchForm(n);
		}
		cout << endl;
	}
	virtual void onPatternFound(tNode *n, tNode *p, float s) {};
protected:
	void searchForm(tNode *n) {
		auto i = n->getNodes().begin();

		for (;i!=n->getNodes().end();i++) {
			if (((*i)->getTagName() == "input") ||
				((*i)->getTagName() == "select") ||
				((*i)->getTagName() == "option") ||
				((*i)->getTagName() == "textarea"))
				cout << (*i)->getTagName()
				<< ";" << (*i)->getAttribute("type")
				<< ";" << (*i)->getAttribute("value")
				<< ";" << (*i)->getAttribute("name")
				<< ";" << (*i)->getAttribute("size")
				<< endl;
			searchForm(*i);
		}
	}
};

class tCustomDOM : public tDOM {
public:
	tCustomDOM() : tDOM() {
		r = 0;
		filterStr = "";
		filterTag = "";
	};
	virtual ~tCustomDOM() {};

	virtual void onTagFound(tNode *n) {
		r++;
		if (verbose) {
			if (n) n->printNode(1,verbose);
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
		cout << "<DIV class=\"pattern\"> " << ++r << " Similarity " << (s*100.0) << "%" << endl;
		if (n) n->printNode(1,verbose);
		cout << "</DIV>" << endl << endl;
	};

	int filter(tNode *n) {
		int ret=1;

		if ((filterStr != "") || (filterTag != "")) {
			if (filterTag == "") filterTag = "#text";
			ret = searchString(n,filterTag,filterStr,0);
		}
		return ret && n->depth>2;
	};

	int r;
	string filterStr,filterTag;
};

void printUsage(char *p)
{
	cout << "usage: "<<p<<" [-i input_file] [-o output_file] [-p pattern file] [-s search_string] [-v] [-f str] [-b] [-g]"<<endl;
	cout << "-i    input file (default stdin)"<<endl;
	cout << "-o    output file (default stdout)"<<endl;
	cout << "-p    pattern file to search for"<<endl;
	cout << "-s    search string: a list of tags to search for (tag1,tag2,...)"<<endl;
	cout << "-v    Verbose (do not abbreviate tags/text content." << endl;
	cout << "-f    text filter string" << endl;
	cout << "-g    mine forms and fields" << endl;
	cout << "-css  Use style definitions when creating tag path sequences." << endl;
	cout << "-z    LZ extraction." << endl << endl;
	exit(-1);
}

#define K 10

int main(int argc, char *argv[])
{
	int opt,mineForms=0,dbg=0;
	string inp="",outp="",search="",pattern="",filterStr="";
	tCustomDOM *d = new tCustomDOM();
	tCustomDOM *p = new tCustomDOM();
	tFormExtractDOM *fe = new tFormExtractDOM();
	fstream patternFile,inputFile;
	filebuf outputFile;

	while ((opt = getopt(argc, argv, "i:o:p:s:f:d:g")) != -1) {
		switch (opt) {
		case 'i':
			inp = optarg;
			break;
		case 'o':
			outp = optarg;
			break;
		case 'p':
			pattern = optarg;
			break;
		case 's':
			search = optarg;
			break;
		case 'f':
			filterStr = optarg;
			break;
		case 'g':
			mineForms = 1;
			break;
		case 'd':
			if (optarg && string(optarg) == "ebug") dbg=1;
			else  printUsage(argv[0]);
			break;
		case 'h':
		default:
	    	printUsage(argv[0]);
	    break;
		}
	}

	if (!dbg) {
		fclose(stderr);
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

	if (outp != "") {
		outputFile.open(outp.c_str(),ios_base::out|ios_base::binary);

		if (!errno) cout.rdbuf(&outputFile);
	}

	if (inp != "") {

		delete (new tlua(inp.c_str()));
		return 0;


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

	/*if (lz) {
		d->buildTagPath("",d->getBody(),false,CSS,false);
		d->LZExtraction();
	}*/

	if (search != "") {
		string t,s=search;

		while ((t=stringTok(s,","))!="") {
			lowerCase(t);
			d->searchTag(t,filterStr);
		}
	}

	if (pattern != "") {
		d->searchPattern(p,1.0);
	}

	delete d;
	delete p;

	cout.flush();
	return 0;
}
