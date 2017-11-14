/*
 * FTPClientActive.cpp
 *
 *  Created on: 14 de nov de 2017
 *      Author: rvelloso
 */

#include "state/FTPClientActive.h"
#include "filesystem/FileSystem.h"
#include <iomanip>
#include <sys/stat.h>

FTPClientActive::FTPClientActive(FTPContext &ctx) : FTPClientState(ctx) {
}

FTPClientActive::~FTPClientActive() {
}

FTPReply FTPClientActive::LIST(const std::string& path, int type) {
	auto fileList = fs.list(fs.resolvePath(context.getCwd(), path));

	if (dataSocket.connectTo(context.getAddress(), context.getPort()) == 0) {
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
				std::get<0>(file) << "\r\n";//std::endl;
		}
		dataSocket.sync();
		return FTPReply::R226;
	}
	return FTPReply::R425;
}

FTPReply FTPClientActive::RETR(const std::string& filename) {
}

FTPReply FTPClientActive::STOR(const std::string& filename) {
}

FTPReply FTPClientActive::REST(const std::string& pos) {
}
