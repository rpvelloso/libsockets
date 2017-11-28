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

#include <state/FTPClientTransfer.h>
#include "filesystem/FileSystem.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sys/stat.h>

FTPClientTransfer::FTPClientTransfer(
		FTPClientInfo &ctx,
		std::ostream &outp,
		std::function<socks::ClientSocket()> getDataSocket) :
		FTPClientState(ctx),
		outp(outp),
		loggedIn(ctx),
		getDataSocket(getDataSocket) {
}

FTPClientTransfer::~FTPClientTransfer() {
}

FTPReply FTPClientTransfer::LIST(const std::string& path) {
	try {
		outp << context.buildReplyString(FTPReply::R150) << std::endl;

		auto dataSocket = getDataSocket();
		auto fileList = fs.list(fs.resolvePath(context.getCwd(), path));

		for (auto &file:fileList) {
			auto mode = std::get<3>(file);
			std::string modeStr(10,'-');
			modeStr[0] =  (S_ISDIR(mode)?'d':'-');
			modeStr[1] = ((S_IRUSR&mode)?'r':'-');
			modeStr[2] = ((S_IWUSR&mode)?'w':'-');
			modeStr[3] = ((S_IXUSR&mode)?'x':'-');
			modeStr[4] = ((S_IRGRP&mode)?'r':'-');
			modeStr[5] = ((S_IWGRP&mode)?'w':'-');
			modeStr[6] = ((S_IXGRP&mode)?'x':'-');
			modeStr[7] = ((S_IROTH&mode)?'r':'-');
			modeStr[8] = ((S_IWOTH&mode)?'w':'-');
			modeStr[9] = ((S_IXOTH&mode)?'x':'-');

			auto nlinks = std::get<5>(file);
			auto user = std::get<1>(file);
			auto group = std::get<2>(file);
			auto filesize = std::get<4>(file);
			auto filedate = std::get<6>(file);
			auto filename = std::get<0>(file);

			std::stringstream ss;
			ss <<
				modeStr << " " <<
				nlinks << " " <<
				std::setw(8) << std::left << user << " " <<
				group << " " <<
				/*std::setw(11) <<*/ std::right << filesize << " " <<
				filedate << " " <<
				filename << "\r\n";
			dataSocket.sendData(ss.str().c_str(), ss.str().size());
		}
		return FTPReply::R226;
	} catch (...) {
		return FTPReply::R425;
	}
}

FTPReply FTPClientTransfer::RETR(const std::string& filename) {
	try {
		outp << context.buildReplyString(FTPReply::R150) << std::endl;

		auto dataSocket = getDataSocket();
		std::fstream file(
			fs.resolvePath(context.getCwd(),filename),
			std::ios::binary|std::ios::in);

		if (file.is_open()) {
			auto restartPos = context.getRestartPos();
			file.seekg(restartPos);
			if (file.tellg() == restartPos) {
				sendFile(file, dataSocket);
				return FTPReply::R226;
			}
		}
		return FTPReply::R425;
	} catch (...) {
		return FTPReply::R425;
	}
}

FTPReply FTPClientTransfer::STOR(const std::string& filename) {
	try {
		outp << context.buildReplyString(FTPReply::R150) << std::endl;

		auto dataSocket = getDataSocket();
		std::fstream file(
			fs.resolvePath(context.getCwd(),filename),
			std::ios::binary|std::ios::out);

		if (file.is_open()) {
			receiveFile(dataSocket, file);
			return FTPReply::R226;
		}
		return FTPReply::R425;
	} catch (...) {
		return FTPReply::R425;
	}
}

FTPReply FTPClientTransfer::APPE(const std::string& filename) {
	try {
		outp << context.buildReplyString(FTPReply::R150) << std::endl;

		auto dataSocket = getDataSocket();
		std::fstream file(
			fs.resolvePath(context.getCwd(),filename),
			std::ios::binary|std::ios::app|std::ios::out);

		if (file.is_open()) {
			receiveFile(dataSocket, file);
			return FTPReply::R226;
		}
		return FTPReply::R425;
	} catch (...) {
		return FTPReply::R425;
	}
}

FTPReply FTPClientTransfer::REST(const std::string& pos) {
	if (pos.empty())
		return FTPReply::R501;

	context.setRestartPos(std::stoul(pos));
	return FTPReply::R350;
}

void FTPClientTransfer::receiveFile(socks::ClientSocket& source, std::fstream &dest) {
	std::unique_ptr<char> bufPtr(new char[bufSize]);
	auto buf = bufPtr.get();

	int len;
	while ((len = source.receiveData(static_cast<void *>(buf), bufSize)) > 0)
		dest.write(static_cast<char *>(buf), len);
}

FTPReply FTPClientTransfer::CWD(const std::string& path) {
	return loggedIn.CWD(path);
}

void FTPClientTransfer::sendFile(std::fstream &source, socks::ClientSocket& dest) {
	std::unique_ptr<char> bufPtr(new char[bufSize]);
	auto buf = bufPtr.get();

	std::fstream::off_type len;
	while ((len = source.read(static_cast<char *>(buf), bufSize).gcount()) > 0)
		dest.sendData(static_cast<void *>(buf), len);
}

StateType FTPClientTransfer::getState() {
	return StateType::Transfer;
}
