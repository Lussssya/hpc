#include <stdio.h>
#include <stdlib.h>

int main () {
	int* ptr_a;

	ptr_a = malloc(sizeof(int));

	if (ptr_a == NULL) {
		printf("memory allocation for integer failed \n");
		return 1;
	}

	*ptr_a = 7;

	printf("a = %d \n", *ptr_a);

	int* arr;

	arr = malloc(5 * sizeof(int));

	if (arr == NULL) {
                printf("memory allocation for array failed \n");
                return 1;
        }

	for (int i = 0; i < 5; i++) {
		arr[i] = i;
		printf("arr[%d] = %d \n", i, arr[i]);
	}

	free(ptr_a);
	free(arr);
	return 0;
}
