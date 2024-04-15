/* 
 * File:   Bot.c
 * Author: Alex Zuo
 *
 * Created on November 16, 2019, 12:52 PM
 */

#include <Bot.h>
#include <BOARD.h>
#include <xc.h>
#include <pwm.h>
#include <serial.h>
#include <AD.h>
#include <peripheral/ports.h>
#include <IO_Ports.h>
#include <LED.h>
#include <RC_Servo.h>
#include <stdio.h>

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/

//#define BOT_TEST

//Directionality Tris
#define LEFT_DIR_TRIS   PORTY07_TRIS
#define RIGHT_DIR_TRIS  PORTY08_TRIS

//Wheel/motor Directionality
//Left is a, Right is b
#define LEFT_DIR    PORTY07_LAT
#define RIGHT_DIR   PORTY08_LAT

//Wheel PWM signals (These set speed of the DC motors and go to the enable pin on the hbridge)
//These are the enables for the motors/wheels.
#define LEFT_WHEEL_PWM      PWM_PORTY10
#define RIGHT_WHEEL_PWM     PWM_PORTY12

//RC Servo
#define RC_SERVO_SIGNAL     RC_PORTZ08

//Stepper Motor
#define STEPPER_STEP            PWM_PORTZ06
#define STEPPER_DIR_TRIS        PORTZ04_TRIS
#define STEPPER_DIR             PORTZ04_LAT
#define STEPPER_ENABLE_TRIS     PORTZ05_TRIS
#define STEPPER_ENABLE          PORTZ05_LAT

//Bumper Sensor Tris
#define MICRO_SWITCH_FRONT_LEFT_TRIS    PORTZ07_TRIS
#define MICRO_SWITCH_FRONT_RIGHT_TRIS   PORTZ11_TRIS
#define MICRO_SWITCH_FRONT_CENTER_TRIS  PORTZ09_TRIS

//Bumper Sensors
#define MICRO_SWITCH_FRONT_LEFT     PORTZ07_BIT
#define MICRO_SWITCH_FRONT_RIGHT    PORTZ11_BIT
#define MICRO_SWITCH_FRONT_CENTER   PORTZ09_BIT

//Analog-Digital Pins
#define BEACON_DETECTOR BEACON_DET   
#define TRACK_WIRE_DETECTOR TRACK_WIRE_DET 
#define FL_TAPE_SENSOR FL_TAPE_SENS
#define FR_TAPE_SENSOR FR_TAPE_SENS
#define BC_TAPE_SENSOR BC_TAPE_SENS
#define LEFT_BALL_TAPE_SENSOR LEFT_BALL_TAPE_SENS 
//#define RIGHT_BALL_TAPE_SENSOR RIGHT_BALL_TAPE_SENS

// AD pin defs in AD.h

//light bar defines
#define NUMLEDS 12

#define LED_SetPinOutput(i) *LED_TRISCLR[i] = LED_bitsMap[i]
#define LED_SetPinInput(i) *LED_TRISSET[i] = LED_bitsMap[i];
#define LED_On(i) *LED_LATCLR[(unsigned int)i] = LED_bitsMap[(unsigned int)i];
#define LED_Off(i) *LED_LATSET[(unsigned int)i] = LED_bitsMap[(unsigned int)i];
#define LED_Get(i) (*LED_LAT[(unsigned int)i]&LED_bitsMap[(unsigned int)i])


/*******************************************************************************
 * PRIVATE VARIABLES                                                           *
 ******************************************************************************/

typedef union {

    struct {
        unsigned bit0 : 1;
        unsigned bit1 : 1;
        unsigned bit2 : 1;
        unsigned bit3 : 1;
        unsigned bit4 : 1;
        unsigned bit5 : 1;
        unsigned bit6 : 1;
        unsigned bit7 : 1;
        unsigned bit8 : 1;
        unsigned bit9 : 1;
        unsigned bit10 : 1;
        unsigned bit11 : 1;
        unsigned : 4;
    };
    uint16_t c;
} LEDBank_t;


