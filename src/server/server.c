#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")

/*
server
初始化需要执行的操作：
int socket ()		建立一个Socket
int bind()			与某个端口绑定
int listen()		开始监听端口
int accept()		等待/接受客户端的连接请求

gcc server.c -o server.exe -lwsock32
*/

int main() {
    // 初始化 socket 库
    WSADATA wsd;
    WORD socketVersion = MAKEWORD(2, 2);
    if (WSAStartup(socketVersion, &wsd) != 0) {
        printf("socket 库初始化失败");
        return 0;
    }

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
        printf("等待连接到来...\n");
        struct sockaddr_in clnt_addr;
        int clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock =
            accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if (clnt_sock == -1) {
            printf("服务 accept 失败");
            continue;
        }

        // 接收客户端发送的数据
        char recvData[255];
        int recvNum = recv(clnt_sock, recvData, clnt_addr_size, 0);
        if(recvNum > 0) {
            recvData[recvNum] = '\0';
            printf("get msg from client:\n%s\n", recvData);
        }
        else {
            printf("没有接收到 client 消息");
        }

        // 向客户端发送数据
        char str[] = "Hello World!";
        send(clnt_sock, str, sizeof(str), 0);

        // 关闭连接
        closesocket(clnt_sock);
    }

    closesocket(serv_sock);
    WSACleanup();
    return 0;
}