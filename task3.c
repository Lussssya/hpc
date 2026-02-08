#include <stdio.h>

void swap (int* a, int* b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

int main () {
	int a = 0;
	int b = 10;
	int* ptr_a = &a;
	int* ptr_b = &b;

	printf("before swap: a = %d, b = %d \n", a, b);

	swap(ptr_a, ptr_b);

	printf("after swap: a = %d, b = %d \n", a, b);

	return 0;
}
