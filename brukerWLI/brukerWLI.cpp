//
// Created by 83787 on 2023/5/5.
//

#include "brukerWLI.h"


brukerWLI::brukerWLI(const std::string& brukerWLIIp, int brukerWLIPort) {
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

float brukerWLI::hexToFloat(const std::string& hex){
    union FloatConverter { //联合体定义，用于16进制和浮点数的转换
        float f;
        unsigned int i;
    };
    // 反转十六进制字符串
    std::string reversedHex = hex.substr(6,2) + hex.substr(4,2) + hex.substr(2,2) + hex.substr(0,2);
    unsigned int num;
    std::stringstream ss;
    ss << std::hex << reversedHex;
    ss >> num;

    FloatConverter converter;
    converter.i = num;

    return converter.f;
}

void brukerWLI::floatToHex(float value, char* hexArray){
    union HexConverter { //联合体定义，用于16进制和浮点数的转换
        float fValue;
        unsigned char buffer[4];
    };
    HexConverter charTemp; // 利用联合体进行转换
    charTemp.fValue = value;
    memcpy(hexArray, charTemp.buffer, sizeof(HexConverter));
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

bool brukerWLI::doAutofocus() {
    tcpSend(autofocusCmd,sizeof(autofocusCmd));//发送自动聚焦命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "56C3000000000000")//判断是否接收到第一次返回命令并且命令值正确
    {
        std::string recv_temp; //建立第二次接收缓冲区
        bool recvTempStatus = tcpRecv(recv_temp); //接收WLI返回的命令
        if(recvTempStatus && recv_temp == "57C3000000000000")//判断是否接收到第二次返回命令并且命令值正确
        return true;
        else
        {
            std::cerr << "Failed to execute Autofocus. Procedure!" << std::endl;//打印错误信息“执行Autofocus失败！”
            return false;
        }
    }
    else
    {
        std::cerr << "Failed to execute Autofocus. Procedure!" << std::endl;//打印错误信息“执行Autofocus失败！”
        return false;
    }
}

bool brukerWLI::doSingleAcquisition(){
    tcpSend(singleAcquisitionCmd,sizeof(singleAcquisitionCmd));//发送单次测量命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "58C3000000000000")//判断是否接收到第一次返回命令并且命令值正确
    {
        std::string recv_temp; //建立第二次接收缓冲区
        bool recvTempStatus = tcpRecv(recv_temp); //接收WLI返回的命令
        if(recvTempStatus && recv_temp == "59C3000000000000")//判断是否接收到第二次返回命令并且命令值正确
            return true;
        else
        {
            std::cerr << "Failed to perform a single acquisition!" << std::endl;//打印错误信息“执行单次测量失败！”
            return false;
        }
    }
    else
    {
        std::cerr << "Failed to perform a single acquisition!" << std::endl;//打印错误信息“执行单次测量失败！”
        return false;
    }
}

bool brukerWLI::doMeasurement(){
    tcpSend(doMeasurementCmd,sizeof(doMeasurementCmd));//发送按配方测量测量命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C300000400000030230000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Failed to do Measurement!" << std::endl;//打印错误信息“测量失败！”
        return false;
    }
}

float brukerWLI::getZScannerPos(){
    tcpSend(getZPosCmd,sizeof(getZPosCmd));//发送获取Z扫描管位置命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus){
        std::string recv_hex = recv_data.substr(16,8); //建立接收缓冲区
        return hexToFloat(recv_hex);//计算当前的位置值
    }
    else{
        std::cerr << "Failed to do Measurement!" << std::endl;//打印错误信息“测量失败！”
        return 0xffffffff;
    }
}

float brukerWLI::getIntensity(){
    tcpSend(getIntensityCmd,sizeof(getIntensityCmd));//发送获取强度值位置命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus){
        std::string recv_hex = recv_data.substr(16,8); //建立接收缓冲区
        return hexToFloat(recv_hex);//计算当前的位置值
    }
    else{
        std::cerr << "Failed to get Intensity!" << std::endl;//打印错误信息“测量失败！”
        return 0xffffffff;
    }
}

bool brukerWLI::setIntensity(float value){
    char hexArray[4];
    floatToHex(value,hexArray); //将浮点数转换为16进制值
    char setIntensityCmd[12] = {0x02,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,hexArray[0],hexArray[1],hexArray[2],hexArray[3]};
    tcpSend(setIntensityCmd,sizeof(setIntensityCmd));//发送设置强度值指令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C3000004000000022B0000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Failed to set Intensity!" << std::endl;//打印错误信息“设置强度值失败！”
        return false;
    }
}

bool brukerWLI::setZScannerPos(float value){
    char hexArray[4];
    floatToHex(value,hexArray); //将浮点数转换为16进制值
    char setZScannerPos[13] = {static_cast<char>(0x93),0x13,0x00,0x00,0x05,0x00,0x00,0x00,0x01,hexArray[0],hexArray[1],hexArray[2],hexArray[3]};
    tcpSend(setZScannerPos,sizeof(setZScannerPos));//发送设置位置指令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "54C3000000000000")//判断是否接收到第一次返回命令并且命令值正确
    {
        std::string recv_temp; //建立第二次接收缓冲区
        bool recvTempStatus = tcpRecv(recv_temp); //接收WLI返回的命令
        if(recvTempStatus && recv_temp == "55C3000000000000")//判断是否接收到第二次返回命令并且命令值正确
            return true;
        else
        {
            std::cerr << "Failed to set Z Scanner Position!" << std::endl;//打印错误信息“设置扫描管位置失败！”
            return false;
        }
    }
    else
    {
        std::cerr << "Failed to set Z Scanner Position!" << std::endl;//打印错误信息“设置扫描管位置失败！”
        return false;
    }

}

bool brukerWLI::setInstrumentControlOnlyMode(){
    tcpSend(setInstrumentControlOnlyCmd,sizeof(setInstrumentControlOnlyCmd));//发送命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C300000400000016000000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Failed to set Instrument Control Only Mode !" << std::endl;//打印错误信息“设置仅仪表控制模式失败！”
        return false;
    }
}

int brukerWLI::getLockoutMode(){
    tcpSend(getLockoutModeCmd,sizeof(getLockoutModeCmd));//发送命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data.length() == 24 && recv_data.substr(0,6) == "6AC300"){//命令接收成功并且验证命令正确性
        switch(recv_data[17]){
            case '0':
                return 0x00;
            case '1':
                return 0x01;
            case '2':
                return 0x02;
            default:
                return 0xff;
        }
    }
    else{
        std::cerr << "Failed to get Lockout Mode!" << std::endl;//打印错误信息“获取锁定模式失败！”
        return 0xff;
    }
}

bool brukerWLI::setNormalMode(){
    tcpSend(setNormalModeCmd,sizeof(setNormalModeCmd));//发送命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C30000040000000B000000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Failed to set Normal Mode !" << std::endl;//打印错误信息“设置正常模式失败！”
        return false;
    }
}

bool brukerWLI::setLockoutMode(){
    tcpSend(setLockoutModeCmd,sizeof(setLockoutModeCmd));//发送命令
    std::string recv_data; //建立接收缓冲区
    bool recvStatus = tcpRecv(recv_data); //接收WLI返回的命令
    if(recvStatus && recv_data == "50C30000040000000A000000")//判断是否接收到返回命令并且命令值正确
        return true;
    else
    {
        std::cerr << "Failed to set Lockout Mode !" << std::endl;//打印错误信息“设置全锁定模式失败！”
        return false;
    }
}