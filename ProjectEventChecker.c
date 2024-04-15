/*
 * File:   ProjectEventChecker.c
 * Author: Alex Zuo
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with both FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This file includes a test harness that will run the event detectors listed in the
 * ES_Configure file in the project, and will conditionally compile main if the macro
 * EVENTCHECKER_TEST is defined (either in the project or in the file). This will allow
 * you to check you event detectors in their own project, and then leave them untouched
 * for your project unless you need to alter their post functions.
 *
 * Created on September 27, 2013, 8:37 AM
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

//!!!!!!!!!!!!!!! PORT Z10 IS BADDDDDDDD !!!!!!!!!!!!!!!!!!!!

#include "ES_Configure.h"
#include "ProjectEventChecker.h"
#include "ES_Events.h"
#include "serial.h"
#include "AD.h"
#include "Bot.h"
#include <BOARD.h>
//#include <xc.h>
#include <pwm.h>
#include <serial.h>
//#include <peripheral/ports.h>
#include "IO_Ports.h"
#include "LED.h"
#include "RC_Servo.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define DELAY(x)    for (wait = 0; wait <= x; wait++) {asm("nop");}

//Sensor and Detector Defines
//was 700
//was 680. Worked pretty nicely.
//670 was a bit off.
//was 675
#define TRACK_WIRE_FOUND_THRESHOLD 650
//Was 550 before.
//was changed to 600.
//550 before.
//was 630
#define NO_TRACK_WIRE_THRESHOLD 600
//Was 600 originally
#define BEACON_DETECTED_THRESHOLD 700
#define BEACON_DISCONNECTED_THRESHOLD 500

//Tape Define
//all of these no longer matter as we are using digital signals now.
#define TAPE_DOWN_LOW_THRESHOLD 300
#define TAPE_DOWN_HIGH_THRESHOLD 600
#define TAPE_FORWARD_LOW_THRESHOLD 300
#define TAPE_FORWARD_HIGH_THRESHOLD 650

//Test function defines

/*******************************************************************************
 * EVENTCHECKER_TEST SPECIFIC CODE                                                             *
 ******************************************************************************/

//#define EVENTCHECKER_TEST
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
#define SaveEvent(x) do {eventName=__func__; storedEvent=x;} while (0)

static const char *eventName;
static ES_Event storedEvent;
#endif

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this EventChecker. They should be functions
   relevant to the behavior of this particular event checker */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

/* Any private module level variable that you might need for keeping track of
   events would be placed here. Private variables should be STATIC so that they
   are limited in scope to this module. */

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function WireSensorChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the Track Wire Sensor
 *        against a hysteresis threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the actual wire voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either WIREFOUND_EVENT or NOWIRE_EVENT if the track wire is detected.
 *        Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo, 2019.11.13 04:49 pm
 **/
uint8_t WireSensorChecker(void) {
    static ES_EventTyp_t wireLastEvent = NO_TRACK_WIRE_EVENT;
    ES_EventTyp_t wireCurEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    //Read the Track Wire Sensor output
    uint16_t wireVoltage = Bot_ReadTrackWireVoltage();
    
    unsigned int wait = 0;
    //DELAY(100000)
    //printf("Wire Voltage is : %d\r\n", wireVoltage);
    
    //Check the wire voltage to see if track wire is triggered. There are two hysteresis bounds.
    if ((wireLastEvent == NO_TRACK_WIRE_EVENT) && (wireVoltage > TRACK_WIRE_FOUND_THRESHOLD)) { // is wire close enough?
        wireCurEvent = TRACK_WIRE_FOUND_EVENT;
    } else if ((wireLastEvent == TRACK_WIRE_FOUND_EVENT) && (NO_TRACK_WIRE_THRESHOLD > wireVoltage)){
        wireCurEvent = NO_TRACK_WIRE_EVENT;
    } else{
        wireCurEvent = wireLastEvent;
    }
    if (wireCurEvent != wireLastEvent) { // check for change from last time
        thisEvent.EventType = wireCurEvent;
        if (wireCurEvent == TRACK_WIRE_FOUND_EVENT)
            printf("\r\nEvent: TRACK_WIRE_FOUND_EVENT  Param: %d", wireVoltage);
        if (wireCurEvent == NO_TRACK_WIRE_EVENT)
            printf("\r\nEvent: NO_TRACK_WIRE_EVENT  Param: %d", wireVoltage);
        //Event Parameter equals the track wire sensor voltage.
        thisEvent.EventParam = wireVoltage;
        returnVal = TRUE;
        wireLastEvent = wireCurEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        //WireSensorChecker(void);
        PostProjectHSM(thisEvent);
#else
        SaveEvent(thisEvent);
        //PostTemplateService(thisEvent);
#endif   
    }
    return (returnVal);
}


