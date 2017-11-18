#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>

namespace net {
enum DOMAIN_TYPE {
  NET_DOMAIN_UDS = AF_UNIX,
  NET_DOMAIN_INET = AF_INET,
  NET_DOMAIN_INET6 = AF_INET6
};

enum SOCK_TYPE {
  NET_SOCK_TCP = SOCK_STREAM,
  NET_SOCK_UDP = SOCK_DGRAM
};

class Client {
  public:
    explicit Client(int domain, int type)
      : domain_(domain),
        type_(type) {
      socket_ = socket(domain_, type_, 0);
    }

    ~Client() {
      close(socket_);
    }


    void Connect() {
      struct sockaddr* server_addr;
      unsigned int len;

      switch(domain_) {
        case AF_INET: 
          struct linger linger_;
          linger_.l_onoff = 1;
          linger_.l_linger = 0;

          setsockopt(socket_, SOL_SOCKET, SO_LINGER, &linger_, sizeof(struct linger));
          memset(&server_addr_.server_addr_in_, 0x00, sizeof(struct sockaddr_in));
          server_addr_.server_addr_in_.sin_family = AF_INET;
          server_addr_.server_addr_in_.sin_addr.s_addr = inet_addr("127.0.0.1");
          server_addr_.server_addr_in_.sin_port = htons((uint16_t)8080);
          server_addr = (struct sockaddr*)&server_addr_.server_addr_in_;
          len = sizeof(server_addr_.server_addr_in_);
        break;
        case AF_UNIX:
          server_addr = (struct sockaddr*)&server_addr_.server_addr_un_;
          len = sizeof(server_addr_.server_addr_un_);
        break;
        default:
          std::cerr << "Unknown Type" << std::endl;
        return;
      }

      std::cout << "Client:Connect" << std::endl;
      
      if(connect(socket_,(struct sockaddr*)server_addr, len) < 0) {
          std::cerr << "Unknown Type" << std::endl;
          return;
      }
    }

    void Send(const void *data) {
    }

    void Recv(const void *data) {
    }

  private:
    int socket_;
    int domain_;
    int type_;

    union {
      struct sockaddr_in server_addr_in_;
      struct sockaddr_un server_addr_un_;
    } server_addr_;
};
}

int main() {
  net::Client client_(AF_INET, SOCK_STREAM);
  client_.Connect();
  return 0;
}
