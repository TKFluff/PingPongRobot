/* 
 * File:   Project_Main.c
 * Author: ahzuo
 *
 * Created on November 12, 2019, 5:38 PM
 */

//!!!!!!!!!!!!!!! PORT Z10 IS BADDDDDDDD !!!!!!!!!!!!!!!!!!!!

#include <stdio.h>
#include <stdlib.h>
#include <BOARD.h>
#include <pwm.h>
#include <AD.h>
#include <Bot.h>

/*
 * 
 */
int main(void) {
    
    //!!!!!!!!!!!!!!! PORT Z10 IS BADDDDDDDD !!!!!!!!!!!!!!!!!!!!
    
    BOARD_Init();
    Bot_Init();
    LED_Init();
    while (1){
        
    }
    
}

