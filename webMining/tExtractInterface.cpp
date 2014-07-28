/*
 * tExtractInterface.cpp
 *
 *  Created on: 27/07/2014
 *      Author: roberto
 */

#include "tExtractInterface.h"

tExtractInterface::tExtractInterface() {
	// TODO Auto-generated constructor stub

}

tExtractInterface::~tExtractInterface() {
	// TODO Auto-generated destructor stub
}

vector<tNode*> tExtractInterface::getRecord(size_t dr, size_t rec) {
	if (dr < dataRegions.size()) {
		vector<vector<tNode *> > table = dataRegions[dr];
		if (rec < table.size())
			return table[rec];
	}
	return vector<tNode *>(0);
}
