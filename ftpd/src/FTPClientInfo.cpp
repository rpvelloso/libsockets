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

#include <FTPClientInfo.h>

FTPClientInfo::FTPClientInfo() {
}

FTPClientInfo::~FTPClientInfo() {
}

const std::string& FTPClientInfo::getUsername() const {
	return username;
}

void FTPClientInfo::setUsername(const std::string& username) {
	this->username = username;
}

const std::string& FTPClientInfo::getCwd() const {
	return cwd;
}

void FTPClientInfo::setCwd(const std::string& cwd) {
	this->cwd = cwd;
}

const std::string& FTPClientInfo::getAddress() const {
	return address;
}

void FTPClientInfo::setAddress(const std::string& address) {
	this->address = address;
}

const std::string& FTPClientInfo::getPort() const {
	return port;
}

void FTPClientInfo::setPort(const std::string& port) {
	this->port = port;
}

const std::string& FTPClientInfo::getType() const {
	return type;
}

void FTPClientInfo::setType(const std::string& type) {
	this->type = type;
}

size_t FTPClientInfo::getSize() const {
	return size;
}

void FTPClientInfo::setSize(size_t size) {
	this->size = size;
}

const std::string& FTPClientInfo::getRenameFrom() const {
	return renameFrom;
}

void FTPClientInfo::setRenameFrom(const std::string& renameFrom) {
	this->renameFrom = renameFrom;
}

socks::ServerSocket& FTPClientInfo::getPassiveSocket() const {
	return *passiveSocket;
}

void FTPClientInfo::setPassiveSocket(std::unique_ptr<socks::ServerSocket> serverSocket) {
	this->passiveSocket = std::move(serverSocket);
}

std::fstream::pos_type FTPClientInfo::getRestartPos() const {
	return restartPos;
}

void FTPClientInfo::setRestartPos(std::fstream::pos_type restartPos) {
	this->restartPos = restartPos;
}

const std::string& FTPClientInfo::getPasvAddr() const {
	return pasvAddr;
}

void FTPClientInfo::setPasvAddr(socks::SocketAddress &addr) {
	pasvAddr = socketAddr2FTPAddr(addr);
}

const std::string& FTPClientInfo::getPeerAddr() const {
	return peerAddr;
}

void FTPClientInfo::setPeerAddr(socks::SocketAddress &addr) {
	peerAddr = addr.getHostname();
}

std::string FTPClientInfo::socketAddr2FTPAddr(socks::SocketAddress &addr) {
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
