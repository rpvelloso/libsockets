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

#ifndef STATE_FTPCLIENTRENAME_H_
#define STATE_FTPCLIENTRENAME_H_

#include "filesystem/FileSystem.h"
#include "state/FTPClientState.h"

class FTPClientRename: public FTPClientState {
public:
	FTPClientRename(FTPClientInfo& ctx);
	virtual ~FTPClientRename();
	StateType getState() override;

	FTPReply RNTO(const std::string &path) override;
};

#endif /* STATE_FTPCLIENTRENAME_H_ */
