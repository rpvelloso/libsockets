/*
 * WindowsFDFactory.h
 *
 *  Created on: 3 de jul de 2017
 *      Author: rvelloso
 */

#ifndef WIN_LINUXFDFACTORY_H_
#define WIN_LINUXFDFACTORY_H_

#include "FDFactory.h"

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

#endif /* WIN_LINUXFDFACTORY_H_ */
