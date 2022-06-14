#include <stdio.h>

#define BUFFER_SIZE 1024
#define DEBUG 1

// 欢迎图案
int welcome() {
    printf("%s\n", "  _____              __   ");
    printf("%s\n", "_/ ____\\____   _____/  |_ ");
    printf("%s\n", "\\   __\\/ ___\\_/ __ \\   __\\");
    printf("%s\n", " |  | / /_/  >  ___/|  |  ");
    printf("%s\n", " |__| \\___  / \\___  >__|  ");
    printf("%s\n", "     /_____/      \\/      ");

    printf("%s\n", "启动成功");
}

// 启用 socket 库
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

// 关闭 socket 库
int socketClean() {
    WSACleanup();
    return 0;
}

// 获取一行文字
int getStrLine(int clnt_sock, char* buffer) {
    int num = recv(clnt_sock, buffer, BUFFER_SIZE, 0);
    if (num <= 0) {
        printf("%s\n", "[getStrLine] 没有接收到 client 消息");
        return -1;
    }

    // 打印显示
    buffer[num] = '\0';
    return 0;
}

// 发送一行文字
int sendStrLine(int clnt_sock, char* buffer, int buffer_size) {
    int num = send(clnt_sock, buffer, buffer_size, 0);
    if (num <= 0) {
        printf("%s\n", "[sendStrLine] 没有成功发送 server 消息");
        return -1;
    }

    // 打印显示
    buffer[num] = '\0';
    return 0;
}

// 获取并保存文件
int getFile(int clnt_sock, char* fname, char* path) {
    char cPath[BUFFER_SIZE] = {0};
    char buffer[BUFFER_SIZE] = {0};
    sprintf(cPath, "%s%s", path, fname);
    // printf("length of cPath: %d\n", sizeof(cPath));
    // printf("length of path: %d\n", sizeof(path));
    // printf("length of filename: %d\n", sizeof(filename));
    // cPath[sizeof(path) + sizeof(filename) - 2] = '\0';

    // 打开文件，准备写入
    FILE* fp;
    if (fopen_s(&fp, cPath, "wb") != 0) {
        printf("[getFile] 文件打开失败：%s\n", cPath);
        printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
        return -1;
    }

    // 从服务器接收数据到 buffer 中
    // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
    int length = 0;
    while ((length = recv(clnt_sock, buffer, BUFFER_SIZE, 0)) > 0) {
        printf("length: %d\n", length);
        if (fwrite(buffer, sizeof(char), length, fp) < length) {
            printf("文件写入失败：%s\n", cPath);
            return -1;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    // 关闭文件
    fclose(fp);
    return 0;
}

// 发送文件
int sendFile(int clnt_sock, char* fname, char* path) {
    char cPath[BUFFER_SIZE] = {0};
    char buffer[BUFFER_SIZE] = {0};
    sprintf(cPath, "%s%s", path, fname);
    // printf("length of cPath: %d\n", sizeof(cPath));
    // printf("length of path: %d\n", sizeof(path));
    // printf("length of filename: %d\n", sizeof(filename));
    // cPath[sizeof(path) + sizeof(filename) - 2] = '\0';

    // 打开文件并读取文件数据
    FILE* fp;
    if (fopen_s(&fp, cPath, "rb") != 0) {
        printf("[sendFile] 文件打开失败:%s\n", cPath);
        printf("open fail errno = %d reason = %s \n", errno, strerror(errno));
        return -1;
    }

    printf("开始传输文件：%s\n", cPath);
    int length = 0;

    // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
    while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
        if (send(clnt_sock, buffer, length, 0) < 0) {
            printf("文件发送失败:%s/n", buffer);
            return -1;
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    // 关闭文件
    fclose(fp);
    return 0;
}

// debug 日志
int debugPrintf(char* msg, int debug) {
    if (debug) {
        printf("debug: %s\n", msg);
    }
    return 0;
}