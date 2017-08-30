/*
 * WindowsPoll.cpp
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#include "WindowsPoll.h"

namespace socks {

WindowsPoll::WindowsPoll() {
}

WindowsPoll::~WindowsPoll() {
}

std::vector<pollTuple> WindowsPoll::pollClients(ClientListType &clients) {
	std::vector<pollTuple> readyClients;

	auto nfds = clients.size();
	WSAPOLLFD fdarray[nfds];

	auto clientIt = clients.begin();
	for (size_t i = 0; i < nfds; ++i, ++clientIt) {
		fdarray[i].fd = clientIt->first;
		fdarray[i].events = POLLIN;
		if (clientIt->second->getHasOutput()) {
			fdarray[i].events |= POLLOUT;
		}
	}
	//std::cout << "# " << nfds << std::endl;

	if (WSAPoll(fdarray,nfds,-1) > 0) {
		for (auto c:fdarray) {
			bool fdError = false;
			bool readFlag = c.revents & POLLIN;
			bool writeFlag = c.revents & POLLOUT;
			if (!(readFlag || writeFlag))
				fdError = (c.revents & POLLERR) || (c.revents & POLLHUP) || (c.revents & POLLNVAL);
			auto &client = *clients[c.fd];

			if (fdError)
				readyClients.push_back(std::forward_as_tuple(client, false, false));
			else if (readFlag || writeFlag)
				readyClients.push_back(std::forward_as_tuple(client, readFlag, writeFlag));
		}
	}

	return readyClients;
}

}
