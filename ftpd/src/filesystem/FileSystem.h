/*
 * FileSystem.h
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
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
	std::string resolvePath(const std::string &cwd, const std::string &path);
	std::vector<ListTuple> list(const std::string &path);
private:
	std::unique_ptr<FileSystemImpl> impl;
};

extern FileSystem fs;

#endif /* FILESYSTEM_FILESYSTEM_H_ */
