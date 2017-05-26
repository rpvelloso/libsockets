/*
 * WindowsMultiplexer.cpp
 *
 *  Created on: 26 de mai de 2017
 *      Author: rvelloso
 */

#include "WindowsMultiplexer.h"

WindowsMultiplexer::WindowsMultiplexer() {
}

WindowsMultiplexer::~WindowsMultiplexer() {
}

void WindowsMultiplexer::addClientSocket(std::unique_ptr<ClientSocket> clientSocket) {
	std::lock_guard<std::mutex> lock(clientsMutex);
	clientSocket->setNonBlockingIO(true);
	clients.emplace_back(std::move(clientSocket));
}

void WindowsMultiplexer::multiplex() {
}

void WindowsMultiplexer::cancel() {
}

size_t WindowsMultiplexer::clientCount() {
	std::lock_guard<std::mutex> lock(clientsMutex);
	return clients.size();
}

void WindowsMultiplexer::interrupt() {
}
