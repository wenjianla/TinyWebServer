#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#include "./threadpool/threadpool.h"
#include "./http/http_conn.h"

const int MAX_FD = 65536;           //最大文件描述符
const int MAX_EVENT_NUMBER = 10000; //最大事件数
const int TIMESLOT = 5;             //最小超时单位

class WebServer
{
public:
    WebServer();
    ~WebServer();

    void init(int port , string user, string passWord, string databaseName,
              int log_write , int opt_linger, int trigmode, int sql_num,
              int thread_num, int close_log, int actor_model);

    void thread_pool();
    void sql_pool();
    void log_write();
    void trig_mode();
    void eventListen();
    void eventLoop();
    void timer(int connfd, struct sockaddr_in client_address);
    void adjust_timer(util_timer *timer);
    void deal_timer(util_timer *timer, int sockfd);
    bool dealclientdata();
    bool dealwithsignal(bool& timeout, bool& stop_server);
    void dealwithread(int sockfd);
    void dealwithwrite(int sockfd);

public:
    //基础
    int m_port;/// 端口号
    char *m_root;/// root 文件路径
    int m_log_write;/// 日志写入方式
    int m_close_log;/// 是否关闭日志
    int m_actormodel;/// 并发模型选择

    int m_pipefd[2];/// 管道文件描述符
    int m_epollfd;/// epoll 文件描述符
    http_conn *users;/// http_conn 对象数组

    //数据库相关
    connection_pool *m_connPool;/// 数据库连接池
    string m_user;         //登陆数据库用户名
    string m_passWord;     //登陆数据库密码
    string m_databaseName; //使用数据库名
    int m_sql_num;/// 数据库数量

    //线程池相关
    threadpool<http_conn> *m_pool;/// 线程池
    int m_thread_num;/// 线程数量

    //epoll_event相关
    epoll_event events[MAX_EVENT_NUMBER];/// epoll_event 数组

    int m_listenfd;/// 监听文件描述符
    int m_OPT_LINGER;/// 优雅关闭链接
    int m_TRIGMode;/// 触发模式选择
    int m_LISTENTrigmode;/// 监听触发模式
    int m_CONNTrigmode;/// 连接触发模式

    //定时器相关
    client_data *users_timer;/// 定时器数组
    Utils utils;
};
#endif
 