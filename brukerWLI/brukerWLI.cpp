//
// Created by 83787 on 2023/5/5.
//

#include "brukerWLI.h"


brukerWLI::brukerWLI() {
    // 初始化 Socket 库
    WSADATA wsaData; //定义一个 WSADATA 结构体变量 wsaData，用于存储 WSAStartup 函数的返回信息
    int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0) {
        std::cerr << "WSAStartup failed: " << ret << std::endl; //初始化Socket库失败，输出报错
        return;
    }

    // 创建套接字
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == INVALID_SOCKET) {
        std::cerr << "Failed to create socket: " << WSAGetLastError() << std::endl;//创建套接字失败，输出报错
        return;
    }

    // 设置服务端地址结构体
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;  //选择地址族为IPv4
    servaddr.sin_addr.s_addr = inet_addr(brukerWLIIp);  //配置连接的目标ip
    servaddr.sin_port = htons(brukerWLIPort);   //配置连接的目标端口号
}

brukerWLI::~brukerWLI() {
    closesocket(sockfd);  // 关闭套接字tcp
    WSACleanup();  // 清理 Socket 库
}

bool brukerWLI::tcpBrukerConnect() {
    // 连接服务端
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to connect to server: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

bool brukerWLI::tcpSend(const char *data, size_t len) {
    // 发送数据
    if (send(sockfd, data, len, 0) == SOCKET_ERROR) {
        std::cerr << "Failed to send data: " << WSAGetLastError() << std::endl;
        return false;
    }
    return true;
}

bool brukerWLI::tcpRecv(std::string &recv_data) {
    char buffer[MAX_BUF_SIZE];  //定义缓冲区最大长度
    int ret = recv(sockfd, buffer, MAX_BUF_SIZE, 0);
    if (ret <= 0) {
        return false;
    }
    //数据转换为Hex格式
    recv_data = "";
    for (int i = 0; i < ret; i++) {
        char hex_str[3];
        sprintf(hex_str, "%02X", static_cast<unsigned char>(buffer[i]));
        recv_data += hex_str;
    }

    return true;
}

bool brukerWLI::changeMMD(int MMDNumber) {
    switch (MMDNumber) {
        case bruker55MMD:
            return tcpSend(changeMMD_55, sizeof(changeMMD_55));
        case bruker1MMD:
            return tcpSend(changeMMD_1, sizeof(changeMMD_1));
        case bruker2MMD:
            return tcpSend(changeMMD_2, sizeof(changeMMD_2));
        default:
            std::cerr << "Incorrect incoming parameter!" << std::endl;
            return false;
    }
}

bool brukerWLI::changeTurret(int TurretNumber) {
    switch (TurretNumber) {
        case MRe3DSensor:
            return tcpSend(changeSensor, sizeof(changeSensor));
        case bruker10Turret:
            return tcpSend(changeTurret_10, sizeof(changeTurret_10));
        case bruker20Turret:
            return tcpSend(changeTurret_20, sizeof(changeTurret_20));
        case bruker50Turret:
            return tcpSend(changeTurret_50, sizeof(changeTurret_50));
        default:
            std::cerr << "Incorrect incoming parameter!" << std::endl;
            return false;
    }
}