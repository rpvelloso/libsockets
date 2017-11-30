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

#include <sstream>

#include "filesystem/FileSystem.h"
#include "state/FTPClientLoggedIn.h"

FTPClientLoggedIn::FTPClientLoggedIn(FTPClientInfo& ctx) : FTPClientState(ctx) {
}

FTPClientLoggedIn::~FTPClientLoggedIn() {
}

FTPReply FTPClientLoggedIn::MODE(const std::string& mode) {
	if (mode == "S") {
		return FTPReply::R200_MODE;
	} else
		return FTPReply::R504;
}

FTPReply FTPClientLoggedIn::STRU(const std::string& structure) {
	if (structure == "F") {
		return FTPReply::R200_STRU;
	} else
		return FTPReply::R504;
}

FTPReply FTPClientLoggedIn::TYPE(const std::string& type) {
	if (type == "A" || type == "I") {
		clientInfo.setType(type);
		return FTPReply::R200_TYPE;
	} else
		return FTPReply::R504;
}

FTPReply FTPClientLoggedIn::CWD(const std::string& path) {
	if (path.empty())
		return FTPReply::R501;

	auto newPath = fs.resolvePath(clientInfo.getChroot(), clientInfo.getCwd(), path);
	if (fs.changeWorkingDir(newPath)) {
		clientInfo.setCwd(newPath);
		return FTPReply::R250;
	}

	return FTPReply::R550_CWD;
}

FTPReply FTPClientLoggedIn::MKD(const std::string& path) {
	if (path.empty())
		return FTPReply::R501;

	if (fs.makeDir(fs.resolvePath(clientInfo.getChroot(), clientInfo.getCwd(), path)))
		return FTPReply::R257;

	return FTPReply::R550_MKD;
}

FTPReply FTPClientLoggedIn::SIZE(const std::string& path) {
	if (path.empty())
		return FTPReply::R501;

	size_t sz;
	if (fs.size(fs.resolvePath(clientInfo.getChroot(), clientInfo.getCwd(), path), sz)) {
		clientInfo.setSize(sz);
		return FTPReply::R213;
	}

	return FTPReply::R550_SIZE2;
}

FTPReply FTPClientLoggedIn::HELP() {
	return FTPReply::R211;
}

FTPReply FTPClientLoggedIn::SYST() {
	return FTPReply::R215;
}

FTPReply FTPClientLoggedIn::PWD() {
	return FTPReply::R257_PWD;
}

FTPReply FTPClientLoggedIn::DELE(const std::string& path) {
	if (path.empty())
		return FTPReply::R501;

	if (fs.deleteFile(fs.resolvePath(clientInfo.getChroot(), clientInfo.getCwd(), path)))
		return FTPReply::R250;

	return FTPReply::R550_DELE;
}

FTPReply FTPClientLoggedIn::RNFR(const std::string& path) {
	if (path.empty())
		return FTPReply::R501;

	clientInfo.setRenameFrom(fs.resolvePath(clientInfo.getChroot(), clientInfo.getCwd(), path));
	return FTPReply::R350_RNFR;
}

FTPReply FTPClientLoggedIn::PORT(const std::string& addr) {
	int port[6];
	int portNumber;
	std::stringstream addrStream(addr);
	std::string param;

	for (size_t i = 0; i < 6; ++i) {
		std::getline(addrStream, param, ',');
		if (param.empty())
			return FTPReply::R501;
		port[i] = std::stoi(param);
		if (port[i] < 0 || port[i] > 255)
			return FTPReply::R501;
	}

	portNumber =
			((port[4] << 8) & 0xff00) |
			((port[5]) & 0xff);

	if (portNumber <= 0 || portNumber > 65535)
		return FTPReply::R501;

	clientInfo.setAddress(
			std::to_string(port[0]) + "." +
			std::to_string(port[1]) + "." +
			std::to_string(port[2]) + "." +
			std::to_string(port[3]));
	clientInfo.setPort(std::to_string(portNumber));
	return FTPReply::R200;
}

FTPReply FTPClientLoggedIn::PASV() {
	std::unique_ptr<socks::ServerSocket> passiveSocket(new socks::ServerSocket());

	if (passiveSocket->listenForConnections("0.0.0.0", "") == 0) {
		clientInfo.setPassiveSocket(std::move(passiveSocket));
		return FTPReply::R227;
	}

	return FTPReply::R425;
}

FTPReply FTPClientLoggedIn::SITE(const std::string &parameters) {
	std::stringstream ss(parameters);
	std::string command, siteParams;

	ss >> command;
	std::getline(ss, siteParams);
	if (!siteParams.empty())
		siteParams = siteParams.substr(siteParams.find_first_not_of(' '));

	std::transform(command.begin(), command.end(), command.begin(), ::toupper);

	if (command == "EXEC")
		return FTPReply::R501_EXEC;

	auto callback = clientInfo.getSiteCommand(command);
	if (callback != nullptr) {
		clientInfo.setCustomSiteReply((*callback)(siteParams, clientInfo));
		return FTPReply::RSITE_CUSTOM;
	}

	return FTPReply::R500;
}

StateType FTPClientLoggedIn::getState() {
	return StateType::LoggedIn;
}
