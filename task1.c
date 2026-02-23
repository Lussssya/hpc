#include <stdio.h>
#include <pthread.h>

void* print_thread (void *arg) {
	int* thread_id = (int*)arg;
	printf("thread %d created!\n", *thread_id);
	return NULL;
}

int main () {	
	pthread_t thread1, thread2, thread3;
	int id_1 = 1, id_2 = 2, id_3 = 3;
	
	if (pthread_create(&thread1, NULL, print_thread, &id_1)) {
		perror("failed to create thread 1\n");
		return 1;
	}

	if (pthread_create(&thread2, NULL, print_thread, &id_2)) {
                perror("failed to create thread 2\n");
                return 1;
        }

	if (pthread_create(&thread3, NULL, print_thread, &id_3)) {
                perror("failed to create thread 3\n");
                return 1;
        }

	if (pthread_join(thread1, NULL) != 0) {
        	perror("Failed to join thread 1\n");
       		return 1;
    	}

   	if (pthread_join(thread2, NULL) != 0) {
        	perror("Failed to join thread 2\n");
        	return 1;
    	}

	if (pthread_join(thread3, NULL) != 0) {
                perror("Failed to join thread 3\n");
                return 1;
        }

	return 0;
}
