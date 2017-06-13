/*
 * WindowsPoll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef WIN_WINDOWSPOLL_H_
#define WIN_WINDOWSPOLL_H_

#include "Poll.h"

class WindowsPoll: public Poll {
public:
	WindowsPoll();
	virtual ~WindowsPoll();
	std::vector<pollTuple> pollClients(ClientListType &clients, std::mutex &clientsMutex) override;
};

#endif /* WIN_WINDOWSPOLL_H_ */
