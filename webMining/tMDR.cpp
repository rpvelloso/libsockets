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

#include <iostream>
#include "tdom.h"
#include "tMDR.h"
#include "misc.h"

tMDR::tMDR() {
}

tMDR::~tMDR() {
}

size_t tMDR::STM(tNode *a, tNode *b, tNode *record)
{
	if (!a->compare(b)) return 0;
	else {
		int k=a->nodes.size();
		int n=b->nodes.size();
		vector<vector<int> > m(k+1,vector<int>(n+1));

		for (int i=0;i<=k;i++) m[i][0]=0;
		for (int j=0;j<=n;j++) m[0][j]=0;

		auto ii = a->nodes.begin();
		for (int i=1;i<=k;i++,ii++) {
			auto jj = b->nodes.begin();
			for (int j=1;j<=n;j++,jj++) {
				int z = m[i-1][j-1]+STM(*ii,*jj,record);

				m[i][j] = max(max(m[i][j-1],m[i-1][j]),z);
			}
		}

		if (record) treeAlign(a,b,m,record);

		return m[k][n]+1;
	}
}

void tMDR::treeAlign(tNode* a, tNode* b, vector<vector<int> > &m, tNode *rec) {
	int pi,i,k=a->nodes.size();
	int pj,j,n=b->nodes.size();
	int insert=1;

	auto ii = a->nodes.begin();
	auto jj = b->nodes.begin();

	cerr << k << "x" << n << endl << "\t\t";
	for (j=1;j<=n;j++,jj++) cerr << (*jj)->tagName << "\t";
	cerr << endl;

	ii--;
	for (i=0;i<=k;i++,ii++) {
		if (!i) cerr << "\t";
		else cerr << (*ii)->tagName << "\t";
		for (j=0;j<=n;j++) {
			cerr << m[i][j] << "\t";
		}
		cerr << endl;
	}
	cerr << "---" << endl;

	pi = i = k;
	pj = j = n;
	ii = a->nodes.end();
	jj = b->nodes.end();
	while (m[i][j]) {
		cerr << i << ", " << j << " ";
		if (m[i-1][j-1] >= m[i][j-1]) {
			if (m[i-1][j-1] >= m[i-1][j]) {
				pi = i-1; ii--;
				pj = j-1; jj--;
			} else {
				pi = i-1; ii--;
			}
		} else {
			if (m[i][j-1] >= m[i-1][j]) {
				pj = j-1; jj--;
			} else {
				pi = i-1; ii--;
			}
		}

		if (m[pi][pj] == m[i][j]) {
			if (j!=pj && i==pi) {
				cerr << "insert" << endl;
				if (insert) {
					ii = a->addNode(ii,*jj);
					(*ii)->align(*jj,rec);
				}
			} else {
				cerr << "mismatch" << endl;
				insert = 0;
			}
		} else {
			cerr << "match" << endl;
			if (ii==a->nodes.end()) {
				ii--;
				pi--;
			}
			if (jj==b->nodes.end()) {
				jj--;
				pj--;
			}

			(*ii)->align(*jj,rec);
			insert = 1;
		}

		i = pi;
		j = pj;
	}
}

string tMDR::getRegEx(tNode* seed, int reccount, int lvl) {
	string ret="",s;

	for (auto i=seed->nodes.begin();i!=seed->nodes.end();i++) {
		ret = ret + "\n";
		for (int j=0;j<lvl;j++) ret = ret + "  ";
		ret = ret + ((*i)->type==0?"<":"") + (*i)->tagName + ((*i)->type==0?">":"") + (((*i)->matches == reccount)?"":"?");
		s = getRegEx((*i),(*i)->matches,lvl+1);
		if (s != "") {
			for (int j=0;j<=lvl;j++) s = "  " + s;
			ret = ret + "(\n" + s + ") ";
		} else ret = ret + " ";
	}
	return trim(ret);
}

vector<tNode *> tMDR::getAlignedRecord(tNode * seed, tNode *rec) {
	vector<tNode *> ret;
	size_t recsize=0;

	getAlignment(seed,rec,ret);
	for (auto i=ret.begin();i!=ret.end();i++) recsize += ((*i)!=NULL);
	if (!recsize) ret.clear();
	return ret;
}

