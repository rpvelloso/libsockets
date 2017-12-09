#ifndef _LIBSOCKETS_H
#define _LIBSOCKETS_H

#include <string>
#include <vector>
#include <atomic>
#include <tuple>
#include <mutex>
#include <thread>
#include <utility>
#include <memory>
#include <functional>
#include <streambuf>
#include <iostream>
#include <sstream>
#include <unordered_map>

namespace socks {

typedef int SocketFDType;
const SocketFDType InvalidSocketFD = -1;

}
namespace socks {

class SocketAddressImpl {
public:
 SocketAddressImpl() {};
 virtual ~SocketAddressImpl() {};
 virtual bool operator==(const SocketAddressImpl &rhs) = 0;
 virtual std::string getHostname() const = 0;
 virtual std::string getPort() const = 0;
 virtual void *getSocketAddress() const = 0;
 virtual void setSocketAddressSize(int saSize) = 0;
 virtual int getSocketAddressSize() const = 0;
};

}
namespace socks {

enum class SocketProtocol {
 TCP,
 UDP
};

class SocketAddress {
friend class SocketImpl;
public:
 SocketAddress() = delete;
 SocketAddress(SocketAddressImpl *impl);

 bool operator==(const SocketAddress &rhs);
 bool operator!=(const SocketAddress &rhs);
 std::string getHostname() const;
 std::string getPort() const;
 void *getSocketAddress() const;
 void setSocketAddressSize(int saSize);
 int getSocketAddressSize() const;
protected:
 std::unique_ptr<SocketAddressImpl> impl;
};

namespace factory {
 SocketAddress makeSocketAddress(
   const std::string &host,
   const std::string &port,
   SocketProtocol protocol = SocketProtocol::UDP);
}

}
namespace socks {

class ClientSocket;

enum class SocketStateType {
 Disconnected = 0,
 Connected = 1,
 Listening = 2,
 Closed = 3
};

class SocketImpl {
public:
 SocketImpl() : socketState(SocketStateType::Disconnected) {};
 virtual ~SocketImpl() {};
 virtual int receiveData(void *buf, size_t len) = 0;
 virtual int sendData(const void *buf, size_t len) = 0;
 virtual std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len) = 0;
 virtual int sendTo(const SocketAddress &addr, const void *buf, size_t len) = 0;
 virtual int connectTo(const std::string &host, const std::string &port) = 0;
 virtual void disconnect() = 0;
 virtual int bindSocket(const std::string &bindAddr, const std::string &port) = 0;
 virtual int listenForConnections(const std::string &bindAddr, const std::string &port) = 0;
 virtual std::unique_ptr<SocketImpl> acceptConnection() = 0;
 virtual int setNonBlockingIO(bool status) = 0;
 virtual int reuseAddress() = 0;
 virtual std::string getPort() = 0;
 virtual size_t getSendBufferSize() = 0;
 virtual size_t getReceiveBufferSize() = 0;
 virtual SocketAddress getLocalAddress() = 0;
 virtual SocketAddress getRemoteAddress() = 0;
 virtual SocketStateType getSocketState() {
  return socketState;
 };
 virtual void setSocketState(SocketStateType socketState) {
  this->socketState = socketState;
 };
 virtual SocketFDType getFD() {
  return fd;
 }
protected:
 SocketImpl(SocketFDType fd) : fd(fd), socketState(SocketStateType::Connected) {};
 SocketFDType fd = InvalidSocketFD;
 SocketStateType socketState;
};

}
namespace socks {

class SocketState : public SocketImpl {
public:
 SocketState(SocketImpl &impl);
 virtual ~SocketState();
 int receiveData(void *buf, size_t len);
 int sendData(const void *buf, size_t len);
 std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len);
 int sendTo(const SocketAddress &addr, const void *buf, size_t len);
 int connectTo(const std::string &host, const std::string &port);
 void disconnect();
 int bindSocket(const std::string &bindAddr, const std::string &port);
 int listenForConnections(const std::string &bindAddr, const std::string &port);
 std::unique_ptr<SocketImpl> acceptConnection();
 std::string getPort();
 size_t getSendBufferSize();
 size_t getReceiveBufferSize();
 int setNonBlockingIO(bool status) override;
 int reuseAddress() override;
 SocketAddress getLocalAddress();
 SocketAddress getRemoteAddress();
protected:
 SocketImpl &impl;
};

}
namespace socks {

class Socket {
public:
 Socket(Socket &&) = default;
 virtual Socket &operator=(Socket &&) = default;
 Socket(SocketImpl *impl);
 virtual ~Socket();
 virtual int setNonBlockingIO(bool status);
 virtual std::string getPort();
 virtual SocketImpl &getImpl();
 virtual SocketAddress getLocalAddress();
 virtual SocketAddress getRemoteAddress();
protected:
 std::unique_ptr<SocketImpl> impl;
 std::unique_ptr<SocketState> state;
private:
 Socket() = delete;
};

}
namespace socks {

class ClientSocket : public Socket {
public:
 ClientSocket(ClientSocket &&) = default;
 ClientSocket &operator=(ClientSocket &&);

