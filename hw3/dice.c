#include <stdio.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define P 4
#define R 3

struct RoundWinner {
        int max;
        int id;
};

pthread_barrier_t barrier;
int results[R];
int round_results[P];

struct RoundWinner findMax (int arr[]) {
    struct RoundWinner rw;
    rw.max = -1;

    for (int i = 0; i < P; i++) {
        if (arr[i] > rw.max) {
            rw.max = arr[i];
            rw.id = i;
        }
    }

    return rw;
}


int findWinner () {
    int max_freq = 0;
    int most_frequent_element = results[0];

    for (int i = 0; i < R; i++) {

        int current_freq = 0;
        for (int j = 0; j < R; j++) {
            if (results[j] == results[i]) {
                current_freq++;
            }
        }

        if (current_freq > max_freq) {
            max_freq = current_freq;
            most_frequent_element = results[i];
        }
    }

    return most_frequent_element;
}


int play (void* (*thread_func)(void*)) {
    pthread_t players[P];
    int player_ids[P];
    int winner_id;

    pthread_barrier_init(&barrier, NULL, P);

    for (int j = 0; j < P; j++) {
        player_ids[j] = j;
        pthread_create(&players[j], NULL, thread_func, &player_ids[j]);
    }

    for (int j = 0; j < P; j++) {
        pthread_join(players[j], NULL);
    }
    
    pthread_barrier_destroy(&barrier);


    printf("All round winners:\n");
    for (int i = 0; i < R; i++) {
        printf("Round %d -> %d\n\n", i + 1, results[i]);
    }

    winner_id = findWinner();

    return winner_id;
}


void* roll (void* args) {
    int player_id = *((int*)args);

    int result;
    for (int i = 0; i < R; i++) {
        result = (rand() % 6 + 1);
    
        printf("Player %d rolled %d\n", player_id, result);

        round_results[player_id] = result;

        printf("Player %d waiting at barrier...\n", player_id);
        pthread_barrier_wait(&barrier);
        printf("Player %d passed the barrier.\n", player_id);
        if (player_id == 0) {
            struct RoundWinner rw = findMax(round_results);
            results[i] = rw.id;
            printf("Round %d winner is player %d\n", i + 1, rw.id);
        }

        pthread_barrier_wait(&barrier);
    }
    return NULL;
}

int main () {
    srand(time(NULL));
    int winner_id = play(roll);
    printf("Overall winner: %d\n", winner_id);
    return 0;
}

