/*
 * File: ProjectService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a simple service to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include <BOARD.h>
#include <AD.h>
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ProjectService.h"
#include "Bot.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define TIMER_0_TICKS 5
int i = 0;
#define BATTERY_DISCONNECT_THRESHOLD 175

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/
/* You will need MyPriority and maybe a state variable; you may need others
 * as well. */

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

int debouncerHelper(unsigned char Debounce[4]) {
    for (i = 0; i < 3; i++) {
        if (Debounce[0] != Debounce[i + 1]) {
            return 0;
        }
    }
    return 1;
}

/**
 * @Function InitTemplateService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitBumperService(uint8_t Priority)
{
    ES_Event ThisEvent;

    MyPriority = Priority;

    // in here you write your initialization code
    // this includes all hardware and software initialization
    // that needs to occur.

    // post the initial transition event
    ES_Timer_InitTimer(BUMPER_SIMPLE_SERVICE_TIMER, TIMER_0_TICKS);
    ThisEvent.EventType = ES_INIT;
    if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostBumperService(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunBumperService(ES_Event ThisEvent)
{
    ES_Event FLReturnEvent;
    ES_Event FRReturnEvent;
    ES_Event FCReturnEvent;
    FLReturnEvent.EventType = NO_FL_BUMP_EVENT; // assume no errors
    FRReturnEvent.EventType = NO_FR_BUMP_EVENT; // assume no errors
    FCReturnEvent.EventType = NO_FC_BUMP_EVENT; // assume no errors

    /********************************************
     in here you write your service code
     *******************************************/

    static ES_EventTyp_t FLlastEvent = NO_FL_BUMP_EVENT;
    static ES_EventTyp_t FRlastEvent = NO_FR_BUMP_EVENT;
    static ES_EventTyp_t FClastEvent = NO_FC_BUMP_EVENT;
    ES_EventTyp_t FLcurEvent;
    ES_EventTyp_t FRcurEvent;
    ES_EventTyp_t FCcurEvent;
    unsigned char Debounce[4];

    switch (ThisEvent.EventType) {
        case ES_INIT:
            // No hardware initialization or single time setups, those
            // go in the init function above.
        case ES_TIMERACTIVE:
            break;
        case ES_TIMERSTOPPED:
            // This section is used to reset service for some reason
            break;

        case ES_TIMEOUT:
            ES_Timer_InitTimer(BUMPER_SIMPLE_SERVICE_TIMER, TIMER_0_TICKS);

            uint8_t returnVal = FALSE;

            if (Bot_ReadFrontLeftBumper() == BUMPER_TRIPPED) { //what event?
                FLcurEvent = FL_BUMP_EVENT;
            } else if (Bot_ReadFrontLeftBumper() == BUMPER_NOT_TRIPPED) {
                FLcurEvent = NO_FL_BUMP_EVENT;
            }

            if (FLcurEvent != FLlastEvent) { // check for change from last time
                //printf("Different Event\n");
                FLReturnEvent.EventType = FLcurEvent;
                FLReturnEvent.EventParam = Bot_ReadBumpers();
                printf("\r\nEvent: %s\tParam: 0x%X",
                EventNames[FLReturnEvent.EventType], FLReturnEvent.EventParam);
                returnVal = TRUE;
                FLlastEvent = FLcurEvent; // update history


#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostProjectHSM(FLReturnEvent);
#else            
                PostBumperService(FLReturnEvent);
#endif   
            }


            if (Bot_ReadFrontRightBumper() == BUMPER_TRIPPED) { //what event?
                FRcurEvent = FR_BUMP_EVENT;
            } else if (Bot_ReadFrontRightBumper() == BUMPER_NOT_TRIPPED) {
                FRcurEvent = NO_FR_BUMP_EVENT;
            }

            if (FRcurEvent != FRlastEvent) { // check for change from last time
                //printf("Different Event\n");
                FRReturnEvent.EventType = FRcurEvent;
                FRReturnEvent.EventParam = Bot_ReadBumpers();
                printf("\r\nEvent: %s\tParam: 0x%X",
                EventNames[FRReturnEvent.EventType], FRReturnEvent.EventParam);
                returnVal = TRUE;
                FRlastEvent = FRcurEvent; // update history


#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostProjectHSM(FRReturnEvent);
#else            
                PostBumperService(FRReturnEvent);
#endif   
            }
            
            if (Bot_ReadFrontCenterBumper() == BUMPER_TRIPPED) { //what event?
                FCcurEvent = FC_BUMP_EVENT;
            } else if (Bot_ReadFrontCenterBumper() == BUMPER_NOT_TRIPPED) {
                FCcurEvent = NO_FC_BUMP_EVENT;
            }

            if (FCcurEvent != FClastEvent) { // check for change from last time
                //printf("Different Event\n");
                FCReturnEvent.EventType = FCcurEvent;
                FCReturnEvent.EventParam = Bot_ReadBumpers();
                printf("\r\nEvent: %s\tParam: 0x%X",
                EventNames[FCReturnEvent.EventType], FCReturnEvent.EventParam);
                returnVal = TRUE;
                FClastEvent = FCcurEvent; // update history


#ifndef SIMPLESERVICE_TEST           // keep this as is for test harness
                PostProjectHSM(FCReturnEvent);
#else            
                PostBumperService(FCReturnEvent);
#endif   
            }

            
#ifdef SIMPLESERVICE_TEST     // keep this as is for test harness      
        default:
            printf("\r\nEvent: %s\tParam: 0x%X",
                EventNames[ThisEvent.EventType], ThisEvent.EventParam);
            break;
#endif
    }
    return FCReturnEvent;
}
/*
void main(void)
{
    ES_Return_t ErrorType;
 
    BOARD_Init();
 
    printf("Starting ES Framework Template\r\n");
    printf("using the 2nd Generation Events & Services Framework\r\n");
 
 
    // Your hardware initialization function calls go here
    Bot_Init();
    // now initialize the Events and Services Framework and start it running
    ErrorType = ES_Initialize();
    if (ErrorType == Success) {
        ErrorType = ES_Run();
 
    }
     
    //if we got to here, there was an error
    switch (ErrorType) {
    case FailedPointer:
        printf("Failed on NULL pointer");
        break;
    case FailedInit:
        printf("Failed Initialization");
        break;
    default:
        printf("Other Failure: %d", ErrorType);
        break;
    }
    for (;;)
        ;
 
};
*/
/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/