 ClientSocket(SocketImpl *impl);
 ClientSocket();
 virtual ~ClientSocket();
 int receiveData(void *buf, size_t len);
 int sendData(const void *buf, size_t len);
 int connectTo(const std::string &host, const std::string &port);
 void disconnect();
 size_t getSendBufferSize() const;
 size_t getReceiveBufferSize() const;

private:
 size_t sendBufferSize = 0;
 size_t receiveBufferSize = 0;
};

namespace factory {
 ClientSocket makeClientSocket();
}

}
namespace socks {

class ServerSocket : public Socket {
public:
 ServerSocket(ServerSocket &&) = default;
 ServerSocket(SocketImpl *impl);
 ServerSocket();
 virtual ~ServerSocket();
 int listenForConnections(const std::string &bindAddr, const std::string &port);
 ClientSocket acceptConnection();
 void disconnect();
};

namespace factory {
 ServerSocket makeServerSocket();
}

}
namespace socks {

class DatagramSocket: public Socket {
public:
 DatagramSocket(DatagramSocket &&) = default;
 DatagramSocket(SocketImpl *impl);
 DatagramSocket();
 virtual ~DatagramSocket();
 std::pair<int, SocketAddress> receiveFrom(void *buf, size_t len);
 int sendTo(const SocketAddress &addr, const void *buf, size_t len);
 int bindSocket(const std::string &bindAddr, const std::string &port);
 ClientSocket makeClientSocket(const SocketAddress &addr);
 ClientSocket makeClientSocket(const std::string &host, const std::string &port);
};

namespace factory {
 ClientSocket makeUDPClientSocket();
 DatagramSocket makeDatagramSocket();
}

}
namespace socks {

class SocketStreamBuf : public std::streambuf {
public:
 SocketStreamBuf(ClientSocket &clientSocket);
 virtual ~SocketStreamBuf();
 ClientSocket &getClientSocket();
protected:
    int_type underflow() override;
    int_type overflow(int_type __c = traits_type::eof()) override;
    int sync() override;
private:
    static constexpr size_t buffSize = 4096;
    std::unique_ptr<char []> inp, outp;
    ClientSocket &clientSocket;

    int_type transmit();
};

class SocketStream : public std::iostream {
public:
 SocketStream(std::unique_ptr<ClientSocket> clientSocket);
 SocketStream();
 SocketStream(SocketStream &&);
 SocketStream(ClientSocket &);
 int connectTo(const std::string &host, const std::string &port);
 ClientSocket &getClientSocket();
private:
 std::unique_ptr<ClientSocket> clientSocket;
 std::unique_ptr<SocketStreamBuf> socketStreamBuf;
};

namespace factory {
 SocketStream makeSocketStream();
 SocketStream makeUDPSocketStream();
}

}
namespace socks {





class BufferedClientSocketInterface {
public:
 BufferedClientSocketInterface() {};
 virtual ~BufferedClientSocketInterface() {};
 virtual bool getHasOutput() = 0;
 virtual std::stringstream &getOutputBuffer() = 0;
 virtual std::stringstream &getInputBuffer() = 0;

 virtual int receiveData(void *buf, size_t len) = 0;
 virtual int sendData(const void *buf, size_t len) = 0;
 virtual size_t getSendBufferSize() const = 0;
 virtual size_t getReceiveBufferSize() const = 0;
 virtual SocketImpl &getImpl() = 0;
 virtual ClientSocket &getSocket() = 0;
 virtual void readCallback() = 0;
 virtual void connectCallback() = 0;
 virtual void disconnectCallback() = 0;
 virtual void writeCallback() = 0;
 virtual int setNonBlockingIO(bool status) = 0;
};

}
namespace socks {

template<class ClientContext>
class Context {
public:
 Context(ClientSocket &clientSocket) :
  localAddress(clientSocket.getLocalAddress()),
  remoteAddress(clientSocket.getRemoteAddress()) {
 }

 ClientContext &getContext() {
  return context;
 }

 SocketAddress &getLocalAddress() {
  return localAddress;
 }

