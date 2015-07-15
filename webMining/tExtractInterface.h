/*
 * tExtractInterface.h
 *
 *  Created on: 27/07/2014
 *      Author: roberto
 */

#ifndef TEXTRACTINTERFACE_H_
#define TEXTRACTINTERFACE_H_

#include <vector>
#include <deque>
#include "tnode.h"

#define LEAF(n) ((n->getNodes().size()==0) && (n->getText()!=""))
#define IMG(n) (n->getTagName() == "img")
#define LINK(n) (n->getTagName() == "a")
#define TEXT(n) (n->getTagName() == "#text")

class tExtractInterface {
public:
	tExtractInterface();
	virtual ~tExtractInterface();

	virtual vector<tNode *> getRecord(size_t, size_t) = 0;
	virtual size_t getRegionCount() = 0;

	void cleanRegion(vector<vector<tNode *> > &);
};

#endif /* TEXTRACTINTERFACE_H_ */
