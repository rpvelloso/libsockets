/*
    Copyright 2017 Roberto Panerai Velloso.
    This file is part of libsockets.
    libsockets is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    libsockets is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with libsockets.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Socket/BufferedClientSocket.h"
#include "Multiplexer/WindowsSelect.h"

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
