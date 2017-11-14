/*
 * FileSystem.cpp
 *
 *  Created on: 13 de nov de 2017
 *      Author: rvelloso
 */

#include "FileSystem.h"

FileSystem::FileSystem(FileSystemImpl *impl) : impl(impl) {
}

FileSystem::~FileSystem() {
}

bool FileSystem::changeWorkingDir(const std::string& dirname) {
	return impl->changeWorkingDir(dirname);
}

bool FileSystem::makeDir(const std::string& dirname) {
	return impl->makeDir(dirname);
}

bool FileSystem::size(const std::string &filename, size_t &sz) {
	return impl->size(filename, sz);
}

bool FileSystem::deleteFile(const std::string& filename) {
	return impl->deleteFile(filename);
}

bool FileSystem::renameFile(const std::string& from,
		const std::string& to) {
	return impl->renameFile(from, to);
}

std::vector<ListTuple> FileSystem::list(const std::string &path) {
	return impl->list(path);
}

std::string FileSystem::resolvePath(const std::string& cwd,
		const std::string& path) {
	return impl->resolvePath(cwd, path);
}
