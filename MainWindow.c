/*************************************************************************
	> File Name: MainWindow.c
	> Author: fly
	> Mail:fly@163.com 
	> Created Time: 2018年03月11日 星期日 23时35分43秒
 ************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->speedSlider->setMinimum(0);
    ui->speedSlider->setMaximum(200);
    ui->speedSlider->setPageStep(50);
    ui->speedSlider->setTickPosition(QSlider::TicksAbove);
    ui->timeSlider->setMinimum(0);
    ui->timeSlider->setMaximum(200);
    ui->timeSlider->setPageStep(50);
    ui->timeSlider->setTickPosition(QSlider::TicksAbove);
    ui->speedLineEdit->setText(tr("0"));
    ui->timeLineEdit->setText(tr("0"));
    ui->timeLineEdit->setEnabled(false);
    ui->speedLineEdit->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);

    ui->sendButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
    ui->IPLineEdit->setText("192.168.3.4");
    ui->portLineEdit->setText("4001");
    tcpSocket = NULL;//使用前先清空

    my_class->g_data[0] = 170;
    my_class->g_data[1] = 85;
    for(int i=2;i<7;++i) my_class->g_data[i] = 0;
    my_class->g_flag = 1;


}

MainWindow::~MainWindow()
{
    delete tcpSocket;
    delete my_class;
    delete ui;

}

void MainWindow::sendMassage(){}

void MainWindow::readMassage()
{
    QByteArray data=tcpSocket->readAll();
    QString p = data.toHex();
    ui->clearLineEdit->setText(p);
    //------把接受到的Qbyte转化成Qstring然后转化成int--------//
    //qDebug()<<p.toInt(0,16);
    qDebug()<<p;
    int temp;
    if(my_class->g_data[3]>8) temp = my_class->g_data[3]-8;
    else temp = my_class->g_data[3];
    temp = temp*16 + my_class->g_roll;
    if(p.toInt(0,16) == temp)
    {
        ui->pushButton_2->setEnabled(false);
        ui->pushButton->setEnabled(true);
        ui->speedSlider->setEnabled(true);
        ui->timeSlider->setEnabled(true);
        my_class->g_flag = 1;
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError)
{
    QMessageBox::warning (this, tr("Warnning"), tcpSocket->errorString ());
    tcpSocket->close ();
    ui->connnectButton->setEnabled (true);
    ui->disconnectButton->setEnabled (false);
    ui->sendButton->setEnabled (false);
}

void MainWindow::on_sendButton_clicked()
{
    QString sendmessage;
    sendmessage = ui->sendLineEdit->text();
   /* if(sendmessage == NULL) return;
    QByteArray block;//暂时存储我们需要发送的数据
    QDataStream out(&block,QIODevice::WriteOnly);//TCP必须和数据流一起使用
    out.setVersion(QDataStream::Qt_5_7);//设置数据流的版本（服务器和主机版本一定相同）
    out << sendmessage;
    tcpSocket->write(block);*/

    QByteArray data;
    data.append(sendmessage);
    tcpSocket->write(QString2Hex(sendmessage));
}

//将字符型进制转化为16进制
QByteArray MainWindow::QString2Hex(QString str)
    {
        QByteArray senddata;
        int hexdata,lowhexdata;
        int hexdatalen = 0;
        int len = str.length();
        senddata.resize(len/2);
        char lstr,hstr;
        for(int i=0; i<len; )
        {
            hstr=str[i].toLatin1();   //字符型
            if(hstr == ' ')
            {
                i++;
                continue;
            }
            i++;
            if(i >= len)
                break;
            lstr = str[i].toLatin1();
            hexdata = ConvertHexChar(hstr);
            lowhexdata = ConvertHexChar(lstr);
            if((hexdata == 16) || (lowhexdata == 16))
                break;
            else
                hexdata = hexdata*16+lowhexdata;
            i++;
            senddata[hexdatalen] = (char)hexdata;
            hexdatalen++;
        }
        senddata.resize(hexdatalen);
        return senddata;
    }
 //将1-9 a-f字符转化为对应的整数
int MainWindow::ConvertHexChar(char ch)
    {
        if((ch >= '0') && (ch <= '9'))
         return ch-'0';
        else if((ch >= 'A') && (ch <= 'F'))
         return ch-'A'+10;
        else if((ch >= 'a') && (ch <= 'f'))
         return ch-'a'+10;
        else return (-1);
    }

