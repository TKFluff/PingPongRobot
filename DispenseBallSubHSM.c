/*
 * File: TemplateSubHSM.c
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
#include "DispenseBallSubHSM.h"
#include "Bot.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    FixOffset,
    TankToFront,
    RamWall,
    BackUpForDrawbridge,
    DeliverBall,
            StayAtTop,
            DescendStepper,
    ReverseFromTower,
    RepositionFromTower,
} DispenseBallSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"FixOffset",
	"TankToFront",
	"RamWall",
	"BackUpForDrawbridge",
	"DeliverBall",
	"StayAtTop",
	"DescendStepper",
	"ReverseFromTower",
	"RepositionFromTower",
};

//Include any defines you need to do
#define OFFSET_TICKS 70
#define DISPENSE_BALL_TICKS 1000
#define DESCEND_STEPPER_TICKS 940
#define AT_TOP_TICKS 500
#define BACK_UP_FOR_BRIDGE_TICKS 125
#define DISPENSE_TANK_TO_FRONT 575
#define DISPENSE_RAM_WALL_TICKS 500
#define REVERSE_FROM_TOWER_TICKS 1000
//
#define ROTATE_15_DEGREES_TICKS 175
#define ROTATE_90_DEGREES_TICKS 575

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

static DispenseBallSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
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
uint8_t InitDispenseBallSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunDispenseBallSubHSM(INIT_EVENT);
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
ES_Event RunDispenseBallSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    DispenseBallSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = FixOffset;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case FixOffset:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(60);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, OFFSET_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = TankToFront;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case TankToFront: // in the first state, replace this with correct names
            if (ThisEvent.EventType == ES_ENTRY) {
                TankLeft(100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, DISPENSE_TANK_TO_FRONT);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = RamWall;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case RamWall:
            if (ThisEvent.EventType == ES_ENTRY) {
                DriveStraight(100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, DISPENSE_RAM_WALL_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&(ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = DeliverBall;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case DeliverBall:
            //Have stepper move upward.
            if (ThisEvent.EventType == ES_ENTRY) {
                StepperForward();
                //StepperReverse();
                DriveStraight(0);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, DISPENSE_BALL_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = StayAtTop;
                makeTransition = TRUE;
                ES_Timer_StopTimer(SUB_TRANSITION_TIMER);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case StayAtTop:
            if (ThisEvent.EventType == ES_ENTRY){
                StepperStop();
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, AT_TOP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = DescendStepper;
                makeTransition = TRUE;
                ES_Timer_StopTimer(SUB_TRANSITION_TIMER);
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case DescendStepper:
            /*
             * We will need to do more than just reverse from the tower as there
             * is a possibility that a tower is behind the tower that we just
             * dispensed a ball into. Thus, we will continuously hit the tower that
             * we just finished and never get to the next towers.
             */
            if (ThisEvent.EventType == ES_ENTRY) {
                DriveStraight(0);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, DESCEND_STEPPER_TICKS);
                StepperReverse();
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = ReverseFromTower;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT) {
                DriveStraight(0);
            }
            break;
            
            case ReverseFromTower:
            /*
             * We will need to do more than just reverse from the tower as there
             * is a possibility that a tower is behind the tower that we just
             * dispensed a ball into. Thus, we will continuously hit the tower that
             * we just finished and never get to the next towers.
             */
            if (ThisEvent.EventType == ES_ENTRY) {
                StepperStop();
                DriveStraight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, DISPENSE_RAM_WALL_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = RepositionFromTower;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT) {
                DriveStraight(0);
            }
            break;

        case RepositionFromTower:
            //Turn away from tower.
            if (ThisEvent.EventType == ES_ENTRY) {
                TankRight(85); 
            }
            
            if (ThisEvent.EventType == NO_BEACON_EVENT) {
                ThisEvent.EventType = BALL_DISPENSED_EVENT;
            }
            break;

        default: // all unhandled states fall into here
            break;
            
            //        case BackUpForDrawbridge:
//            if (ThisEvent.EventType == ES_ENTRY) {
//                //Have stepper move downward here. Stepper starts at the bottom.
//                //StepperReverse();
//                DriveStraight(-90);
//                ES_Timer_StopTimer(SUB_TRANSITION_TIMER);
//                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, BACK_UP_FOR_BRIDGE_TICKS);
//            }
//            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
//                nextState = DeliverBall;
//                makeTransition = TRUE;
//                ThisEvent.EventType = ES_NO_EVENT;
//            }
//            if (ThisEvent.EventType == BC_TAPE_SEE_BLACK_EVENT) {
//                DriveStraight(0);
//            }
//            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunDispenseBallSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunDispenseBallSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/