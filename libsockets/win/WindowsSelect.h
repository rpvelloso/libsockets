/*
 * LinuxPoll.h
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#ifndef WINDOWS_WINDOWSSELECT_H_
#define WINDOWS_WINDOWSSELECT_H_

#include "Poll.h"

namespace socks {

class WindowsSelect: public Poll {
public:
	WindowsSelect();
	virtual ~WindowsSelect();
	std::vector<pollTuple> pollClients(ClientListType &clients, std::mutex &clientsMutex) override;
};

}
#endif /* LINUX_LINUXSELECT_H_ */