void MainWindow::on_clearButton_clicked()
{
    ui->clearLineEdit->clear();
}

void MainWindow::on_connnectButton_clicked()
{
    flag = false;
    if(tcpSocket) delete tcpSocket;//如果有指向其他空间直接删除
    tcpSocket = new QTcpSocket(this);//申请堆空间有TCP发送和接受操作
    tcpIp = ui->IPLineEdit->text();
    tcpPort = ui->portLineEdit->text();
    if(tcpIp==NULL||tcpPort==NULL)//判断IP和PORT是否为空
    {
        QMessageBox msgBox;
        msgBox.setText("IP or PORT is Empty");
        msgBox.exec();
        return;
    }
    tcpSocket->connectToHost(tcpIp,tcpPort.toInt());//连接主机
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,
            SLOT(displayError(QAbstractSocket::SocketError)));//错误连接
    connect(tcpSocket,SIGNAL(connected()),this,
            SLOT(connectUpdata()));//更新连接之后按钮的使能
    connect(tcpSocket,SIGNAL(readyRead()),this,
            SLOT(readMassage()));//读取信息的连接
    ui->connnectButton->setEnabled (false);
    ui->disconnectButton->setEnabled (true);

}

void MainWindow::on_disconnectButton_clicked()
{
    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->speedSlider->setEnabled(true);
    ui->timeSlider->setEnabled(true);
    my_class->g_flag = 1;

    tcpSocket->abort();
    delete tcpSocket;
    tcpSocket=NULL;
    disconnectUpdata();   
}

void MainWindow::connectUpdata()
{
    if(!flag)
    {
        QMessageBox msgBox;
        msgBox.setText("TCP connect successful");
        msgBox.exec();

        ui->pushButton->setEnabled(true);//发送使能

        ui->connnectButton->setEnabled(false);
        ui->sendButton->setEnabled(true);
        ui->disconnectButton->setEnabled(true);
        ui->IPLineEdit->setEnabled(false);
        ui->portLineEdit->setEnabled(false);
    }
    flag=true;
}

void MainWindow::disconnectUpdata()
{
    ui->connnectButton->setEnabled(true);
    ui->sendButton->setEnabled(false);
    ui->disconnectButton->setEnabled(false);
    ui->IPLineEdit->setEnabled(true);
    ui->portLineEdit->setEnabled(true);
    ui->pushButton->setEnabled(false);//发送使能
    ui->pushButton_2->setEnabled(false);
}

void MainWindow::on_speedSlider_valueChanged(int value)
{
    value = value;
    int speedValue = ui->speedSlider->value();
    if(speedValue>0&&speedValue<=25)
    {
        ui->speedSlider->setValue(0);
        my_class->g_data[4] = 1;
    }
    else if(speedValue>25&&speedValue<=75)
    {
        ui->speedSlider->setValue(50);
        my_class->g_data[4] = 3;
    }
    else if(speedValue>75&&speedValue<=125)
    {
        ui->speedSlider->setValue(100);
        my_class->g_data[4] = 5;
    }
    else if(speedValue>125&&speedValue<=175)
    {
        ui->speedSlider->setValue(150);
        my_class->g_data[4] = 7;
    }
    else if(speedValue>175)
    {
        ui->speedSlider->setValue(200);
        my_class->g_data[4] = 9;
    }
    speedValue = ui->speedSlider->value();
    QString str = QString("%1").arg(speedValue);//int型转化成QString显示
    ui->speedLineEdit->setText(str);
   // my_class->g_speed = speedValue;
}

void MainWindow::on_timeSlider_valueChanged(int value)
{
    value = value;
    int timeValue = ui->timeSlider->value();
    if(timeValue>0&&timeValue<=25)
    {
        ui->timeSlider->setValue(0);
        my_class->g_data[5] = 9;
    }
    else if(timeValue>25&&timeValue<=75)
    {
        ui->timeSlider->setValue(50);
        my_class->g_data[5] = 7;
    }
    else if(timeValue>75&&timeValue<=125)
    {
        ui->timeSlider->setValue(100);
        my_class->g_data[5] = 5;
    }
    else if(timeValue>125&&timeValue<=175)
    {
        ui->timeSlider->setValue(150);
        my_class->g_data[5] = 3;
    }
    else if(timeValue>175)
    {
        ui->timeSlider->setValue(200);
        my_class->g_data[5] = 1;
    }
    timeValue = ui->timeSlider->value();
    QString str = QString("%1").arg(timeValue);//int型转化成QString显示
    ui->timeLineEdit->setText(str);
    //my_class->g_time = timeValue;
}

