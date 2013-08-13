//============================================================================
// Name        : pblink.c
// Description : MPI based system for blinking RGB leds on a cluster of
// 		 Raspberry Pis
// wiringPi pin numbers:
//
//      pin 0 = (Header pin 11) (Red)
//      pin 1 = (Header pin 12) (Blue)
//      pin 2 = (Header pin 13)
//      pin 3 = (Header pin 15)
//      pin 4 = (Header pin 16)
//      pin 5 = (Header pin 18)
//      pin 6 = (Header pin 22)
//      pin 7 = (Header pin 7) (Green)
//
// Author: Joshua Kiepert
// Date: 2013-03-08
// Modified: 2013-05-14
//============================================================================

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <mpi.h>
#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>

const int R_PIN = 0;
const int B_PIN = 1;
const int G_PIN = 7;
const int OFF = 1;
const int ON = 0;

const int JOB_COMPLETED = 1;
const int CHASE = 2;
const int STROBE = 3;
const int BLINK = 4;

#define NETWORK_LAYOUT

#ifdef NETWORK_LAYOUT
  int stackwise_up[] = {29,25,21,17,13,9,5,1,30,26,22,18,14,10,6,2,31,27,23,19,15,11,7,3,32,28,24,20,16,12,8,4};
  int stackwise_up_size = 32;
  int stackwise_down[] = {1,5,9,13,17,21,25,29,2,6,10,14,18,22,26,30,3,7,11,15,19,23,27,31,4,8,12,16,20,24,28,32};                                                                                                                             
  int stackwise_down_size = 32;
  int horizontal_lr[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};                                                                                                                              
  int horizontal_lr_size = 32;
  int horizontal_rl[] = {4,3,2,1,8,7,6,5,12,11,10,9,16,15,14,13,20,19,18,17,24,23,22,21,28,27,26,25,32,31,30,29};
  int horizontal_rl_size = 32;
  int spiral[] = {1,2,3,4,8,12,16,20,24,28,32,31,30,29,25,21,17,13,9,5,6,7,11,15,19,23,27,26,22,18,14,10,11,15,19,18,14,15,19,18};
  int spiral_size = 40;
  int zigzag[] = {1,2,3,4,8,7,6,5,9,10,11,12,16,15,14,13,17,18,19,20,24,23,22,21,25,26,27,28,32,31,30,29};
  int zigzag_size = 32;
  int circle_outer[] = {29,25,21,17,13,9,5,1,2,3,4,8,12,16,20,24,28,32,31,30};
  int circle_outer_size = 20;
  int circle_inner_ccw[] = {7,6,10,14,18,22,26,27,23,19,15,11};
  int circle_inner_ccw_size = 12;
  int circle_inner_cw[] = {7,11,15,19,23,27,26,22,18,14,10,6};  
  int circle_inner_cw_size = 12;

  const int ROW0 = 0x0000000F;
  const int ROW1 = 0x000000F0;
  const int ROW2 = 0x00000F00;
  const int ROW3 = 0x0000F000;
  const int ROW4 = 0x000F0000;
  const int ROW5 = 0x00F00000;
  const int ROW6 = 0x0F000000;
  const int ROW7 = 0xF0000000;
  const int COL0 = 0x11111111;
  const int COL1 = 0x22222222;
  const int COL2 = 0x44444444;
  const int COL3 = 0x88888888;
