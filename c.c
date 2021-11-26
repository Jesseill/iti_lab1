#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_BUFFER 10000
#define PORT 80

int socketfd;
void socket_to_host(const char *url, in_port_t port){
	struct sockaddr_in addr;
	struct hostent *hostt;
	int sockfd;     
	
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//printf("%d\n", sockfd);
	if (sockfd<0){
		printf("socket create faild");
		exit(1);

	}

	if((hostt = gethostbyname(url)) == NULL){
		herror("gethostbyname faild");
		exit(1);
	}

	memcpy( &addr.sin_addr, hostt->h_addr, hostt->h_length);

	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;

	// if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr )<0){
	// 	printf("invalid addr");
	// 	exit(1);
	// }

	//printf("create socket\n");
	

	
	if(connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
		perror("connection fail");
		exit(1);

	}
	//printf("done connect\n");
	
	socketfd = sockfd;

	return;
}

typedef struct pp{
	int cnt;
	char href[100][MAX_BUFFER];
}P;

P gethref(int obj, char html[100][MAX_BUFFER] ){
	//char ** html = (char**)buff;
	P p;
	p.cnt = 0;
	memset(p.href, 0, sizeof(p.href[0][0])*100*MAX_BUFFER);
	char para[] = {"<a href=\"\""};
	int cmp=0;
	int recording = 0;
	int index = 0;
	//printf("start finding\n");
	
	//printf("%c\n", html[0][0]);
	
	
	for(int i=0; i<obj; i++){
		for(int j=0; ;j++){
			//printf("%c", html[i][j]);
			if(html[i][j]=='\0')
				break;
			
			if(html[i][j]==para[cmp])
			{
				//printf("%d %c %c\n",cmp,para[cmp], html[i][j]);
				if(cmp == 9){
					cmp = 0;
					recording = 0;
					p.cnt++;
					index = 0;
				}else if(cmp == 8){
					recording = 1;
					cmp++;
				}else{
					cmp++;
				}
				
			}else if(recording == 1){
				p.href[p.cnt][index] = html[i][j];
				index++;
				//printf("%c", html[i][j]);
			}else if(cmp!=0 && recording==0)
				cmp=0;
			
		}

	}


	return p;
}


int main(int argc, char *argv[]){
	char buffer[100][MAX_BUFFER];
	char url[MAX_BUFFER];
	char dmn[MAX_BUFFER];
	char path[MAX_BUFFER];
	int recv_size;
	//strcpy(dmn, argv[1]);
	scanf("%s", dmn);
	//printf("%s\n", dmn);
	char *split = strchr(dmn, '/');

	// recieve 100 times at most
	memset(buffer, 0, sizeof(buffer[0][0])*100*MAX_BUFFER);
	bzero(path, MAX_BUFFER);

	strcat(path, split);
	*split = '\0';
	char msg[MAX_BUFFER];
	char cnt[] = "HTTP/1.0\r\n\r\n";
	//printf("hi\n");
	//printf("%s %s\n", dmn, path);
	

	//create socket and connect to server
	socket_to_host(dmn, PORT); 

	
	//printf("socketfd\n");
	// strcat(msg, url);
	// strcat(msg, cnt);
	snprintf(msg, sizeof(msg), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, dmn); 
	
	//printf("send msg: %s\n", msg);

	if (send(socketfd, msg, strlen(msg), 0) == -1){
		perror("fail send rqst");
		exit(1);
	}
	//printf("succesful send request");
	int i = 0;

	while(recv_size = recv(socketfd,buffer[i], MAX_BUFFER, 0)){
		if(recv_size<0){
			perror("fail recv response");
			break;
		}
		buffer[i][recv_size] = '\0';
		i++;
	}
	int j=0;
	// while(j<i){   //obj0, obj1, ... obji-1  
	// 	printf("\nobj: %d\n: %s\n", j,buffer[j]);
	// 	j++;
	// }
	//printf("\nreturn objs: %d\n", i);

	P p = gethref(i, buffer);
	j=0;
	printf("%d\n",p.cnt);
	while(j<p.cnt){
		printf("%s\n", p.href[j]);
		j++;
	}


	shutdown(socketfd, SHUT_RDWR); 
	close(socketfd); 

	return 0;
}

//http://can.cs.nthu.edu.tw/

//can.cs.nthu.edu.tw/index.php
