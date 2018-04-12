/*************************************************************************
	> File Name: MainWindow.c
	> Author: fly
	> Mail:fly@163.com 
	> Created Time: 2018年03月11日 星期日 23时50分40秒
 ************************************************************************/
#ifndef WJY_CLASS_H
#define WJY_CLASS_H
//#include <vector>

using namespace std;
class wjy_class
{
public:
    wjy_class();
    //vector<int> g_motor;//预留多路电机同时控制
    /*int g_motor;
    int g_speed;
    int g_time;*/
    int g_roll;
    long int g_data[10];
    bool g_flag;
};

#endif // WJY_CLASS_H
