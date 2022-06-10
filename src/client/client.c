#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

/*
client
初始化需要执行的操作：
int socket ()		建立一个Socket
int connect() 		连接到服务器

编译
gcc client.c -o client.exe -lwsock32
*/

// 全局参数
int debug = 1;

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

// debug 日志
int debugPrintf(char* msg) {
    if (debug) {
        printf("%s\n", msg);
    }
    return 0;
}

// 业务函数
int sendMsg(char* msg) {
    return 0;
}

int sendFile(char* path) {
    return 0;
}

int getFile(char* path) {
    return 0;
}

int getFileList() {
    return 0;
}

int help() {
    printf("用法：");
    printf("1. getFile [filename]");
    printf("2. sendFile [filename]");
    printf("3. sendMsg [message]");
    printf("4. ls");
    printf("5. quit");
    printf("6. help");
    return 0;
}

int main() {
    socketInit();
    while (1) {
        char cmd[255];
        char param[255];
        scanf("%s", cmd);
        if (strcmp(cmd, "getFile") == 0) {
            debugPrintf("使用命令 getFile [filename]");
            scanf("%s", param);
            debugPrintf("getFile 参数为");
            debugPrintf(param);
            int ret = getFile(param);
            if(ret == -1) {
                printf("getFile 失败");
            }
        } else if (strcmp(cmd, "sendFile" == 0)) {
            debugPrintf("使用命令 sendFile [filename]");
            scanf("%s", param);
            debugPrintf("sendFile 参数为");
            debugPrintf(param);
            int ret = sendFile(param);
            if(ret == -1) {
                printf("sendFile 失败");
            }
        }else if (strcmp(cmd, "sendMsg" == 0)) {
            debugPrintf("使用命令 sendMsg [message]");
            scanf("%s", param); // 消息有空格怎么办
            debugPrintf("sendFile 参数为");
            debugPrintf(param);
            int ret = sendFile(param);
            if(ret == -1) {
                printf("sendMsg 失败");
            }
        }else if (strcmp(cmd, "ls" == 0)) {
            debugPrintf("使用命令 ls");
            int ret = getFileList();
            if(ret == -1) {
                printf("ls 失败");
            }
        }else if (strcmp(cmd, "quit" == 0)) {
            debugPrintf("使用命令 quit");
            break;
        }else if (strcmp(cmd, "help" == 0)) {
            debugPrintf("使用命令 help");
            help();
        }
    }

    // 建立一个 socket
    int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock == -1) {
        printf("socket 建立失败");
        return 0;
    }

    // while (1)
    // {
    //     /* code */
    // }

    // tcp 连接
    struct sockaddr_in clnt_addr;
    int clnt_addr_size = sizeof(clnt_addr);
    memset(&clnt_addr, 0, sizeof(clnt_addr));  //每个字节都用0填充
    clnt_addr.sin_family = AF_INET;            //使用IPv4地址
    clnt_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    clnt_addr.sin_port = htons(1234);                    //端口
    int connRet =
        connect(clnt_sock, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
    if (connRet == -1) {
        printf("socket 端口连接失败");
        return 0;
    }

    // 保存 server 发送的数据到文件中
    // int fileP = open("test.txt", 0777);
    // char buffer[40];
    // read(clnt_sock, buffer, sizeof(buffer) - 1);
    // write(fileP, buffer, sizeof(buffer));
    // close(fileP);

    // 向 server 发送信息
    char sendData[255] = "i am client";
    send(clnt_sock, sendData, sizeof(sendData), 0);

    // 接收 server 发送的信息
    char recvData[255];
    int recvNum = recv(clnt_sock, recvData, clnt_addr_size, 0);
    if (recvNum > 0) {
        recvData[recvNum] = '\0';
        printf("get msg from server:\n%s\n", recvData);
    } else {
        printf("没有接收到 server 消息");
    }

    closesocket(clnt_sock);
    WSACleanup();
    return 0;
}