#else
  int stackwise_up[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32};      
  int stackwise_up_size = 32;
  int stackwise_down[] = {8,7,6,5,4,3,2,1,16,15,14,13,12,11,10,9,24,23,22,21,20,19,18,17,32,31,30,29,28,27,26,25};       
  int stackwise_down_size = 32;
  int horizontal_lr[] = {8,16,24,32,7,15,23,31,6,14,22,30,5,13,21,29,4,12,20,28,3,11,19,27,2,10,18,26,1,9,17,25};       
  int horizontal_lr_size = 32;
  int horizontal_rl[] = {32,24,16,8,31,23,15,7,30,22,14,6,29,21,13,5,28,20,12,4,27,19,11,3,26,18,10,2,25,17,9,1};       
  int horizontal_rl_size = 32;
  int spiral[] = {8,16,24,32,31,30,29,28,27,26,25,17,9,1,2,3,4,5,6,7,15,23,22,21,20,19,18,10,11,12,13,14,22,21,20,12,13,21,20,12};
  int spiral_size = 40;
  int zigzag[] = {8,16,24,32,31,23,15,7,6,14,22,30,29,21,13,5,4,12,20,28, 27,19,11,3,2,10,18,26,25,17,9,1};
  int zigzag_size = 32;
  int circle_outer[] = {1,2,3,4,5,6,7,8,16,24,32,31,30,29,28,27,26,25,17,9};
  int circle_outer_size = 20;
  int circle_inner_ccw[] = {23,15,14,13,12,11,10,18,19,20,21,22};
  int circle_inner_ccw_size = 12;
  int circle_inner_cw[] = {23,22,21,20,19,18,10,11,12,13,14,15};
  int circle_inner_cw_size = 12;

  const int ROW0 = 0x01010101;
  const int ROW1 = 0x02020202;
  const int ROW2 = 0x04040404;
  const int ROW3 = 0x08080808;
  const int ROW4 = 0x10101010;
  const int ROW5 = 0x20202020;
  const int ROW6 = 0x40404040;
  const int ROW7 = 0x80808080;
  const int COL0 = 0x000000FF;
  const int COL1 = 0x0000FF00;
  const int COL2 = 0x00FF0000;
  const int COL3 = 0xFF000000;
#endif

const int MSK_ALL = 0x7;
const int MSK_R = 0x1;
const int MSK_G = 0x2;
const int MSK_B = 0x4;

struct pstruct {
    int *pattern;
    int pattern_size;
    int me;
    int blinkrate;
    int mask;
    int iterations;
};

static bool Abort = false;

void blink(int rate, int mask) {
    int us = 1000 * rate;

    if ((mask & MSK_R) > 0) {
        digitalWrite(R_PIN, ON);
        usleep(us);
        digitalWrite(R_PIN, OFF);
        usleep(us);
    } else {
        digitalWrite(R_PIN, OFF);
    }

    if ((mask & MSK_G) > 0) {
        digitalWrite(G_PIN, ON);
        usleep(us);
        digitalWrite(G_PIN, OFF);
        usleep(us);
    } else {
        digitalWrite(G_PIN, OFF);
    }

    if ((mask & MSK_B) > 0) {
        digitalWrite(B_PIN, ON);
        usleep(us);
        digitalWrite(B_PIN, OFF);
    } else {
        digitalWrite(B_PIN, OFF);
    }
}

void intHandler(int dummy) {
    Abort = true;
}

void blink_row_column(int me, int brate, int mask, int iterations) {
    int rows[] = {ROW0, ROW1, ROW2, ROW3, ROW4, ROW5, ROW6, ROW7};
    int rows_size = 8;
    int columns[] = {COL0, COL1, COL2, COL3};
    int columns_size = 4;
    int root = 0;
    bool done = false;
    bool timed = true;
    int countdown = iterations;
    bool keepRunning = true;
    int blinkrate = brate;
    int data[2];
    int delayms = 6;

    MPI_Barrier(MPI_COMM_WORLD);

    if (me == 0) {

        if (iterations < 0) {
            timed = false;
        }

        while (1) {
            if (timed) {
                if (countdown == 0) {
                    keepRunning = false;
                } else {
                    countdown--;
                }
            }

            if (!keepRunning || Abort) {
                if (done) {
                    return;
                } else {
                    blinkrate = -1;
                    done = true;
                }
            }

            if (blinkrate > 0) {
                for (int i = 0; i < rows_size; i++) {
                    data[0] = blinkrate;
                    data[1] = rows[i];
                    MPI_Bcast(data, 2, MPI_INT, root, MPI_COMM_WORLD);

                    if (blinkrate > 0)
                        usleep(1000 * blinkrate * delayms);
                }


                for (int i = rows_size - 1; i > -1; i--) {
                    data[0] = blinkrate;
                    data[1] = rows[i];
                    MPI_Bcast(data, 2, MPI_INT, root, MPI_COMM_WORLD);

                    if (blinkrate > 0)
                        usleep(1000 * blinkrate * delayms);
                }


                for (int i = 0; i < columns_size; i++) {
                    data[0] = blinkrate;
                    data[1] = columns[i];

                    MPI_Bcast(data, 2, MPI_INT, root, MPI_COMM_WORLD);

                    if (blinkrate > 0)
                        usleep(1000 * blinkrate * delayms);
                }


                for (int i = columns_size - 1; i > -1; i--) {
                    data[0] = blinkrate;
                    data[1] = columns[i];

                    MPI_Bcast(data, 2, MPI_INT, root, MPI_COMM_WORLD);

                    if (blinkrate > 0)
                        usleep(1000 * blinkrate * delayms);
                }
            } else {
                data[0] = blinkrate;
                data[1] = 0;
                MPI_Bcast(data, 2, MPI_INT, root, MPI_COMM_WORLD);
            }
        }
    } else {
        while (1) {
            MPI_Bcast(data, 2, MPI_INT, root, MPI_COMM_WORLD);

            if (data[0] < 0) {
                return;
            }

            if (data[1] & (1 << (me - 1))) {
                blink(data[0], mask);
            }
        }
    }

}

