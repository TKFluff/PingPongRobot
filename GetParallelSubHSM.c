/*
 * File: GetParallelSubHSM.c
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
#include "GetParallelSubHSM.h"
#include "Bot.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
typedef enum {
    InitPSubState,
    HardLeft,
    TurnLeft,
    LeftBump,
    LeftRightBump,
    LeftCenterBump,
    LBackingUp,
    TurnRight,
    RightBump,
    RightCenterBump,
    RBackingUp,
    LAllBump,
    LBackOneMore,
    LForwardOneMore,
    RAllBump,
    RBackOneMore,
    RForwardOneMore,
    ForwardFast,
} GetParallelSubHSMState_t;

static const char *StateNames[] = {
	"InitPSubState",
	"HardLeft",
	"TurnLeft",
	"LeftBump",
	"LeftRightBump",
	"LeftCenterBump",
	"LBackingUp",
	"TurnRight",
	"RightBump",
	"RightCenterBump",
	"RBackingUp",
	"LAllBump",
	"LBackOneMore",
	"LForwardOneMore",
	"RAllBump",
	"RBackOneMore",
	"RForwardOneMore",
	"ForwardFast",
};

//Include any defines you need to do
#define GET_PARALLEL_HARD_LEFT_TICKS 1000
#define GET_PARALLEL_TURN_LEFT_TICKS 250
#define GET_PARALLEL_LEFT_BUMP_TICKS 250
#define GET_PARALLEL_LEFT_RIGHT_BUMP_TICKS 250
#define GET_PARALLEL_LEFT_CENTER_BUMP_TICKS 500
#define GET_PARALLEL_L_BACKING_UP_TICKS 125
#define GET_PARALLEL_TURN_RIGHT_TICKS 500
#define GET_PARALLEL_RIGHT_BUMP_TICKS 250
#define GET_PARALLEL_RIGHT_CENTER_BUMP_TICKS 500
#define GET_PARALLEL_R_BACKING_UP_TICKS 125
#define L_ALL_BUMP_TICKS 500
#define L_BACK_ONE_MORE_TICKS 125
#define L_FORWARD_ONE_MORE_TICKS 250
#define R_ALL_BUMP_TICKS 500
#define R_BACK_ONE_MORE_TICKS 125
#define R_FORWARD_ONE_MORE_TICKS 250
//
#define ROTATE_45_DEGREES_TICKS 300

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

static GetParallelSubHSMState_t CurrentState = InitPSubState; // <- change name to match ENUM
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
uint8_t InitGetParallelSubHSM(void) {
    ES_Event returnEvent;

    CurrentState = InitPSubState;
    returnEvent = RunGetParallelSubHSM(INIT_EVENT);
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
ES_Event RunGetParallelSubHSM(ES_Event ThisEvent) {
    uint8_t makeTransition = FALSE; // use to flag transition
    GetParallelSubHSMState_t nextState; // <- change type to correct enum

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
        case InitPSubState: // If current state is initial Psedudo State
            if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
            {
                // this is where you would put any actions associated with the
                // transition from the initial pseudo-state into the actual
                // initial state

                // now put the machine into the actual initial state
                nextState = HardLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case HardLeft:
            if (ThisEvent.EventType == ES_ENTRY){
                TurnHardLeft(100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_HARD_LEFT_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = LBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            break;
            
        case TurnLeft: // in the first state, replace this with correct names
            if (ThisEvent.EventType == ES_ENTRY){
                //was half second
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_TURN_LEFT_TICKS);
                TurnNormalLeft(100);
            }
            if (ThisEvent.EventType == FL_BUMP_EVENT) {
                nextState = LeftBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FC_BUMP_EVENT) {
                nextState = RightBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FR_BUMP_EVENT) {
                nextState = RightBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = LBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            /* Previous code.
            if (ThisEvent.EventType == FR_BUMP_EVENT) {
                nextState = RightBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FC_BUMP_EVENT) {
                nextState = LeftCenterBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = LBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
             */
            break;
            
        case LeftBump:
            if (ThisEvent.EventType == ES_ENTRY){
                //was half.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_LEFT_BUMP_TICKS);
            }
            if (ThisEvent.EventType == FC_BUMP_EVENT){
                nextState = LeftCenterBump;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FR_BUMP_EVENT){
                nextState = LeftRightBump;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = LBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            break;
            
        case LeftRightBump:
            if (ThisEvent.EventType == ES_ENTRY){
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
                //was half.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_LEFT_RIGHT_BUMP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }
            break;
            
        case LeftCenterBump:
            if (ThisEvent.EventType == ES_ENTRY){
                //was one second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_LEFT_CENTER_BUMP_TICKS);
            }
            if (ThisEvent.EventType == FR_BUMP_EVENT){
                nextState = LAllBump;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                //nextState = LBackingUp;
                nextState = LAllBump;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            /* Previous code
            if (ThisEvent.EventType == ES_ENTRY){
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, HALF_SECOND_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }*/
            break;

        case LBackingUp:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_L_BACKING_UP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                //Find out the amount of time to tank turn 45 degrees at 90 speed.
                //Rotational distance is about 3.5 inches. Speed is 12.35 inches/s
                
                nextState = TurnLeft;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case TurnRight:
            if (ThisEvent.EventType == ES_ENTRY) {
                TurnNormalRight(100);
                //was one second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_TURN_RIGHT_TICKS);
            }
            if (ThisEvent.EventType == FR_BUMP_EVENT){
                nextState = RightBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FL_BUMP_EVENT){
                nextState = LeftBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if (ThisEvent.EventType == FC_BUMP_EVENT){
                nextState = RightBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = RBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            break;
            
        case RightBump:
            if (ThisEvent.EventType == ES_ENTRY) {
                //was 100
                TurnNormalRight(80);
                //was half second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_RIGHT_BUMP_TICKS);
            }
            if (ThisEvent.EventType == FC_BUMP_EVENT) {
                nextState = RightCenterBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = RBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case RightCenterBump:
            if (ThisEvent.EventType == ES_ENTRY) {
                //was one second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_RIGHT_CENTER_BUMP_TICKS);
            }
            if (ThisEvent.EventType == FL_BUMP_EVENT){
                nextState = RAllBump;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = RAllBump;
                makeTransition = TRUE;
                ThisEvent.EventType == ES_NO_EVENT;
            }
            /* Previous code
            if (ThisEvent.EventType == ES_ENTRY) {
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, HALF_SECOND_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }*/
            break;

        case RBackingUp:
            if (ThisEvent.EventType == ES_ENTRY) {
                DriveStraight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, GET_PARALLEL_R_BACKING_UP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                //Find out the amount of time to tank turn 45 degrees at 90 speed.
                //Rotational distance is about 3.5 inches. Speed is 12.35 inches/s
                nextState = TurnRight;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case LAllBump:
            if (ThisEvent.EventType == ES_ENTRY) {
                //ThisEvent.EventType = GOT_PARALLEL_EVENT;
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, L_ALL_BUMP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = LBackOneMore;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                //ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }
            break;
            
        case LBackOneMore:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, L_BACK_ONE_MORE_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                //Find out the amount of time to tank turn 45 degrees at 90 speed.
                //Rotational distance is about 3.5 inches. Speed is 12.35 inches/s
                
                nextState = LForwardOneMore;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case LForwardOneMore:
            if (ThisEvent.EventType == ES_ENTRY){
                //was half second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, L_FORWARD_ONE_MORE_TICKS);
                TurnNormalLeft(100);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }
            break;

        case RAllBump:
            if (ThisEvent.EventType == ES_ENTRY) {
                //ThisEvent.EventType = GOT_PARALLEL_EVENT;
                //was half second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, R_ALL_BUMP_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = RBackOneMore;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
                //ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }
            break;
            
        case RBackOneMore:
            if (ThisEvent.EventType == ES_ENTRY){
                DriveStraight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, R_BACK_ONE_MORE_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                //Find out the amount of time to tank turn 45 degrees at 90 speed.
                //Rotational distance is about 3.5 inches. Speed is 12.35 inches/s
                
                nextState = RForwardOneMore;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
            
        case RForwardOneMore:
            if (ThisEvent.EventType == ES_ENTRY){
                //Was half second.
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, R_FORWARD_ONE_MORE_TICKS);
                TurnNormalRight(100);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                ThisEvent.EventType = GOT_PARALLEL_EVENT;
            }
            break;
            
            /*
        case TankingRight:
            if (ThisEvent.EventType == ES_ENTRY){
                TankRight(-100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, ROTATE_45_DEGREES_TICKS);
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) &&
                    (ThisEvent.EventParam == SUB_TRANSITION_TIMER)) {
                nextState = ForwardFast;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;

        case ForwardFast:
            if (ThisEvent.EventType == ES_ENTRY) {
                DriveStraight(100);
                ES_Timer_InitTimer(SUB_TRANSITION_TIMER, ONE_POINT_FIVE_SECOND_TICKS);
            }
            if (ThisEvent.EventType == FR_BUMP_EVENT) {
                nextState = RightBump;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            if ((ThisEvent.EventType == ES_TIMEOUT) && (ThisEvent.EventParam == SUB_TRANSITION_TIMER)){
                nextState = LBackingUp;
                makeTransition = TRUE;
                ThisEvent.EventType = ES_NO_EVENT;
            }
            break;
*/

        default: // all unhandled states fall into here
            break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunGetParallelSubHSM(EXIT_EVENT); // <- rename to your own Run function
        CurrentState = nextState;
        RunGetParallelSubHSM(ENTRY_EVENT); // <- rename to your own Run function
    }

    ES_Tail(); // trace call stack end
    return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/

