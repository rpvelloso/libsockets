/*
 * FTPClientRename.cpp
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
 */

#include <state/FTPClientRename.h>

FTPClientRename::FTPClientRename(FTPContext& ctx) : FTPClientState(ctx) {
}

FTPClientRename::~FTPClientRename() {
}

FTPReply FTPClientRename::RNTO(const std::string& path) {
	if (path.empty())
		return FTPReply::R501;

	if (fs.renameFile(context.getRenameFrom(), fs.resolvePath(context.getCwd(), path)))
		return FTPReply::R250;

	return FTPReply::R550_RNTO;
}