void MainWindow::on_pushButton_clicked()
{

    ui->pushButton->setEnabled(false);//发送使能
    ui->pushButton_2->setEnabled(true);
    //板子编号取值
    bool flag =true;
    if(my_class->g_data[3]>8)
    {
        my_class->g_data[2] =2;
        my_class->g_data[3]-=8;
        flag = true;
    }
    else
    {
        flag = false;
        my_class->g_data[2] = 1;
    }//电机号和正反转取值
    my_class->g_data[3] =my_class->g_data[3]*16 + my_class->g_roll;
    //校验位取值
    int num;
    num = my_class->g_data[3]+my_class->g_data[4]+my_class->g_data[5];
    my_class->g_data[6] = num % 256;
    //------------十进制转化成QString然后再转化成长十六进制发送------//
    qDebug()<<"number:";
    QString a1;
    for(int j=0;j<7;j++)
    {
        if(my_class->g_data[j]<16)
        a1+='0'+QString::number(my_class->g_data[j], 16).toUpper();
        else a1+=QString::number(my_class->g_data[j], 16).toUpper();
    }
    qDebug()<<a1;
    //QByteArray data;
    //data.append(a1);
    //tcpSocket->write(data);
    tcpSocket->write(QString2Hex(a1));
    my_class->g_data[3] = (my_class->g_data[3]-my_class->g_roll)/16;
   if(flag) my_class->g_data[3]+=8;


    ui->speedSlider->setEnabled(false);
    ui->timeSlider->setEnabled(false);
    my_class->g_flag = 0;
    switch (my_class->g_data[3]) {
    case 1:if(my_class->g_roll==1) ui->display1->setChecked(true);
           else ui->display1->setChecked(false);
        break;
    case 2:if(my_class->g_roll==1) ui->display2->setChecked(true);
           else ui->display2->setChecked(false);
        break;
    case 3:if(my_class->g_roll==1) ui->display3->setChecked(true);
           else ui->display3->setChecked(false);
        break;
    case 4:if(my_class->g_roll==1) ui->display4->setChecked(true);
           else ui->display4->setChecked(false);
        break;
    case 5:if(my_class->g_roll==1) ui->display5->setChecked(true);
           else ui->display5->setChecked(false);
        break;
    case 6:if(my_class->g_roll==1) ui->display6->setChecked(true);
           else ui->display6->setChecked(false);
        break;
    case 7:if(my_class->g_roll==1) ui->display7->setChecked(true);
           else ui->display7->setChecked(false);
        break;
    case 8:if(my_class->g_roll==1) ui->display8->setChecked(true);
           else ui->display8->setChecked(false);
        break;
    case 9:if(my_class->g_roll==1) ui->display9->setChecked(true);
           else ui->display9->setChecked(false);
        break;
    case 10:if(my_class->g_roll==1) ui->display10->setChecked(true);
           else ui->display10->setChecked(false);
        break;
    case 11:if(my_class->g_roll==1) ui->display11->setChecked(true);
           else ui->display11->setChecked(false);
        break;
    case 12:if(my_class->g_roll==1) ui->display12->setChecked(true);
           else ui->display12->setChecked(false);
        break;
    case 13:if(my_class->g_roll==1) ui->display13->setChecked(true);
           else ui->display13->setChecked(false);
        break;
    case 14:if(my_class->g_roll==1) ui->display14->setChecked(true);
           else ui->display14->setChecked(false);
        break;
    case 15:if(my_class->g_roll==1) ui->display15->setChecked(true);
           else ui->display15->setChecked(false);
        break;
    default:
        break;
    }
}


void MainWindow::on_motor1_clicked()
{
if(my_class->g_flag==1){
    //my_class->g_motor = 1;
    my_class->g_data[3] =1;
    ui->motor1->setStyleSheet("color: red");
    ui->motor2->setStyleSheet("color: black");//按键字颜色改变
    ui->motor3->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");
}
}

void MainWindow::on_motor2_clicked()
{
if(my_class->g_flag==1){
    //my_class->g_motor = 2;
    my_class->g_data[3] =2;
    ui->motor2->setStyleSheet("color: red");
    ui->motor1->setStyleSheet("color: black");
    ui->motor3->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");
}
}

