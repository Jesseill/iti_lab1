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
#define BUFFER_SIZE 10000
#define PORT 80

int socket_connect(const char *url, in_port_t port){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1, sock;     
	
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//printf("%d\n", sock);
	if (sock<0){
		printf("socket create faild");
		exit(1);

	}

	if((hp = gethostbyname(url)) == NULL){
		herror("gethostbyname");
		exit(1);
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	//printf("host\n");

	//setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
	
	// if(inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr )<0){
	// 	printf("invalid addr");
	// 	exit(1);
	// }

	//printf("create socket\n");
	

	// setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));
	
	// if(sock == -1){
	// 	perror("set sockopt fail");
	// 	exit(1);
	// }
	// 	printf("set sockopt");
	

	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0){
		perror("connection fail");
		exit(1);

	}
	//printf("done connect\n");
	
	return sock;
}

typedef struct pp{
	int cnt;
	char href[100][BUFFER_SIZE];
}P;

P gethref(int obj, char html[100][BUFFER_SIZE] ){
	//char ** html = (char**)buff;
	P p;
	p.cnt = 0;
	memset(p.href, 0, sizeof(p.href[0][0])*100*BUFFER_SIZE);
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
	int fd;
	char buffer[100][BUFFER_SIZE];
	char url[BUFFER_SIZE];
	char dmn[BUFFER_SIZE];
	char path[BUFFER_SIZE];
	int recv_size;
	//strcpy(dmn, argv[1]);
	scanf("%s", dmn);
	//printf("%s\n", dmn);
	char *split = strchr(dmn, '/');
	memset(buffer, 0, sizeof(buffer[0][0])*100*BUFFER_SIZE);
	bzero(path, BUFFER_SIZE);
	strcat(path, split);
	*split++ = '\0';
	char msg[BUFFER_SIZE];
	char cnt[] = "HTTP/1.0\r\n\r\n";
	//printf("hi\n");
	//printf("%s %s\n", dmn, path);
	
	fd = socket_connect(dmn, PORT); 
	//printf("fd\n");
	// strcat(msg, url);
	// strcat(msg, cnt);
	snprintf(msg, sizeof(msg), "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, dmn);
	// write(fd, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	bzero(buffer, BUFFER_SIZE);
	
	// while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
	// 	fprintf(stderr, "%s", buffer);
	// 	bzero(buffer, BUFFER_SIZE);
	// }
	//printf("send msg: %s\n", msg);

	if (send(fd, msg, strlen(msg), 0) == -1){
		perror("fail send rqst");
		exit(1);
	}
	//printf("succesful send request");
	int i = 0;

	while(recv_size = recv(fd,buffer[i], BUFFER_SIZE, 0)){
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


	shutdown(fd, SHUT_RDWR); 
	close(fd); 

	return 0;
}

//http://can.cs.nthu.edu.tw/

//can.cs.nthu.edu.tw/index.php
