# libsockets (Work In Progress!!!)
A platform independent C++ socket library with transparent SSL support (and some more stuff).

Get it here: [[Windows x64 static & shared libraries with SSL support]](https://github.com/rpvelloso/libsockets/raw/master/libsockets-mingw64-ssl.zip) or [[no SSL support]](https://github.com/rpvelloso/libsockets/raw/master/libsockets-mingw64-nossl.zip)


# Examples
## socks::ClientSocket & socks::ServerSocket
```cpp
#include "libsockets.h"

void client() {
  socks::ClientSocket clientSocket;

  if (clientSocket.connectTo("127.0.0.1", "10000") == 0) {
    std::string hello = "Hello World";
    clientSocket.sendData(hello.c_str(), hello.size());
  }
};

int main() {
  client();
};
```

```cpp
#include "libsockets.h"

void server() {
  socks::ServerSocket serverSocket;
  
  serverSocket.listenForConnections("127.0.0.1", "10000");
  socks::ClientSocket clientSocket = serverSocket.acceptConnection();
  
  char buf[512];
  auto len = clientSocket.receiveData(buf, 512);
  if (len > 0) {
    buf[len]=0;
    std::cout << buf << std::endl;
  }
};

int main() {
  server();
};
```

```sh
$> ./server &
$> ./client
```

## socks::Server (`socks::makeThreaded[SSL]Server` & `socks::makeMultiplexed[SSL]Server`)
```cpp
#include "libsockets.h"

class MyContext {
public:
  int value; // whatever context you need to maintain
};

void onReceive(socks::Context<MyContext> &context, std::istream &inp, std::ostream &outp) {
  std::string word;

  while (inp >> word)
    outp << context.getContext().value++ << " " << word << std::endl;
};

void onConnect(socks::Context<MyContext> &context, std::istream &inp, std::ostream &outp) {
  context.getContext().value = 0;
  outp << "Hello" << std::endl;
};

int main() {
  auto myServer = socks::factory::makeThreadedServer<MyContext>(
    onReceive,
    onConnect /*,
    onDisconnect,
    afterWrite
    */
    ); // onReceive is mandatory, onConnect, onDisconnect & afterWrite are optional
    
  myServer.listen("127.0.0.1", "10000"); // serves
};
```
You can replace `socks::factory::makeThreadedServer` with `socks::factory::makeMultiplexedServer`. The first instantiates a server that creates one thread per client (using blocking I/O), the second instantiates a server with a more scalable architecture, using the reactor pattern (several clients are served per thread using non-blocking I/O).

## socks::DatagramSocket
```cpp
#include "libsockets.h"

constexpr size_t bufferSize = 4096;

int main() {
  socks::DatagramSocket datagramSocket;

  datagramSocket.bindSocket("0.0.0.0", "10000");
  char buffer[bufferSize];

  std::pair<int, socks::SocketAddress> ret = datagramSocket.receiveFrom(buffer, bufferSize);
  // ret = pair<bytes received, sender address>
  if (ret.first > 0) {
    auto peer = std::move(ret.second);
    std::string reply = "received " + std::to_string(ret.first) + " bytes";
    datagramSocket.sendTo(peer, reply.c_str(), reply.size());
  } else {
    std::cerr << "error receiving." << std::endl;
  }
}
```
You can also turn a `socks::DatagramSocket` into a 'connected' datagram socket and use `socks::ClientSocket`'s interface.
```cpp
socks::ClientSocket clientSocket = datagramSocket.makeClientSocket(peer); 
/* 
 * from this point on 'datagramSocket' cannot be used anymore,
 * its implementation has been moved to 'clientSocket'.
 */
```
or
```cpp
socks::ClientSocket clientSocket = datagramSocket.makeClientSocket(host, port); // same here
```

## socks::SocketStream
It's a `socks::ClientSocket` wrapped in an `std::iostream` class.
```cpp
#include "libsockets.h"

int main() {
  socks::SocketStream socketStream;

  if (socketStream.connectTo("127.0.0.1", "10000") == 0) {
    std::string buf;

    std::getline(socketStream, buf); // receiving data
    std::cout << buf << std::endl;
    socketStream << "message received." << std::endl; // transmitting data
  } else {
    std::cerr << "error connecting." << std::endl;
  }
}
```

## SSL Support
Just replace the declarations:
```cpp
socks::ClientSocket clientSocket = socks::factory::makeSSLClientSocket;
socks::ServerSocket serverSocket = socks::factory::makeSSLServerSocket;
socks::SocketStream socketStream = socks::factory::makeSSLSocketStream;
socks::Server server = socks::factory::makeThreadedSSLServer<...>(...);
socks::Server server = socks::factory::makeMultiplexedSSLServer<...>(...);
```
That's it. For the `socks::ServerSocket` and `socks::Server` you'll also need certificate and key files (default names are 'cert.pem' and 'key.pem').
To generate test certificate and key files: 
```sh
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
```

## socks::factory
This namespace contains all library's factory methods.
# FTP Server (ftpd/)
A more elaborate example: a simple, yet fully functional, FTP server implementation using libsockets (including SSL support, passive mode, resume & FXP support). 
An authentication callback is provided for the user. You can also register custom SITE commands.

You can try it: [Windows x64 static binary](https://github.com/rpvelloso/libsockets/raw/master/ftpd-mingw64-static.zip)

```cpp
AuthenticationFunction FTPClientInfo::authenticate =
[](const std::string &username, const std::string &password, FTPClientInfo& clientInfo) {
  /*
   * in here a user profile can be loaded into 'clientInfo'
   * upon authentication in order to define, for example,
   * a home dir, chroot, etc.
   */
  return authService.authenticate(username, password);
};

int main(int argc, char **argv) {
  FTPServer ftpServer;

  // SITE CLIENT COUNT
  ftpServer.registerSiteCommand(
    "CLIENT", 
    [&ftpServer](const std::string &params, FTPClientInfo &clientInfo) {
      std::stringstream ss(params);
      std::string p1;
      ss >> p1;
      std::transform(p1.begin(), p1.end(), p1.begin(), ::toupper);
      if (p1 == "COUNT")
        return "200 There is/are " + std::to_string(ftpServer.getClientCount()) + " client(s) online.";
      else
        return std::string("501 Invalid SITE CLIENT parameter.");
  });

  ftpServer.start();
}
```
# HTTP Server (httd/)
Coming soon...
(under development/refactoring, currently only working as a library...)
