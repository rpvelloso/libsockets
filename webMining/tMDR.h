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

#ifndef TMDR_H_
#define TMDR_H_

#include <vector>
#include <list>
#include <deque>
#include "tnode.h"
#include "tdataregion.h"
#include "tExtractInterface.h"

class tMDR : public tExtractInterface {
public:
	tMDR();
	virtual ~tMDR();

	void mineDataRecords(tNode *, int, float, int);

	static int STM(tNode *, tNode *, tNode *);
	virtual size_t getRegionCount();
	virtual vector<tNode*> getRecord(size_t, size_t);
protected:
	list<tDataRegion> MDR(tNode *, int , float , int );
	static void treeAlign(tNode *, tNode *, vector<vector<int> > &, tNode *);
	static string getRegEx(tNode*, int, int = 0);
	static vector<tNode *> getAlignedRecord(tNode *, tNode *);
	static void getAlignment(tNode *, tNode *, vector<tNode *> &);
	static vector<tNode *> partialTreeAlignment(tDataRegion dr);

	virtual void onDataRegionFound(tDataRegion, int, float);
	virtual void onDataRecordFound(tDataRegion);
};

#endif /* TMDR_H_ */
