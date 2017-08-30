/*
 * WindowsFDFactory.h
 *
 *  Created on: 3 de jul de 2017
 *      Author: rvelloso
 */

#ifndef WIN_WINDOWSFDFACTORY_H_
#define WIN_WINDOWSFDFACTORY_H_

#include "FDFactory.h"

namespace socks {

class WindowsTCPFDFactory : public FDFactory {
public:
	WindowsTCPFDFactory();
	virtual ~WindowsTCPFDFactory();
	virtual SocketFDType operator()() override;
};

class WindowsUDPFDFactory : public FDFactory {
public:
	WindowsUDPFDFactory();
	virtual ~WindowsUDPFDFactory();
	virtual SocketFDType operator()() override;
};

}

#endif /* WIN_WINDOWSFDFACTORY_H_ */
