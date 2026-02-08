#include <stdio.h>

int main () {
	int a = 150;
	int* ptr = &a;

	printf("using int, a = %d \n", a);
	printf("using pointer, a = %d \n", *ptr);

	*ptr = 155;
	printf("after modification, a = %d \n", a);
	return 0;
}	
