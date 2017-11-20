/*
 * FTPClientActive.cpp
 *
 *  Created on: 14 de nov de 2017
 *      Author: rvelloso
 */

#include "state/FTPClientActive.h"
#include "filesystem/FileSystem.h"
#include <iomanip>
#include <fstream>
#include <sys/stat.h>

FTPClientActive::FTPClientActive(FTPContext &ctx) : FTPClientState(ctx) {
	if (dataSocket.connectTo(context.getAddress(), context.getPort()) != 0)
		throw std::runtime_error("Can't open data connection");
}

FTPClientActive::~FTPClientActive() {
}

FTPReply FTPClientActive::LIST(const std::string& path, int type) {
	auto fileList = fs.list(fs.resolvePath(context.getCwd(), path));

	for (auto &file:fileList) {
		auto mode = std::get<3>(file);
		std::string modeStr = S_ISDIR(mode)?"d---------":"----------";
		modeStr[1] = ((S_IRUSR&mode)?'r':'-');
		modeStr[2] = ((S_IWUSR&mode)?'w':'-');
		modeStr[3] = ((S_IXUSR&mode)?'x':'-');
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
}

FTPReply FTPClientActive::RETR(const std::string& filename) {
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
}

FTPReply FTPClientActive::STOR(const std::string& filename) {
	std::fstream file(
		fs.resolvePath(context.getCwd(),filename),
		std::ios::binary|std::ios::out);

	if (file.is_open()) {
		receiveFile(dataSocket, file);
		return FTPReply::R226;
	}
	return FTPReply::R425;
}

FTPReply FTPClientActive::APPE(const std::string& filename) {
	std::fstream file(
		fs.resolvePath(context.getCwd(),filename),
		std::ios::binary|std::ios::app|std::ios::out);

	if (file.is_open()) {
		receiveFile(dataSocket, file);
		return FTPReply::R226;
	}
	return FTPReply::R425;
}

FTPReply FTPClientActive::REST(const std::string& pos) {
	if (pos.empty())
		return FTPReply::R501;

	context.setRestartPos(std::stoul(pos));
	return FTPReply::R350;
}

void FTPClientActive::receiveFile(socks::ClientSocket& source, std::fstream &dest) {
	std::unique_ptr<char> bufPtr(new char[bufSize]);
	auto buf = bufPtr.get();

	int len;
	while ((len = source.receiveData(static_cast<void *>(buf), bufSize)) > 0)
		dest.write(static_cast<char *>(buf), len);
}

void FTPClientActive::sendFile(std::fstream &source, socks::ClientSocket& dest) {
	std::unique_ptr<char> bufPtr(new char[bufSize]);
	auto buf = bufPtr.get();

	std::fstream::off_type len;
	while ((len = source.read(static_cast<char *>(buf), bufSize).gcount()) > 0)
		dest.sendData(static_cast<void *>(buf), len);
}
