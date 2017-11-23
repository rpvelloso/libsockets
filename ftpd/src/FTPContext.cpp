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

#include "FTPContext.h"

FTPContext::FTPContext() {
}

FTPContext::~FTPContext() {
}

const std::string& FTPContext::getUsername() const {
	return username;
}

void FTPContext::setUsername(const std::string& username) {
	this->username = username;
}

const std::string& FTPContext::getCwd() const {
	return cwd;
}

void FTPContext::setCwd(const std::string& cwd) {
	this->cwd = cwd;
}

const std::string& FTPContext::getAddress() const {
	return address;
}

void FTPContext::setAddress(const std::string& address) {
	this->address = address;
}

const std::string& FTPContext::getPort() const {
	return port;
}

void FTPContext::setPort(const std::string& port) {
	this->port = port;
}

const std::string& FTPContext::getType() const {
	return type;
}

void FTPContext::setType(const std::string& type) {
	this->type = type;
}

size_t FTPContext::getSize() const {
	return size;
}

void FTPContext::setSize(size_t size) {
	this->size = size;
}

const std::string& FTPContext::getRenameFrom() const {
	return renameFrom;
}

void FTPContext::setRenameFrom(const std::string& renameFrom) {
	this->renameFrom = renameFrom;
}

socks::ServerSocket& FTPContext::getPassiveSocket() const {
	return *passiveSocket;
}

void FTPContext::setPassiveSocket(std::unique_ptr<socks::ServerSocket> serverSocket) {
	this->passiveSocket = std::move(serverSocket);
}

std::fstream::pos_type FTPContext::getRestartPos() const {
	return restartPos;
}

void FTPContext::setRestartPos(std::fstream::pos_type restartPos) {
	this->restartPos = restartPos;
}

const std::string& FTPContext::getPasvAddr() const {
	return pasvAddr;
}

void FTPContext::setPasvAddr(socks::SocketAddress &addr) {
	pasvAddr = socketAddr2FTPAddr(addr);
}

const std::string& FTPContext::getPeerAddr() const {
	return peerAddr;
}

void FTPContext::setPeerAddr(socks::SocketAddress &addr) {
	peerAddr = addr.getHostname();
}

std::string FTPContext::socketAddr2FTPAddr(socks::SocketAddress &addr) {
	std::string ftpAddr = addr.getHostname() + ",";
	std::transform(
			ftpAddr.begin(),
			ftpAddr.end(),
			ftpAddr.begin(),
		[](char c){
		if (c == '.')
			return ',';
		else
			return c;
	});

	return ftpAddr;
}
