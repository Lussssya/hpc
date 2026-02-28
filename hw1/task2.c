#include <stdio.h>

int main () {
	int arr[5] = {1, 2, 3, 4, 5};
	int* ptr = &arr[0];

	for (int i = 0; i < 5; i++) {
		printf("arr[%d] = %d \n", i, *(ptr + i));
		*(ptr+i) *= 2;
	}

	printf("\nprint the array after modification: \n");

	for (int i = 0; i < 5; i++) {
		printf("using pointer: arr[%d] = %d \n", i, *(ptr + i));
		printf("using array: arr[%d] = %d \n\n", i, arr[i]);
	}


	return 0;
}
