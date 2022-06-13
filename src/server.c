#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024
#define DEBUG 1

/*
server
初始化需要执行的操作：
int socket ()		建立一个Socket
int bind()			与某个端口绑定
int listen()		开始监听端口
int accept()		等待/接受客户端的连接请求

gcc server.c -o server.exe -lwsock32
*/

// 初始化
int socketInit() {
    // 初始化 socket 库
    WSADATA wsd;
    WORD socketVersion = MAKEWORD(2, 2);
    if (WSAStartup(socketVersion, &wsd) != 0) {
        printf("socket 库初始化失败");
        exit(0);
    }
    return 0;
}

int welcome() {
    printf("%s\n", "  _____  __       ___________           .__   ");
    printf("%s\n", "_/ ____\\/  |______\\__    ___/___   ____ |  |  ");
    printf("%s\n", "\\   __\\\\   __\\____ \\|    | /  _ \\ /  _ \\|  |  ");
    printf("%s\n", " |  |   |  | |  |_> >    |(  <_> |  <_> )  |__");
    printf("%s\n", " |__|   |__| |   __/|____| \\____/ \\____/|____/");
    printf("%s\n", "             |__|                             ");

    printf("%s\n", "启动成功");
}

// 结束清理
int socketClean() {
    WSACleanup();
    return 0;
}

// debug 日志
int debugPrintf(char* msg) {
    if (DEBUG) {
        printf("debug: %s\n", msg);
    }
    return 0;
}

// 获取一行文字
int getStrLine(int clnt_sock, char* msg) {
    int num = recv(clnt_sock, msg, BUFFER_SIZE, 0);
    if (num <= 0) {
        printf("%s\n", "[getStrLine] 没有接收到 client 消息");
        return -1;
    }

    // 打印显示
    msg[num] = '\0';
    debugPrintf("接收 client 信息:");
    debugPrintf(msg);
    return 0;
}

// 发送一行文字
int sendStrLine(int clnt_sock, char* msg, int msg_size) {
    int num = send(clnt_sock, msg, msg_size, 0);
    if (num <= 0) {
        printf("%s\n", "[sendStrLine] 没有成功发送 server 消息");
        return -1;
    }

    // 打印显示
    msg[num] = '\0';
    debugPrintf("发送信息:");
    debugPrintf(msg);
    return 0;
}

// 获取并保存文件
int getAndSaveFile(char* filename) {
    return 0;
}

// 发送文件
int sendFile(char* filename) {
    return 0;
}

int main() {
    socketInit();

    // 建立一个 socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1) {
        printf("socket 建立失败");
        return 0;
    }

    // 端口绑定
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;            //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);                    //端口
    int bindRet =
        bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (bindRet == -1) {
        printf("socket 端口绑定失败");
        return 0;
    }

    // 监听端口
    int listenRet = listen(serv_sock, 20);
    if (listenRet == -1) {
        printf("socket 端口监听失败");
        return 0;
    }

    // 进行服务
    while (1) {
        printf("> 等待连接到来...\n");

        int ok = 0;                    // 判断操作是否成功
        char cmd[255] = {0};           // 命令
        char param[255] = {0};         // 参数
        struct sockaddr_in clnt_addr;  // 客户端 ip + 端口信息
        int clnt_addr_size = sizeof(clnt_addr);

        // 一次就能服务一个连接 -- 单线程
        int clnt_sock =
            accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            printf("服务 accept 失败");
            continue;
        }

        // 接收客户端发送的命令
        if (getStrLine(clnt_sock, cmd) == -1) {
            printf("获取 cmd 失败");
            continue;
        }
        char msg[255] = "[cmd] 成功接收数据";
        if (sendStrLine(clnt_sock, msg, sizeof(msg)) == -1) {
            printf("[cmd] 发送消息确认应答失败");
            continue;
        }

        // 进入不同的服务
        if (strcmp(cmd, "getFile") == 0) {
            debugPrintf("使用命令 getFile [filename]");
        }

        if (strcmp(cmd, "sendFile") == 0) {
            debugPrintf("使用命令 sendFile [filename]");
        }

        if (strcmp(cmd, "sendMsg") == 0) {
            // 接收消息
            debugPrintf("使用命令 sendMsg [message]");
            if (getStrLine(clnt_sock, param) == -1) {
                printf("%s\n", "[getFile] 获取 param 失败");
                continue;
            }

            // 展示消息
            printf("接收 client 的 msg: %s\n", param);

            // 表示接收成功
            char msg[255] = "sendMsg: 数据成功接收";
            if (sendStrLine(clnt_sock, msg, sizeof(msg)) == -1) {
                printf("%s\n", "[getFile] 发送消息确认应答失败");
                continue;
            }
        }

        if (strcmp(cmd, "ls") == 0) {
            debugPrintf("使用命令 ls");
        }

        // 关闭连接
        closesocket(clnt_sock);
    }

    closesocket(serv_sock);
    socketClean();
    return 0;
}