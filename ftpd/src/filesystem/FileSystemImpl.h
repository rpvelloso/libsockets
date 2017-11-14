/*
 * FileSystemImpl.h
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
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
