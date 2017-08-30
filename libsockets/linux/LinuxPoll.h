/*
 * LinuxPoll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef LINUX_LINUXPOLL_H_
#define LINUX_LINUXPOLL_H_

#include "Poll.h"

namespace socks {

class LinuxPoll: public Poll {
public:
	LinuxPoll();
	virtual ~LinuxPoll();
	std::vector<pollTuple> pollClients(ClientListType &clients) override;
};

}
#endif /* LINUX_LINUXPOLL_H_ */
