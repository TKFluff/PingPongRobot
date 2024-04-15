/* 
 * File:   Bot.h
 * Author: Alex Zuo
 *
 * Created on November 15, 2019, 1:23 PM
 */

#ifndef BOT_H
#define	BOT_H

#include <BOARD.h>

//!!!!!!!!!!!!!!! PORT Z10 IS BADDDDDDDD !!!!!!!!!!!!!!!!!!!!
/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/
//!!!!!!!!!!!!!!! PORT Z10 IS BADDDDDDDD !!!!!!!!!!!!!!!!!!!!
#define BUMPER_TRIPPED 1
#define BUMPER_NOT_TRIPPED 0
#define TAPE_TRIGGERED 1
#define TAPE_NOT_TRIGGERED 0

#define BOT_MAX_SPEED 100 

/**
 * @Function Bot_Init(void)
 * @param None.
 * @return None.
 * @brief  Performs all the initialization necessary for the roach. this includes initializing
 * the PWM module, the A/D converter, the data directions on some pins, and
 * setting the initial motor directions.
 * @note  None.
 */
void Bot_Init(void);


/**
 * @Function Bot_LeftMtrSpeed(char newSpeed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the left motor.
 */
char Bot_LeftMtrSpeed(char newSpeed);

/**
 * @Function Bot_RightMtrSpeed(char newSpeed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the left motor.
 */
char Bot_RightMtrSpeed(char newSpeed);

/**
 * @Function DriveStraight(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 */
char DriveStraight(char speed);

/**
 * @Function TankRight(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * Additionally, the bot will spin in place. Forward leads to clockwise rotation and
 * reverse leads to counterclockwise rotation.
 */
char TankRight(char speed);

/**
 * @Function TurnGentleRight(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * This will cause the bot to take a gentle right turn.
 */
char TurnGentleRight(char speed);

/**
 * @Function TurnNormalRight(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * This will cause the bot to take a normal right turn.
 */
char TurnNormalRight(char speed);

/**
 * @Function TurnSharpRight(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * This will cause the bot to take a sharp right turn.
 */
char TurnSharpRight(char speed);

/**
 * @Function TankLeft(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * Additionally, the bot will spin in place. Forward leads to counterclockwise 
 * rotation and reverse leads to clockwise rotation.
 */
char TankLeft(char speed);

/**
 * @Function TurnGentleLeft(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * This will cause the bot to take a gentle left turn.
 */
char TurnGentleRight(char speed);

/**
 * @Function TurnNormalLeft(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * This will cause the bot to take a normal left turn.
 */
char TurnNormalLeft(char speed);

/**
 * @Function TurnSharpLeft(char speed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the both motors.
 * This will cause the bot to take a sharp left turn.
 */
char TurnSharpLeft(char speed);

/**
 * @Function StepperForward()
 * @param none
 * @return none
 * @brief  This function is used to set the direction of the stepper to forward.
 */
void StepperForward();

/**
 * @Function StepperReverse()
 * @param none
 * @return none
 * @brief  This function is used to set the direction of the stepper to reverse.
 */
void StepperReverse();

/**
 * @Function StepperStop()
 * @param none
 * @return none
 * @brief  This function is used to set the direction of the stepper to reverse.
 */
void StepperStop();

/**
 * @Function Bot_BatteryVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the roach
 * @brief  returns a 10:1 scaled value of the roach battery level
 */
unsigned int Bot_BatteryVoltage(void);


/**
 * @Function Bot_ReadBeaconDetector(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the beacon detector
 * @brief returns the state of the Beacon Detector
 */
unsigned int Bot_ReadBeaconVoltage(void);


/**
 * @Function Bot_ReadTrackWireSensor(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the track wire sensor.
 * @brief returns the state of the Track Wire Sensor.
 */
unsigned int Bot_ReadTrackWireVoltage(void);


/**
 * @Function Bot_LEDSSet( unsigned char pattern)
 * @param pattern - sets LEDs on (1) or off (0) as in the pattern.
 * @return SUCCESS or ERROR
 * @brief  Forces the LEDs in (bank) to on (1) or off (0) to match the pattern.
 */
char Bot_LEDSSet(uint16_t pattern);


/**
 * @Function Bot_BarGraph(uint8_t Number)
 * @param Number - value to light between 0 and 12 leds
 * @return SUCCESS or ERROR
 * @brief  allows all leds to be used as a bar graph
 */
char Bot_BarGraph(uint8_t Number);


/**
 * @Function Bot_ReadFrontLeftBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the front left bumper 
 */
unsigned char Bot_ReadFrontLeftBumper(void);


/**
 * @Function Bot_ReadFrontRightBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the front right bumper 
 */
unsigned char Bot_ReadFrontRightBumper(void);


/**
 * @Function Bot_ReadFrontcenterBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the rear left bumper
 */
unsigned char Bot_ReadFrontCenterBumper(void);


/**
 * @Function Bot_ReadBumpers(void)
 * @param None.
 * @return 3-bit value representing all three bumpers in following order: front left,front right, front center
 * @brief  Returns the state of all 3 bumpers
 */
unsigned char Bot_ReadBumpers(void);

/**
 * @Function Bot_ReadFLTapeVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the FL tape voltage.
 * @brief returns the state of the front left tape voltage.
 */
unsigned int Bot_ReadFLTapeVoltage(void);

/**
 * @Function Bot_ReadFRTapeVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the FR tape voltage.
 * @brief returns the state of the front right tape voltage.
 */
unsigned int Bot_ReadFRTapeVoltage(void);

/**
 * @Function Bot_ReadBCTapeVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the BC tape voltage.
 * @brief returns the state of the back center tape voltage.
 */
unsigned int Bot_ReadBCTapeVoltage(void);

/**
 * @Function Bot_ReadLeftBallTapeVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the left ball tape voltage.
 * @brief returns the state of the back center tape voltage.
 */
unsigned int Bot_ReadLeftBallTapeVoltage(void);

#endif	/* BOT_H */

