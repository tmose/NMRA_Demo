/** --------------------------------------------------------------------------
 * File: NMRA_Demo.cpp                                                         *
 * Created Date: 10-29-2019                                                    *
 * Author: Thomas Ose                                                          *
 * --------------------------------------------------------------------------- *
 * Last Modified:                                                              *
 * Modified By:                                                                *
 * --------------------------------------------------------------------------- *
 * Copyright (c) 2019 OMS(re)Models                                            *
 *                                                                             *
 *  This source file is subject of the GNU general public license 2,           *
 *  that is available at the world-wide-web at                                 *
 *  http://www.gnu.org/licenses/gpl.txt                                        *
 * --------------------------------------------------------------------------- *
 * HISTORY:                                                                    *
 * Date      	By	Comments                                                   *
 * ----------	---	---------------------------------------------------------- *
 * 11-03-2019	tmo	Created initial code
 * --------------------------------------------------------------------------- */


#include "NMRA_Demo.h"

/**
 * @brief  Check the system heart beat
 * @note   The heart beat indicates that the processor is working 
 *          there are two states fast for when the gate is down and slow when not
 * @retval None
 */
void HeartBeat_Callback()
{
    if ((heart_time + heart_rate) > millis())
        return;
    heart_beat_state = ~heart_beat_state;
    digitalWrite(HEARBEAT, heart_beat_state);
    heart_time = millis();
}

/**
 *
 * @brief  This is the select key call back
 * @note   When the select key is pressed then toggle the speed type
 * @retval None
 */
void CheckGateSensor_Callback()
{
    ButtonPressed = GateSensor.isActive() ? false : true;
    if (ButtonPressed)
    {
        Serial.println("Button Pressed: ");
        if (occupied)
        {
            GateFunction.setCallback(&Gate_Stop_CallBack);
            Serial.println("Gate Turned OFF");
        }
        else
        {
            GateFunction.setCallback(&Gate_Start_CallBack);
            GateFunction.enable();
            Serial.println("Gate Turned ON");
        }
    }
}

/**
 * @brief  Check to see if the light button has been pressed
 * @note   
 * @retval None
 */
void CheckLightButton_Callback()
{
    LightPressed = LightButton.isActive() ? false : true;
    CheckLightButton();
}

/**
 * @brief  Checks to see if the light button has been pressed
 * @note   Cycles through the three colors and off
 * @retval None
 */
void CheckLightButton()
{
    if (LightPressed)
    {
        button_state += 1;
        if (button_state > 3)
            button_state = 0;

        switch (button_state)
        {
        case 0:
            leds[0] = CRGB::Black;
            break;
        case 1:
            leds[0] = CRGB::Green;
            break;
        case 2:
            leds[0] = CRGB::Yellow;
            break;
        case 3:
            leds[0] = CRGB::Red;
            break;
        default:
            break;
        }
        FastLED.show();
    }
    LightPressed = false;
}

/**
 * @brief  Check all and update all the lights
 * @note   
 * @retval None
 */
void CheckAll_Callback()
{
    if (occupied)
    {
        myCrossBar.update();
        myCrossBuck.update();
    }
}

/**
 * @brief  Callback Routine to start the Gate Down
 * @note   
 * @retval None
 */
void Gate_Start_CallBack()
{
    occupied = true;
    LightsOn();
    moveDown();
    heart_rate = HEARTRATEFAST;
}

/**
 * @brief  Callback routine to bring gate back up
 * @note   
 * @retval None
 */
void Gate_Stop_CallBack()
{
    occupied = false;
    LightsOff();
    moveUp();
    heart_rate = HEARTRATESLOW;
    GateFunction.setCallback(&Gate_Start_CallBack);
    GateFunction.disable();
}

/**
 * @brief  Initialize the runner and add the tasks 
 * @note   Each task is enabled after it has been added
 * @retval None
 */
void setupTaskRunner()
{
    runner.init();

    runner.addTask(HeartBeat);
    HeartBeat.enable();
    Serial.println("Heartbeat task added");

    runner.addTask(GateSensorTask);
    GateSensorTask.enable();
    Serial.println("CheckGateSensor  task added");

    runner.addTask(LightButtonTask);
    LightButtonTask.enable();
    Serial.println("Light button task added");

    runner.addTask(GateFunction);
    GateFunction.disable();
    Serial.println("Gate Function Task Added but stopped");

    runner.addTask(CheckAll);
    CheckAll.enable();
    Serial.println("Light Flasher enabled");
}

/**
 * @brief  the setup function runs once when you press reset or power the board
 * @note   
 * @retval None
 */