void tMDR::getAlignment(tNode *seed, tNode *tree, vector<tNode *> &record) {
	for (auto i=seed->nodes.begin();i!=seed->nodes.end();i++) {
		if ((*i)->nodes.size() == 0) {
			if ((*i)->alignments.find(tree) != (*i)->alignments.end()) {
				tNode *n;

				if ((*i)->nodes.size() == 0) {
					if ((*i)->alignments[tree]->parent->tagName == "a")
						n = (*i)->alignments[tree]->parent;
					else
						n = (*i)->alignments[tree];
					if (find(record.begin(),record.end(),n) == record.end())
						record.push_back(n);
				}
			} else record.push_back(NULL);
		} else getAlignment(*i,tree,record);
	}
}

bool compareNodesSize(tNode *a, tNode *b) {
	cerr << a << " " << b << endl;
	return (a->getSize()>=b->getSize());
}

vector<tNode *> tMDR::partialTreeAlignment(tDataRegion dr) {
	vector<tNode *> trees;
	tNode *rec = NULL;
	int seedSize=-1;

	if (dr.groupSize > 1) {
		for (auto i=dr.s;i!=dr.e;) {
			rec = new tNode(0,"");
			rec->depth = 3;
			for (size_t j=0;j<dr.groupSize;j++,i++) {
				rec->addNode(*i);
			}
			if (rec->size > 2) trees.push_back(rec);
		}
	} else {
		if ((*(dr.s))->size > 1) {
			trees.insert(trees.begin(),dr.s,dr.e);
			for (;dr.s!=dr.e;dr.s++) cerr << *dr.s << endl;
		}
	}

	if (trees.size() > 1) {
		std::stable_sort(trees.begin(),trees.end(),compareNodesSize); // seed == trees[0]

		tMDR::STM(trees[0],trees[0],trees[0]);
		while (seedSize != trees[0]->size) {
			seedSize = trees[0]->size;
			for (size_t j=1;j<trees.size();j++)
				tMDR::STM(trees[0],trees[j],trees[j]);
		}
	}
	return trees;
}

void tMDR::mineDataRecords(tNode* n, int k, float st, int mineRegions) {
	dataRegions.clear();
	MDR(n,k,st,mineRegions);
}


