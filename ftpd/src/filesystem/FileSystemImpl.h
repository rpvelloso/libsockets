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

#ifndef FILESYSTEM_FILESYSTEMIMPL_H_
#define FILESYSTEM_FILESYSTEMIMPL_H_

#include <string>
#include <vector>
#include <time.h>

// <filename, user, group, mode, size, nlink, date>
using ListTuple = std::tuple<std::string, std::string, std::string, size_t, size_t, size_t, std::string>;

class FileSystemImpl {
public:
	FileSystemImpl();
	virtual ~FileSystemImpl();

	virtual bool changeWorkingDir(const std::string &) = 0;
	virtual bool makeDir(const std::string &) = 0;
	virtual bool size(const std::string &, size_t &) = 0;
	virtual bool deleteFile(const std::string &) = 0;
	virtual bool renameFile(const std::string &, const std::string &) = 0;
	virtual std::vector<ListTuple> list(const std::string &path) = 0;

	std::string resolvePath(const std::string &cwd, const std::string &path);
protected:
	std::string dateToString(struct tm& t);
};

#endif /* FILESYSTEM_FILESYSTEMIMPL_H_ */
