/*
 * Poll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef SRC_POLL_H_
#define SRC_POLL_H_

#include <vector>
#include <mutex>
#include "MultiplexerImpl.h"

// <client, read, write>, error: read=false, write=false
using pollTuple = std::tuple<MultiplexedClientSocket &, bool, bool>;

class Poll {
public:
	Poll() {};
	virtual ~Poll() {};
	virtual std::vector<pollTuple> pollClients(ClientListType &clients, std::mutex &clientsMutex) = 0;
};

#endif /* SRC_POLL_H_ */