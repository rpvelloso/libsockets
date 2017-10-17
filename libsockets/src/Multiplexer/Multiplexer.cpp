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

#include "Factory/SocketFactory.h"
#include "Multiplexer/Multiplexer.h"

namespace socks {

Multiplexer::Multiplexer(MultiplexerImpl *impl) : impl(impl) {
	thread.reset(new std::thread([](MultiplexerImpl &multiplexer){
		multiplexer.multiplex();
	},
	std::ref(*(this->impl)) ));
};

Multiplexer::~Multiplexer() {
	if (impl)
		impl->cancel();

	if (thread)
		thread->join();
};

void Multiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
	impl->addClientSocket(std::move(clientSocket));
};

void Multiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket,
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {
	impl->addClientSocket(
			std::move(clientSocket),
			readCallback,
			connectCallback,
			disconnectCallback,
			writeCallback
	);
};

void Multiplexer::multiplex() {
	impl->multiplex();
};
void Multiplexer::cancel() {
	impl->cancel();
};
void Multiplexer::interrupt() {
	impl->interrupt();
};
size_t Multiplexer::getClientCount() {
	return impl->getClientCount();
};

namespace factory {
	Multiplexer makeMultiplexer(
		MultiplexerCallback readCallback,
		MultiplexerCallback connectCallback,
		MultiplexerCallback disconnectCallback,
		MultiplexerCallback writeCallback) {
		return Multiplexer(new MultiplexerImpl(
				socketFactory.createPoll(),
				readCallback,
				connectCallback,
				disconnectCallback,
				writeCallback));
	};
}

}




