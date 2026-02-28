#include <stdio.h>

int main () {
	char* arr[2] = {"Breaking", "bad"};

	for (int i = 0; i < 2; i++) {
		printf("arr[%d] = %s \n", i, arr[i]);
	}

	arr[1] = "bread";
	printf("modified array \n");

	for (int i = 0; i < 2; i++) {
                printf("arr[%d] = %s \n", i, arr[i]);
        }

	return 0;
}