void blink_all(int me, int brate, int mask, int iterations) {
    int root = 0;
    bool done = false;
    bool timed = true;
    int countdown = iterations;
    bool keepRunning = true;
    int blinkrate = brate;

    MPI_Barrier(MPI_COMM_WORLD);

    if (me == 0) {

        if (iterations < 0) {
            timed = false;
        }

        while (1) {
            if (timed) {
                if (countdown == 0) {
                    keepRunning = false;
                } else {
                    countdown--;
                }
            }

            if (!keepRunning || Abort) {
                if (done) {
                    return;
                } else {
                    blinkrate = -1;
                    done = true;
                }
            }

            MPI_Bcast(&blinkrate, 1, MPI_INT, root, MPI_COMM_WORLD);

            if (blinkrate > 0)
                usleep(1000 * blinkrate * 5);
        }
    } else {
        while (1) {
            MPI_Bcast(&blinkrate, 1, MPI_INT, root, MPI_COMM_WORLD);

            if (blinkrate < 0) {
                return;
            }

            blink(blinkrate, mask);
        }
    }
}

void chase(int me, int *pattern, int pattern_size, int brate, int mask, int iterations) {
    bool done = false;
    bool timed = true;
    int countdown = iterations;
    bool keepRunning = true;
    int blinkrate = brate;
    MPI_Status status;
    int who;

    MPI_Barrier(MPI_COMM_WORLD);

    if (me == 0) {

        if (iterations < 0) {
            timed = false;
        }

        while (1) {
            if (timed) {
                if (countdown == 0) {
                    keepRunning = false;
                } else {
                    countdown--;
                }
            }

            if (!keepRunning || Abort) {
                if (done) {
                    return;
                } else {
                    blinkrate = -1;
                    done = true;
                }
            }

            for (int i = 0; i < pattern_size; i++) {
                MPI_Send(&blinkrate, 1, MPI_INT, pattern[i], CHASE, MPI_COMM_WORLD);
                MPI_Recv(&who, 1, MPI_INT, pattern[i], CHASE, MPI_COMM_WORLD, &status);
            }
        }
    } else {
        while (1) {
            MPI_Recv(&blinkrate, 1, MPI_INT, 0, CHASE, MPI_COMM_WORLD, &status);
            if (blinkrate < 0) {
                MPI_Send(&me, 1, MPI_INT, 0, CHASE, MPI_COMM_WORLD);
                return;
            } else {
                blink(blinkrate, mask);
                MPI_Send(&me, 1, MPI_INT, 0, CHASE, MPI_COMM_WORLD);
            }
        }
    }
}

