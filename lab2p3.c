#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "hps_0.h"

#define LW_BRIDGE_BASE 0xff200000
#define LW_BRIDGE_SPAN 0x00200000

void *base;
uint32_t *led;
uint32_t *button;
uint32_t *hex03;
uint32_t *hex45;
uint32_t *switchx10;
int fd;

void handler(int signo) {
    *led = 0;  // Turn off LED
    *hex03=0;
    *hex45=0;
    munmap(base, LW_BRIDGE_SPAN);  // Unmap memory
    close(fd);  // Close the file descriptor
    exit(0);  // Exit the program
}

int main() {
    int counter=0;
    int f=0;
    int dd=0,ss=0,mm=0;
    int tendd=0,tenss=0,onedd=0,oness=0;
    uint8_t seven_seg_encoding[] = {
        0xC0,  // 0
        0xF9,  // 1
        0xA4,  // 2
        0xB0,  // 3
        0x99,  // 4
        0x92,  // 5
        0x82,  // 6
        0xF8,  // 7
        0x80,  // 8
        0x90   // 9
    };
    fd = open("/dev/mem", O_RDWR | O_SYNC);  // Open memory for access
    if (fd < 0) {
        printf("Can't open memory.\n");
        return -1;
    }

    base = mmap(NULL, LW_BRIDGE_SPAN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, LW_BRIDGE_BASE);
    if (base == MAP_FAILED) {
        printf("Can't map memory.\n");
        close(fd);
        return -1;
    }

    led = (uint32_t *)(base + LED_BASE);  // Map LED address
    button = (uint32_t *)(base + BUTTON_BASE);  // Map Button address
    hex03 =(uint32_t *)(base+ HEX03_BASE);
    hex45= (uint32_t *)(base + HEX45_BASE);
    switchx10 = (uint32_t *)(base + SWITCHES_BASE);
    uint32_t prev_button_state = *button & 0x1;
    int ten=0;
    int one=0;
    signal(SIGINT, handler);  // Set up signal handler for CTRL+C to clean up
    *hex03=(seven_seg_encoding[0] << 24) |(seven_seg_encoding[0] << 16) |(seven_seg_encoding[0] << 8)  | seven_seg_encoding[0];
    *hex45=(seven_seg_encoding[0]<<8) | seven_seg_encoding[0];
    *led = 0;  // Initially turn off the LED
    while (1) {
        uint32_t button_state = *button;
        uint32_t button0_state = *button & 0x1;
        uint32_t switch0= *switchx10;

        *led=0;
        
         if (button0_state==0 && prev_button_state ==1) {  // Button 0 pressed (toggle start/pause)
            f = !f;  // Toggle the start/pause flag
        *led = (f == 1) ? 0x1 : 0x0;  // Turn on/off LED 0 based on `f`
        }
        prev_button_state=button0_state;
        if (f == 1) {  // Timer is running
            usleep(10000);  // Control the speed of decrement
            dd--;  // Decrement hundredths of a second
            if (dd < 0) {
                dd = 99;
                ss--;  // Decrement seconds
                if (ss < 0) {
                    ss = 59;
                    mm--;  // Decrement minutes
                    if (mm < 0) {  // Timer ends at 00:00.00
                        mm = 0;
                        ss = 0;
                        dd = 0;
                    }
                }
            }
            tenss=ss/10;
            oness=ss%10;
            tendd=dd/10;
            onedd=dd%10;
            ten=mm/10;
            one=mm%10;
            *hex03=( seven_seg_encoding[tenss]<< 24) | (seven_seg_encoding[oness]<< 16)|( seven_seg_encoding[tendd]<< 8) | (seven_seg_encoding[onedd]);
            *hex45=( seven_seg_encoding[ten]<< 8) | (seven_seg_encoding[one]);
        }
        if (!(button_state & 0x2)) {  // Button 1 pressed (DD)
            if(switch0>99){
                tendd=9;
                onedd=9;
                dd=99;
            }
            else{
                tendd=switch0/10;
                onedd=switch0%10;
                dd=tendd*10+onedd;
            }
            *led = 0x2;  // Turn on LED
            *hex03=( seven_seg_encoding[tenss]<< 24) | (seven_seg_encoding[oness]<< 16)|( seven_seg_encoding[tendd]<< 8) | (seven_seg_encoding[onedd]);
        }
        if (!(button_state & 0x4)) {  // Button 2 pressed (SS)
            ///ss=ten+one;
            if(switch0>59){
                tenss=5;
                oness=9;
                ss=59;
            }
            else{
                tenss=switch0/10;
                oness=switch0%10;
                ss=tenss*10+oness;
            }
            *led = 0x4;  // Turn on LED 2
           *hex03=( seven_seg_encoding[tenss]<< 24) | (seven_seg_encoding[oness]<< 16)|( seven_seg_encoding[tendd]<< 8) | (seven_seg_encoding[onedd]);
        }
        if (!(button_state & 0x8)) {  // Button 3 pressed (MM)
            
            if(switch0>59){
                ten=5;
                one=9;
                mm=59;
            }
            else{
                ten=switch0/10;
                one=switch0%10;
                mm=ten+one;
            }
            *led = 0x8;  // Turn on LED 3
            
        *hex45=( seven_seg_encoding[ten]<< 8) | (seven_seg_encoding[one]);
        }
        
    }

    return 0;
}