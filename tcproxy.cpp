#define PROGHOST "127.0.0.1"
#define PROGPORT 8023

#include <tcproxy.h>
#include <time.h>

using namespace tcproxy;

std::string curentTime();

int main(int argc, char *argv[])
{
    int msgNum = 0;
    std::string remoteHost = argv[1];
    std::string remotePort = argv[2];

    std::cout << curentTime() << "; " << \
           "Хост для передаресации: " << remoteHost << "; " << \
           "Порт для передаресации: " << remotePort << "; "  << std::endl;

    TCPRoxy roxy;

    int newSocket = roxy.createSocket();
    roxy.bindSocket(&newSocket, PROGHOST, PROGPORT);
    
    while (true)
    {
        sockaddr_in connAddr;
        socklen_t connLen = sizeof(connAddr);

        std::cout << "===============================" << std::endl;
        std::cout << "Ожидание входящего запроса, количество поступивших сообщений: " << \
                            std::to_string(msgNum) << \
                            std::endl;

        int newConnection = accept(newSocket,
                                    (struct sockaddr *) &connAddr,
                                        &connLen);
        msgNum++;

        std::cout << curentTime() << "; " << \
                    "<Новый запрос> " << \
                    "Хост: " << inet_ntoa(connAddr.sin_addr) << "; " << \
                    "Порт: " << connAddr.sin_port << "; " << std::endl;

        int bufLen = 4096;
        char buf[bufLen];

        int recvLen = recv(newConnection, buf, bufLen, 0);

        std::cout << std::endl << "Входящее сообщение:" << std::endl;
        std::string bufData = std::string(buf, 0, recvLen);

        std::cout << bufData << std::endl;

        std::string regex = "User-Agent:";
        std::string httpHeader = bufData.substr(0, bufData.find(regex));
        std::string httpRequest = httpHeader + "Connection: close\r\n\r\n";

        std::string responce = roxy.returnMessage(httpRequest,
                                                    recvLen,
                                                    remoteHost,
                                                    remotePort);

        std::cout << curentTime() << "; <Сообщение перенаправлено>\n\n" << \
                        "Сообщение после переадресации:\n" << \
                        responce << std::endl;

        send(newConnection, responce.c_str(),
                            strlen(responce.c_str()), 0);

        close(newConnection);
    }

    close(newSocket);

    return 0;
}

std::string curentTime()
{
    time_t now;
    time(&now);
    std::string date = asctime(localtime(&now));
    return date.erase(date.size() - 1);
}