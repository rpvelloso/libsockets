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

#ifndef LINUX_FACTORIES_LINUXFDFACTORY_H_
#define LINUX_FACTORIES_LINUXFDFACTORY_H_

#include "Factory/FDFactory.h"

namespace socks {

class LinuxTCPFDFactory : public FDFactory {
public:
	LinuxTCPFDFactory();
	virtual ~LinuxTCPFDFactory();
	virtual SocketFDType operator()() override;
};

class LinuxUDPFDFactory : public FDFactory {
public:
	LinuxUDPFDFactory();
	virtual ~LinuxUDPFDFactory();
	virtual SocketFDType operator()() override;
};

}

#endif /* LINUX_FACTORIES_LINUXFDFACTORY_H_ */
