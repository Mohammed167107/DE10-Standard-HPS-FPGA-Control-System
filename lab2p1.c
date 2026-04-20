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
    int f=1;
    uint8_t seven_seg_encoding[] = {
        0xFB,   /// i
        0xAB,   /// n
        0x07,   /// t
        0x06,   /// e
        0xCF,   /// l
        0x92,   /// s
        0xA3,   /// o
        0xA7,   /// c
        0x8E,   /// F
        0x8C,   /// P
        0x82,   /// G
        0x88,   /// A
        0xFF    /// space (screen off)
    };
    uint32_t prev_button_state = 1;
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

    signal(SIGINT, handler);  // Set up signal handler for CTRL+C to clean up
    *hex03=(seven_seg_encoding[12] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[12];
    *hex45=(seven_seg_encoding[12]<<8) | seven_seg_encoding[12];
    *led = 0;  // Initially turn off the LED
    usleep(300000);
    while (1) {
        uint32_t button0_state = *button & 0x1;  // Read the state of button0 (lowest bit)
        uint32_t switch0= *switchx10 & 0x1;

        /// intel soc FPGA
        if(switch0){
            *led=0x5;
        }
        else{
            *led=0x0;
        }
        if (button0_state == 1) {
            f=0;
            if(f==0){
                if(counter>19){
                    usleep(300000);
                    counter=0;
                }
                else{
                    usleep(300000);
                    counter++; 
                }
            }
        }
        else{
            f=1;
        }
        if(counter==0){
            *hex45 =(seven_seg_encoding[0]<<8) | seven_seg_encoding[1];
            *hex03 =(seven_seg_encoding[2] << 24) |(seven_seg_encoding[3] << 16) |(seven_seg_encoding[4] << 8)  | seven_seg_encoding[12]; //intel_
        }
        
        if(counter==1){
            *hex45 =(seven_seg_encoding[1]<<8) | seven_seg_encoding[2];
            *hex03 =(seven_seg_encoding[3] << 24) |(seven_seg_encoding[4] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[5];  //ntel_s
        }

        if(counter==2){
            *hex45 =(seven_seg_encoding[2]<<8) | seven_seg_encoding[3];
            *hex03 =(seven_seg_encoding[4] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[5] << 8)  | seven_seg_encoding[6];  //tel_so
        }
        if(counter==3){
            *hex45 =(seven_seg_encoding[3]<<8) | seven_seg_encoding[4];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[5] << 16) |(seven_seg_encoding[6] << 8)  | seven_seg_encoding[7];  //el_soc
        }
        if(counter==4){
            *hex45 =(seven_seg_encoding[4]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[5] << 24) |(seven_seg_encoding[6] << 16) |(seven_seg_encoding[7] << 8)  | seven_seg_encoding[12];  //l_soc_
        }
        if(counter==5){    
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[5];
            *hex03 =(seven_seg_encoding[6] << 24) |(seven_seg_encoding[7] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[8];  //_soc_F
        }
        if(counter==6){
            *hex45 =(seven_seg_encoding[5]<<8) | seven_seg_encoding[6];
            *hex03 =(seven_seg_encoding[7] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[8] << 8)  | seven_seg_encoding[9];  //soc_FP
        }
        if(counter==7){
            *hex45 =(seven_seg_encoding[6]<<8) | seven_seg_encoding[7];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[8] << 16) |(seven_seg_encoding[9] << 8)  | seven_seg_encoding[10];  //oc_FPG
        }
        if(counter==8){
            *hex45 =(seven_seg_encoding[7]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[8] << 24) |(seven_seg_encoding[9] << 16) |(seven_seg_encoding[10] << 8)  | seven_seg_encoding[11];  //c_FPGA
        }
        if(counter==9){
             *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[8];
             *hex03 =(seven_seg_encoding[9] << 24) |(seven_seg_encoding[10] << 16) |(seven_seg_encoding[11] << 8)  | seven_seg_encoding[12];  //_FPGA_
        }
        if(counter==10){
            *hex45 =(seven_seg_encoding[8]<<8) | seven_seg_encoding[9];
            *hex03 =(seven_seg_encoding[10] << 24) |(seven_seg_encoding[11] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[12];  //FPGA__
        }
        if(counter==11){
            *hex45 =(seven_seg_encoding[9]<<8) | seven_seg_encoding[10];
            *hex03 =(seven_seg_encoding[11] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[12];  //PGA___
        }
        if(counter==12){
            *hex45 =(seven_seg_encoding[10]<<8) | seven_seg_encoding[11];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[12];  //GA____
        }
        if(counter==13){
            *hex45 =(seven_seg_encoding[11]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[12];  //A_____
        }
        if(counter==14){
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[12];  //______
        }
        if(counter==15){
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[12] << 8)  | seven_seg_encoding[0];  //_____i
        }
        if(counter==16){
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[12] << 16) |(seven_seg_encoding[0] << 8)  | seven_seg_encoding[1];  //____in
        }
        if(counter==17){
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[12] << 24) |(seven_seg_encoding[0] << 16) |(seven_seg_encoding[1] << 8)  | seven_seg_encoding[2];  //___int
        }
        if(counter==18){
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[12];
            *hex03 =(seven_seg_encoding[0] << 24) |(seven_seg_encoding[1] << 16) |(seven_seg_encoding[2] << 8)  | seven_seg_encoding[3];  //__inte
        }
        if(counter==19){
            *hex45 =(seven_seg_encoding[12]<<8) | seven_seg_encoding[0];
            *hex03 =(seven_seg_encoding[1] << 24) |(seven_seg_encoding[2] << 16) |(seven_seg_encoding[3] << 8)  | seven_seg_encoding[4];  //_intel
        }

        ///counter = (counter + 1) % 4;  // Wrap counter after 3
        ///if (button0_state == 0 && prev_button_state==1) {
        ///    ///*led = counter;
        ///    ///*hex03=(seven_seg_encoding[0])|(seven_seg_encoding[1])|(seven_seg_encoding[2])|seven_seg_encoding[3];
        ///    ///counter=counter+1;
    ///
        ///      // Light up the LED if button0 is pressed
        ///}
        ///if(counter>9){
        ///    counter=0;
        ///    //*hex=seven_seg_encoding[counter];
        ///}
        ///prev_button_state=button0_state;
        
    }

    return 0;
}