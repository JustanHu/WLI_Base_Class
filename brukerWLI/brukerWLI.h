//
// Created by 83787 on 2023/5/5.
//

#ifndef WLI_BASE_CLASS_BRUKERWLI_H
#define WLI_BASE_CLASS_BRUKERWLI_H

#include <vector>
#include "winsock2.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <bitset>

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

    /******************************读取目镜位置指令*********************************/
    char getMMDPos[8] = {0x0B, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************读取物镜位置指令*********************************/
    char getTurretPos[8] = {0x0D, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************初始化物镜位置指令*********************************/
    char initTurretCmd[8] = {0x0E, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************自动聚焦指令*********************************/
    char autofocusCmd[8] = {0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************单次测量指令*********************************/
    char singleAcquisitionCmd[8] = {0x2F, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************按当前配方测量指令*********************************/
    char doMeasurementCmd[8] = {0x30, 0x23, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************获取当前扫描管位置指令*********************************/
    char getZPosCmd[8] = {static_cast<char>(0xA0), 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************获取当前强度值指令*********************************/
    char getIntensityCmd[8] = {0x01, 0x2B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************设置为仅允许仪器控制模式*********************************/
    char setInstrumentControlOnlyCmd[8] = {0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************获取当前锁定模式状态*********************************/
    char getLockoutModeCmd[8] = {0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************设置正常模式*********************************/
    char setNormalModeCmd[8] = {0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /******************************设置为完全锁定模式*********************************/
    char setLockoutModeCmd[8] = {0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

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

/**
 * @brief  16进制数据转换为浮点数（适配 bruker从最低位到最高位的排序）
 * @param  存储16进制的内存空间
 *
 * @return 转换后的浮点数值
 */
    float hexToFloat(const std::string& hex);

/**
 * @brief  浮点数转换为 16进制数（适配 bruker从最低位到最高位的排序）
 * @param  value: 传入需要计算的浮点数
 * @param  hexArray: 存储 16进制的内存空间
 */
    void floatToHex(float value, char* hexArray);

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

/**
 * @brief  Bruker读取目镜位置
 *
 *
 * @return 返回值为当前MMD镜头位置：
 * bruker55MMD 0x00
 * bruker1MMD 0x01
 * bruker2MMD 0x02
 * 操作失败 0xff
 */
    int getMMDPosition(); //Bruker读取目镜位置

/**
 * @brief  Bruker读取物镜位置
 *
 *
 * @return 返回值为当前Turret镜头位置：
 * MRe3DSensor 0x00
 * bruker10Turret 0x01
 * bruker20Turret 0x02
 * bruker20Turret 0x03
 * 操作失败 0xff
 */
    int getTurretPosition(); //Bruker读取目镜位置

/**
 * @brief  Bruker物镜初始化
 * @return true:执行成功，false：执行失败
 */
    bool initTurret();    //Bruker物镜初始化

/**
 * @brief  Bruker自动聚焦
 * @return true:执行成功，false：执行失败
 */
    bool doAutofocus();    //Bruker自动聚焦

/**
 * @brief  Bruker单次测量
 * @return true:执行成功，false：执行失败
 */
    bool doSingleAcquisition();    //Bruker单次测量

/**
 * @brief  Bruker按当前配方测量
 * @return true:执行成功，false：执行失败
 */
    bool doMeasurement();    //Bruker当前配方测量

/**
 * @brief  获取当前Z扫描管位置
 * @return 扫描管当前所处位置值 (单位：nm)
 *          0xffffffff:执行命令失败
 */
    float getZScannerPos();    //获取当前Z扫描管位置

/**
 * @brief  获取当前白光强度值
 * @return 当前白光强度值（%）
 *          0xffffffff:执行命令失败
 */
    float getIntensity();    //获取当前白光强度值

/**
 * @brief  设置当前白光强度值
 * @param  需要设置的强度值（%）
 * @return true:执行成功，false：执行失败
 */
    bool setIntensity(float value); //设置当前白光强度值

/**
 * @brief  移动扫描管位置（绝对运动）
 * @param  需要移动的绝对位置（nm）
 * @return true:执行成功，false：执行失败
 */
    bool setZScannerPos(float value); //移动扫描管位置

/**
 * @brief  设置为仅允许仪器控制模式
 * @return true:执行成功，false：执行失败
 */
    bool setInstrumentControlOnlyMode();

/**
 * @brief  获取当前锁定模式
 * @return
 * 0x00: 正常模式
 * 0x01: 仅仪表控制模式
 * 0x02: 完全锁定模式
 * 0Xff: 获取失败
 */
    int getLockoutMode();

/**
  * @brief  设置当前模式为正常模式
  * @return true:执行成功，false：执行失败
  */
    bool setNormalMode();

/**
  * @brief  设置当前模式为全锁定模式
  * @return true:执行成功，false：执行失败
  */
    bool setLockoutMode();
};








#endif //WLI_BASE_CLASS_BRUKERWLI_H
