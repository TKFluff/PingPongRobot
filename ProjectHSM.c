/*
 * File: ProjectHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel Elkaim and Soja-Marie Morgens
 * 
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
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
#include "ProjectInitalSubHSM.h" //#include all sub state machines called
#include "ProjectBeaconFindingSubHSM.h"
#include "TapeFollowingSubSubHSM.h"
#include "WallFollowingSubHSM.h"
#include "StayingInBoundsTowerSubHSM.h"
#include "GetParallelSubHSM.h"
#include "FindingCorrectHoleSubHSM.h"
#include "DispenseBallSubHSM.h"
#include "Bot.h"
#include <stdio.h>

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do
#define HALF_SECOND_TICKS 500
#define ONE_SECOND_TICKS 1000
#define ONE_POINT_FIVE_SECOND_TICKS 1500
#define TWO_SECOND_TICKS 2000
#define WIRE_CONFIRM_TICKS 8500
#define THREE_SECOND_TICKS 3000

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/


typedef enum {
    InitPState,
    InitialState,
    BeaconFinding,
    WallFollowing,
    StayingInBoundsTower,
    GetParallel,
    FindingCorrectHole,
    DispenseBall,
} ProjectHSMState_t;

static const char *StateNames[] = {
	"InitPState",
	"InitialState",
	"BeaconFinding",
	"WallFollowing",
	"StayingInBoundsTower",
	"GetParallel",
	"FindingCorrectHole",
	"DispenseBall",
};


/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine
   Example: char RunAway(uint_8 seconds);*/
