#include <stdio.h>

int str_length (char* ptr) {
	int i = 0;
	while (*ptr) {
		i = i + 1;
		ptr = ptr + 1;
	}
	return i;
}

int main () {
	char* str = "hello";

	for (int i = 0; i < 5; i++) {
		printf("char[%d] = %c \n", i, *(str + i));
	}

	char* user_str;
	scanf("%s", user_str);
	printf("length of your word is %d! \n", str_length(user_str));

	return 0;
}
