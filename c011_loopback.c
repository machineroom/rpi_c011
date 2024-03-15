/*
  Test C011 (mode2) loopback. Loop LinkIn-LinkOut on C011
 */

#define _POSIX_C_SOURCE 200809L

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <stdbool.h>
#include <time.h>

#include "c011.h"

static uint64_t get_ms() {
    struct timespec spec;
    clock_gettime (CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000 + spec.tv_nsec/1000000;
}

int test_all_byte_values(void) {
    int ret;
    uint8_t i=0;
    uint8_t read;
    int count=0;
    bool good=true;
    bool timeout = false;
    while (true) {
        c011_reset();
        good = true;
        timeout = false;
        for (count=0; count < 100000 && good && !timeout; count++) {
            ret = c011_write_byte(i,200);
            if (ret == -1) {
                printf ("write timeout\n");
                c011_set_byte_mode();
                timeout = true;
                break;
            }
            ret = c011_read_byte(&read, 200);
            if (ret == -1) {
                printf ("read timeout\n");
                c011_set_byte_mode();
                timeout = true;
                break;
            } else {        
                if (read != i) {
                    good = false;
                    break;
                } else {
                }
            }
        }
        if (timeout) {
            printf ("0x%X T after %d iterations\n",i,count);
            c011_set_byte_mode();
        } else if (!good) {
            printf ("0x%X X after %d iterations\n",i,count);
            c011_set_byte_mode();
        }
        else {
            printf ("0x%X OK\n", i);
        }
        i++;
    }
    return 0;
}

int test_single_byte_value (uint8_t val) {
    int ret;
    uint8_t read;
    int count=0;
    while (true) {
        printf ("W 0x%X ", val);
        ret = c011_write_byte(val,200);
        if (ret == -1) {
            printf ("write timeout\n");
            break;
        }
        ret = c011_read_byte(&read, 200);
        if (ret == -1) {
            printf ("read timeout\n");
            break;
        } else {
            printf ("R 0x%X ", read);
            if (read != val) {
                printf ("*E* write=0x%X read=0x%X\n",val,read);
                break;
            } else {
                printf ("OK %d\n", count++);
            }
        }
    }
    return 0;
}

int test_alternating_byte_value (uint8_t val) {
    int ret;
    uint8_t read;
    int count=0;
    uint64_t start;
    start = get_ms();
    while (true) {
        ret = c011_write_byte(val,200);
        if (ret == -1) {
            c011_set_byte_mode();
            printf ("write timeout\n");
            break;
        }
        ret = c011_read_byte(&read, 200);
        if (ret == -1) {
            c011_set_byte_mode();
            printf ("read timeout\n");
            break;
        } else {
            if (read != val) {
                c011_set_byte_mode();
                printf ("*E* write=0x%X read=0x%X\n",val,read);
                break;
            }
            count++;
            if (count%1000000==0) {
                uint64_t now = get_ms();
                //1M bytes *8 for bits * 2 for Tx & Rx
                double mbits = 8.0f * 2.0f;
                double per_second = (double)1000.0f/(now-start);
                printf ("%0.1f Mbits/second (total %dM bytes)\n",per_second*mbits, count/1000000);
                start = now;
            }
        }
        val = ~val;
    }
    return 0;
}

int test_random_byte_values (void) {
    int ret;
    uint8_t read;
    int count=0;
    uint64_t start;
    start = get_ms();

    while (true) {
        uint8_t val = rand();
        ret = c011_write_byte(val,200);
        if (ret == -1) {
            c011_set_byte_mode();
            printf ("write timeout, count %d\n", count);
            break;
        }
        ret = c011_read_byte(&read, 200);
        if (ret == -1) {
            c011_set_byte_mode();
            printf ("read timeout\n");
            break;
        } else {        
            if (read != val) {
                c011_set_byte_mode();
                printf ("*E* write=0x%X read=0x%X count=%d\n",val,read,count);
                break;
            } else {
            }
        }
        count++;
        if (count%1000000==0) {
            uint64_t now = get_ms();
            //1M bytes *8 for bits * 2 for Tx & Rx
            double mbits = 8.0f * 2.0f;
            double per_second = (double)1000.0f/(now-start);
            printf ("%0.1f Mbits/second (total %dM bytes)\n",per_second*mbits, count/1000000);
            start = now;
        }
    }
    return 0;
}

int test_perf (void) {
    int ret;
    uint8_t read;
    int count=0;
    uint64_t start;
    start = get_ms();

    uint8_t val = 0x55;
    while (true) {
        ret = c011_write_byte(val,200);
        if (ret == -1) {
            c011_set_byte_mode();
            printf ("write timeout\n");
            break;
        }
        ret = c011_read_byte(&read, 0);
        if (ret == -1) {
            c011_set_byte_mode();
            printf ("read timeout\n");
            break;
        } else {        
            if (read != val) {
                c011_set_byte_mode();
                printf ("*E* write=0x%X read=0x%X\n",val,read);
                break;
            } else {
            }
        }
        count++;
        if (count%1000000==0) {
            uint64_t now = get_ms();
            //1M bytes *8 for bits * 2 for Tx & Rx
            double mbits = 8.0f * 2.0f;
            double per_second = (double)1000.0f/(now-start);
            printf ("%0.1f Mbits/second (total %dM bytes)\n",per_second*mbits, count/1000000);
            c011_dump_stats("loop");
            start = now;
        }
    }
    return 0;
}
int main(int argc, char *argv[])
{
    c011_init();
    c011_reset();

    c011_clear_byte_mode();
    //test_perf();
    //test_random_byte_values();
    //test_alternating_byte_value(0x66);
    test_all_byte_values();
    //test_single_byte_value(0x40);
}
