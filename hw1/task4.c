#include <stdio.h>

int main () {
	int a = 0;
	int* ptr = &a;
	int** ptr_to_ptr = &ptr;

	printf("using pointer, a = %d \n", *ptr);
	printf("using double pointer, a = %d \n", **ptr_to_ptr);

	return 0;
}
