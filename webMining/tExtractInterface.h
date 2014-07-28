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

class tExtractInterface {
public:
	tExtractInterface();
	virtual ~tExtractInterface();

	virtual vector<tNode *> getRecord(size_t, size_t);
protected:
	deque<vector<vector<tNode *> > > dataRegions;
};

#endif /* TEXTRACTINTERFACE_H_ */
