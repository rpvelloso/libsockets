/*
 * WindowsFileSystem.h
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
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
};

#endif /* FILESYSTEM_WINDOWSFILESYSTEM_H_ */