void strobe(int me, int *pattern, int pattern_size, int brate, int mask, int iterations) {
    bool done = false;
    bool timed = true;
    int countdown = iterations;
    bool keepRunning = true;
    int blinkrate = brate;
    MPI_Status status;

    MPI_Barrier(MPI_COMM_WORLD);

    if (me == 0) {
        if (iterations < 0) {
            timed = false;
        }

        while (1) {
            if (timed) {
                if (countdown == 0) {
                    keepRunning = false;
                } else {
                    countdown--;
                }
            }

            if (!keepRunning || Abort) {
                if (done) {
                    return;
                } else {
                    blinkrate = -1;
                    done = true;
                }
            }

            if (blinkrate > 0) {
                for (int i = 0; i < pattern_size; i++) {
                    MPI_Send(&blinkrate, 1, MPI_INT, pattern[i], STROBE, MPI_COMM_WORLD);

                    if (blinkrate > 0) {
                        usleep(1000 * blinkrate);
                    } else {
                        usleep(1000);
                    }
                }
            } else {
                for (int i = 0; i < stackwise_up_size; i++) {
                    MPI_Send(&blinkrate, 1, MPI_INT, stackwise_up[i], STROBE, MPI_COMM_WORLD);
                }
            }
        }
    } else {
        while (1) {
            MPI_Recv(&blinkrate, 1, MPI_INT, 0, STROBE, MPI_COMM_WORLD, &status);

            if (blinkrate < 0) {
                return;
            }

            blink(blinkrate, mask);
        }
    }
}

void *threaded_strobe(void *pattern_struct) {
    struct pstruct *p = (struct pstruct*) pattern_struct;
    strobe(p->me, p->pattern, p->pattern_size, p->blinkrate, p->mask, p->iterations);
    return NULL;
}

void multistrobe(int me, int *pattern1, int pattern1_size, int *pattern2, int pattern2_size, int brate, int mask, int iterations) {
    static struct pstruct pstruct_inner;
    static struct pstruct pstruct_outer;
    int blinkrate = brate;
    MPI_Status status;

    pthread_t outerThread, innerThread;
    pstruct_inner.me = me;
    pstruct_inner.pattern = pattern2;
    pstruct_inner.pattern_size = pattern2_size;
    pstruct_inner.blinkrate = blinkrate << 2;
    pstruct_inner.iterations = iterations;
    pstruct_inner.mask = mask;
    pstruct_outer.me = me;
    pstruct_outer.pattern = pattern1;
    pstruct_outer.pattern_size = pattern1_size;
    pstruct_outer.blinkrate = blinkrate;
    pstruct_outer.iterations = iterations;
    pstruct_outer.mask = mask;

    if (me == 0) {
        usleep(1000 * blinkrate);
        pthread_create(&outerThread, NULL, threaded_strobe, (void*) &pstruct_outer);
        usleep(1000 * blinkrate);
        pthread_create(&innerThread, NULL, threaded_strobe, (void*) &pstruct_inner);

        pthread_join(outerThread, NULL);
        pthread_join(innerThread, NULL);

    } else {
        while (1) {
            MPI_Recv(&blinkrate, 1, MPI_INT, 0, JOB_COMPLETED, MPI_COMM_WORLD, &status);

            if (blinkrate < 0) {
                return;
            }

            blink(blinkrate, mask);
        }
    }
}

