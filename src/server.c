#include <direct.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <winsock2.h>
#include "help.h"
#pragma comment(lib, "ws2_32.lib")

#define MAX_LEN 4096

/*
server
初始化需要执行的操作：
int socket ()		建立一个Socket
int bind()			与某个端口绑定
int listen()		开始监听端口
int accept()		等待/接受客户端的连接请求

gcc server.c -o server.exe -lwsock32
*/

int getFileListS(char* list) {
    long file;
    struct _finddata_t find;
    _chdir("../runtime/serverFile");
    if ((file = _findfirst("*.*", &find)) == -1L) {
        printf("空白!\n");
        exit(0);
    }
    printf("%s\n", find.name);
    while (_findnext(file, &find) == 0) {
        printf("%s\n", find.name);
        if (strcmp(find.name, ".") != 0 && strcmp(find.name, "..") != 0)
            strcat(list, find.name);
        strcat(list, "\n");
    }
    _findclose(file);
    return 0;
}

int sendFileS(int clnt_sock, char* filename) {
    sendFile(clnt_sock, filename, "../runtime/serverFile/");
    return 0;
}

int getFileS(int clnt_sock, char* filename) {
    if (getAndSaveFile(clnt_sock, filename, "../runtime/serverFile/") != 0) {
        printf("111");
    }
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

        // 进入不同的服务
        if (strcmp(cmd, "getFile") == 0) {
            debugPrintf("使用命令 getFile [filename]", DEBUG);

            // 获取文件名
            if (getStrLine(clnt_sock, param) == -1) {
                printf("%s\n", "[getFile] 获取 fname 失败");
                continue;
            }

            // 发送文件流
            if(sendFileS(clnt_sock, param) == -1) {
                printf("%s\n", "[getfile] 发送文件流失败");
                continue;
            };
        }

        if (strcmp(cmd, "sendFile") == 0) {
            debugPrintf("使用命令 sendFile [filename]", DEBUG);

            // 获取文件名
            if (getStrLine(clnt_sock, param) == -1) {
                printf("%s\n", "[sendFile] 获取 fname 失败");
                continue;
            }

            // 接收文件流
            if(getFileS(clnt_sock, param) == -1){
                printf("%s\n", "[sendFile] 接收文件流失败");
                continue;
            }
        }

        if (strcmp(cmd, "ping") == 0) {
            debugPrintf("使用命令 ping", DEBUG);

            char buffer[BUFFER_SIZE] = "pong";
            if (sendStrLine(clnt_sock, buffer, sizeof(buffer)) == -1) {
                printf("%s\n", "[ping] pong 发送失败");
                continue;
            }
        }

        if (strcmp(cmd, "ls") == 0) {
            debugPrintf("使用命令 ls", DEBUG);

            char buffer[BUFFER_SIZE] = {0};
            getFileList(buffer);
            if (sendStrLine(clnt_sock, buffer, sizeof(buffer)) == -1) {
                printf("%s\n", "[ls] 获取文件列表失败");
                continue;
            }
        }

        // 关闭连接
        closesocket(clnt_sock);
    }

    closesocket(serv_sock);
    socketClean();
    return 0;
}