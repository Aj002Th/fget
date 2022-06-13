#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#include "help.h"
#pragma comment(lib, "ws2_32.lib")

/*
client
初始化需要执行的操作：
int socket ()		建立一个Socket
int connect() 		连接到服务器

编译
gcc client.c -o client.exe -lwsock32
*/

// 建立 tcp 连接
int socketCoon() {
    // 建立一个 socket
    int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock == -1) {
        printf("socket 建立失败");
        return -1;
    }

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
        return -1;
    }

    return clnt_sock;
}

// 业务函数
int ping() {
    int clnt_sock = socketCoon();

    // 向 server 发送 cmd
    char buffer[BUFFER_SIZE] = "ping";
    sendStrLine(clnt_sock, buffer, sizeof(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    // 接收 server 发送的 pong
    char recvMsgData[BUFFER_SIZE];
    if (getStrLine(clnt_sock, buffer) == -1) {
        printf("没有接收到 server 的消息");
        closesocket(clnt_sock);
        return -1;
    }
    printf("%s\n", buffer);

    closesocket(clnt_sock);
    return 0;
}

int sendFileC(char* fname, int fname_size) {
    int clnt_sock = socketCoon();

    // 向 server 发送 cmd
    char buffer[BUFFER_SIZE] = "getFile";
    sendStrLine(clnt_sock, buffer, sizeof(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    // 向 server 发送 fname
    if (send(clnt_sock, fname, fname_size, 0) < 0) {
        printf("[getAndSaveFileC] 发送文件名失败");
        return -1;
    }

    // 向 server 发送文件流
    sendFile(clnt_sock, fname, "../runtime/clientFile/");
    return 0;
}

int getFileC(char* fname, int fname_size) {
    int clnt_sock = socketCoon();

    // 向 server 发送 cmd
    char buffer[BUFFER_SIZE] = "getFile";
    sendStrLine(clnt_sock, buffer, sizeof(buffer));
    memset(buffer, 0, BUFFER_SIZE);

    // 向 server 发送 fname
    if (send(clnt_sock, fname, fname_size, 0) < 0) {
        printf("[getAndSaveFileC] 发送文件名失败");
        return -1;
    }

    // 接收 server 发送的文件流
    if (getFile(clnt_sock, fname, "../runtime/clientFile/") != 0) {
        printf("[getAndSaveFileC] 接收文件失败");
        return -1;
    }

    return 0;
}

int getFileListC() {
    int clnt_sock = socketCoon();

    // 向 server 发送 cmd 信息
    char sendData[255] = "ls";
    send(clnt_sock, sendData, sizeof(sendData), 0);

    // 接收 server 发送的 文件列表
    char recvMsgData[BUFFER_SIZE];
    int recvMsgNum = recv(clnt_sock, recvMsgData, BUFFER_SIZE, 0);
    if (recvMsgNum > 0) {
        recvMsgData[recvMsgNum] = '\0';
        printf("server 的回应: %s\n", recvMsgData);
    } else {
        printf("没有接收到 server 的消息");
        closesocket(clnt_sock);
        return -1;
    }

    return 0;
}

int help() {
    printf("用法：\n");
    printf("1. getFile [filename]\n");
    printf("2. sendFile [filename]\n");
    printf("3. ping\n");
    printf("4. ls\n");
    printf("5. quit\n");
    printf("6. help\n");
    return 0;
}

int main() {
    socketInit();
    welcome();

    while (1) {
        printf("> ");
        char cmd[255];
        char param[255];
        scanf("%s", cmd);
        if (strcmp(cmd, "getFile") == 0) {
            debugPrintf("使用命令 getFile [filename]", DEBUG);
            scanf("%s", param);
            debugPrintf("getFile 参数为", DEBUG);
            debugPrintf(param, DEBUG);
            int ret = getAndSaveFileC(param);
            if (ret == -1) {
                printf("%s\n", "getFile 失败");
            }
        } else if (strcmp(cmd, "sendFile") == 0) {
            debugPrintf("使用命令 sendFile [filename]", DEBUG);
            scanf("%s", param);
            debugPrintf("sendFile 参数为", DEBUG);
            debugPrintf(param, DEBUG);
            int ret = sendFileC(param, sizeof(param));
            if (ret == -1) {
                printf("%s\n", "sendFile 失败");
            }
        } else if (strcmp(cmd, "ping") == 0) {
            debugPrintf("使用命令 ping", DEBUG);
            int ret = ping();
            if (ret == -1) {
                printf("%s\n", "sendMsg 失败");
            }
        } else if (strcmp(cmd, "ls") == 0) {
            debugPrintf("使用命令 ls", DEBUG);
            int ret = getFileListC();
            if (ret == -1) {
                printf("%s\n", "ls 失败");
            }
        } else if (strcmp(cmd, "quit") == 0) {
            debugPrintf("使用命令 quit", DEBUG);
            break;
        } else if (strcmp(cmd, "help") == 0) {
            debugPrintf("使用命令 help", DEBUG);
            help();
        } else {
            printf("%s\n", "输入无效命令");
            printf("%s\n", "请参见用法");
            help();
        }
    }

    socketClean();
    return 0;
}