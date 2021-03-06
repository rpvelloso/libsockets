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

#ifndef FILESYSTEM_LINUXFILESYSTEM_H_
#define FILESYSTEM_LINUXFILESYSTEM_H_

#include <sys/types.h>

#include "filesystem/FileSystem.h"
#include "filesystem/FileSystemImpl.h"

class LinuxFileSystem: public FileSystemImpl {
public:
	LinuxFileSystem();
	virtual ~LinuxFileSystem();

	bool changeWorkingDir(const std::string &dirname) override;
	bool makeDir(const std::string &dirname) override;
	bool size(const std::string &filename, size_t &sz) override;
	bool deleteFile(const std::string &filename) override;
	bool renameFile(const std::string &from, const std::string &to) override;
	std::vector<ListTuple> list(const std::string &path) override;
private:
	std::pair<std::string, std::string> fileOwner(uid_t uid);
};

#endif /* FILESYSTEM_LINUXFILESYSTEM_H_ */