uint8_t BeaconDetectorChecker(void) {
    static ES_EventTyp_t beaconLastEvent = NO_BEACON_EVENT;
    ES_EventTyp_t beaconCurEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    //Read the Beacon Detector output
    uint16_t beaconVoltage = Bot_ReadBeaconVoltage();
    //printf("Beacon Detector Voltage is : %d\r\n", beaconVoltage);
    
    //Check the beacon detector voltage to see if the beacon been detected. There are two hysteresis bounds.
    if ((beaconLastEvent == NO_BEACON_EVENT) && (beaconVoltage > BEACON_DETECTED_THRESHOLD)) { // is wire close enough?
        beaconCurEvent = BEACON_FOUND_EVENT;
    } else if ((beaconLastEvent == BEACON_FOUND_EVENT) && (BEACON_DISCONNECTED_THRESHOLD > beaconVoltage)){
        beaconCurEvent = NO_BEACON_EVENT;
    } else{
        beaconCurEvent = beaconLastEvent;
    }
    if (beaconCurEvent != beaconLastEvent) { // check for change from last time
        thisEvent.EventType = beaconCurEvent;
        if (beaconCurEvent == BEACON_FOUND_EVENT)
            printf("\r\nEvent: BEACON_FOUND_EVENT  Param: %d", beaconVoltage);
        if (beaconCurEvent == NO_BEACON_EVENT)
            printf("\r\nEvent: NO_BEACON_EVENT  Param: %d", beaconVoltage);
        //Event Parameter equals the Beacon Detector voltage.
        thisEvent.EventParam = beaconVoltage;
        returnVal = TRUE;
        beaconLastEvent = beaconCurEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        //BeaconDetectorChecker(void);
        PostProjectHSM(thisEvent);
#else
        SaveEvent(thisEvent);
        //PostTemplateService(thisEvent);
#endif   
    }
    return (returnVal);
}

uint8_t FLTapeChecker(void) {
    static ES_EventTyp_t FLTapeLastEvent = FL_TAPE_SEE_BLACK_EVENT;
    ES_EventTyp_t FLTapeCurEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    //Read the FL Tape Sensor output
    uint16_t FLTapeVoltage = Bot_ReadFLTapeVoltage();
    //printf("FL Tape Voltage is : %d\r\n", FLTapeVoltage);
    
    //Check the front left tape voltage to see if the tape sensor is triggered. There are two hysteresis bounds.
    if ((FLTapeLastEvent == FL_TAPE_SEE_BLACK_EVENT) && (FLTapeVoltage < TAPE_DOWN_LOW_THRESHOLD)) { // is wire close enough?
        FLTapeCurEvent = FL_TAPE_SEE_WHITE_EVENT;
        //printf("FL Tape Voltage is : %d\r\n", FLTapeVoltage);
    } else if ((FLTapeLastEvent == FL_TAPE_SEE_WHITE_EVENT) && (TAPE_DOWN_HIGH_THRESHOLD < FLTapeVoltage)){
        FLTapeCurEvent = FL_TAPE_SEE_BLACK_EVENT;
        //printf("FL Tape Voltage is : %d\r\n", FLTapeVoltage);
    } else{
        FLTapeCurEvent = FLTapeLastEvent;
    }
    if (FLTapeCurEvent != FLTapeLastEvent) { // check for change from last time
        thisEvent.EventType = FLTapeCurEvent;
        if (FLTapeCurEvent == FL_TAPE_SEE_BLACK_EVENT)
            printf("\r\nEvent: FL_TAPE_SEE_BLACK_EVENT  Param: %d", FLTapeVoltage);
        if (FLTapeCurEvent == FL_TAPE_SEE_WHITE_EVENT)
            printf("\r\nEvent: FL_TAPE_SEE_WHITE_EVENT  Param: %d", FLTapeVoltage);
        //Event Parameter equals the FL Tape sensor voltage.
        thisEvent.EventParam = FLTapeVoltage;
        returnVal = TRUE;
        FLTapeLastEvent = FLTapeCurEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        //FLTapeChecker(void);
        PostProjectHSM(thisEvent);
#else
        SaveEvent(thisEvent);
        //PostTemplateService(thisEvent);
#endif   
    }
    return (returnVal);
}