static volatile unsigned int * const LED_TRISCLR[] = {&TRISECLR, &TRISDCLR, &TRISDCLR, &TRISDCLR,
    &TRISDCLR, &TRISDCLR, &TRISDCLR, &TRISFCLR, &TRISFCLR, &TRISGCLR, &TRISFCLR, &TRISFCLR};

static volatile unsigned int * const LED_TRISSET[] = {&TRISESET, &TRISDSET, &TRISDSET, &TRISDSET,
    &TRISDSET, &TRISDSET, &TRISDSET, &TRISFSET, &TRISFSET, &TRISGSET, &TRISFSET, &TRISFSET};

static volatile unsigned int * const LED_LATCLR[] = {&LATECLR, &LATDCLR, &LATDCLR, &LATDCLR,
    &LATDCLR, &LATDCLR, &LATDCLR, &LATFCLR, &LATFCLR, &LATGCLR, &LATFCLR, &LATFCLR};

static volatile unsigned int * const LED_LATSET[] = {&LATESET, &LATDSET, &LATDSET, &LATDSET,
    &LATDSET, &LATDSET, &LATDSET, &LATFSET, &LATFSET, &LATGSET, &LATFSET, &LATFSET};

static volatile unsigned int * const LED_LAT[] = {&LATE, &LATD, &LATD, &LATD,
    &LATD, &LATD, &LATD, &LATF, &LATF, &LATG, &LATF, &LATF};

static unsigned short int LED_bitsMap[] = {BIT_7, BIT_5, BIT_10, BIT_11, BIT_3, BIT_6, BIT_7, BIT_6, BIT_4, BIT_6, BIT_5, BIT_1};

//static unsigned short int LED_ShiftAmount[] = {7, 5, 10, 11, 3, 6, 7, 6, 4, 6, 5, 1};

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                           *
 ******************************************************************************/

/**
 * @Function Bot_Init(void)
 * @param None.
 * @return None.
 * @brief  Performs all the initialization necessary for the bot. this includes initializing
 * the PWM module, the A/D converter, the data directions on some pins, and
 * setting the initial motor directions.
 * @note  None.
 */
void Bot_Init(void) {
    BOARD_Init();
    PWM_Init();
    AD_Init();
    LED_Init();
    PWM_SetFrequency(1000);
    PWM_AddPins(LEFT_WHEEL_PWM | RIGHT_WHEEL_PWM | RC_SERVO_SIGNAL | STEPPER_STEP);
    
    AD_AddPins(BC_TAPE_SENSOR | LEFT_BALL_TAPE_SENSOR  |  BEACON_DETECTOR | 
               TRACK_WIRE_DETECTOR | FL_TAPE_SENSOR | FR_TAPE_SENSOR);

    //DC Motors (wheels )
    LEFT_DIR_TRIS = 0;
    RIGHT_DIR_TRIS = 0;
    LEFT_DIR = 0;
    RIGHT_DIR = 0;

    //Stepper
    STEPPER_ENABLE_TRIS = 0;      //output
    STEPPER_DIR_TRIS = 0;         //output
    //StepperStop();
    PWM_SetDutyCycle(STEPPER_STEP, 0);
    STEPPER_ENABLE = 0;
    STEPPER_DIR = 0;

    //set up the micro switch sensors (bumpers)
    MICRO_SWITCH_FRONT_LEFT_TRIS = 1;
    MICRO_SWITCH_FRONT_RIGHT_TRIS = 1;
    MICRO_SWITCH_FRONT_CENTER_TRIS = 1;

    //set up the light bank
    uint8_t CurPin;
    for (CurPin = 0; CurPin < NUMLEDS; CurPin++) {
        LED_SetPinOutput(CurPin);
        LED_Off(CurPin);
    }
}

/*-----------------------------------------------------------------------------
 * DC Motor Functions
 * ----------------------------------------------------------------------------
 */ 
 
/**
 * @Function Bot_LeftMtrSpeed(char newSpeed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the left motor.
 */
