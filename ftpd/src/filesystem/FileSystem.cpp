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

std::string FileSystem::resolvePath(
	const std::string& chroot,
	const std::string& cwd,
	const std::string& path) {

	auto resolvedPath = impl->resolvePath(cwd, path);
	auto res = std::mismatch(chroot.begin(), chroot.end(), resolvedPath.begin());

	if (!(res.first == chroot.end()))
		resolvedPath = chroot;

	return resolvedPath;
}
