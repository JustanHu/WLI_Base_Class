//
// Created by 83787 on 2023/5/5.
//

#ifndef WLI_BASIC_BRUKERWLI_H
#define WLI_BASIC_BRUKERWLI_H
#include <vector>
#include "winsock2.h"
#include <string>
#include <iostream>

#define MAX_BUF_SIZE 1024 //将TCP缓冲区的最大大小定义为1024字节
#define brukerWLIIp "127.0.0.1" //bruker WLI ip地址
#define brukerWLIPort 1025 //bruker WLI 端口号
#define bruker55MMD 0x00   //.55倍MMD代号
#define bruker1MMD  0x01   //1倍MMD代号
#define bruker2MMD  0x02   //2倍MMD代号


class brukerWLI {

private: //私有命令
    /******************************目镜更换指令*********************************/
    char changeMMD_55[12] = {0x0A,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; //更换为.55倍
    char changeMMD_1[12] = {0x0A,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00};  //更换为1倍
    char changeMMD_2[12] = {0x0A,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x02,0x00,0x00,0x00};  //更换为2倍

    /******************************物镜更换指令*********************************/
    char changeSensor[12] = {0x0C,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x00,0x00};//更换为传感器
    char changeTurret_10[12] = {0x0C,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x01,0x00,0x00,0x00};//更换为10倍
    char changeTurret_20[12] = {0x0C,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x02,0x00,0x00,0x00};//更换为20倍
    char changeTurret_50[12] = {0x0C,0x2B,0x00,0x00,0x04,0x00,0x00,0x00,0x03,0x00,0x00,0x00};//更换为50倍

private: //私有属性
    SOCKET sockfd;  // 套接字文件描述符
    struct sockaddr_in servaddr;  // 服务端地址结构

protected:  //保护方法
    bool tcpSend(const char* data, size_t len);     //TCP数据发送（hex数据形式发送）
    bool tcpRecv(std::string& recv_data);         //TCP数据接收（hex数据形式接收）

public:     //公有方法
    brukerWLI();    //构造函数
    ~brukerWLI();   //析构函数
    bool tcpBrukerConnect();    //Bruker WLI TCP连接
    bool changeMMD(int MMDNumber);    //Bruker目镜切换函数

};


#endif //WLI_BASIC_BRUKERWLI_H