char Bot_LeftMtrSpeed(char newSpeed) {
    if ((newSpeed < -BOT_MAX_SPEED) || (newSpeed > BOT_MAX_SPEED)) {
        return (ERROR);
    }
    newSpeed = -newSpeed;
    if (newSpeed < 0) {
        LEFT_DIR = 1;
        newSpeed = newSpeed * (-1); // set speed to a positive value
    } else {
        LEFT_DIR = 0;
    }
    if (PWM_SetDutyCycle(LEFT_WHEEL_PWM, newSpeed * (MAX_PWM / BOT_MAX_SPEED)) == ERROR) {
        //printf("ERROR: setting channel 1 speed!\n");
        return (ERROR);
    }
    return (SUCCESS);
}

/**
 * @Function Bot_RightMtrSpeed(char newSpeed)
 * @param newSpeed - A value between -100 and 100 which is the new speed
 * @param of the motor. 0 stops the motor. A negative value is reverse.
 * @return SUCCESS or ERROR
 * @brief  This function is used to set the speed and direction of the left motor.
 */
char Bot_RightMtrSpeed(char newSpeed) {
    if ((newSpeed < -BOT_MAX_SPEED) || (newSpeed > BOT_MAX_SPEED)) {
        return (ERROR);
    }
    if (newSpeed < 0) {
        RIGHT_DIR = 1;
        newSpeed = newSpeed * (-1); // set speed to a positive value
    } else {
        RIGHT_DIR = 0;
    }
    if (PWM_SetDutyCycle(RIGHT_WHEEL_PWM, newSpeed * (MAX_PWM / BOT_MAX_SPEED)) == ERROR) {
        //puts("\aERROR: setting channel 1 speed!\n");
        return (ERROR);
    }
    return (SUCCESS);
}

