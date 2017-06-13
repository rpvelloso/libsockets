/*
 * LinuxPoll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef LINUX_LINUXSELECT_H_
#define LINUX_LINUXSELECT_H_

#include "Poll.h"

class LinuxSelect: public Poll {
public:
	LinuxSelect();
	virtual ~LinuxSelect();
	std::vector<pollTuple> pollClients(ClientListType &clients, std::mutex &clientsMutex) override;
};

#endif /* LINUX_LINUXSELECT_H_ */
