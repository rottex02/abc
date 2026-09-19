#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
	// 1 ->
	printf("First part ->\n");
	char arr[100];
	
	printf("Type ->\n");
	read(0, arr, 100);
	
	printf("Array -> %s", arr);
	
	int x = atoi(arr);
	
	printf("After converting into integer -> %d\n", x);
	printf("\n");
	
	// 2 ->
	printf("Second part ->\n");
	
	int z = 999666333;
	int divisor = 1;
	int digit;
	char ch;
	
	while (z/divisor >= 10) {
		divisor *= 10;
	}
	
	printf("Writing value of z(integer) to terminal(str/arr) ->\n");
	
	while (divisor > 0) {
		digit = z/divisor;
		
		ch = digit + '0'; // equivalent to digit + 48 [converting to char]
		write(1, &ch, 1);
		
		z = z % divisor;
		divisor /= 10;
	}
	
	printf("\n");
	
	return 0;
}
