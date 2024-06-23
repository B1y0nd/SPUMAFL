#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<poll.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctype.h>

#define MAX_CLIENTS 3
#define BUFFER_SIZE 1024

int startWith(const char *originString, char *prefix) {
	// 参数校验
	if (originString == NULL || prefix == NULL || strlen(prefix) > strlen(originString)) {
		printf("参数异常，请重新输入！\n");
		return -1;
	}
	
	int n = strlen(prefix);
	int i;
	for (i = 0; i < n; i++) {
	    if (originString[i] != prefix[i]) {
		    return 1;
		}
	}
	return 0;
}

int main(){
    // 创建服务器socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1){
       printf("[PRO]Failed to create socket");
       return 1;
    }
    // 绑定服务器socket和指定和IP和PORT
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(80);
    if(bind(server_fd,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) == -1){
        printf("[PRO]Failed to bind socket");
        return 1;
    }
    // 监听即将到来的客户端连接
    if(listen(server_fd, 20) == -1){
        printf("[PRO]Failed to listen for connections");
        return 1;
    }
    printf("[PRO]Server is listening on port 80\n");
    // 记录pollfd数组中最大的下标
    int index = 0;
    // pollfd结构体数组
    struct pollfd pofds[MAX_CLIENTS];
    for(int i = 0;i < MAX_CLIENTS;i++){
        pofds[i].fd = -1;
    }
    pofds[0].fd = server_fd;
    pofds[0].events = POLLIN;

    // 状态控制
    int state = 0;
    while(1){
        printf("[PRO]block\n");
        int readySockets = poll(pofds, index+1, -1);
        printf("[PRO]non-block\n");
        if (readySockets == -1) {
            printf("[PRO]Poll error");
            break;
        }
        // 检查服务器socket的events
        if(pofds[0].revents & POLLIN){
            struct sockaddr_in clientAddress;
            socklen_t clientAddressLength = sizeof(clientAddress);
            int client_fd = accept(server_fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
            if (client_fd == -1) {
                printf("[PRO]Failed to accept connection");
                continue;
            }
            for(int i = 1;i < MAX_CLIENTS;i++){
                if(pofds[i].fd == -1){
                    pofds[i].fd = client_fd;
                    pofds[i].events = POLLIN;
                    if(index < i){
                        index = i;
                    }
                    printf("[PRO]New client connected: %d\n", client_fd);
                    break;
                }
            }
        }
        // 检查客户端socket的events
        for(int i = 1;i < MAX_CLIENTS;i++){
            if(pofds[i].fd != -1 && pofds[i].revents & POLLIN){
                char buffer[BUFFER_SIZE];
                int bytesRead = recv(pofds[i].fd, buffer, BUFFER_SIZE, 0);
                if(bytesRead == -1)
                {
                    sleep(2);
                }else{
                    buffer[bytesRead] = '\0';
                    printf("[PRO]Received: %s\n", buffer);
                }
                if(!startWith(buffer,"UserLogin")){
                    if(state == 0){
                        if(!strcmp(buffer,"UserLogin:young;123qwe!\r\n\r\n")){
                            char message1[100];
                            strcpy(message1, "Login success!\r\n\r\n");
                            send(pofds[i].fd, message1, strlen(message1), 0);
                            state = 1;
                            continue;
                        }else{
                            char message1[100];
                            strcpy(message1, "Login fail!\r\n\r\n");
                            send(pofds[i].fd, message1, strlen(message1), 0);
                            continue;
                        }
                    }
                }
                if(!startWith(buffer,"UserAddShopCart")){
                    if(state == 1){
                        char message2[100];
                        strcpy(message2, "Add shop cart success!\r\n\r\n");
                        send(pofds[i].fd, message2, strlen(message2), 0);
                        state = 2;
                        continue;
                    }else{
                        char message2[100];
                        strcpy(message2, "Login fail!Cannot add shop cart!\r\n\r\n");
                        send(pofds[i].fd, message2, strlen(message2), 0);
                        continue;
                    }
                }
                if(!startWith(buffer,"UserBuy")){
                    if(state == 1 || state == 2){
                        char message3[100];
                        strcpy(message3, "User buy success!\r\n\r\n");
                        send(pofds[i].fd, message3, strlen(message3), 0);
                        if(state == 2){
                            state = 1;
                        }
                        continue;
                    }else{
                        char message3[100];
                        strcpy(message3, "Login fail!Cannot buy!\r\n\r\n");
                        send(pofds[i].fd, message3, strlen(message3), 0);
                        continue;
                    }
                }
                // 关闭客户端socket，并从pofds中移除
                // todo;
                // 写回客户端
                // send(pofds[i].fd, buffer, bytesRead, 0);
            }
        }
    }
    close(server_fd);
    return 0;
}