#include <stdio.h>
// Ben Gertz bitwise c lab. I used the function shown in lab to print out a decimal number as binary
int MSB(unsigned char x){
    unsigned char mask = 0x80; // 1000 0000
    return (x & mask) != 0;
}

void print_bin(unsigned char number)
{
    
    for(int i = 0; i < 8; i++)
    {
        printf("%d", MSB(number));
        number = number << 1; // Shift left by one bit 
    }
}

int prefix(unsigned int x, int n){
	x >>= (8-n);
	print_bin(x);
	return x;

}

int suffix(unsigned int x, int n){
	x <<= (8-n);
	x &= 255;
	x >>= (8-n);
	print_bin(x);
	return x;
}
int suffix1(unsigned int x, int n){
	x <<= (8-n);
	x &= 255;
	x >>= (8-n);
	
	return x;
}

int toggle(unsigned int x, int n){
	unsigned int y= x;
	x >>= (n-1);
	
	x ^= 1;
	
	x <<= (n-1);
	
	x |= suffix1(y, (n-1));
	print_bin(x);
	return x;
}

int main(void){
	prefix(255, 5);
	printf("\n");
	suffix(255, 5);
	printf("\n");
	toggle(1, 7);
	printf("\n");

	return 0;
}

