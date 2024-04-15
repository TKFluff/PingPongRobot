/*
 * File: WallFollowingSubHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "BOARD.h"
#include "ProjectHSM.h"
#include "WallFollowingSubHSM.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Forward,
    ForwardLeft,
    WallHardLeft,
    CornerHardLeft,
    BackUpLeft,
    TrackBackUpLeft1,
    TrackBackUpLeft2,
    TrackBackUpLeft3,        
    TrackForwardLeft1,
    TrackForwardLeft2,
    TrackForwardLeft3,        
    TrackWallHardLeft1,
    TrackWallHardLeft2,
    TrackWallHardLeft3,        
    WallFound,
} WallFollowingSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Forward",
	"ForwardLeft",
	"WallHardLeft",
	"CornerHardLeft",
	"BackUpLeft",
	"TrackBackUpLeft1",
	"TrackBackUpLeft2",
	"TrackBackUpLeft3",
	"TrackForwardLeft1",
	"TrackForwardLeft2",
	"TrackForwardLeft3",
	"TrackWallHardLeft1",
	"TrackWallHardLeft2",
	"TrackWallHardLeft3",
	"WallFound",
};

//Include any defines you need to do
//was 375 - 6:50pm
//was 500 7:11pm
#define WALL_FOLLOW_BACK_UP_LEFT_TICKS 400
//was 350
//was 1000 7:11 pm
#define WALL_FOLLOW_FORWARD_LEFT_TICKS 800
//was 1750 - 6:50pm
//1500 7:11pm
#define WALL_FOLLOW_HARD_LEFT_TICKS 1500

//These two summed up should equal wall follow hard left ticks.
#define WALL_HARD_LEFT_TICKS 900
#define CORNER_HARD_LEFT_TICKS 600

#define TRACK_WIRE_THRESHOLD_NEW 590
// 3:55 pm 12/12/19 ----- Changed from 485 to 1000
#define WALL_ONE_FORWARD_TICKS 140
#define WALL_ONE_BACK_UP_TICKS 300

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static WallFollowingSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;
static int wireCounter = 0;

static int TrackWireReadOne;
static int TrackWireReadTwo;
static int TrackWireReadThree;
static int TrackWireReadFour;
static int TrackWireReadFive;
static int TrackWireReadSix;
static int TrackWireReadSeven;
static int TrackWireReadEight;
static int TrackWireReadAverage;
static int StateCount;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitWallFollowingSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunWallFollowingSubHSM(INIT_EVENT);
    if (returnEvent.EventType == ES_NO_EVENT) {
        return TRUE;
    }
    return FALSE;
}

/**
 * @Function RunTemplateSubHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunWallFollowingSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    WallFollowingSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Pseudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = BackUpLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                wireCounter = 0;
            }
            break;

        case BackUpLeft:
            if (ThisEvent.EventType == ES_ENTRY){
                TurnSharpLeft(-90);
                
                TrackWireReadOne = 0;
                TrackWireReadTwo = 0;
                TrackWireReadThree = 0;
                TrackWireReadFour = 0;
                TrackWireReadFive = 0;
                TrackWireReadSix = 0;
                TrackWireReadSeven = 0;
                TrackWireReadAverage = 0;
                TrackWireReadEight = 0;
                StateCount = 0;

                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_FOLLOW_BACK_UP_LEFT_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = ForwardLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT)) {
                nextState = ForwardLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case ForwardLeft:
            if (ThisEvent.EventType == ES_ENTRY){
                //TurnNormalLeft(100);
                TurnNormalLeft(90);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_FOLLOW_FORWARD_LEFT_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = WallHardLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
                (ThisEvent.EventType == FC_BUMP_EVENT) ||
                (ThisEvent.EventType == FR_BUMP_EVENT)) {
                nextState = BackUpLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case WallHardLeft:
            if (ThisEvent.EventType == ES_ENTRY){
                TurnHardLeft(90);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_HARD_LEFT_TICKS);
            }
            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
                (ThisEvent.EventType == FC_BUMP_EVENT) ||
                (ThisEvent.EventType == FR_BUMP_EVENT)) {
                nextState = BackUpLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = CornerHardLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case CornerHardLeft:
            if (ThisEvent.EventType == ES_ENTRY) {
                TurnHardLeft(90);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, CORNER_HARD_LEFT_TICKS);
                
                TrackWireReadOne = 0;
                TrackWireReadTwo = 0;
                TrackWireReadThree = 0;
                TrackWireReadFour = 0;
                TrackWireReadFive = 0;
                TrackWireReadSix = 0;
                TrackWireReadSeven = 0;
                TrackWireReadAverage = 0;
                TrackWireReadEight = 0;
                StateCount = 0;
                
                printf("\r\n\r\n");
                printf("!!!!!!!!!!!!!!!!!!!!!!!!!!Corner!!!!!!!!!!!!!!!!!!!!!!!!!!");
                printf("\r\n\r\n");
            }
            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
                    (ThisEvent.EventType == FC_BUMP_EVENT) ||
                    (ThisEvent.EventType == FR_BUMP_EVENT)) {
                nextState = TrackBackUpLeft1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = TrackBackUpLeft1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

            
        case TrackBackUpLeft1:
            if (ThisEvent.EventType == ES_ENTRY){
                TurnHardLeft(-90);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_ONE_BACK_UP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = TrackForwardLeft1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
//            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT){
//                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
//                nextState = WallFound;
//                makeTransition = TRUE;
//                printf("\r\n\r\n");
//                printf("----------------------------------Track Wire-----------------------------------------");
//                printf("\r\n\r\n");
//                //ThisEvent.EventType = ES_NO_EVENT;
//            }
            if ((ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT)) {
                nextState = TrackForwardLeft1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case TrackForwardLeft1:
            if (ThisEvent.EventType == ES_ENTRY){
                //TurnNormalLeft(100);
                TurnNormalLeft(90);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_ONE_FORWARD_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = TrackWallHardLeft1;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
//            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
//                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
//                nextState = WallFound;
//                makeTransition = TRUE;
//                //ThisEvent.EventType = ES_NO_EVENT;
//            }
            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
                (ThisEvent.EventType == FC_BUMP_EVENT) ||
                (ThisEvent.EventType == FR_BUMP_EVENT)) {
                if (StateCount == 0) {
                    TrackWireReadOne = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadOne: %d", TrackWireReadOne);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 1) {
                    TrackWireReadTwo = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadTwo: %d", TrackWireReadTwo);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 2) {
                    TrackWireReadThree = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadThree: %d", TrackWireReadThree);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 3) {
                    TrackWireReadFour = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadFour: %d", TrackWireReadFour);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 4) {
                    TrackWireReadFive = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadFive: %d", TrackWireReadFive);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 5) {
                    TrackWireReadSix = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadSix: %d", TrackWireReadSix);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 6) {
                    TrackWireReadSeven = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadSeven: %d", TrackWireReadSeven);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }else if (StateCount == 7) {
                    TrackWireReadEight = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadEight: %d", TrackWireReadEight);
                    TrackWireReadAverage = (TrackWireReadOne + TrackWireReadEight)/2;
                    
                    printf("\r\n\r\n");
                    printf("                 The value of TrackWireReadAverage (1 and 8) is: %d", TrackWireReadAverage);
                    printf("\r\n\r\n");
                    
                    if (TrackWireReadAverage > TRACK_WIRE_THRESHOLD_NEW) {
                        ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
                        //nextState = WallFound;
                        makeTransition = TRUE;
                    } else {
                        nextState = BackUpLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                   
                }
              
            }
            break;

        case TrackWallHardLeft1:
            if (ThisEvent.EventType == ES_ENTRY){
                TurnHardLeft(90);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_HARD_LEFT_TICKS);
            }
            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
                (ThisEvent.EventType == FC_BUMP_EVENT) ||
                (ThisEvent.EventType == FR_BUMP_EVENT)) {
                if (StateCount == 0) {
                    TrackWireReadOne = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadOne: %d", TrackWireReadOne);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 1) {
                    TrackWireReadTwo = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadTwo: %d", TrackWireReadTwo);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 2) {
                    TrackWireReadThree = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadThree: %d", TrackWireReadThree);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 3) {
                    TrackWireReadFour = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadFour: %d", TrackWireReadFour);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 4) {
                    TrackWireReadFive = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadFive: %d", TrackWireReadFive);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 5) {
                    TrackWireReadSix = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadSix: %d", TrackWireReadSix);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                } else if (StateCount == 6) {
                    TrackWireReadSeven = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadSeven: %d", TrackWireReadSeven);
                    
                    StateCount++;
                    nextState = TrackBackUpLeft1;
                    makeTransition = TRUE;
                    ThisEvent.EventType = ES_NO_EVENT;
                }else if (StateCount == 7) {
                    TrackWireReadEight = Bot_ReadTrackWireVoltage();
                    printf("\r\n\r\n");
                    printf("TrackWireReadEight: %d", TrackWireReadEight);
                    TrackWireReadAverage = (TrackWireReadOne + TrackWireReadEight) / 2;
                    
                    printf("\r\n\r\n");
                    printf("               The value of TrackWireReadAverage (1 and 8) is: %d", TrackWireReadAverage);
                    printf("\r\n\r\n");
                    
                    if (TrackWireReadAverage > TRACK_WIRE_THRESHOLD_NEW) {
                        ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
                        //nextState = WallFound;
                        makeTransition = TRUE;
                    } else {
                        nextState = BackUpLeft;
                        makeTransition = TRUE;
                        ThisEvent.EventType = ES_NO_EVENT;
                    }
                   
                }
              
            }
//            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
//                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
//                nextState = WallFound;
//                makeTransition = TRUE;
//                //ThisEvent.EventType = ES_NO_EVENT;
//            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = CornerHardLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
//        case TrackBackUpLeft2:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                TurnSharpLeft(-60);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_FOLLOW_BACK_UP_LEFT_TICKS);
//            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = TrackForwardLeft2;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
////            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT){
////                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
////                nextState = WallFound;
////                makeTransition = TRUE;
////            }
//            if ((ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT)) {
//                nextState = TrackForwardLeft2;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            break;
//
//        case TrackForwardLeft2:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                TurnNormalLeft(60);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_FOLLOW_FORWARD_LEFT_TICKS);
//            }
////            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
////                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
////                nextState = WallFound;
////                makeTransition = TRUE;
////            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = TrackWallHardLeft2;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FC_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FR_BUMP_EVENT)) {
//                nextState = TrackBackUpLeft3;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            break;
//
//        case TrackWallHardLeft2:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                TurnHardLeft(60);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_HARD_LEFT_TICKS);
//            }
//            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FC_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FR_BUMP_EVENT)) {
//                nextState = TrackBackUpLeft3;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
////            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
////                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
////                //nextState = WallFound;
////                //makeTransition = TRUE;
////            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = CornerHardLeft;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            break;
//        case TrackBackUpLeft3:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                TurnSharpLeft(-60);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_FOLLOW_BACK_UP_LEFT_TICKS);
//            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = TrackForwardLeft3;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
////            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT){
////                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
////                nextState = WallFound;
////                makeTransition = TRUE;
////            }
//            if ((ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT)) {
//                nextState = TrackForwardLeft3;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            break;
//
//        case TrackForwardLeft3:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                TurnNormalLeft(60);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_FOLLOW_FORWARD_LEFT_TICKS);
//            }
////            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
////                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
////                nextState = WallFound;
////                makeTransition = TRUE;
////            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = TrackWallHardLeft3;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FC_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FR_BUMP_EVENT)) {
//                if (Bot_ReadTrackWireVoltage() > TRACK_WIRE_THRESHOLD_NEW) {
//                    ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
//                    //nextState = WallFound;
//                    makeTransition = TRUE;
//                    
//                } else {
//                    nextState = BackUpLeft;
//                    makeTransition = TRUE;
//                    ThisEvent.EventType = ES_NO_EVENT;
//                }
//            }
//            break;
//
//        case TrackWallHardLeft3:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                TurnHardLeft(60);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, WALL_HARD_LEFT_TICKS);
//            }
//            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FC_BUMP_EVENT) ||
//                    (ThisEvent.EventType == FR_BUMP_EVENT)) {
//                
//                if (Bot_ReadTrackWireVoltage() > TRACK_WIRE_THRESHOLD_NEW) {
//                    ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
//                    //nextState = WallFound;
//                    makeTransition = TRUE;
//                    
//                } else {
//                    nextState = BackUpLeft;
//                    makeTransition = TRUE;
//                    ThisEvent.EventType = ES_NO_EVENT;
//                }
//                    
//            }
////            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
////                ThisEvent.EventType = CORRECT_WALL_DETECTED_EVENT;
////                nextState = WallFound;
////                makeTransition = TRUE;
////            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = CornerHardLeft;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }    
//            break;
            
//        case WallFound:
//            if (ThisEvent.EventType == ES_ENTRY){
//                ThisEvent.EventType == CORRECT_WALL_DETECTED_EVENT;
//                DriveStraight(0);
//                ES_Timer_InitTimer(TOP_TRANSITION_TIMER, 100);
//            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == TOP_TRANSITION_TIMER)){
//                ThisEvent.EventType == CORRECT_WALL_DETECTED_EVENT;
//            }
//            break;
            //        case Forward: // in the first state, replace this with correct names
//            if (ThisEvent.EventType == ES_ENTRY){
//                DriveStraight(100);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, QUARTER_SECOND_TICKS);
//            }
//            
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == 2)) {
//                nextState = ForwardLeft;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
//                (ThisEvent.EventType == FC_BUMP_EVENT) ||
//                (ThisEvent.EventType == FR_BUMP_EVENT)) {
//                nextState = BackUpLeft;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            break;
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunWallFollowingSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunWallFollowingSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

