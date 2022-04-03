// tcproxy.h
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

namespace tcproxy
{
    class TCPRoxy
    {
    public:

        int createSocket()
        {
            // Создаём сокет с TCP опцией
            int newSocket = socket(AF_INET, SOCK_STREAM, 0);
            return newSocket;
        }

        void bindSocket(int *newSocket, std::string progHost, int progPort)
        {
            // Создаем структуру для формирования параметров соединения
            struct sockaddr_in connAddr;

            connAddr.sin_family = AF_INET;
            connAddr.sin_port = htons(progPort);

            // Преобразование адреса ipv4/v6 из текстового в сетевой формат
            // и сохраняем в структуре connAddr
            inet_pton(AF_INET, progHost.c_str(), &connAddr.sin_addr);

            bind(*newSocket,
                    (struct sockaddr *) &connAddr,
                        sizeof(connAddr));

            listen(*newSocket, SOMAXCONN);
        }

        int sendSocket(std::string progHost, int progPort)
        {
            struct sockaddr_in connAddr;

            connAddr.sin_family = AF_INET;
            connAddr.sin_port = htons(progPort);

            inet_pton(AF_INET, progHost.c_str(), &connAddr.sin_addr);

            int newSocket = socket(AF_INET, SOCK_STREAM, 0);

            connect(newSocket,
                    (const sockaddr *) &connAddr,
                        sizeof(connAddr));

            return newSocket;
        }

        std::string returnMessage(std::string message, int messageLen, std::string remoteHost, std::string remotePort)
        {
            int newSocket = TCPRoxy::sendSocket(remoteHost.c_str(), std::stoi(remotePort));

            // Размер буфера для хранения символов сообщения
            int bufLen = 15000;
            char buf[bufLen];

            send(newSocket, message.c_str(), messageLen+1, 0);
            recv(newSocket, buf, bufLen, 0);
            close(newSocket);

            return buf;
        }
    };

}