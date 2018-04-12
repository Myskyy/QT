/*************************************************************************
	> File Name: 1.cpp
	> Author: fly
	> Mail:fly@163.com 
	> Created Time: 2018年03月12日 星期一 21时12分16秒
 ************************************************************************/
#include "mainwindow.h"
#include <QApplication>
#include "logindialog.h"
#include "wjy_class.cpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    LoginDialog login;
    if(login.exec() ==QDialog::Accepted)
    {
        w.show();
        return a.exec();
    }
    else return 0;

}