uint8_t FRTapeChecker(void) {
    static ES_EventTyp_t FRTapeLastEvent = FR_TAPE_SEE_BLACK_EVENT;
    ES_EventTyp_t FRTapeCurEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    //Read the FR Tape Sensor output
    uint16_t FRTapeVoltage = Bot_ReadFRTapeVoltage();
    //printf("\rFR Tape Voltage is : %d\n", FRTapeVoltage);
    
    //Check the front right tape voltage to see if the tape sensor is triggered. There are two hysteresis bounds.
    if ((FRTapeLastEvent == FR_TAPE_SEE_BLACK_EVENT) && (FRTapeVoltage < TAPE_DOWN_LOW_THRESHOLD)) { // is wire close enough?
        FRTapeCurEvent = FR_TAPE_SEE_WHITE_EVENT;
    } else if ((FRTapeLastEvent == FR_TAPE_SEE_WHITE_EVENT) && (TAPE_DOWN_HIGH_THRESHOLD < FRTapeVoltage)){
        FRTapeCurEvent = FR_TAPE_SEE_BLACK_EVENT;
    } else{
        FRTapeCurEvent = FRTapeLastEvent;
    }
    if (FRTapeCurEvent != FRTapeLastEvent) { // check for change from last time
        thisEvent.EventType = FRTapeCurEvent;
        if (FRTapeCurEvent == FR_TAPE_SEE_BLACK_EVENT)
            printf("\r\nEvent: FR_TAPE_SEE_BLACK_EVENT  Param: %d", FRTapeVoltage);
        if (FRTapeCurEvent == FR_TAPE_SEE_WHITE_EVENT)
            printf("\r\nEvent: FR_TAPE_SEE_WHITE_EVENT  Param: %d", FRTapeVoltage);
        //Event Parameter equals the FR Tape sensor voltage.
        thisEvent.EventParam = FRTapeVoltage;
        returnVal = TRUE;
        FRTapeLastEvent = FRTapeCurEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        //FRTapeChecker(void);
        PostProjectHSM(thisEvent);
#else
        SaveEvent(thisEvent);
        //PostTemplateService(thisEvent);
#endif   
    }
    return (returnVal);
}

uint8_t BCTapeChecker(void) {
    static ES_EventTyp_t BCTapeLastEvent = BC_TAPE_SEE_BLACK_EVENT;
    ES_EventTyp_t BCTapeCurEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    //Read the BC Tape Sensor output
    uint16_t BCTapeVoltage = Bot_ReadBCTapeVoltage();
    //printf("\rBC Tape Voltage is : %d\n", BCTapeVoltage);
    
    //Check the Back Center tape voltage to see if the tape sensor is triggered. There are two hysteresis bounds.
    if ((BCTapeLastEvent == BC_TAPE_SEE_BLACK_EVENT) && (BCTapeVoltage < TAPE_DOWN_LOW_THRESHOLD)) { // is wire close enough?
        BCTapeCurEvent = BC_TAPE_SEE_WHITE_EVENT;
    } else if (BCTapeLastEvent == BC_TAPE_SEE_WHITE_EVENT && (TAPE_DOWN_HIGH_THRESHOLD < BCTapeVoltage)){
        BCTapeCurEvent = BC_TAPE_SEE_BLACK_EVENT;
    } else{
        BCTapeCurEvent = BCTapeLastEvent;
    }
    if (BCTapeCurEvent != BCTapeLastEvent) { // check for change from last time
        thisEvent.EventType = BCTapeCurEvent;
        if (BCTapeCurEvent == BC_TAPE_SEE_BLACK_EVENT)
            printf("\r\nEvent: BC_TAPE_SEE_BLACK_EVENT  Param: %d", BCTapeVoltage);
        if (BCTapeCurEvent == BC_TAPE_SEE_WHITE_EVENT)
            printf("\r\nEvent: BC_TAPE_SEE_WHITE_EVENT  Param: %d", BCTapeVoltage);
        //Event Parameter equals the BC Tape sensor voltage.
        thisEvent.EventParam = BCTapeVoltage;
        returnVal = TRUE;
        BCTapeLastEvent = BCTapeCurEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        //BCTapeChecker(void);
        PostProjectHSM(thisEvent);
#else
        SaveEvent(thisEvent);
        //PostTemplateService(thisEvent);
#endif   
    }
    return (returnVal);
}

