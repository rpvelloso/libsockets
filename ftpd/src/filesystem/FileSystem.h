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

#ifndef FILESYSTEM_FILESYSTEM_H_
#define FILESYSTEM_FILESYSTEM_H_

#include "FileSystemImpl.h"
#include <memory>
#include <tuple>

class FileSystem {
public:
	FileSystem(FileSystemImpl *impl);
	virtual ~FileSystem();

	bool changeWorkingDir(const std::string &dirname);
	bool makeDir(const std::string &dirname);
	bool size(const std::string &filename, size_t &sz);
	bool deleteFile(const std::string &filename);
	bool renameFile(const std::string &from, const std::string &to);
	std::string resolvePath(const std::string &cwd, const std::string &path = "");
	std::vector<ListTuple> list(const std::string &path);
private:
	std::unique_ptr<FileSystemImpl> impl;
};

extern FileSystem fs;

#endif /* FILESYSTEM_FILESYSTEM_H_ */
