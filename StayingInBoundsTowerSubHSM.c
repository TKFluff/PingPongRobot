/*
 * File: StayingInBoundsTowerSubHSM.c
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
#include "StayingInBoundsTowerSubHSM.h"
#include "TapeFollowingSubSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Reverse,
    TurnAround,
    Forward,
    BackUpRight,
    ForwardRight,
    TurnAroundAgain,
} StayingInBoundsTowerSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Reverse",
	"TurnAround",
	"Forward",
	"BackUpRight",
	"ForwardRight",
	"TurnAroundAgain",
};

//Include any defines you need to do
#define IN_BOUNDS_TOWER_TURN_AROUND_TICKS 1500
#define IN_BOUNDS_TOWER_FORWARD_TICKS 500
#define IN_BOUNDS_TOWER_BACK_UP_RIGHT_TICKS 500
#define IN_BOUNDS_TOWER_TURN_AROUND_AGAIN_TICKS 1500

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

static StayingInBoundsTowerSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
static uint8_t MyPriority;


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
uint8_t InitStayingInBoundsTowerSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunStayingInBoundsTowerSubHSM(INIT_EVENT);
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
ES_Event RunStayingInBoundsTowerSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    StayingInBoundsTowerSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = Reverse;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;

            }
            break;

        case Reverse:
            DriveStraight(-100);
            if ((ThisEvent.EventType == FL_TAPE_SEE_WHITE_EVENT) ||
                (ThisEvent.EventType == FR_TAPE_SEE_WHITE_EVENT)) {
                nextState = TurnAround;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case TurnAround: // in the first state, replace this with correct names
            if (ThisEvent.EventType == ES_ENTRY){
                TankRight(100);
                //find out the amount of time required to make a full 180 turn.
                //Rotational distance is about 14 inches. Speed is about 8 inches/s
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, IN_BOUNDS_TOWER_TURN_AROUND_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == 2)) {
                nextState = Forward;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT; 
            }
            break;

        case Forward:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, IN_BOUNDS_TOWER_FORWARD_TICKS);
            }
            if ((ThisEvent.EventType == FR_BUMP_EVENT) ||
                    (ThisEvent.EventType == FL_BUMP_EVENT)) {
                nextState = BackUpRight;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == 2)) {
                nextState = ForwardRight;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case ForwardRight:
            TurnSharpRight(100);
            if ((ThisEvent.EventType == FL_TAPE_SEE_BLACK_EVENT) ||
                    (ThisEvent.EventType == FR_TAPE_SEE_BLACK_EVENT)) {
                nextState = TurnAroundAgain;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == FL_BUMP_EVENT) ||
                    (ThisEvent.EventType == FR_BUMP_EVENT)) {
                nextState = BackUpRight;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case BackUpRight:
            if (ThisEvent.EventType == ES_ENTRY){
                TurnSharpRight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, IN_BOUNDS_TOWER_BACK_UP_RIGHT_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == 2)) {
                nextState = Forward;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == FL_TAPE_SEE_BLACK_EVENT) ||
                (ThisEvent.EventType == FR_TAPE_SEE_BLACK_EVENT)) {
                nextState = TurnAroundAgain;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case TurnAroundAgain:
            if (ThisEvent.EventType == ES_ENTRY){
                TankRight(100);
                //find out the amount of time required to make a full 180 turn.
                //Rotational distance is about 14 inches. Speed is about 8 inches/s
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, IN_BOUNDS_TOWER_TURN_AROUND_AGAIN_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == 2)) {
                ThisEvent.EventType = WALL_DETECTED_EVENT;
            }
            break;
            
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunStayingInBoundsTowerSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunStayingInBoundsTowerSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