uint8_t LeftBallTapeChecker(void) {
    static ES_EventTyp_t LeftBallTapeLastEvent = L_BALL_TAPE_SEE_BLACK_EVENT;
    ES_EventTyp_t LeftBallTapeCurEvent;
    ES_Event thisEvent;
    uint8_t returnVal = FALSE;
    
    //Read the Left Ball Tape Sensor output
    uint16_t LeftBallTapeVoltage = Bot_ReadLeftBallTapeVoltage();
    //printf("\rLeft Ball Tape Voltage is : %d\n", LeftBallTapeVoltage);
    
    //Check the left ball tape voltage to see if the tape sensor is triggered. There are two hysteresis bounds.
    if (LeftBallTapeLastEvent == L_BALL_TAPE_SEE_BLACK_EVENT && (LeftBallTapeVoltage < TAPE_FORWARD_LOW_THRESHOLD)) { // is wire close enough?
        LeftBallTapeCurEvent = L_BALL_TAPE_SEE_WHITE_EVENT;
    } else if (LeftBallTapeLastEvent == L_BALL_TAPE_SEE_WHITE_EVENT && (TAPE_FORWARD_HIGH_THRESHOLD < LeftBallTapeVoltage)){
        LeftBallTapeCurEvent = L_BALL_TAPE_SEE_BLACK_EVENT;
    } else{
        LeftBallTapeCurEvent = LeftBallTapeLastEvent;
    }
    if (LeftBallTapeCurEvent != LeftBallTapeLastEvent) { // check for change from last time
        thisEvent.EventType = LeftBallTapeCurEvent;
        if (LeftBallTapeCurEvent == L_BALL_TAPE_SEE_BLACK_EVENT)
            printf("\r\nEvent: L_BALL_TAPE_SEE_BLACK_EVENT  Param: %d", LeftBallTapeVoltage);
        if (LeftBallTapeCurEvent == L_BALL_TAPE_SEE_WHITE_EVENT)
            printf("\r\nEvent: L_BALL_TAPE_SEE_WHITE_EVENT  Param: %d", LeftBallTapeVoltage);
        //Event Parameter equals the BC Tape sensor voltage.
        thisEvent.EventParam = LeftBallTapeVoltage;
        returnVal = TRUE;
        LeftBallTapeLastEvent = LeftBallTapeCurEvent; // update history
#ifndef EVENTCHECKER_TEST           // keep this as is for test harness
        //LeftBallTapeChecker(void);
        PostProjectHSM(thisEvent);
#else
        SaveEvent(thisEvent);
        //PostTemplateService(thisEvent);
#endif   
    }
    return (returnVal);
}

/* 
 * The Test Harness for the event checkers is conditionally compiled using
 * the EVENTCHECKER_TEST macro (defined either in the file or at the project level).
 * No other main() can exist within the project.
 * 
 * It requires a valid ES_Configure.h file in the project with the correct events in 
 * the enum, and the correct list of event checkers in the EVENT_CHECK_LIST.
 * 
 * The test harness will run each of your event detectors identically to the way the
 * ES_Framework will call them, and if an event is detected it will print out the function
 * name, event, and event parameter. Use this to test your event checking code and
 * ensure that it is fully functional.
 * 
 * If you are locking up the output, most likely you are generating too many events.
 * Remember that events are detectable changes, not a state in itself.
 * 
 * Once you have fully tested your event checking code, you can leave it in its own
 * project and point to it from your other projects. If the EVENTCHECKER_TEST marco is
 * defined in the project, no changes are necessary for your event checkers to work
 * with your other projects.
 */
#ifdef EVENTCHECKER_TEST
#include <stdio.h>
static uint8_t(*EventList[])(void) = {EVENT_CHECK_LIST};

void PrintEvent(void);

void main(void) {
    BOARD_Init();
    // user initialization code goes here 
    Bot_Init();
    // Do not alter anything below this line
    int i;

    printf("\r\nEvent checking test harness for %s", __FILE__);

    while (1) {
        if (IsTransmitEmpty()) {
            for (i = 0; i< sizeof (EventList) >> 2; i++) {
                if (EventList[i]() == TRUE) {
                    PrintEvent();
                    break;
                }

            }
        }
    }
}
void PrintEvent(void) {
    printf("\r\nFunc: %s\tEvent: %s\tParam: 0x%X", eventName,
            EventNames[storedEvent.EventType], storedEvent.EventParam);
}
#endif