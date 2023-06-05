//
// Created by 83787 on 2023/5/5.
//

#ifndef WLI_BASE_CLASS_BRUKERWLI_H
#define WLI_BASE_CLASS_BRUKERWLI_H

#include <vector>
#include "winsock2.h"
#include <string>
#include <iostream>

#define MAX_BUF_SIZE 1024 //将TCP缓冲区的最大大小定义为1024字节
#define bruker55MMD 0x00   //.55倍MMD代号
#define bruker1MMD  0x01   //1倍MMD代号
#define bruker2MMD  0x02   //2倍MMD代号
#define MRe3DSensor 0x00 //MRe3D传感器代号
#define bruker10Turret 0x01 //10倍Turret代号
#define bruker20Turret 0x02 //20倍Turret代号
#define bruker50Turret 0x03 //50倍Turret代号


class brukerWLI {

private: //私有命令
    /******************************目镜更换指令*********************************/
    char changeMMD_55[12] = {0x0A, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; //更换为.55倍
    char changeMMD_1[12] = {0x0A, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};  //更换为1倍
    char changeMMD_2[12] = {0x0A, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};  //更换为2倍

    /******************************物镜更换指令*********************************/
    char changeSensor[12] = {0x0C, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00};//更换为传感器
    char changeTurret_10[12] = {0x0C, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00};//更换为10倍
    char changeTurret_20[12] = {0x0C, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};//更换为20倍
    char changeTurret_50[12] = {0x0C, 0x2B, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00};//更换为50倍

private: //私有属性
    SOCKET sockfd;  // 套接字文件描述符
    struct sockaddr_in servaddr;  // 服务端地址结构

protected:  //保护方法
/**
 * @brief  TCP数据发送（hex数据形式发送）
 * @param  data 发送数据的地址
 * @param  len 发送数据长度
 *
 * @return 数据发送成功返回true，发送失败返回false并且输出报错
 */
    bool tcpSend(const char *data, size_t len);     //TCP数据发送（hex数据形式发送）

/**
 * @brief  TCP数据接收（hex数据形式接收）（Socket阻塞读取）
 * @param  recv_data 接收数据字符串名
 *
 * @return 数据接收成功返回true，接收失败返回false
 */
    bool tcpRecv(std::string &recv_data);         //TCP数据接收（hex数据形式接收）

public:     //公有方法
/**
 * @brief  初始化TCP配置
 *
 * @return null
 */
    brukerWLI(const std::string& brukerWLIIp,int brukerWLIPort);    //构造函数

/**
 * @brief  清除TCP配置
 *
 * @return null
 */
    ~brukerWLI();   //析构函数

/**
 * @brief  Bruker TCP连接
 *
 * @return 连接成功返回true，连接失败返回false并且输出报错
 */
    bool tcpBrukerConnect();    //Bruker WLI TCP连接

/**
 * @brief  Bruker目镜转换
 * @param  MMDNumber 可根据实际情况取：bruker55MMD、bruker1MMD、bruker2MMD
 *
 * @return 转换命令发送成功则返回true，命令发送失败或传入参数错误则返回false
 */
    bool changeMMD(int MMDNumber);    //Bruker目镜切换函数

/**
 * @brief  Bruker物镜转换
 * @param  TurretNumber 可根据实际情况取：changeSensor、bruker10Turret、bruker20Turret、bruker50Turret
 *
 * @return 转换命令发送成功则返回true，命令发送失败或传入参数错误则返回false
 */
    bool changeTurret(int TurretNumber);    //Bruker物镜切换函数
};


#endif //WLI_BASE_CLASS_BRUKERWLI_H
