/* 
 * File:   WireEventChecker.h
 * Author: Gabriel Hugh Elkaim
 *
 * Template file to set up typical EventCheckers for the  Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the
 * names will have to be changed to match your code.
 *
 * This EventCheckers file will work with simple services, FSM's and HSM's.
 *
 * Remember that EventCheckers should only return TRUE when an event has occured,
 * and that the event is a TRANSITION between two detectable differences. They
 * should also be atomic and run as fast as possible for good results.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on September 27, 2013, 8:37 AM
 * Modified on September 12, 2016, 7:59 PM
 */

#ifndef PROJECTEVENTCHECKER_H
#define	PROJECTEVENTCHECKER_H

/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "ES_Configure.h"   // defines ES_Event, INIT_EVENT, ENTRY_EVENT, and EXIT_EVENT
#include "BOARD.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC TYPEDEFS                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function WireSensorChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the track wire sensor voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the wire sensor voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either TRACK_WIRE_FOUND_EVENT or NO_TRACK_WIRE_EVENT if the track wire 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.16 14:28 */
uint8_t WireSensorChecker(void);


/**
 * @Function BeaconDetectorChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the beacon detector voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the beacon detector voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either BEACON_FOUND_EVENT or NO_BEACON_EVENT if the beacon 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.20 20:28 */
uint8_t BeaconDetectorChecker(void);


/**
 * @Function FLTapeChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the front left tape sensor voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the beacon detector voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either FL_TAPE_SEE_BLACK_EVENT or FL_TAPE_SEE_WHITE_EVENT if the beacon 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.20 20:28 */
uint8_t FLTapeChecker(void);


/**
 * @Function FRTapeChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the front right tape sensor voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the beacon detector voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either FR_TAPE_SEE_BLACK_EVENT or FR_TAPE_SEE_WHITE_EVENT if the beacon 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.20 20:28 */
uint8_t FRTapeChecker(void);


/**
 * @Function BCTapeChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the back center tape sensor voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the beacon detector voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either BC_TAPE_SEE_BLACK_EVENT or BC_TAPE_SEE_WHITE_EVENT if the beacon 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.20 20:28 */
uint8_t BCTapeChecker(void);


/**
 * @Function LeftBallTapeChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the left ball tape sensor voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the beacon detector voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either L_BALL_TAPE_SEE_BLACK_EVENT or L_BALL_TAPE_SEE_WHITE_EVENT if the beacon 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.20 20:28 */
uint8_t LeftBallTapeChecker(void);



/**
 * @Function RightBallTapeChecker(void)
 * @param none
 * @return TRUE or FALSE
 * @brief This function is an event checker that checks the right ball tape sensor voltage
 *        against a fixed threshold (#defined in the .c file). Note that you need to
 *        keep track of previous history, and that the beacon detector voltage is checked
 *        only once at the beginning of the function. The function will post an event
 *        of either R_BALL_TAPE_SEE_BLACK_EVENT or R_BALL_TAPE_SEE_WHITE_EVENT if the beacon 
 *        is detected. Returns TRUE if there was an event, FALSE otherwise.
 * @author Alex Zuo 2019.11.20 20:28 */
#endif	/* ProjectEventChecker.H */


