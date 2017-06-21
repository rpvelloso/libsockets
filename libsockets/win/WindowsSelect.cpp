/*
 * LinuxPoll.cpp
 *
 *  Created on: 12 de jun de 2017
 *      Author: rvelloso
 */

#include "WindowsSelect.h"

namespace socks {

WindowsSelect::WindowsSelect() {
}

WindowsSelect::~WindowsSelect() {
}

std::vector<pollTuple> WindowsSelect::pollClients(ClientListType& clients) {
	std::vector<pollTuple> readyClients;

	int nfds = 0;
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	for (auto clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
		nfds = std::max(static_cast<SocketFDType>(nfds), clientIt->first);
		FD_SET(clientIt->first, &readfds);
		FD_SET(clientIt->first, &exceptfds);
		if (clientIt->second->getHasOutput())
			FD_SET(clientIt->first, &writefds);
	}
	++nfds;

	if (select(nfds, &readfds, &writefds, &exceptfds, NULL) > 0) {
		for (auto clientIt = clients.begin(); clientIt != clients.end(); ++clientIt) {
			bool fdError = false;
			bool readFlag = FD_ISSET(clientIt->first, &readfds);
			bool writeFlag = FD_ISSET(clientIt->first, &writefds);
			if (!(readFlag || writeFlag))
				fdError = FD_ISSET(clientIt->first, &exceptfds);
			auto &client = *clients[clientIt->first];

			if (fdError)
				readyClients.push_back(std::forward_as_tuple(client, false, false));
			else if (readFlag || writeFlag)
				readyClients.push_back(std::forward_as_tuple(client, readFlag, writeFlag));
		}
	}

	return readyClients;
}
}