void MainWindow::on_motor3_clicked()
{
    if(my_class->g_flag==1){
    //my_class->g_motor = 3;
    my_class->g_data[3] =3;
    ui->motor3->setStyleSheet("color: red");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor4_clicked()
{
    if(my_class->g_flag==1){
    //my_class->g_motor = 4;
    my_class->g_data[3] =4;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: red");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor5_clicked()
{
    if(my_class->g_flag==1){
    //my_class->g_motor = 5;
    my_class->g_data[3] =5;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: red");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor6_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 6;
    my_class->g_data[3] =6;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: red");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor7_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 7;
    my_class->g_data[3] =7;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: red");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor8_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 8;
    my_class->g_data[3] =8;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: red");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor9_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 9;
    my_class->g_data[3] =9;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: red");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor10_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 10;
    my_class->g_data[3] =10;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: red");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor11_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 11;
    my_class->g_data[3] =11;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: red");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor12_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 12;
    my_class->g_data[3] =12;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: red");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor13_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 13;
    my_class->g_data[3] =13;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: red");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor14_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 14;
    my_class->g_data[3] =14;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: red");
    ui->motor15->setStyleSheet("color: black");}
}

void MainWindow::on_motor15_clicked()
{if(my_class->g_flag==1){
    //my_class->g_motor = 15;
    my_class->g_data[3] =15;
    ui->motor3->setStyleSheet("color: black");
    ui->motor1->setStyleSheet("color: black");
    ui->motor2->setStyleSheet("color: black");
    ui->motor4->setStyleSheet("color: black");
    ui->motor5->setStyleSheet("color: black");
    ui->motor6->setStyleSheet("color: black");
    ui->motor7->setStyleSheet("color: black");
    ui->motor8->setStyleSheet("color: black");
    ui->motor9->setStyleSheet("color: black");
    ui->motor10->setStyleSheet("color: black");
    ui->motor11->setStyleSheet("color: black");
    ui->motor12->setStyleSheet("color: black");
    ui->motor13->setStyleSheet("color: black");
    ui->motor14->setStyleSheet("color: black");
    ui->motor15->setStyleSheet("color: red");}
}

void MainWindow::on_pushButton_3_clicked()
{
    my_class->g_roll = 1;//正转
    if(my_class->g_flag==1){
    ui->pushButton_3->setStyleSheet("color: red");
    ui->pushButton_4->setStyleSheet("color: black");}
}

void MainWindow::on_pushButton_4_clicked()
{
    my_class->g_roll = 2;//反转
    if(my_class->g_flag==1){
    ui->pushButton_3->setStyleSheet("color: black");
    ui->pushButton_4->setStyleSheet("color: red");}
}

void MainWindow::on_pushButton_2_clicked()
{
    //板子编号取值
    bool flag = true;
    if(my_class->g_data[3]>8)
    {
        flag =true;
        my_class->g_data[2] =2;
        my_class->g_data[3]-=8;
    }
    else
    {
        flag = false;
        my_class->g_data[2] = 1;
    }//电机号和正反转取值
    my_class->g_data[3] =my_class->g_data[3]*16 + my_class->g_roll;
    //校验位取值
    int num1;
    num1 = my_class->g_data[3]+my_class->g_data[4]+my_class->g_data[5];
    my_class->g_data[6] = num1 % 256;
    my_class->g_data[3] = (my_class->g_data[3]-my_class->g_roll)/16;
    if(flag)    my_class->g_data[3]+=8;
    int temp3 = my_class->g_data[3];
    int temp6 = my_class->g_data[6];

    my_class->g_data[3] =255;
    //校验位取值
    int num;
    num = my_class->g_data[3]+my_class->g_data[4]+my_class->g_data[5];
    my_class->g_data[6] = num % 256;
    //------------十进制转化成QString然后再转化成长十六进制发送------//
    qDebug()<<"number:";
    QString a1;
    for(int j=0;j<7;j++)
    {
        if(my_class->g_data[j]<16)
        a1+='0'+QString::number(my_class->g_data[j], 16).toUpper();
        else a1+=QString::number(my_class->g_data[j], 16).toUpper();
    }
    qDebug()<<a1;
    tcpSocket->write(QString2Hex(a1));


    ui->pushButton_2->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->speedSlider->setEnabled(true);
    ui->timeSlider->setEnabled(true);

    my_class->g_data[3] = temp3;
    my_class->g_data[6] = temp6;
    my_class->g_flag = 1;
}
