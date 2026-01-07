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

#include <memory>
#include "WindowsPoll.h"
#include "Socket/BufferedClientSocketInterface.h"

namespace socks {

WindowsPoll::WindowsPoll() {
}

WindowsPoll::~WindowsPoll() {
}

std::vector<pollTuple> WindowsPoll::pollClients(ClientListType &clients, int timeout) {
	std::vector<pollTuple> readyClients;

	auto nfds = clients.size();
	auto fdarray = std::make_unique<WSAPOLLFD[]>(nfds);

	auto clientIt = clients.begin();
	for (size_t i = 0; i < nfds; ++i, ++clientIt) {
		fdarray[i].fd = clientIt->first;
		fdarray[i].events = POLLIN;
		if (clientIt->second->getHasOutput()) {
			fdarray[i].events |= POLLOUT;
		}
	}
	//std::cout << "# " << nfds << std::endl;

	if (WSAPoll(fdarray.get(),nfds,timeout) > 0) {
		for (size_t i = 0; i < nfds; ++i) {
			auto & c = fdarray[i];
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
