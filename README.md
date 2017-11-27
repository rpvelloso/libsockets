# libsockets (Work In Progress!!!)
A platform independent C++ socket library with transparent SSL support (and some more stuff).
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
  auto clientSocket = serverSocket.acceptConnection();
  
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

## socks::Server (socks::makeThreaded[SSL]Server & socks::makeMultiplexed[SSL]Server)
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
  inp.clear();
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

## socks::DatagramSocket
```cpp
#include "libsockets.h"

socks::DatagramSocket datagramSocket;

datagramSocket.bindSocket("0.0.0.0", "10000");
size_t bufferSize = 4096;
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
```
You can also turn a socks::DatagramSocket into a 'connected' datagram socket and use socks::ClientSocket's interface.
```cpp
  socks::ClientSocket = datagramSocket.makeClientSocket(peer); 
  /* 
   * from this point on 'datagramSocket' can not be used anymore,
   * its implementation has been moved to 'clientSocket'.
   */
```
or
```cpp
  socks::ClientSocket = datagramSocket.makeClientSocket(host, port); // same here
```

## socks::SocketStream
It's a socks::ClientSocket wrapped in an STL stream class.
```cpp
#include "libsockets.h"

socks::SocketStream socketStream;

if (socketStream.connecTo("127.0.0.1", "10000") == 0) {
  std::string buf;

  socketStream >> buf; // receiving data
  std::cout << buf << std::endl;
  socketStream << "message received." << std::endl; // transmitting data
} else {
  std::cerr << "error connecting." << std::endl;
}
```

## SSL Support
Just replace the declarations:
```cpp
socks::ClientSocket clientSocket = socks::factory::makeSSLClientSocket;
socks::ServerSocket serverSocket = socks::factory::makeSSLServerSocket;
socks::SocketStream serverSocket = socks::factory::makeSSLSocketStream;
socks::Server server = socks::makeThreadedSSLServer<...>(...);
```
That's it. For the socks::ServerSocket and socks::Server you'll also need certificate and key files (default names are 'cert.pem' and 'key.pem').
To generate test certificate and key files: 
```sh
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365 -nodes
```

## socks::factory
This namespace contains all library's factory methods.
# FTP Server (ftpd/)
A more elaborate example: a fully functional FTP server implementation using libsockets (including SSL support, passive mode, resume & FXP support). 
An authentication callback is provided for the user.
# HTTP Server (httd/)
Coming soon...
(under development/refactoring, currently not working...)