void run(int me, int mode, int brate, int mask, int iterations) {
    int msk = mask;
    int i = 0;

    switch (mode) {
        case 0:
            chase(me, stackwise_up, stackwise_up_size, brate, mask, iterations);
            break;
        case 1:
            chase(me, stackwise_down, stackwise_down_size, brate, mask, iterations);
            break;
        case 2:
            chase(me, horizontal_lr, horizontal_lr_size, brate, mask, iterations);
            break;
        case 3:
            chase(me, horizontal_rl, horizontal_rl_size, brate, mask, iterations);
            break;
        case 4:
            chase(me, spiral, spiral_size, brate, mask, iterations);
            break;
        case 5:
            chase(me, zigzag, zigzag_size, brate, mask, iterations);
            break;
        case 6:
            strobe(me, stackwise_up, stackwise_up_size, brate, mask, iterations);
            break;
        case 7:
            strobe(me, stackwise_down, stackwise_down_size, brate, mask, iterations);
            break;
        case 8:
            strobe(me, horizontal_lr, horizontal_lr_size, brate, mask, iterations);
            break;
        case 9:
            strobe(me, horizontal_rl, horizontal_rl_size, brate, mask, iterations);
            break;
        case 10:
            strobe(me, spiral, spiral_size, brate, mask, iterations);
            break;
        case 11:
            strobe(me, zigzag, zigzag_size, brate, mask, iterations);
            break;
        case 12:
            blink_all(me, brate, mask, iterations);
            break;
        case 13:
            multistrobe(me, circle_outer, circle_outer_size, circle_inner_ccw, circle_inner_ccw_size, brate, mask, iterations);
            break;
        case 14:
            blink_row_column(me, brate, mask, iterations);
            break;
        default:

            if (me == 0) {
                printf("Running all...\r\n");
                fflush(stdout);
            }

            while (!Abort) {

                if (!Abort) chase(me, stackwise_up, stackwise_up_size, brate, msk, iterations);
                if (!Abort) strobe(me, stackwise_up, stackwise_up_size, brate, msk, iterations);
                if (!Abort) strobe(me, stackwise_down, stackwise_down_size, brate, msk, iterations);
                if (!Abort) strobe(me, horizontal_lr, horizontal_lr_size, brate, msk, iterations);
                if (!Abort) strobe(me, horizontal_rl, horizontal_rl_size, brate, msk, iterations);
                if (!Abort) strobe(me, spiral, spiral_size, brate, msk, iterations);
                if (!Abort) blink_all(me, brate, MSK_R, iterations * 7);
                if (!Abort) blink_all(me, brate, MSK_G, iterations * 7);
                if (!Abort) blink_all(me, brate, MSK_B, iterations * 7);
                if (!Abort) blink_all(me, brate, MSK_ALL, iterations * 7);
                if (!Abort) blink_row_column(me, brate / 2, msk, iterations);

                switch (i) {
                    case(0):
                        msk = MSK_R;
                        break;
                    case(1):
                        msk = MSK_G;
                        break;
                    case(2):
                        msk = MSK_B;
                        break;
                    default:
                        msk = mask;
                }

                i = (i + 1) % 4;
            }
    }

    digitalWrite(R_PIN, OFF);
    digitalWrite(G_PIN, OFF);
    digitalWrite(B_PIN, OFF);

    return;
}

int main(int argc, char **argv) {
    double timeStart = 0;
    double timeElapsed = 0;
    int iterations = -1;
    int blinkrate = 100;
    int mode = 12;
    int me;
    int nproc;
    int mask = MSK_ALL;

    if (argc < 2) {
        fprintf(stderr, "Usage: sudo %s <blink rate (ms)> [mode] [iterations] [mask]\n", argv[0]);
        exit(1);
    }

    sscanf(argv[1], "%d", &blinkrate);

    if (argc >= 3) {
        sscanf(argv[2], "%d", &mode);
    }

    if (argc >= 4) {
        sscanf(argv[3], "%d", &iterations);
    }

    if (argc == 5) {
        sscanf(argv[4], "%d", &mask);
    }

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &me);

    if (wiringPiSetup() == -1) {
        printf("Error opening GPIO!\n");
        fflush(stdout);
        exit(1);
    }

    pinMode(R_PIN, OUTPUT);
    digitalWrite(R_PIN, OFF);
    pinMode(G_PIN, OUTPUT);
    digitalWrite(R_PIN, OFF);
    pinMode(B_PIN, OUTPUT);
    digitalWrite(R_PIN, OFF);

    if (me == 0) {
        timeStart = MPI_Wtime();

    }

    signal(SIGINT, intHandler);

    if (me == 0) {
        printf("Blinking at %d ms on %d processors...\n", blinkrate, nproc);
        printf("Set mode: %d\n", mode);
        printf("Set iterations: %d\n", iterations);
        fflush(stdout);
    }

    run(me, mode, blinkrate, mask, iterations);

    if (me == 0) {
        timeElapsed = MPI_Wtime() - timeStart;
        printf("Elapsed time:  %lf seconds\n", timeElapsed);
        fflush(stdout);
    }

    MPI_Finalize();
    exit(0);
}
