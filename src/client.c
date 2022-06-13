#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUFFER_SIZE 1024
#define DEBUG 1

/*
client
初始化需要执行的操作：
int socket ()		建立一个Socket
int connect() 		连接到服务器

编译
gcc client.c -o client.exe -lwsock32
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
int sendMsg(char* msg) {
    int clnt_sock = socketCoon();
    // 向 server 发送 cmd 信息
    char sendData[255] = "sendMsg";
    send(clnt_sock, sendData, sizeof(sendData), 0);

    // 接收 server 发送的 cmd 信息回应
    char recvCmdData[255];
    int recvCmdNum = recv(clnt_sock, recvCmdData, BUFFER_SIZE, 0);
    if (recvCmdNum > 0) {
        recvCmdData[recvCmdNum] = '\0';
        printf("server 的 cmd 回应: %s\n", recvCmdData);
    } else {
        printf("没有接收到 server 的 cmd 回应");
        closesocket(clnt_sock);
        return 0;
    }

    // 向 server 发送 msg 信息
    send(clnt_sock, msg, sizeof(msg), 0);

    // 接收 server 发送的 msg 成功接收信息
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

    closesocket(clnt_sock);
    return 0;
}

int sendFile(char* path) {
    return 0;
}

int getFile(char* path) {
    return 0;
}

int getFileList() {
    int clnt_sock = socketCoon();
    // 向 server 发送 cmd 信息
    char sendData[255] = "sendMsg";
    send(clnt_sock, sendData, sizeof(sendData), 0);

    // 接收 server 发送的 cmd[ls] 信息回应
    char recvCmdData[255];
    int recvCmdNum = recv(clnt_sock, recvCmdData, BUFFER_SIZE, 0);
    if (recvCmdNum > 0) {
        recvCmdData[recvCmdNum] = '\0';
        printf("server 的 cmd 回应: %s\n", recvCmdData);
    } else {
        printf("没有接收到 server 的 cmd 回应");
        closesocket(clnt_sock);
        return -1;
    }
    return 0;
}

int help() {
    printf("用法：\n");
    printf("1. getFile [filename]\n");
    printf("2. sendFile [filename]\n");
    printf("3. sendMsg [message]\n");
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
            debugPrintf("使用命令 getFile [filename]");
            scanf("%s", param);
            debugPrintf("getFile 参数为");
            debugPrintf(param);
            int ret = getFile(param);
            if (ret == -1) {
                printf("%s\n", "getFile 失败");
            }
        } else if (strcmp(cmd, "sendFile") == 0) {
            debugPrintf("使用命令 sendFile [filename]");
            scanf("%s", param);
            debugPrintf("sendFile 参数为");
            debugPrintf(param);
            int ret = sendFile(param);
            if (ret == -1) {
                printf("%s\n", "sendFile 失败");
            }
        } else if (strcmp(cmd, "sendMsg") == 0) {
            debugPrintf("使用命令 sendMsg [message]");
            scanf("%s", param);  // 消息有空格怎么办,先不管了
            debugPrintf("sendMsg 参数为");
            debugPrintf(param);
            int ret = sendMsg(param);
            if (ret == -1) {
                printf("%s\n", "sendMsg 失败");
            }
        } else if (strcmp(cmd, "ls") == 0) {
            debugPrintf("使用命令 ls");
            int ret = getFileList();
            if (ret == -1) {
                printf("%s\n", "ls 失败");
            }
        } else if (strcmp(cmd, "quit") == 0) {
            debugPrintf("使用命令 quit");
            break;
        } else if (strcmp(cmd, "help") == 0) {
            debugPrintf("使用命令 help");
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