/*
 * File: FindingCorrectHoleSubHSM.c
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
#include "FindingCorrectHoleSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    Reverse,
    RotateToSide,
    Wait,
    Forward,
    Stop,
    IgnoringBoundaries,
    SeeBlack,
    StayingInBounds,
    OffEdge,
    BackingIntoTape,
} FindingCorrectHoleSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"Reverse",
	"RotateToSide",
	"Wait",
	"Forward",
	"Stop",
	"IgnoringBoundaries",
	"SeeBlack",
	"StayingInBounds",
	"OffEdge",
	"BackingIntoTape",
};

//Include any defines you need to do
//was 900
#define CORRECT_HOLE_FORWARD_TICKS 400
#define BACK_UP_TICKS 100
#define OFF_EDGE_TICKS 240
//increase slightly.
//was 1250
#define ROTATE_TO_SIDE_TICKS 1350
#define CORRECT_HOLE_WAIT_TICKS 530
#define IGNORING_BOUNDRIES_TICKS 250
#define CORRECT_HOLE_SEE_BLACK_TICKS 250
#define BACKING_INTO_TAPE_TICKS 250
//was 600 originally.
//550 is about 90
#define ROTATE_90_DEGREES_TICKS 530

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

static FindingCorrectHoleSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
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
uint8_t InitFindingCorrectHoleSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunFindingCorrectHoleSubHSM(INIT_EVENT);
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
ES_Event RunFindingCorrectHoleSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    FindingCorrectHoleSubHSMState_t nextState; // <- change type to correct enum

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
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-40);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, BACK_UP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = RotateToSide;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            /*if (ThisEvent.EventType == ES_EXIT){
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, ROTATE_90_DEGREES_TICKS);
            }*/
            break;
            
        case RotateToSide:
            if (ThisEvent.EventType == ES_ENTRY){
                //DriveStraight(0);
                TankRight(50);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, ROTATE_TO_SIDE_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = Wait;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case Wait:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(0);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, CORRECT_HOLE_WAIT_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = Forward;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case Forward: // in the first state, replace this with correct names
            //We want to go past the edge of the tower.
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(75);
                //DriveStraight(0);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, CORRECT_HOLE_FORWARD_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = Stop;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            //Added to reduce slipping of wheels when going forward and reversing.
        case Stop:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(0);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, CORRECT_HOLE_WAIT_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = StayingInBounds;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
        /*
        case IgnoringBoundaries:
            if (ThisEvent.EventType == ES_ENTRY){
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, IGNORING_BOUNDRIES_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = OffEdge;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
       
        case SeeBlack:
            if (ThisEvent.EventType == ES_ENTRY){
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, CORRECT_HOLE_SEE_BLACK_TICKS);
            }
            //Saw the black tape and is now off the tower.
            if ((ThisEvent.EventType == FL_TAPE_SEE_BLACK_EVENT) ||
                (ThisEvent.EventType == FR_TAPE_SEE_BLACK_EVENT)) {
                nextState = StayingInBounds;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            //Went past the black tape.
            if (ThisEvent.EventType = L_BALL_TAPE_SEE_WHITE_EVENT) {
                nextState = BackingIntoTape;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            //Went off the edge and needs to now come back.
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
               (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = OffEdge;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
        */
        case StayingInBounds:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-40);
            }
            if (ThisEvent.EventType == L_BALL_TAPE_SEE_WHITE_EVENT) {
                nextState = BackingIntoTape;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
/*
        case OffEdge:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-40);
            }
            if (ThisEvent.EventType == L_BALL_TAPE_SEE_WHITE_EVENT) {
                nextState = BackingIntoTape;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
*/
        case BackingIntoTape:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-40);
            }
            if (ThisEvent.EventType == L_BALL_TAPE_SEE_BLACK_EVENT) {
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, BACKING_INTO_TAPE_TICKS);
                ThisEvent.EventType = CORRECT_HOLE_FOUND_EVENT;
                DriveStraight(0);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
               (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                ThisEvent.EventType = CORRECT_HOLE_FOUND_EVENT;
            }
            break;
            
        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunFindingCorrectHoleSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunFindingCorrectHoleSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/