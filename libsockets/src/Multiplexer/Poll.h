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

#ifndef SRC_MULTIPLEXER_POLL_H_
#define SRC_MULTIPLEXER_POLL_H_

#include <vector>

#include "Multiplexer/MultiplexerImpl.h"

namespace socks {

// <client, read, write>, error: read=false, write=false
using pollTuple = std::tuple<BufferedClientSocketInterface &, bool, bool>;

class Poll {
public:
	Poll() {};
	virtual ~Poll() {};
	virtual std::vector<pollTuple> pollClients(ClientListType &clients, int timeout = -1) = 0;
	// timeout in milliseconds, default -1 = no timeout
};

}
#endif /* SRC_MULTIPLEXER_POLL_H_ */