 SocketAddress &getRemoteAddress() {
  return remoteAddress;
 }
private:
 ClientContext context;
 SocketAddress localAddress, remoteAddress;
};

template<class ClientContext>
using ClientCallback = std::function<void(Context<ClientContext> &, std::istream &, std::ostream &)>;





template<class ClientContext>
class BufferedClientSocket : public BufferedClientSocketInterface {
public:
 BufferedClientSocket(
  std::unique_ptr<ClientSocket> impl,
  ClientCallback<ClientContext> readCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) :
  impl(std::move(impl)),
  readCB(readCallback),
  connectCB(connectCallback),
  disconnectCB(disconnectCallback),
  writeCB(writeCallback),
  outputSocketBuffer(*(this->impl)),
  outp(&outputSocketBuffer),
  clientData(*(this->impl)) {};
 virtual ~BufferedClientSocket() {};
 bool getHasOutput() override {return outputBuffer.rdbuf()->in_avail() > 0;};
 std::stringstream &getOutputBuffer() override {return outputBuffer;};
 std::stringstream &getInputBuffer() override {return inputBuffer;};

 int receiveData(void *buf, size_t len) override {return impl->receiveData(buf, len);};
 int sendData(const void *buf, size_t len) override {return impl->sendData(buf, len);};
 size_t getSendBufferSize() const override {return impl->getSendBufferSize();};
 size_t getReceiveBufferSize() const override {return impl->getReceiveBufferSize();};
 int setNonBlockingIO(bool status) override {
  if (!status)
   outp = &outputSocketBuffer;
  else
   outp = &outputBuffer;

  return impl->setNonBlockingIO(status);
 };
 SocketImpl &getImpl() override {return impl->getImpl();};
 ClientSocket &getSocket() override {return *impl;};
 void readCallback() override {
  inputBuffer.clear();
  readCB(clientData, inputBuffer, *outp);
 };
 void connectCallback() override {
  inputBuffer.clear();
  connectCB(clientData, inputBuffer, *outp);
 };
 void disconnectCallback() override {
  inputBuffer.clear();
  disconnectCB(clientData, inputBuffer, *outp);
 };
 void writeCallback() override {
  inputBuffer.clear();
  writeCB(clientData, inputBuffer, *outp);
 };
private:
 std::unique_ptr<ClientSocket> impl;
 ClientCallback<ClientContext> readCB, connectCB, disconnectCB, writeCB;
 std::stringstream outputBuffer;
 std::stringstream inputBuffer;
 SocketStream outputSocketBuffer;
 std::ostream *outp;
 Context<ClientContext> clientData;
};

}
namespace socks {

class ConnectionPoolImpl {
public:
 ConnectionPoolImpl() {};
 virtual ~ConnectionPoolImpl() {};
 virtual void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket) = 0;
};

}
namespace socks {

class ConnectionPool {
public:
 ConnectionPool(ConnectionPoolImpl *impl);
 void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket);
private:
 std::unique_ptr<ConnectionPoolImpl> impl;
};

namespace factory {
ConnectionPool makeMultiplexedConnectionPool(size_t numThreads);
ConnectionPool makeThreadedConnectionPool();
}

}
namespace socks {

class ThreadedConnectionPoolImpl: public ConnectionPoolImpl {
public:
 ThreadedConnectionPoolImpl();
 virtual ~ThreadedConnectionPoolImpl();
 void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket) override;
};

}
namespace socks {

class Poll;
class BufferedClientSocketInterface;

using ClientListType = std::unordered_map<SocketFDType, std::unique_ptr<BufferedClientSocketInterface>>;

class MultiplexerImpl {
public:
 MultiplexerImpl(Poll *pollStrategy);
 virtual ~MultiplexerImpl();
 virtual void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket);
 virtual size_t getClientCount();

 virtual void cancel();
 virtual void interrupt();
 virtual void multiplex(int timeout = -1);
protected:
 std::unique_ptr<Poll> pollStrategy;
 std::unique_ptr<ClientSocket> sockIn;
 std::mutex commandMutex, incomingClientsMutex;
 ClientListType clients;
 std::atomic<size_t> clientCount;
 std::vector<std::unique_ptr<BufferedClientSocketInterface>> incomingClients;
 SocketFDType sockOutFD = InvalidSocketFD;

 virtual void sendMultiplexerCommand(int cmd);
 virtual void removeClientSocket(BufferedClientSocketInterface &clientSocket);
 virtual bool selfPipe(BufferedClientSocketInterface &clientSocket);




 bool readHandler(BufferedClientSocketInterface &client);
 bool writeHandler(BufferedClientSocketInterface &client);
};

}
namespace socks {




class Multiplexer {
public:
 Multiplexer() = delete;
 Multiplexer(Multiplexer &&) = default;
 Multiplexer &operator=(Multiplexer &&) = default;

 Multiplexer(MultiplexerImpl *impl);
 ~Multiplexer();
 void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket);
 void multiplex();
 void cancel();
 void interrupt();
 size_t getClientCount();
protected:
 std::unique_ptr<MultiplexerImpl> impl;
 std::unique_ptr<std::thread> thread;
};

