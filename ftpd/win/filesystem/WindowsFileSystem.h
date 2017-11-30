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

#ifndef FILESYSTEM_WINDOWSFILESYSTEM_H_
#define FILESYSTEM_WINDOWSFILESYSTEM_H_

#include "filesystem/FileSystem.h"
#include "filesystem/FileSystemImpl.h"

class WindowsFileSystem: public FileSystemImpl {
public:
	WindowsFileSystem();
	virtual ~WindowsFileSystem();

	bool changeWorkingDir(const std::string &dirname) override;
	bool makeDir(const std::string &dirname) override;
	bool size(const std::string &filename, size_t &sz) override;
	bool deleteFile(const std::string &filename) override;
	bool renameFile(const std::string &from, const std::string &to) override;
	std::vector<ListTuple> list(const std::string &path) override;
private:
	std::string driveLetter;
	std::string path2Windows(const std::string &path);
	std::pair<std::string, std::string> fileOwner(const std::string &filename);
};

#endif /* FILESYSTEM_WINDOWSFILESYSTEM_H_ */
