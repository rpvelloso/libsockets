/*
 * FTPContext.cpp
 *
 *  Created on: 31 de out de 2017
 *      Author: rvelloso
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

size_t FTPContext::getRestartPos() const {
	return restartPos;
}

void FTPContext::setRestartPos(size_t restartPos) {
	this->restartPos = restartPos;
}
