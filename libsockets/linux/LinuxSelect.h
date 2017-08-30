/*
 * LinuxPoll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef LINUX_LINUXSELECT_H_
#define LINUX_LINUXSELECT_H_

#include "Poll.h"

namespace socks {

class LinuxSelect: public Poll {
public:
	LinuxSelect();
	virtual ~LinuxSelect();
	std::vector<pollTuple> pollClients(ClientListType &clients) override;
};

}
#endif /* LINUX_LINUXSELECT_H_ */
