/*************************************************************************
	> File Name: MainWindow.c
	> Author: fly
	> Mail:fly@163.com 
	> Created Time: 2018年03月11日 星期日 22时31分40秒
 ************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QCheckBox>
#include "wjy_class.h"
#include <QDebug>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class wjy_class;
public:
    wjy_class *my_class = new wjy_class;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
   void sendMassage();
   void readMassage();
   void displayError(QAbstractSocket::SocketError);
   void connectUpdata();
   void disconnectUpdata();
   void on_sendButton_clicked();
   void on_clearButton_clicked();
   void on_connnectButton_clicked();
   void on_disconnectButton_clicked();  
   void on_speedSlider_valueChanged(int value);
   void on_timeSlider_valueChanged(int value);
   QByteArray QString2Hex(QString str);
   int ConvertHexChar(char ch);

   void on_motor1_clicked();
   void on_motor2_clicked();
   void on_motor3_clicked();
   void on_pushButton_clicked();

   void on_motor4_clicked();

   void on_motor5_clicked();

   void on_motor6_clicked();

   void on_motor7_clicked();

   void on_motor8_clicked();

   void on_motor9_clicked();

   void on_motor10_clicked();

   void on_motor11_clicked();

   void on_motor12_clicked();

   void on_motor13_clicked();

   void on_motor14_clicked();

   void on_motor15_clicked();

   void on_pushButton_3_clicked();

   void on_pushButton_4_clicked();

   void on_pushButton_2_clicked();

private:
   //QTcpServer *tcpServer;//不用再建立服务器类了，直接建立下面的套接字
   QTcpSocket *tcpSocket;//直接建立TCP套接字类
   QString tcpIp;//存储IP地址
   QString tcpPort;//存储端口地址
   bool flag;
   Ui::MainWindow *ui;

};


#endif // MAINWINDOW_H