char DriveStraight(char speed){
    if (Bot_LeftMtrSpeed(speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TankRight(char speed){
    if (Bot_LeftMtrSpeed(speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(-speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TurnGentleRight(char speed){
    if (Bot_LeftMtrSpeed(speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(0.9*speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TurnNormalRight(char speed){
    if (Bot_LeftMtrSpeed(speed) == ERROR)
        return (ERROR);
    //originally 0.8
    if (Bot_RightMtrSpeed(0.75*speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}


char TurnSharpRight(char speed){
    if (Bot_LeftMtrSpeed(speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(0.5*speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TankLeft(char speed){
    if (Bot_LeftMtrSpeed(-speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TurnGentleLeft(char speed){
    if (Bot_LeftMtrSpeed(0.9*speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TurnNormalLeft(char speed){
    //originally 0.8
    if (Bot_LeftMtrSpeed(0.75*speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TurnSharpLeft(char speed){
    //was 0.
    if (Bot_LeftMtrSpeed(0.5*speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

char TurnHardLeft(char speed){
    //was originally 0. Follows nicely, sometimes far from wall.
    //changed to -0.7. Closer but slow.
    if (Bot_LeftMtrSpeed(-0.3*speed) == ERROR)
        return (ERROR);
    if (Bot_RightMtrSpeed(speed) == ERROR)
        return (ERROR);
    return (SUCCESS);
}

/*-----------------------------------------------------------------------------
 * Stepper Motor Functions
 * Stepper direction 1 goes down.
 * Stepper direction 0 goes up.
 * ----------------------------------------------------------------------------
 */ 

void StepperForward(){
    PWM_SetDutyCycle(STEPPER_STEP, 100);
    PWM_SetFrequency(50);
    STEPPER_ENABLE = 1;
    STEPPER_DIR = 0;
}

void StepperReverse(){
    PWM_SetDutyCycle(STEPPER_STEP, 100);
    PWM_SetFrequency(50);
    STEPPER_ENABLE = 1;
    STEPPER_DIR = 1;
}

void StepperStop(){
    STEPPER_ENABLE = 0;
    PWM_SetDutyCycle(STEPPER_STEP, 0);
    PWM_SetFrequency(1000);
}

/*------------------------------------------------------------------------------
 * Voltage Reading Functions
 * ----------------------------------------------------------------------------
 */ 

/**
 * @Function Bot_BatteryVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the bot
 * @brief  returns a 10:1 scaled value of the bot battery level
 */
unsigned int Bot_BatteryVoltage(void) {
    return AD_ReadADPin(BAT_VOLTAGE);
}

/**
 * @Function Bot_ReadBeaconDetector(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the beacon detector
 * @brief returns the state of the Beacon Detector
 */
unsigned int Bot_ReadBeaconVoltage(void) {
    return AD_ReadADPin(BEACON_DETECTOR);
}

/**
 * @Function Bot_ReadTrackWireSensor(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the track wire sensor.
 * @brief returns the state of the Track Wire Sensor.
 */
unsigned int Bot_ReadTrackWireVoltage(void) {
    return AD_ReadADPin(TRACK_WIRE_DETECTOR);
}

/*------------------------------------------------------------------------------
 * LED Functions
 * ----------------------------------------------------------------------------
 */

/**
 * @Function Bot_LEDSSet( unsigned char pattern)
 * @param pattern - sets LEDs on (1) or off (0) as in the pattern.
 * @return SUCCESS or ERROR
 * @brief  Forces the LEDs in (bank) to on (1) or off (0) to match the pattern.
 * @author Gabriel Hugh Elkaim, 2011.12.25 01:16 Max Dunne 2015.09.18 */
char Bot_LEDSSet(uint16_t pattern) {
    char i;
    for (i = 0; i < NUMLEDS; i++) {
        if (pattern & (1 << i)) {
            LED_On(i);
        } else {
            LED_Off(i);
        }
    }
    return SUCCESS;
}

char Bot_BarGraph(uint8_t Number) {
    if (Number > NUMLEDS) {
        return ERROR;
    }
    uint16_t Pattern = 0;
    uint8_t iterations;

    for (iterations = 0; iterations < Number; iterations++) {
        Pattern <<= 1;
        Pattern |= 1;
    }
    Bot_LEDSSet(Pattern);
    return SUCCESS;
}

/*------------------------------------------------------------------------------
 * Bumper Sensors Functions
 * ----------------------------------------------------------------------------
 */

/**
 * @Function Bot_ReadFrontLeftBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the front left bumper 
 */
unsigned char Bot_ReadFrontLeftBumper(void) {
    return MICRO_SWITCH_FRONT_LEFT;
}

/**
 * @Function Bot_ReadFrontRightBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the front right bumper 
 */
unsigned char Bot_ReadFrontRightBumper(void) {
    return MICRO_SWITCH_FRONT_RIGHT;
}

/**
 * @Function Bot_ReadFrontcenterBumper(void)
 * @param None.
 * @return BUMPER_TRIPPED or BUMPER_NOT_TRIPPED
 * @brief  Returns the state of the rear left bumper
 */
unsigned char Bot_ReadFrontCenterBumper(void) {
    return MICRO_SWITCH_FRONT_CENTER;
}

/**
 * @Function Bot_ReadBumpers(void)
 * @param None.
 * @return 3-bit value representing all three bumpers in following order: front left,front center, front right
 * @brief  Returns the state of all 3 bumpers
 */
unsigned char Bot_ReadBumpers(void) {
    //unsigned char bump_state;
    //bump_state = (!MICRO_SWITCH_FRONT_LEFT + ((!MICRO_SWITCH_FRONT_RIGHT) << 1)+((!MICRO_SWITCH_FRONT_CENTER) << 2));
    return (!MICRO_SWITCH_FRONT_LEFT + ((!MICRO_SWITCH_FRONT_CENTER) << 1)+((!MICRO_SWITCH_FRONT_RIGHT) << 2));
}

/*------------------------------------------------------------------------------
 * Tape Sensors Functions
 * ----------------------------------------------------------------------------
 */

/**
 * @Function Bot_ReadFLTapeVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the FL tape voltage.
 * @brief returns the state of the front left tape sensor
 */
unsigned int Bot_ReadFLTapeVoltage(void) {
    return AD_ReadADPin(FL_TAPE_SENSOR);
}

/**
 * @Function Bot_ReadFRTapeSensor(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the FR tape voltage.
 * @brief returns the state of the front left tape sensor
 */
unsigned int Bot_ReadFRTapeVoltage(void) {
    return AD_ReadADPin(FR_TAPE_SENSOR);
}

/**
 * @Function Bot_ReadBCTapeSensor(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the BC tape voltage.
 * @brief returns the state of the front left tape sensor
 */
unsigned int Bot_ReadBCTapeVoltage(void) {
    return AD_ReadADPin(BC_TAPE_SENSOR);
}

/**
 * @Function Bot_ReadLeftBallTapeVoltage(void)
 * @param None.
 * @return a 10-bit value corresponding to the current voltage of the left ball tape voltage.
 * @brief returns the state of the back center tape voltage.
 */
unsigned int Bot_ReadLeftBallTapeVoltage(void){
    return AD_ReadADPin(LEFT_BALL_TAPE_SENSOR);
}

#ifdef BOT_TEST

// These are the different possible tests
//#define LED_TEST
//#define BUMPER_TEST
#define MOTOR_TEST
//#define TAPE_SENSOR_TEST
//#define BEACON_DETECTOR_TEST
//#define TRACK_WIRE_TEST
//#define STEPPER_TEST

#define DELAY(x)    for (wait = 0; wait <= x; wait++) {asm("nop");}
#define ONE_SECOND          666667
#define HALF_SECOND         (ONE_SECOND>>1)
#define QUARTER_SECOND      (ONE_SECOND>>2)
#define EIGTH_SECOND        (ONE_SECOND>>3)
#define SIXTHEENTH_SECOND   (ONE_SECOND>>4)
#define TWO_SECONDS         (ONE_SECOND<<1)
#define ONE_AND_HALF_SECOND 1000000

#define NO_BUMPERS 0b0000
#define ALL_BUMPERS 0b1111
#define FILL_LED 12
#define CLEAR_LED 0
#define LOW_BAT 263
#define HIGH_BAT 310

#define FL_BUMP_MASK (1)
#define FR_BUMP_MASK (1<<1)
#define FC_BUMP_MASK (1<<2)

void FlashLEDBar(uint8_t numtimes);

int main(void) {
    
    BOARD_Init();
    Bot_Init();
    LED_Init();
    uint16_t LED_Input = 0;
    char Bumpers = 0;
    unsigned int Battery = 0;
    unsigned int wait = 0;
    char speed = 0;
    
    //Introductions
    printf("Welcome to the Bot Test Harness\r\n");
    printf("Uncomment the #define of whatever test you want to run in the code\r\n");
    
    #ifdef LED_TEST
    FlashLEDBar(10);
    #endif

    #ifdef BUMPER_TEST
    uint16_t lastFCEvent = 0;
    uint16_t lastFREvent = 0;
    uint16_t lastFLEvent = 0;
    while (1) {
        //Bumpers = Bot_ReadBumpers();
        //Setting Events.
        uint16_t curFLEvent = Bot_ReadFrontLeftBumper();
        //printf("Front Left bumper state is: %d\r\n", curFLEvent);
        uint16_t curFREvent = Bot_ReadFrontRightBumper();
        //printf("Front Right bumper state is: %d\r\n", curFREvent);
        uint16_t curFCEvent = Bot_ReadFrontCenterBumper();
        //printf("Front Center bumper state is: %d\r\n", curFCEvent);
        
        if ((lastFLEvent == BUMPER_NOT_TRIPPED) && (curFLEvent == BUMPER_TRIPPED)){
            printf("Front Left Bumper is bumped\r\n");
        }
        if ((lastFCEvent == BUMPER_NOT_TRIPPED) && (curFCEvent == BUMPER_TRIPPED)){
            printf("Front Center Bumper is bumped\r\n");
        }
        if ((lastFREvent == BUMPER_NOT_TRIPPED) && (curFREvent == BUMPER_TRIPPED)){
            printf("Front Right Bumper is bumped\r\n");
        }
        if ((lastFLEvent == BUMPER_TRIPPED) && (curFLEvent == BUMPER_NOT_TRIPPED)){
            printf("Front Left Bumper is not bumped\r\n");
        }
        if ((lastFREvent == BUMPER_TRIPPED) && (curFREvent == BUMPER_NOT_TRIPPED)){
            printf("Front Right Bumper is not bumped\r\n");
        }
        if ((lastFCEvent == BUMPER_TRIPPED) && (curFCEvent == BUMPER_NOT_TRIPPED)){
            printf("Front Center Bumper is not bumped\r\n");
        }
        lastFLEvent = curFLEvent;
        lastFREvent = curFREvent;
        lastFCEvent = curFCEvent;
        DELAY(10000);
    }
    
    #endif

    #ifdef MOTOR_TEST
    while (1){
        if (DriveStraight(75) == ERROR)
            printf("Error: Motor not ran properly\r\n");
        else
            printf("Success: Motor ran properly\r\n");
        DELAY(250000);
        Battery = Bot_BatteryVoltage();
        printf("Battery Voltage is: %d\r\n", Battery);
    }
    #endif 

    #ifdef STEPPER_TEST
    while (1){
        StepperForward();
        DELAY(250000);
        StepperReverse();
        DELAY(250000);
    }
    #endif
    
    #ifdef TAPE_SENSOR_TEST
    while (1){
        //Tape Sensor voltage readings.
        uint16_t FLTapeVolt = Bot_ReadFLTapeVoltage();
        uint16_t FRTapeVolt = Bot_ReadFRTapeVoltage();
        uint16_t BCTapeVolt = Bot_ReadBCTapeVoltage();
        uint16_t LeftBallTapeVolt = Bot_ReadLeftBallTapeVoltage();
        
        
        //Front Left Tape Sensor
        if (FLTapeVolt > 700){
            printf("Front Left Tape has been detected.\r\n");
            printf("Front Left Tape Voltage reads: %d\r\n", FLTapeVolt);
        }
        else if (FLTapeVolt < 400){
            printf("Front Left Tape has not been detected.\r\n");
            printf("Front Left Tape Voltage reads: %d\r\n", FLTapeVolt);
        }
        else {
            printf("Front Left Tape detection has not changed.\r\n");
            printf("Front Left Tape Voltage reads: %d\r\n", FLTapeVolt);
        }
        DELAY(400000);
        /*
        //Front Right Tape Sensor
        if (FRTapeVolt > 700){
            printf("Front Right Tape has been detected.\r\n");
            printf("Front Right Tape Voltage reads: %d\r\n", FRTapeVolt);
        }
        else if (FRTapeVolt < 400){
            printf("Front Right Tape has not been detected.\r\n");
            printf("Front Right Tape Voltage reads: %d\r\n", FRTapeVolt);
        }
        else {
            printf("Front Right Tape detection has not changed.\r\n");
            printf("Front Right Tape Voltage reads: %d\r\n", FRTapeVolt);
        }
        DELAY(400000);*/
        /*
        //Back Center Tape Sensor
        if (BCTapeVolt > 700){
            printf("Back Center Tape has been detected.\r\n");
            printf("Back Center Tape Voltage reads: %d\r\n", BCTapeVolt);
        }
        else if (BCTapeVolt < 400){
            printf("Back Center Tape has not been detected.\r\n");
            printf("Back Center Tape Voltage reads: %d\r\n", BCTapeVolt);
        }
        else {
            printf("Back Center Tape detection has not changed.\r\n");
            printf("Back Center Tape Voltage reads: %d\r\n", BCTapeVolt);
        }
        DELAY(400000);*/
        Battery = Bot_BatteryVoltage();
        printf("Battery Voltage is: %d\r\n", Battery);
        //Not yet connected
        /*
        //Left Ball Tape Sensor
        if (LeftBallTapeVolt > 700){
            printf("Left Ball Tape has been detected.\r\n");
            printf("Left Ball Tape Voltage reads: %d\r\n", LeftBallTapeVolt);
        }
        else if (LeftBallTapeVolt < 400){
            printf("Front Left Tape has not been detected.\r\n");
            printf("Front Left Tape Voltage reads: %d\r\n", LeftBallTapeVolt);
        }
        else {
            printf("Front Left Tape detection has not changed.\r\n");
            printf("Front Left Tape Voltage reads: %d\r\n", LeftBallTapeVolt);
        }
        DELAY(400000);
         */
    }
    
    
    #endif 

    #ifdef BEACON_DETECTOR_TEST
    while (1){
        uint16_t beaconVolt = Bot_ReadBeaconVoltage();
        if (beaconVolt > 700){
            printf("Beacon has been detected.\r\n");
            printf("Beacon Voltage reads: %d\r\n", beaconVolt);
        }
        else if (beaconVolt < 400){
            printf("Beacon has not been detected.\r\n");
            printf("Beacon Voltage reads: %d\r\n", beaconVolt);
        }
        else {
            printf("Beacon detection has not changed.\r\n");
            printf("Beacon Voltage reads: %d\r\n", beaconVolt);
        }
        Battery = Bot_BatteryVoltage();
        printf("Battery Voltage is: %d\r\n", Battery);
        DELAY(750000);
    }
    #endif 

    #ifdef TRACK_WIRE_TEST
    while (1){
        uint16_t trackWireVolt = Bot_ReadTrackWireVoltage();
        if (trackWireVolt > 700){
            printf("Track Wire has been detected.\r\n");
            printf("Track Wire Voltage reads: %d\r\n", trackWireVolt);
        }
        else if (trackWireVolt < 400){
            printf("Track Wire has not been detected.\r\n");
            printf("Track Wire Voltage reads: %d\r\n", trackWireVolt);
        }
        else {
            printf("Track Wire detection has not changed.\r\n");
            printf("Track Wire Voltage reads: %d\r\n", trackWireVolt);
        }
        Battery = Bot_BatteryVoltage();
        printf("Battery Voltage is: %d\r\n", Battery);
        DELAY(750000);
    }
    
    
    #endif 
/*
    Bot_BarGraph(FILL_LED);
    DELAY(HALF_SECOND);
    //LED BUmper Test
    while (Bumpers != ALL_BUMPERS) {
        LED_Input = 0;
        Bumpers = Bot_ReadBumpers();
        LED_Input = ((FC_BUMP_MASK & Bumpers)<<8)|((FC_BUMP_MASK & Bumpers)<<7)|((FC_BUMP_MASK & Bumpers)<<6)|
                ((FR_BUMP_MASK & Bumpers)<<5)|((FR_BUMP_MASK & Bumpers)<<4)|((FR_BUMP_MASK & Bumpers)<3)|
                ((FL_BUMP_MASK & Bumpers)<<2)|((FL_BUMP_MASK & Bumpers)<<1)|((FL_BUMP_MASK & Bumpers));

        Bot_LEDSSet(LED_Input);
 
    }
 
    // Wait for Bumper Release
    printf("Release all bumpers to Proceed.\n");
    while (Bumpers != NO_BUMPERS) {
        Bumpers = Bot_ReadBumpers();
    }
 
    Bot_BarGraph(FILL_LED);
    DELAY(HALF_SECOND);
    Bot_LEDSSet(CLEAR_LED);
    //Directions
    printf("Front Left Bumper = Left Motor Test\r\n");
    printf("Front Right Bumper = Right Motor Test\r\n");
    printf("Back Center Bumper = Display Battery Voltage \r\n");
    printf("BL:Right Motor \r\n");
 
 
 
 
    // Four Tests
    while (1) {
        Bumpers = Bot_ReadBumpers();
        
        //Left Motor
        if (Bumpers == FL_BUMP_MASK) {
            LED_Input = 0b11111;
            speed = 100;
            while (speed != 0) {
                DELAY(QUARTER_SECOND);
                Bot_LEDSSet(LED_Input);
                Bot_LeftMtrSpeed(speed);
                printf("LeftMotorSpeed = %d\n", speed);
                DELAY(ONE_AND_HALF_SECOND);
                speed -= 20;
                LED_Input >>= 1;
 
            }
            Bot_LeftMtrSpeed(CLEAR_LED);
            LED_Input = 0b11111;
            speed = -100;
            while (speed != 0) {
                DELAY(QUARTER_SECOND);
                Bot_LEDSSet(LED_Input);
                Bot_LeftMtrSpeed(speed);
                printf("LeftMotorSpeed = %d\n", speed);
                DELAY(ONE_AND_HALF_SECOND);
                speed += 20;
                LED_Input >>= 1;
            }
            Bot_LeftMtrSpeed(CLEAR_LED);
            printf("FR:Light \r\n");
            printf("FL:Battery \r\n");
            printf("BR:Left Motor \r\n");
            printf("BL:Right Motor \r\n");
 
            Roach_LEDSSet(CLEAR_LED);
        }
        
        // Right Motor
        if (Bumpers == B_R) {
            LED_Input = 0b11111;
            speed = 100;
            while (speed != 0) {
                DELAY(A_LOT);
                Roach_LEDSSet(LED_Input);
                printf("RightMotorSpeed = %d\n", speed);
                Roach_RightMtrSpeed(speed);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                speed -= 20;
                LED_Input >>= 1;
            }
            Roach_RightMtrSpeed(CLEAR_LED);
            speed = -100;
            LED_Input = 0b11111;
            while (speed != 0) {
                DELAY(A_LOT);
                Roach_LEDSSet(LED_Input);
                printf("RightMotorSpeed = %d\n", speed);
                Roach_RightMtrSpeed(speed);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                DELAY(A_LOT);
                speed += 20;
                LED_Input >>= 1;
 
            }
            Roach_RightMtrSpeed(CLEAR_LED);
            printf("FR:Light \r\n");
            printf("FL:Battery \r\n");
            printf("BR:Left Motor \r\n");
            printf("BL:Right Motor \r\n");
 
            Roach_LEDSSet(CLEAR_LED);
        }
        
        //Battery
        if (Bumpers == F_R) {
            DELAY(A_BIT);
            printf("Release all bumpers to Proceed.\n");
            while (Bumpers != NO_BUMPERS) {
                Bumpers = Roach_ReadBumpers();
            }
            DELAY(A_BIT);
            while (Bumpers == NO_BUMPERS) {
                DELAY(A_LOT);
                DELAY(A_LOT);
                Bumpers = Roach_ReadBumpers();
                Battery = Roach_BatteryVoltage();
                LED_Input = (Battery-LOW_BAT )/ 4;
                if (LED_Input > 12)
                    LED_Input = 12;
                if (Battery < LOW_BAT)
                    LED_Input = 0;
                Roach_BarGraph(LED_Input);
                printf("Battery = %d\n", Battery);
            }
            DELAY(A_BIT);
            printf("Release all bumpers to Proceed.\n");
            while (Bumpers != NO_BUMPERS) {
                Bumpers = Roach_ReadBumpers();
            }
            DELAY(A_BIT);
            printf("FR:Light \r\n");
            printf("FL:Battery \r\n");
            printf("BR:Left Motor \r\n");
            printf("BL:Right Motor \r\n");
 
            Roach_LEDSSet(CLEAR_LED);
        }
        
        //light
        if (Bumpers == F_L) {
            DELAY(A_BIT);
            printf("Release all bumpers to Proceed.\n");
            while (Bumpers != NO_BUMPERS) {
                Bumpers = Bot_ReadBumpers();
            }
            DELAY(A_BIT);
            while (Bumpers == NO_BUMPERS) {
                DELAY(A_LOT);
                DELAY(A_LOT);
 
                Bumpers = Bot_ReadBumpers();
                Light = Bot_LightLevel();
                LED_Input = Light;
                //Roach_LEDSSet(LED_Input);
                Roach_BarGraph((1023 - LED_Input) / 86);
                printf("Light = %d\n", Light);
            }
            DELAY(A_BIT);
            printf("Release all bumpers to Proceed.\n");
            while (Bumpers != NO_BUMPERS) {
                Bumpers = Roach_ReadBumpers();
            }
            DELAY(A_BIT);
            printf("FR:Light \r\n");
            printf("FL:Battery \r\n");
            printf("BR:Left Motor \r\n");
            printf("BL:Right Motor \r\n");
 
            Roach_LEDSSet(CLEAR_LED);
        }
    }
*/
}
void FlashLEDBar(uint8_t numtimes) {
    unsigned int wait, i;
    Bot_LEDSSet(0);
    for (i = 0; i < numtimes; i++) {
        Bot_LEDSSet(0xFFF);
        DELAY(QUARTER_SECOND);
        Bot_LEDSSet(0x000);
        DELAY(QUARTER_SECOND);
    }
}

#endif