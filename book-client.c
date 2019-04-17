#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define SERVER_PORT 5432
#define MAX_LINE 256
const char *unsigned_to_binary(unsigned long msg, int size){
	//pointer to a character list the size of msg +1
	char* b = (char *)malloc(size + 1);
	// initial char is list is '/0'
	b[0] = '\0';

	int z;
	//z to start is 1 x 2^(size -1); goes until z is greater than 0; every iteration z is divided by 2
	for (z = (1<<(size-1)); z > 0; z >>= 1)
	{
		//adds a one if msg & z ==1 else 0 to the next spot in the string
		strcat(b, ((msg & z) == z) ? "1" : "0");
	}

	return b;
}
//function to create a remainder for CRC out of the message a divisor and the lengths of both
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

int main(int argc, char * argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s;
	int len;
	unsigned long msg, msg2;
	unsigned long divisor;
	unsigned long newmsg;
	unsigned long rem;

	if (argc==2) {
		host = argv[1];
	}
	else {
		fprintf(stderr, "usage: simplex-talk host\n");
		exit(1);
	}

	/* translate host name into peer's IP address */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	/* active open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}
	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		
		char * ptr;
		msg = strtoul(buf, &ptr, 2);
		len = strlen(buf)-1;
		
		divisor = strtoul("1011", &ptr, 2);
		

 		rem = crc_remainder(msg, divisor, len, 4);
		srand(time(0));
		int randNum = (rand() % 10);
		
		if (randNum <= 2){
			int rand1 = (rand() % (len));
			if (buf[rand1] == '0'){
				buf[rand1] = '1';
			}
			else{
				buf[rand1] = '0';
			}
			
			printf("Oops, the bit at index %d was flipped! The new message is: %s\n", rand1, buf);
	}
		msg2 = strtoul(buf, &ptr, 2);
		unsigned long newmsg = (msg2 << 3) + rem;
		

		strcpy(buf, unsigned_to_binary(newmsg, len+3));
		
		
		buf[MAX_LINE-1] = '\0';
		
		len = strlen(buf)+1;
		
		send(s, buf, len, 0);
	}
}

