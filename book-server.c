#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#define SERVER_PORT 5432
#define MAX_PENDING 1
#define MAX_LINE 256

unsigned long crc_remainder(unsigned long msg, unsigned long check, int mlen, int clen) {
	//new message which is the message multiplied by 2^(length of divisor-1)
	unsigned long newmsg = msg << (clen-1);
	//new unsigned long to take part of remainder
	unsigned long n;
	int i;
	//i starts at the size of the message; iterates while i is greater than zero and decreases by one each time
	for (i = mlen; i > 0; i--) {
		//if a 1 in the new message and the 1 in the 1 << i+ divisor length -2 dont line up do
		if ((newmsg & (1 << (i+clen-2))) != 0) {
			//n becomes the divisor * 2 ^(i-1)
			n = check << (i - 1);
			//new message is taken to the power of the n defined above
			newmsg = newmsg ^ n;
		}
	}
	// we return the new value and & it with 1<< length of remainder -1 which should be 1 less than 1<<clen bits and all 1's
	// This should only return as many bits of the new value as we need which is the last clen bits
	return newmsg & ((1 << clen) - 1);
}

int main()
{
	struct sockaddr_in sin;
	char buf[MAX_LINE];
	int len, l;
	int s, new_s;
	char * ptr;
	unsigned long divisor = strtoul("1011", &ptr, 2);
	
	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);

	/* wait for connection, then receive and print text */
	while(1) {
		l = sizeof(sin);
		if ((new_s = accept(s, (struct sockaddr *)&sin, &l)) < 0) {
			perror("simplex-talk: accept");
			exit(1);
		}
		while (len = recv(new_s, buf, sizeof(buf), 0)>0){
			//creates char array to hold what is put into buffer
			char* recieved = (char *)malloc(sizeof(char)*MAX_LINE);
			strcpy(recieved,buf);
			
			//loads final three characters of message into array
			char* seekDiv = (char *) malloc(sizeof('1')*3);
				seekDiv[0] = recieved[strlen(recieved)-3];
				seekDiv[1] = recieved[strlen(recieved)-2];
				seekDiv[2] = recieved[strlen(recieved)-1];
			//creates array to hold the first n many bits of message without the final 3 validation bits
			char* message = (char *) malloc(sizeof('1') * (strlen(recieved)-3));
			//loads first n many bits into array
			for(int i = 0; i < (strlen(recieved)-3);i++){
				message[i] = recieved[i];
			}
			printf("Content of message: %s\n", message);
			
			//determines if message is valid or not
			if(crc_remainder(strtoul(message, &ptr, 2), divisor, (strlen(recieved) -3), 4) == strtoul(seekDiv, &ptr,2)){
				printf("valid message recieved\n");
			}
			else{
				printf("message Invalid\n");
			}
			//clears mem in arrays so that they can be reused
			free(recieved);
			free(seekDiv);
			
		}
		close(new_s);
	}
}
