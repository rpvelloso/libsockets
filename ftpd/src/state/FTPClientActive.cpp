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

		dataSocket <<
			modeStr << " " <<
			std::get<5>(file) << " " <<
			std::setw(8) << std::left << std::get<1>(file) << " " <<
			std::get<2>(file) << " " <<
			std::setw(11) << std::right << std::get<4>(file) << " " <<
			std::get<6>(file) << " " <<
			std::get<0>(file) << std::endl;
	}
	dataSocket.sync();
	return FTPReply::R226;
}

FTPReply FTPClientActive::RETR(const std::string& filename) {
	std::fstream file(
		fs.resolvePath(context.getCwd(),filename),
		std::ios::binary|std::ios::in);

	if (file.is_open()) {
		dataSocket << file.rdbuf();
		dataSocket.sync();
		return FTPReply::R226;
	}
	return FTPReply::R425;
}

FTPReply FTPClientActive::STOR(const std::string& filename) {
	std::fstream file(
		fs.resolvePath(context.getCwd(),filename),
		std::ios::binary|std::ios::out);

	if (file.is_open()) {
		file << dataSocket.rdbuf();
		file.sync();
		dataSocket.sync();
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