void setup()
{
    Serial.begin(9600);
    heart_rate = HEARTRATESLOW;

    // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
    Dcc.pin(0, 2, 0);
    // Call the main DCC Init function to enable the DCC Receiver
    Dcc.init(MAN_ID_DIY, 10, FLAGS_MY_ADDRESS_ONLY, 0);
    notifyCVResetFactoryDefault();

    delay(2000);
    FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);

    setupTaskRunner();
    LightsOff();
    moveUp();
}

/**
 * @brief  Turn on the lights
 * @note   
 * @retval None
 */
void LightsOn()
{
    if (!myCrossBar.isRunning())
        myCrossBar.on();
    if (!myCrossBuck.isRunning())
        myCrossBuck.on();
}

/**
 * @brief  Turn the Lights Off
 * @note   
 * @retval None
 */
void LightsOff()
{
    myCrossBar.off();
    myCrossBuck.off();
}

/**
 * @brief  Move the crossing gate up
 * @note   
 * @retval None
 */
void moveUp()
{
    if (isUP == false)
    {
        myServo.attach(L_SERVO_PIN);
        int step = GATEDOWN;
        while (step > GATEUP)
        {
            step -= MOTIONSTEP;
            myServo.write(step);
            delay(MOTIONDELAY);
        }
        delay(SERVODELAY);
        myServo.detach();
        isUP = true;
        isDown = false;
    }
}

/**
 * @brief  Moves the gates up
 * @note   
 * @retval None
 */
void moveDown()
{
    if (isDown == false)
    {
        myServo.attach(L_SERVO_PIN);
        int step = GATEUP;
        while (step < GATEDOWN)
        {
            step += MOTIONSTEP;
            myServo.write(step);
            delay(MOTIONDELAY);
        }

        delay(SERVODELAY);
        myServo.detach();
        isUP = false;
        isDown = true;
    }
}

/**
 * @brief   Make FactoryDefaultCVIndex non-zero and equal to num CV's to be reset
 *          to flag to the loop() function that a reset to Factory Defaults needs to be done
 * @note   
 * @retval None
 */
void notifyCVResetFactoryDefault()
{
    FactoryDefaultCVIndex = sizeof(FactoryDefaultCVs) / sizeof(CVPair);
};

/**
 * @brief  is called when ever a packet is received and used to decode the functions
 * @note   
 * @param  Addr:        DCC address of device
 * @param  AddrType:    Type of address Long or Short
 * @param  FuncGrp:     The decoder function group
 * @param  FuncState:   THe state ( on or off) for each function within the group
 * @retval None
 */
void notifyDccFunc(uint16_t Addr, DCC_ADDR_TYPE AddrType, FN_GROUP FuncGrp, uint8_t FuncState)
{
#ifdef DEBUG_ON
    Serial.print("notifyDccFunc: Addr: ");
    Serial.print(Addr, DEC);
    Serial.print((AddrType == DCC_ADDR_SHORT) ? 'S' : 'L');
    Serial.print("  Function Group: ");
    Serial.print(FuncGrp, DEC);
#endif

    switch (FuncGrp)
    {
    case FN_0_4:
    {
        if (Dcc.getCV(CV_29_CONFIG) & CV29_F0_LOCATION) // Only process Function 0 in this packet if we're not in Speed Step 14 Mode
        {
            //int fn0 = (FuncState & FN_BIT_00) ? 1 : 0;
        }

        int fn1 = (FuncState & FN_BIT_01) ? 1 : 0;
        int fn3 = (FuncState & FN_BIT_03) ? 1 : 0;

        if (fn1 != F1State)
        {
            F1State = fn1;
            if (isUP)
            {
                Gate_Start_CallBack();
            }
            else
            {
                Gate_Stop_CallBack();
            }
        }

        if (fn3 != F3State)
        {
            F3State = fn3;
            LightPressed = true;
            CheckLightButton();
        }
    }
    break;
        /*  The following is sample code for the other groups of functions */
    case FN_5_8:
    {
        //int fn5 = (FuncState & FN_BIT_05) ? 1 : 0;
    }
    break;

    case FN_9_12:
    {
        //int fn9 = (FuncState & FN_BIT_09) ? 1 : 0;
    }
    break;

    case FN_13_20:
    {
        //int fn13 = (FuncState & FN_BIT_13) ? 1 : 0;
    }
    break;

    case FN_21_28:
    {
        //int fn21 = (FuncState & FN_BIT_21) ? 1 : 0;
    }
    break;
    default:
        break;
    }
}

/**
 * @brief  the loop function runs over and over again forever.
 * @note   
 * @retval None
 */
void loop()
{
    runner.execute();
    // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
    Dcc.process();
    if (FactoryDefaultCVIndex && Dcc.isSetCVReady())
    {
        FactoryDefaultCVIndex--; // Decrement first as initially it is the size of the array
        Dcc.setCV(FactoryDefaultCVs[FactoryDefaultCVIndex].CV, FactoryDefaultCVs[FactoryDefaultCVIndex].Value);
    }
}