namespace factory {
 Multiplexer makeMultiplexer();
}

}
namespace socks {

class MultiplexedConnectionPoolImpl : public ConnectionPoolImpl {
public:
 MultiplexedConnectionPoolImpl() = delete;
 MultiplexedConnectionPoolImpl(size_t nthreads);
 virtual ~MultiplexedConnectionPoolImpl();

 void addClientSocket(std::unique_ptr<BufferedClientSocketInterface> clientSocket) override;
private:
 std::vector<Multiplexer> multiplexers;

 Multiplexer &getMultiplexer();
};

}
namespace socks {

class ServerImplInterface {
public:
 ServerImplInterface() {};
 virtual ~ServerImplInterface() {};
 virtual void listen(const std::string &bindAddr, const std::string &port) = 0;
 virtual std::string getPort() = 0;
};

template<class ClientContext>
class ServerImpl : public ServerImplInterface {
public:
 ServerImpl(
  ServerSocket *serverSocket,
  ConnectionPool *connectionPool,
  ClientCallback<ClientContext> readCallback,
  ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) :
  ServerImplInterface(),
  serverSocket(serverSocket),
  connectionPool(connectionPool),
  readCB(readCallback),
  connectCB(connectCallback),
  disconnectCB(disconnectCallback),
  writeCB(writeCallback) {};

 virtual ~ServerImpl() {};
 void listen(const std::string &bindAddr, const std::string &port) override {
  serverSocket->listenForConnections(bindAddr, port);

  while (true) {
   std::unique_ptr<BufferedClientSocketInterface> clientSocket(
     new BufferedClientSocket<ClientContext>(
       std::make_unique<ClientSocket>(serverSocket->acceptConnection()),
       readCB,
       connectCB,
       disconnectCB,
       writeCB));

   connectionPool->addClientSocket(std::move(clientSocket));
  }
 };
 std::string getPort() override {
  return serverSocket->getPort();
 };

private:
 std::unique_ptr<ServerSocket> serverSocket;
 std::unique_ptr<ConnectionPool> connectionPool;
 ClientCallback<ClientContext> readCB, connectCB, disconnectCB, writeCB;
};

}
namespace socks {

class Server {
public:
 Server(ServerImplInterface *impl);
 void listen(const std::string &bindAddr, const std::string &port);
 std::string getPort();
private:
 std::unique_ptr<ServerImplInterface> impl;
};

namespace factory {
 template<class ClientContext>
 Server makeMultiplexedServer(
  size_t numThreads,
  ClientCallback<ClientContext> readCallback,
  ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) {
  return Server(new ServerImpl<ClientContext>(
    new ServerSocket(),
    new ConnectionPool(new MultiplexedConnectionPoolImpl(numThreads)),
    readCallback,
    connectCallback,
    disconnectCallback,
    writeCallback));

 };

 template<class ClientContext>
 Server makeThreadedServer(
  ClientCallback<ClientContext> readCallback,
  ClientCallback<ClientContext> connectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> disconnectCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){},
  ClientCallback<ClientContext> writeCallback = [](Context<ClientContext> &ctx, std::istream &inp, std::ostream &outp){}) {
  return Server(new ServerImpl<ClientContext>(
    new ServerSocket(),
    new ConnectionPool(new ThreadedConnectionPoolImpl()),
    readCallback,
    connectCallback,
    disconnectCallback,
    writeCallback));
 };
}

}
namespace socks {


using pollTuple = std::tuple<BufferedClientSocketInterface &, bool, bool>;

class Poll {
public:
 Poll() {};
 virtual ~Poll() {};
 virtual std::vector<pollTuple> pollClients(ClientListType &clients, int timeout = -1) = 0;

};

}
namespace socks {

class SocketFactoryImpl {
public:
 SocketFactoryImpl() {};
 virtual ~SocketFactoryImpl() {};
 virtual SocketImpl *createSocketImpl() = 0;
 virtual SocketImpl *createUDPSocketImpl() = 0;
 virtual Poll *createPoll() = 0;
 virtual SocketAddressImpl *createSocketAddressImpl(
   const std::string &host,
   const std::string &port,
   SocketProtocol protocol = SocketProtocol::UDP) = 0;
 virtual std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair() = 0;
};

}
namespace socks {

class SocketFactory {
public:
 SocketFactory() = delete;
 SocketFactory(SocketFactoryImpl *impl);

 SocketImpl *createSocketImpl();
 SocketImpl *createUDPSocketImpl();
 SocketAddressImpl *createSocketAddressImpl(
   const std::string &host,
   const std::string &port,
   SocketProtocol protocol = SocketProtocol::UDP);
 Poll *createPoll();
 std::pair<std::unique_ptr<ClientSocket>, std::unique_ptr<ClientSocket> > createSocketPair();
private:
 std::unique_ptr<SocketFactoryImpl> impl;
};

extern SocketFactory socketFactory;

}

#endif // #define _LIBSOCKETS_H
