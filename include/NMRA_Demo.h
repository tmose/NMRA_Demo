/** -------------------------------------------------------------------------- *
 * File: NMRA_Demo.h                                                           *
 * Created Date: 10-29-2019                                                    *
 * Author: Thomas Ose                                                          *
 * --------------------------------------------------------------------------- *
 * Last Modified:                                                              *
 * Modified By:                                                                *
 * --------------------------------------------------------------------------- *
 * Copyright (c) 2016 - 2020 OMS(re)Models                                     *
 *                                                                             *
 *  This source file is subject of the GNU general public license 2,           *
 *  that is available at the world-wide-web at                                 *
 *  http://www.gnu.org/licenses/gpl.txt                                        *
 * --------------------------------------------------------------------------- *
 * HISTORY:                                                                    *
 * Date      	By	Comments                                                   *
 * ----------	---	---------------------------------------------------------- *
 * 11-03-2019	tmo	Created initial file
 * --------------------------------------------------------------------------- */

#ifndef _ClubRRCrossing_H_
#define _ClubRRCrossing_H_
//#define DEBUG_ON    /* Uncomment this line to turn on debug mode */

#include "Arduino.h"        // internal library
#include "Servo.h"          // internal library
#include "BuckBlink.h"      // OMS library
#include "CrossBarStrobe.h" // OMS library
#include "OMS_Sensor.h"     // OMS library
#include "TaskScheduler.h"  // external library - by Anatoli Arkhipenko
#include "FastLED.h"        // external library - by Daniel Garcia

/* heart beat */
#define HEARBEAT 13
#define HEARTRATESLOW 2000
#define HEARTRATEFAST 200

/* servo information for the cross bar*/
#define L_SERVO_PIN 10
#define SERVODELAY 200
#define MOTIONSTEP 1
#define MOTIONDELAY 60
#define GATEDOWN 20
#define GATEUP 0
#define GATEDELAY 700

#define NUM_LEDS 1
#define DATA_PIN 11

CRGB leds[NUM_LEDS];

/* Object definition */
CrossBarStrobe myCrossBar(5, 6, 7);
BuckBlink myCrossBuck(8, 9);
OMS_Sensor GateSensor(12);
OMS_Sensor LightButton(4);
Scheduler runner;
Servo myServo;

/* crossing signal information */
bool occupied = false;
bool isUP = false;
bool isDown = false;
bool ButtonPressed = false;
bool LightPressed = false;

String sensorTripped = "L";

/* heart beat or process monitoring*/
unsigned long heart_time = 0;
unsigned long heart_rate = HEARTRATESLOW;
int heart_beat_state = 0;
int button_state = 0;

void moveUp();
void moveDown();

void LightsOn();
void LightsOff();

// call back functions
void CheckGateSensor_Callback();
void CheckLightButton_Callback();
void CheckLightButton();
void HeartBeat_Callback();
void Gate_Start_CallBack();
void Gate_Stop_CallBack();
void CheckAll_Callback();

// definition of tasks
Task GateSensorTask(500, TASK_FOREVER, &CheckGateSensor_Callback);
Task LightButtonTask(1000, TASK_FOREVER, &CheckLightButton_Callback);
Task CheckAll(20, TASK_FOREVER, &CheckAll_Callback);
Task HeartBeat(300, TASK_FOREVER, &HeartBeat_Callback);
Task GateFunction(1000, TASK_FOREVER, &Gate_Start_CallBack);
Task CheckGateLights(20, TASK_FOREVER, &CheckAll_Callback);

// DCC Stuff

#include "NmraDcc.h"

NmraDcc Dcc;
DCC_MSG Packet;
uint8_t CV_DECODER_MASTER_RESET = 120;
int F1State;
int F3State;

struct CVPair
{
    uint16_t CV;
    uint8_t Value;
};

#define This_Decoder_Address 24

CVPair FactoryDefaultCVs[] =
    {
        // The CV Below defines the Short DCC Address
        {CV_MULTIFUNCTION_PRIMARY_ADDRESS, This_Decoder_Address},

        // These two CVs define the Long DCC Address
        {CV_MULTIFUNCTION_EXTENDED_ADDRESS_MSB, 0},
        {CV_MULTIFUNCTION_EXTENDED_ADDRESS_LSB, This_Decoder_Address},

        // ONLY uncomment 1 CV_29_CONFIG line below as appropriate
        //  {CV_29_CONFIG,                                      0}, // Short Address 14 Speed Steps
        {CV_29_CONFIG, CV29_F0_LOCATION}, // Short Address 28/128 Speed Steps
                                          //  {CV_29_CONFIG, CV29_EXT_ADDRESSING | CV29_F0_LOCATION}, // Long  Address 28/128 Speed Steps
};
uint8_t FactoryDefaultCVIndex = 0;

#endif
