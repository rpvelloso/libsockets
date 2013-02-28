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
#include <cmath>
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
		g = r = xml = 0;
		filterStr = "";
		filterTag = "";
	};
	virtual ~tCustomDOM() {
		if (r) {
			if (xml) cout << "<region-count>" << g << "</region-count>" << endl << "<record-count>" << r << "</record-count>" << endl << "</extraction>" << endl;
			else cout << "<h3>Found " << r << " result(s) in " << g << " region(s).</h3></html>" << endl;
		}
		exit(0);
	};
	virtual void onTagFound(tNode *n) {
		r++;
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
		cout << "<DIV class=\"pattern\"> " << ++r << " Similarity " << (s*100.0) << "%" << endl;
		printNode(n,1);
		cout << "</DIV>" << endl << endl;
	};

	virtual void onDataRecordFound(tDataRegion dr) {
		vector<tNode *> recs = partialTreeAlignment(dr);
		list<tNode *> alignments[recs.size()];
		size_t reccount=0,recsize=0;

		for (size_t i=0;i<recs.size();i++) {
			alignments[i] = getRecord(recs[0],recs[i]);
			reccount += (alignments[i].size()>0);
			recsize = max(recsize,alignments[i].size());
		}

		if (reccount > 0) {
			int gr=0;

			if (!g++) {
				if (xml)
					cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl << "<extraction>" << endl;
				else
					cout << "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'/></head>" << endl;
			}

			if (xml)
				cout << "\t<region number=\"" << g << "\" recsize=\"" << recsize << "\" reccount=\"" << reccount << "\">" << endl;
			else
				cout << "<b>Region " << g << " / recsize " << recsize << " / reccount " << reccount << "</b><br>" << endl << "<table border=1>" << endl;

			for (size_t i=0;i<recs.size();i++) {
				list<tNode *> fields = alignments[i];

				if (alignments[i].size() > 0) {
					r++; gr++;

					if (xml)
						cout << "\t\t<record number=\"" << gr << "\">" << endl;
					else
						cout << "<tr><td>" << gr << "</td>";

					for (list<tNode *>::iterator j=fields.begin();j!=fields.end();j++) {
						if (xml) {
							cout << "\t\t\t<field tag=\"";
							if ((*j)) cout << (*j)->tagName << "\">" << (*j)->text;
							else cout << ">";
							cout << "</field>" << endl;
						} else {
							cout << "<td>";
							if ((*j)) printNode(*j,1);
							cout << "</td>";
						}
					}
					if (xml)
						cout << "\t\t</record>" << endl;
					else
						cout << "</tr>" << endl;
				}
			}

			if (xml)
				cout << "\t</region>" << endl;
			else
				cout << "</table><br>" << endl;
		}
	};

	int filter(tNode *n) {
		int ret=1;

		if ((filterStr != "") || (filterTag != "")) {
			if (filterTag == "") filterTag = "#text";
			ret = searchString(n,filterTag,filterStr,0);
		}
		return ret && n->depth>2;
	};

	int g,r,xml;
	string filterStr,filterTag;
};

void printUsage(char *p)
{
	cout << "usage: "<<p<<" [-i input_file] [-p pattern file] [-s search_string] [-v] [-t ###.##] [-m] [-xml] [-f str] [-x] [-d]"<<endl;
	cout << "-i   input file (default stdin)"<<endl;
	cout << "-p   pattern file to search for"<<endl;
	cout << "-s   search string: a list of tags to search for (tag1,tag2,...)"<<endl;
	cout << "-t   value. Similarity threshold. default 100%. Ex.: -t 90.7 (90.7%)" << endl;
	cout << "-v   Verbose (do not abbreviate tags/text content." << endl;
	cout << "-m   performs MDR" << endl;
	cout << "-xml outputs MDR results in XML format" << endl;
	cout << "-f   text filter string" << endl;
	cout << "-x   display tag path of input" << endl;
	cout << "-d   mine forms and fields" << endl;
	exit(-1);
}

#define K 10

int main(int argc, char *argv[])
{
	int opt,mdr=0,tp=0,mineForms=0,dbg=0;
	float st=1.0; // similarity threshold
	string inp="",search="",pattern="",filterStr="";
	tCustomDOM *d = new tCustomDOM();
	tCustomDOM *p = new tCustomDOM();
	tFormExtractDOM *fe = new tFormExtractDOM();
	fstream patternFile,inputFile;

	while ((opt = getopt(argc, argv, "i:t:s:p:f:x:d:mhv")) != -1) {
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
			if (!optarg) tp = 1;
			else if (string(optarg) == "ml") d->xml=1;
			break;
		case 'd':
			if (!optarg) mineForms = 1;
			else if (string(optarg) == "ebug") dbg=1;
			break;
		case 'h':
		default:
	    	printUsage(argv[0]);
	    break;
		}
	}

	if (!dbg) fclose(stderr);

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
