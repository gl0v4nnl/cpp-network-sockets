#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

const int SERVER_PORT = 8080;
const int MAX_BUFFER_SIZE = 1024;

struct Command {
    int commandType;
};

class Client {
    int clientSock;
public:
    Client() : clientSock(-1) {}

    ~Client() { StopClient(); }

    void StartClient() {
        clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);

        if (clientSock == -1) {
            std::cerr << "Не удалось создать сокет" << std::endl;
            exit(1);
        }

        struct sockaddr_in serverAddress{};
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(SERVER_PORT);

        if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
            std::cerr << "Неверный IP адрес" << std::endl;
            exit(1);
        }

        if (connect(clientSock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
            std::cerr << "Ошибка соединения с сервером" << std::endl;
            exit(1);
        }

        std::cout << "Соединение с сервером успешно установлено" << std::endl;
    }

    void send_command() {
        Command command;
        command.commandType = 0;

        while (true) {
            if (send(clientSock, &command, sizeof(command), 0) < 0) {
                std::cerr << "Ошибка при отправке команды серверу" << std::endl;
                break;
            }
        }
    }

    void recv_command() {
        Command command;
        command.commandType = 0;

        while (true) {
            if (recv(clientSock, &command, sizeof(command), 0) < 0) 
            {
                std::cerr << "Ошибка при приеме данных от сервера" << std::endl;
                break;
            }
        }
    }

    void StopClient() {
        shutdown(clientSock, 2);

        if (clientSock != -1) {
            close(clientSock);
            clientSock = -1;
        }
    }
};

int main() {
    Client client;

    client.StartClient();

    client.send_command();
    client.recv_command();

    client.StopClient();

    return 0;
}