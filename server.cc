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

class Server {
  public:
    explicit Server(int domain, int type)
      : domain_(domain),
        type_(type) {
      socket_ = socket(domain_, type_, 0);
    }

    ~Server() {
      close(socket_);
    }

    void Start() {
      switch(domain_) {
        case AF_INET: 
          std::cout << "Inet Type" << std::endl;
          memset(&server_addr_.server_addr_in_, 0x0, sizeof(struct sockaddr_in));
          server_addr_.server_addr_in_.sin_family = AF_INET;
          server_addr_.server_addr_in_.sin_addr.s_addr = htonl(INADDR_ANY);
          server_addr_.server_addr_in_.sin_port = htons((uint16_t)8080);

          if(bind(socket_, (struct sockaddr*)&server_addr_.server_addr_in_,
                sizeof(server_addr_.server_addr_in_)) < 0) {
            std::cerr<<"Server : Cannot bind local address.\n" << std::endl;
          }
          break;
        case AF_UNIX:
          std::cout << "Unix Type" << std::endl;
          break;
        default:
          std::cerr << "Unknown Type" << std::endl;
          return;
       }
       
       if(listen(socket_, 5) < 0) {
         std::cerr << "Server: listen error" << std::endl;
         return;
       }
    }


    void Run() {
      int client_fd;
      struct sockaddr* client_addr;
      unsigned int len;
      char client_ip[20];

      switch(domain_) {
        case AF_INET: 
          client_addr = (struct sockaddr*)&client_addr_.client_addr_in_;
          len = sizeof(client_addr_.client_addr_in_);
        break;
        case AF_UNIX:
          client_addr = (struct sockaddr*)&client_addr_.client_addr_un_;
          len = sizeof(client_addr_.client_addr_un_);
        break;
        default:
          std::cerr << "Unknown Type" << std::endl;
        return;
      }

      std::cout << "Start Running" << std::endl;
      
      while(1) {
        client_fd = accept(socket_,(struct sockaddr*)client_addr, &len);
        if(client_fd < 0) {
          std::cerr<<"Server: accept failed.\n"<<std::endl;
          break;
        }

        if(AF_INET == domain_) {
          inet_ntop(AF_INET, &client_addr_.client_addr_in_.sin_addr.s_addr,
                    client_ip, sizeof(client_ip));
          std::cout <<"Server: "<<client_ip<< " client connected" << std::endl;
        } else if(AF_UNIX == type_) {
        } else
          break;  
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

    union {
      struct sockaddr_in client_addr_in_;
      struct sockaddr_un client_addr_un_;
    } client_addr_;

};
}

int main() {
  net::Server server(AF_INET, SOCK_STREAM);
  server.Start();
  server.Run();
  return 0;
}
