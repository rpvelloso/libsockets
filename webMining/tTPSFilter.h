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

#ifndef TTPSFILTER_H_
#define TTPSFILTER_H_

#include <map>
#include <vector>
#include <string>
#include <deque>
#include "tnode.h"
#include "tExtractInterface.h"
#include "misc.h"

struct tTPSRegion {
	long int len,pos;
	wstring tps;
	vector<tNode *> nodeSeq;
	tLinearCoeff lc;
	vector<vector<tNode *> > records;
	double stddev;
	bool content;
};


class tTPSFilter : public tExtractInterface {
public:
	tTPSFilter();
	virtual ~tTPSFilter();

    void buildTagPath(string, tNode *, bool, bool, bool);
	map<long int, tTPSRegion> tagPathSequenceFilter(tNode *, bool);
	map<long int, tTPSRegion> SRDEFilter(tNode *, bool);
	void DRDE(tNode *, bool, float);
	void SRDE(tNode *, bool);
	const wstring& getTagPathSequence(int = -1);
	tTPSRegion *getRegion(size_t);
	virtual size_t getRegionCount();
	virtual vector<tNode*> getRecord(size_t, size_t);
protected:
	long int searchRegion(wstring);
	bool prune(tNode *);
	vector<unsigned int> locateRecords(wstring, double);
	vector<unsigned int> SRDElocateRecords(tTPSRegion &, double &);
	map<int,int> symbolFrequency(wstring, set<int> &);
	map<int,int> frequencyThresholds(map<int,int>);
	double estimatePeriod(vector<float>);

	virtual void onDataRecordFound(vector<wstring> &, vector<unsigned int> &, tTPSRegion *);

	map<string, int> tagPathMap;
	wstring tagPathSequence;
	vector<tNode *> nodeSequence;
	int count=0,pathCount=0;

	map<long int, tTPSRegion> _regions;
	vector<tTPSRegion> regions;
};

#endif /* TTPSFILTER_H_ */