/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static ProjectHSMState_t CurrentState = InitPState; // <- change enum name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitTemplateHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunTemplateFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitProjectHSM(uint8_t Priority)
{
    MyPriority = Priority;
    // put us into the Initial PseudoState
    CurrentState = InitPState;
    // post the initial transition event
    if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * @Function PostProjectHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostProjectHSM(ES_Event ThisEvent)
{
    return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunTemplateHSM(ES_Event ThisEvent)
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
ES_Event RunProjectHSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    ProjectHSMState_t nextState; // <- change type to correct enum
    ES_Tattle(); // trace call stack
    switch (CurrentState) {
    case InitPState: // If current state is initial Pseudo State
        if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
        {
            // this is where you would put any actions associated with the
            // transition from the initial pseudo-state into the actual
            // initial state
            // Initialize all sub-state machines
//            InitProjectInitialSubHSM();
            InitProjectBeaconFindingSubHSM();
            //InitTapeFollowingSubSubHSM();
            InitWallFollowingSubHSM();
            InitStayingInBoundsTowerSubHSM();
            InitGetParallelSubHSM();
            InitFindingCorrectHoleSubHSM();
            InitDispenseBallSubHSM();
            // now put the machine into the actual initial state
//            ES_Timer_InitTimer(TOP_TRANSITION_TIMER, TWO_SECOND_TICKS);
            nextState = BeaconFinding;
            makeTransition = TRUE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
            break;

//        case InitialState: // in the first state, replace this with correct names
//            // run sub-state machine for this state
//            //NOTE: the SubState Machine runs and responds to events before anything in the this
//            //state machine does
//            ThisEvent = RunProjectInitialSubHSM(ThisEvent);
//            switch (ThisEvent.EventType) {
//                case ES_NO_EVENT:
//                    break;
//
//                case ES_TIMEOUT:
//                    ES_Timer_StopTimer(TOP_TRANSITION_TIMER);
//                    nextState = BeaconFinding;
//                    makeTransition = TRUE;
//                    ThisEvent.EventType = ES_NO_EVENT;
//                    break;
//            }
//            break;
            
        case BeaconFinding:
            ThisEvent = RunProjectBeaconFindingSubHSM(ThisEvent);
            if ((ThisEvent.EventType == FL_BUMP_EVENT) || 
                (ThisEvent.EventType == FR_BUMP_EVENT) ||
                 ThisEvent.EventType == FC_BUMP_EVENT){
                nextState = WallFollowing;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case WallFollowing:
            ThisEvent = RunWallFollowingSubHSM(ThisEvent);
            if (ThisEvent.EventType == ES_ENTRY){
//                ES_Timer_StopTimer(TOP_TRANSITION_TIMER);
            }
            //Took out FR_TAPE for now.
            if ((ThisEvent.EventType == FL_TAPE_SEE_BLACK_EVENT)){
                nextState = StayingInBoundsTower;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            /*This will currently transition right when track wire is detected.
             * Need to find a way to confirm track wire. Maybe set a timer?
             */ 
            if (ThisEvent.EventType == CORRECT_WALL_DETECTED_EVENT){
                nextState = GetParallel;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
//            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT) {
//                
//
////                if (wireCounter == 1) {
////                    ES_Timer_InitTimer(TOP_TRANSITION_TIMER, WIRE_CONFIRM_TICKS);
////                }
////                if (wireCounter == 7) {
////                    nextState = GetParallel;
////                    makeTransition = TRUE;
////                    ThisEvent.EventType = ES_NO_EVENT;
////                }
//              }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == TOP_TRANSITION_TIMER)) {
//                nextState = GetParallel;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
            /* Previous Code.
            if (ThisEvent.EventType == TRACK_WIRE_FOUND_EVENT){
                //Originally is two seconds.
                ES_Timer_InitTimer(TOP_TRANSITION_TIMER, WIRE_CONFIRM_TICKS);
            }
            if (ThisEvent.EventType == NO_TRACK_WIRE_EVENT){
                ES_Timer_StopTimer(TOP_TRANSITION_TIMER);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == TOP_TRANSITION_TIMER)){
                nextState = GetParallel;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }*/
            break;
            
        case StayingInBoundsTower:
            ThisEvent = RunStayingInBoundsTowerSubHSM(ThisEvent); 
            if (ThisEvent.EventType == WALL_DETECTED_EVENT){
                nextState = WallFollowing;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case GetParallel:
            //printf("At GetParallel but before entering EEEEEEEEEEEEEEEEEEEEEEEEEEEEE\r\n");
            ThisEvent = RunGetParallelSubHSM(ThisEvent);
            //printf("In GetParallel IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\r\n");
            if (ThisEvent.EventType == GOT_PARALLEL_EVENT){
                nextState = FindingCorrectHole;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (Bot_ReadBumpers() == 0){
                nextState = FindingCorrectHole;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case FindingCorrectHole:
            ThisEvent = RunFindingCorrectHoleSubHSM(ThisEvent);
            if (ThisEvent.EventType == CORRECT_HOLE_FOUND_EVENT){
                nextState = DispenseBall;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case DispenseBall:
            ThisEvent = RunDispenseBallSubHSM(ThisEvent);
            if (ThisEvent.EventType == BALL_DISPENSED_EVENT){
                nextState = BeaconFinding;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
    default: // all unhandled states fall into here
        break;
    } // end switch on Current State

    
    
    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunProjectHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunProjectHSM(ENTRY_EVENT); // <- rename to your own Run function
        if(nextState == InitPState){
            
        }
//    if(nextState ==InitialState){
//        InitProjectInitialSubHSM();
//    }
        if(nextState == BeaconFinding){
            InitProjectBeaconFindingSubHSM();
        }
        else if(nextState == WallFollowing){
            InitWallFollowingSubHSM();
        }
        else if(nextState == StayingInBoundsTower){
            InitStayingInBoundsTowerSubHSM();
        }
        else if(nextState == GetParallel){
            InitGetParallelSubHSM();
        }
        else if(nextState == FindingCorrectHole){
            InitFindingCorrectHoleSubHSM();
        }
        else if(nextState == DispenseBall){
            InitDispenseBallSubHSM();
        } 
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