list<tDataRegion> tMDR::MDR(tNode *p, int k, float st, int mineRegions) {
	list<tDataRegion> ret;

	if (p->depth >= 3 && p->size > 1) {
		tNode *a,*b;
		int n=0,DRFound;
		size_t r=0;
		vector<vector<float> > simTable(k+1,vector<float>(p->nodes.size()));
		//float simTable[k+1][p->nodes.size()];
		tDataRegion bestDR, currentDR;
		vector<tNode *>v(p->nodes.begin(),p->nodes.end()); // Remaining children, not covered by any DR, to call MDR recursively

		// *** Initialize similarity table
		for (int x=1;x<=k;x++)
			for (size_t y=0;y<p->nodes.size();y++) simTable[x][y]=0;

		// *** Create fake parent nodes to group children nodes of 'p' for STM()
		a = new tNode(0,"");
		b = new tNode(0,"");

		// *** Combine & Compare
		auto f = p->nodes.begin();
		for (int ff=0;f!=p->nodes.end();f++, ff++) { // iterate the start child of 'p'
			for (int i=ff+1;i<=k;i++) { // iterate group size
				a->clear();
				b->clear();
				auto j = f; n = 0;
				for (int jj=0;j!=p->nodes.end();j++,jj++) { // iterate from 'f' child to the end

					if (!(jj%i)) n = !n; // Combine nodes under fake parents (a and b) in groups of size 'i'
					if (n) a->addNode(*j);
					else b->addNode(*j);

					if (a->nodes.size() == b->nodes.size()) {
						int score = tMDR::STM(a,b,NULL)-1; // subtract 1 match (a=b)

						simTable[i][ff+jj-(2*i)+1] = ((float)score / ((float)max(a->size,b->size)-1)); // subtract 1 node from size (the fake parent)

						if (!n) a->clear();
						else b->clear();
					}
				}
			}
		}
		a->clear();
		b->clear();
		delete a;
		delete b;

		if (p->nodes.size()>1) {
			cerr << "* --- Sim Table --- *" << endl;
			for (int x=1;x<=k;x++) {
				for (size_t y=0;y<p->nodes.size()-1;y++) {
					cerr << (simTable[x][y])*100 << ";";
				}
				cerr << endl;
			}
			cerr << endl;
		}

		// *** Identify Data Regions
		for (size_t x=0;x<p->nodes.size()-1;x++) {
			bestDR.clear();
			bestDR.start = p->nodes.size();
			for (int y=1;y<=k;y++) {
				currentDR.clear();
				DRFound = 0;
				for (size_t z=x;z<p->nodes.size();z++/*=y*/) {
					if (!DRFound && (z > bestDR.start)) break; // interrupts if nothing was found before 'best' DR
					if (simTable[y][z] >= st) {
						if (!DRFound) {
							currentDR.groupSize=y; // number of combined nodes to form the data region
							currentDR.start=z; // start node
							DRFound = 1;
						}
						z+=y-1;
					} else {
						if (DRFound) {
							currentDR.end=z+y-1; // end node
							currentDR.DRLength = currentDR.end - currentDR.start + 1; // length of DR

							cerr << "L(" << currentDR.groupSize << ") - [" << currentDR.start << "," << currentDR.end << "]" << endl;

							if ((currentDR.DRLength > bestDR.DRLength) &&
								((currentDR.start <= bestDR.start) || (bestDR.DRLength == 0)))
								bestDR=currentDR;

							break;
						}
					}
				}
			}
			if (bestDR.DRLength) {
				cerr << "best: L(" << bestDR.groupSize << ") [" << bestDR.start << "," << bestDR.end << "]" << endl;
				bestDR.s = std::find(p->nodes.begin(),p->nodes.end(),v[bestDR.start-r]);
				bestDR.e = ++std::find(p->nodes.begin(),p->nodes.end(),v[bestDR.end-r]);
				bestDR.p = p;
				v.erase(v.begin()+bestDR.start-r,v.begin()+bestDR.end-r+1);
				r += bestDR.end - bestDR.start + 1;
				ret.push_back(bestDR);
				if (mineRegions) onDataRegionFound(bestDR,k,st);
				x=bestDR.end;
			}
		}

		if (p->nodes.size()>1) cerr << "* --- end --- *" << endl << endl;

		for (r=0;r<v.size();r++) {
			if (v[r]->nodes.size()) {
				list<tDataRegion> dr = MDR(v[r],k,st,mineRegions);
				ret.insert(ret.end(),dr.begin(),dr.end());
			}
		}
	}
	return ret;
};

void tMDR::onDataRegionFound(tDataRegion region, int K, float st) {
	auto i=region.s;
	tNode *n = new tNode(0,"");
	list<tDataRegion> records;

	n->depth = 3;

	if (region.groupSize > 1) {
		size_t j=0,k;

		while (j<region.DRLength) {
			n->clear();
			for (k=0;k<region.groupSize;k++,j++,i++)
				n->addNode(*i);
			records = MDR(n,K,st,0);
			if (records.size() == 1 && records.front().groupSize < region.groupSize) {
				region.groupSize = (*(records.begin())).groupSize;
				region.DRLength = (*(records.begin())).DRLength;
				break;
			}
		}
	} else {
		for (i=region.s;i!=region.e;i++) {
			auto l=i;

			records = MDR((*i),K,st,0);
			if (records.size()==1 && records.front().DRLength == n->nodes.size()) {
				n->clear();
				for (i=region.s;i!=region.e;i++) {
					for (l=(*i)->nodes.begin();l!=(*i)->nodes.end();l++) {
						n->addNode(*l);
					}
				}
				region.clear();
				region.p = n;
				region.s = n->nodes.begin();
				region.e = n->nodes.end();
				region.groupSize = records.front().groupSize;
				region.DRLength = n->nodes.size();
				break;
			}
		}
	}

	onDataRecordFound(region);
	n->clear();
	delete n;
}

void tMDR::onDataRecordFound(tDataRegion dr) {
	vector<tNode *> trees = tMDR::partialTreeAlignment(dr);
	vector<vector<tNode *> > records;

	for (size_t i=0;i<trees.size();i++) {
		vector<tNode *> rec = tMDR::getAlignedRecord(trees[0],trees[i]);

		if (rec.size())
			records.push_back(rec);
	}

	if (records.size())
		dataRegions.push_back(records);
}
