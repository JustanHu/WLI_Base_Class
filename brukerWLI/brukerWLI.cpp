//
// Created by 83787 on 2023/5/5.
//

#include "brukerWLI.h"


brukerWLI::brukerWLI(const std::string& brukerWLIIp,int brukerWLIPort) {
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
    servaddr.sin_addr.s_addr = inet_addr(brukerWLIIp.c_str());  //配置连接的目标ip
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
    switch (MMDNumber) {    //根据所选MMD镜头倍数发送对应的命令
        case bruker55MMD:
             tcpSend(changeMMD_55, sizeof(changeMMD_55));
             break;
        case bruker1MMD:
             tcpSend(changeMMD_1, sizeof(changeMMD_1));
             break;
        case bruker2MMD:
             tcpSend(changeMMD_2, sizeof(changeMMD_2));
             break;
        default:
            std::cerr << "Incorrect incoming parameter!" << std::endl;
            return false;
    }
    std::string recv_data;
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C30000040000000A2B0000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Replacement of MMD multiplier failed!" << std::endl;//打印错误信息“更换MMD倍数失败！”
        return false;
    }

}

bool brukerWLI::changeTurret(int TurretNumber) {
    switch (TurretNumber) {
        case MRe3DSensor:
             tcpSend(changeSensor, sizeof(changeSensor));
             break;
        case bruker10Turret:
             tcpSend(changeTurret_10, sizeof(changeTurret_10));
             break;
        case bruker20Turret:
             tcpSend(changeTurret_20, sizeof(changeTurret_20));
             break;
        case bruker50Turret:
             tcpSend(changeTurret_50, sizeof(changeTurret_50));
             break;
        default:
            std::cerr << "Incorrect incoming parameter!" << std::endl;
            return false;
    }
    std::string recv_data;
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C30000040000000C2B0000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Replacement of Turret multiplier failed!" << std::endl;//打印错误信息“更换Turret倍数失败！”
        return false;
    }
}

int brukerWLI::getMMDPosition(){
    tcpSend(getMMDPos,sizeof(getMMDPos));//发送读取位置命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data.length() == 24 && recv_data.substr(0,6) == "68C300"){//命令接收成功并且验证命令正确性
        switch(recv_data[17]){
            case '0':
                return bruker55MMD;
            case '1':
                return bruker1MMD;
            case '2':
                return bruker2MMD;
            default:
                return 0xff;
        }
    }
    else{
        std::cerr << "Description Failed to read the MMD position!" << std::endl;//打印错误信息“读取MMD倍数失败！”
        return 0xff;
    }
}

int brukerWLI::getTurretPosition() {
    tcpSend(getTurretPos,sizeof(getTurretPos));//发送读取位置命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data.length() == 24 && recv_data.substr(0,6) == "69C300"){//命令接收成功并且验证命令正确性
        switch(recv_data[17]){
            case '0':
                return MRe3DSensor;
            case '1':
                return bruker10Turret;
            case '2':
                return bruker20Turret;
            case '3':
                return bruker50Turret;
            default:
                return 0xff;
        }
    }
    else{
        std::cerr << "Description Failed to read the Turret position!" << std::endl;//打印错误信息“读取Turret倍数失败！”
        return 0xff;
    }
}

bool brukerWLI::initTurret(){
    tcpSend(initTurretCmd,sizeof(initTurretCmd));//发送初始化Turret命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C30000040000000E2B0000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Failed to initialize the Turret!" << std::endl;//打印错误信息“初始化Turret失败！”
        return false;
    }
}
