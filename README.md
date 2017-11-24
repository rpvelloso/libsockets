# libsockets (work in progress!!!)
A platform independent socket library with transparent SSL support (and some more stuff).
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

## SSL Support
Just replace the declarations:
```cpp
socks::ClientSocket clientSocket = socks::factory::makeSSLClientSocket;
socks::ServerSocket serverSocket = socks::factory::makeSSLServerSocket;
```
That's it.
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
## socks::factory
This namespace contains all library's factory methods.
# FTP Server (ftpd/)
A more elaborate example: a fully functional FTP server implementation using libsockets (including SSL support, passive mode, resume & FXP support). 
An authentication callback is provided for the user.
# HTTP Server (httd/)
Coming soon...
(under development/refactoring, currently not working...)
