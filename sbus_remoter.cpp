# include "include/serial.h"  //ROS已经内置了的串口包
# include <math.h>
# include "string.h"
# include <iostream>
# include <map>
// ===========================================
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <ifaddrs.h>
#define MAX_LENGTH 254

int port = 8000;
std::string hostIP = "127.0.0.1";

struct sockaddr_in addrCli;
struct sockaddr_in addrSer;
int sockCli;
socklen_t addrlen;

char sendbuf[MAX_LENGTH];

// ===========================================
# define step_angle 0.05

# define DEG2RAD 3.1415926/180.0
# define RAD2DEG 180.0/3.1415926

serial::Serial ser; //声明串口对象

/* serial */
std::string param_port_path_ = "/dev/ttyUSB0";
int param_baudrate_ = 9600;
int param_loop_rate_ = 20;
serial::parity_t param_patity_;

// ===========================================
int i = 0;
int j = 0;

double center_angle = 0.0;
double center_angle_uplim = 90.0;
double center_angle_dwlim = 0.0;
double center_angle_step = 1.0;

double single_angle = 0.0;
double single_angle_uplim = 90.0;
double single_angle_dwlim = 0.0;
double single_angle_step = 1.0;


double double_angle = 0.0;
double double_angle_uplim = 90.0;
double double_angle_dwlim = 0.0;
double double_angle_step = 1.0;


void UDP_send(char *ch)
{
    sendto(sockCli, ch, strlen(ch)+1, 0, (struct sockaddr*)&addrSer, addrlen);
    ch[strlen(ch)] == '\0';
    printf("%s\n",ch);
}


int main (int argc, char** argv) 
{ 

    uint8_t rec[2000] = {'\0'}; 

    //发布主题 

    sockCli = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockCli == -1)
    {
        perror("socket");
        return 0;
    }

    addrSer.sin_family = AF_INET;
    addrSer.sin_port = htons(port);//????
    addrSer.sin_addr.s_addr = inet_addr(hostIP.c_str());//????????? 0?127.0.0.1?? 1?????ip
 
    addrlen = sizeof(struct sockaddr);

    printf("udp init ok,IP:%s,port %d\n", hostIP.c_str(), port);
    try 
    { 
    //设置串口属性，并打开串口 
        ser.setPort(param_port_path_); 
        ser.setBaudrate(param_baudrate_); 
        serial::Timeout to = serial::Timeout::simpleTimeout(1000); 
        ser.setTimeout(to); 
        ser.open(); 
    } 
    catch (serial::IOException& e) 
    { 
        printf("Unable to open port\n"); 
        return -1; 
    } 

    //检测串口是否已经打开，并给出提示信息 
    if(ser.isOpen()) 
    { 
        std::cout<<"Serial Port"<<param_port_path_<<" initialized"<< std::endl; 
    } 
    else 
    { 
        return -1; 
    } 

    uint8_t enable = 0;
    
    while(1) 
    { 
        // while(ser.available() < 10);
        if(ser.available()){
            
            ser.read(rec,1);

            if (rec[0] & (1<<7) == 1){
                sprintf(sendbuf,"EnMotor(0,-1)\n");
                UDP_send(sendbuf);
            }
            if (rec[0] & (1<<6) == 1){
                sprintf(sendbuf,"DisMotor(0,-1)\n");
                UDP_send(sendbuf);
            }

            if (rec[0] & (1<<5) == 1){
                enable = 1;
                center_angle += center_angle_step;
                if (center_angle > center_angle_uplim)
                    center_angle = center_angle_uplim;
            }

            if (rec[0] & (1<<4) == 1){
                enable = 1;
                center_angle += -center_angle_step;
                if (center_angle < center_angle_dwlim)
                    center_angle = center_angle_dwlim;
            }
            
            if (rec[0] & (1<<3) == 1){
                enable = 1;
                single_angle += single_angle_step;
                if (single_angle > single_angle_uplim)
                    single_angle = single_angle_uplim;
            }

            if (rec[0] & (1<<2) == 1){
                enable = 1;
                single_angle += -single_angle_step;
                if (single_angle < single_angle_dwlim)
                    single_angle = single_angle_dwlim;
            }
            
            if (rec[0] & (1<<1) == 1){
                enable = 1;
                double_angle += double_angle_step;
                if (double_angle > double_angle_uplim)
                    double_angle = double_angle_uplim;
            }

            if (rec[0] & (1<<0) == 1){
                enable = 1;
                double_angle += -double_angle_step;
                if (double_angle < double_angle_dwlim)
                    double_angle = double_angle_dwlim;
            }


            if (enable == 1)
            {
                enable = 0;
                sprintf(sendbuf,"servoJ(0,%.3f,%.3f,%.3f,%.3f,0,0,0)\n", center_angle * DEG2RAD, double_angle * DEG2RAD, single_angle * DEG2RAD, double_angle * DEG2RAD);
                UDP_send(sendbuf);
            }
            
        }

    } 
    ser.close();
    shutdown(sockCli, SHUT_RDWR);

    return 0;
}
