#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct Command {
    int commandType = 1;
};

Command command;

class Server {
    int serverSock;
    int clientSock;
public:
    Server() : serverSock(-1), clientSock(-1) {}

    ~Server() { StopServer(); }

    void StartServer() {
        serverSock = socket(AF_INET, SOCK_STREAM, 0);

        if (serverSock == -1) 
        {
            std::cerr << "Не удалось создать сокет" << std::endl;
            exit(1);
        }

        struct sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_LOOPBACK;
        serverAddress.sin_port = htons(INADDR_LOOPBACK);

        if (bind(serverSock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Ошибка привязки сокета к адресу и порту" << std::endl;
            exit(1);
        }

        std::cout << "Сервер запущен. Ожидание подключения клиента..." << std::endl;

        if (listen(serverSock, 1) < 0) {
            std::cerr << "Ошибка при прослушивании входящих соединений" << std::endl;
            exit(1);
        }

        sockaddr_in clientAddress{};
        socklen_t clientAddressSize = sizeof(clientAddress);

        clientSock = accept(serverSock, (struct sockaddr *)&clientAddress, &clientAddressSize);
        if (clientSock < 0) {
            std::cerr << "Ошибка при принятии входящего соединения" << std::endl;
            exit(1);
        }

        std::cout << "Клиент успешно подключен" << std::endl;
    }

    void send_command() {
        if (send(clientSock, &command, sizeof(command),0) < 0) {
            std::cerr << "Ошибка при отправке данных клиенту" << std::endl;
            exit(1);
        }
    }

    void recv_command() {
        if (recv(clientSock, &command, sizeof(command),0)< 0) {
            std::cerr << "Ошибка при приеме команды от клиента" << std::endl;
            exit(1);
        }

        std::cout << "Получена команда: " << command.commandType << std::endl;
    }

    void StopServer() {
        shutdown(clientSock, 2);
        shutdown(serverSock, 2);
        
        if (clientSock != -1) {
            close(clientSock);
            clientSock = -1;
        }

        if (serverSock != -1) {
            close(serverSock);
            serverSock = -1;
        }
    }
};

int main() {
    Server server;

    server.StartServer();

    server.send_command();
    server.recv_command();

    server.StopServer();

    return 0;
}