#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define SERVPORT 5555
#define DEST_HOST "1.gpsserver.duapp.com"
#define BACKLOG 10		//the counts of connect can keep in wait queen
int main(int argc, char **argv)
{
    int sockfd, sock_dt;
    printf("#####################################################\n");
    printf("socket test      by pafone   19th,April,2009\n");
    printf("#####################################################\n");
    struct sockaddr_in my_addr;	//local ip info
    struct sockaddr_in dest_addr;	//destnation ip info
    struct sockaddr_in server_addr;
    struct hostent *h;
    socklen_t sin_size;
    /*
    if (argc != 3) {
	printf
	    ("useage:socket_client ipaddress port\n eg:socket_client \\par             192.168.1.158 5555\n");
	return -1;
    }
    */
    //int destport = atoi(argv[2]);
	int destport = 80;
//dns
	//if((h=gethostbyname(argv[1])) == NULL)
	if((h=gethostbyname(DEST_HOST)) == NULL)
	{
	/* 如果 gethostbyname 失败,则给出错误信息 */
	herror("gethostbyname");
	/* 然后退出 */
	exit(1);
	}
	printf(" Address : %s\n", inet_ntoa(*((struct in_addr *)h->h_addr)));

    if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0))) {
	perror("error in create socket\n");
	exit(0);
    }
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(destport);
    dest_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)h->h_addr)));
//    bzero(&dest_addr.sin_zero,0,8);
    memset(&dest_addr.sin_zero, 0, 8);
    int opt=1,new_pid,sta_,new_sockfd;
	my_addr.sin_family = AF_INET;
	my_addr.sin_addr.s_addr = INADDR_ANY;
	my_addr.sin_port = htons(SERVPORT);
	memset(&my_addr.sin_zero,0,8);
	//strncpy((char *)&server_addr,(char *)&dest_addr,sizeof(struct sockaddr_in));
	memcpy(&server_addr , &dest_addr , sizeof(struct sockaddr_in));

//connect
	char str[]="GET /index.php HTTP/1.1\r\nHost:" DEST_HOST "\r\n\r\n";
	//char str[]="GET /index.htm HTTP/1.1\r\nHost:" DEST_HOST "\r\n\r\n";
    if (-1 ==
	connect(sockfd, (struct sockaddr *)&dest_addr,
		sizeof(struct sockaddr))) {
	perror("connect error\n");
	exit(0);
    }
    int n_send_len;
    n_send_len = send(sockfd, str, strlen(str), 0);
    printf("%d bytes sent\n", n_send_len);

    char buf[1024];
    char * port;
	int i, read_bytes=1,new_fd,child_pid,status;

	 while (1)
	{
		read_bytes = read(sockfd, buf, 1024);
		//printf("%d\n", read_bytes);
		printf("%s\n", buf);
		memset(buf,0,1024);
		if (read_bytes==0)
			break;
		if (port = strstr(buf,"server port:"))
		{
			port+=strlen("server port:");
			server_addr.sin_port = htons(atoi(port));
			break;
		}
		//printf("%d\n",read_bytes);
	}

	new_pid = fork();
	if (!new_pid)
	{
		sock_dt = socket(AF_INET,SOCK_STREAM,0);
		setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR, &opt,sizeof(opt));
		if(!bind(sock_dt , (struct sockaddr *)&my_addr.sin_addr,sizeof(struct sockaddr *)))
		{
			perror("bind error\n");
			exit(1);
		}
		if (-1 == listen(sock_dt, BACKLOG)) {
			perror("listen error.\n");
			exit(1);
		}

		if(-1 == (new_fd = accept(sock_dt,(struct sockaddr *)&server_addr,&sin_size))){
			perror("accept error.\n");
			exit(1);
		}
		else
		{
			printf("connect success.\n");
			exit(0);
		}

	}
	else
	{
		child_pid = wait(&status);
		printf("child pid: %d,exit status: %d\n",child_pid,status);
	}
    close(sockfd);
}
