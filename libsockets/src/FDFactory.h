/*
 * FDFactory.h
 *
 *  Created on: 3 de jul de 2017
 *      Author: rvelloso
 */

#ifndef SRC_FDFACTORY_H_
#define SRC_FDFACTORY_H_

#include <memory>
#include "defs.h"

namespace socks {

class FDFactory {
public:
	FDFactory() {};
	FDFactory(FDFactory *impl) : impl(impl) {};
	virtual ~FDFactory() {};
	virtual SocketFDType operator()() { return (*impl)(); };
private:
	std::unique_ptr<FDFactory> impl;
};

extern FDFactory TCPFDFactory;
extern FDFactory UDPFDFactory;

}

#endif /* SRC_FDFACTORY_H_ */
