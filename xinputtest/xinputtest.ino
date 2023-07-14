/*
 *  Project     Arduino XInput Library
 *  @author     David Madison
 *  @link       github.com/dmadison/ArduinoXInput
 *  @license    MIT - Copyright (c) 2019 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 *  Example:      GamepadPins
 *  Description:  Uses all of the available pin inputs to build a 'complete'
 *                Xbox gamepad, with both analog joysticks, both triggers,
 *                and all of the main buttons.
 *
 *                * Joysticks should be your typical 10k dual potentiometers.
 *                  To prevent random values caused by floating inputs,
                    joysticks are disabled by default.
 *                * Triggers can be either analog (pots) or digital (buttons).
 *                  Set the 'TriggerButtons' variable to change between the two.
 *                * Buttons use the internal pull-ups and should be connected
 *                  directly to ground.
 *
 *                These pins are designed around the Leonardo's layout. You
 *                may need to change the pin numbers if you're using a
 *                different board type
 *
 */

#include <XInput.h>
#include "HID.h"

// Setup
const boolean UseLeftJoystick   = false;  // set to true to enable left joystick
const boolean InvertLeftYAxis   = false;  // set to true to use inverted left joy Y

const boolean UseRightJoystick  = false;  // set to true to enable right joystick
const boolean InvertRightYAxis  = false;  // set to true to use inverted right joy Y

const boolean UseTriggerButtons = false;   // set to false if using analog triggers

const boolean UseL3R3Buttons = false;

const int ADC_Max = 1023;  // 10 bit

// Joystick Pins
const int Pin_LeftJoyX = 110;
//A0;
const int Pin_LeftJoyY = 111;
//A1;
const int Pin_RightJoyX = 112;
//A2;
const int Pin_RightJoyY = 113;
//A3;

// Trigger Pins
const int Pin_TriggerL = 114;
//A4;
const int Pin_TriggerR = 115;
//A5;

// Button Pins
const int Pin_ButtonA = 6;
// 0;
const int Pin_ButtonB = 7;
// 1;
const int Pin_ButtonX = 8;
// 2;
const int Pin_ButtonY = 9;
// 3;

const int Pin_ButtonLB = 10;
// 4;
const int Pin_ButtonRB = 16;
// 5;

const int Pin_ButtonBack = 14;
// 6;

// start 버튼 작동안함
const int Pin_ButtonStart = 15;
//7;

const int Pin_ButtonLogo = 18;

const int Pin_ButtonL3 = 108;
// 8;
const int Pin_ButtonR3 = 109;
// 9;

// Directional Pad Pins
const int Pin_DpadUp    = 2; //10;
const int Pin_DpadDown  = 3; //11;
const int Pin_DpadLeft  = 4; //12;
const int Pin_DpadRight = 5; //13;

const uint8_t xbox_gamepad_hid_report_descriptor[] = {
	0x05, 0x01, // Usage Page (Generic Desktop Ctrls)
	0x09, 0x05, // Usage (Game Pad)
	0xa1, 0x01, // Collection (Application)
	0xa1, 0x00, //   Collection (Physical)

	// 버튼들에 대한 리포트 디스크립터
	0x05, 0x09, //     Usage Page (Button)
	0x19, 0x01, //     Usage Minimum (Button 1)
	0x29, 0x0A, //     Usage Maximum (Button 10)
	0x15, 0x00, //     Logical Minimum (0)
	0x25, 0x01, //     Logical Maximum (1)
	0x75, 0x01, //     Report Size (1)
	0x95, 0x0A, //     Report Count (10)
	0x81, 0x02, //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

	// 축들에 대한 리포트 디스크립터
	0x05, 0x01, //     Usage Page (Generic Desktop Ctrls)
	0xa1, 0x00, //     Collection (Physical)
	0x09, 0x30, //       Usage (X)
	0x09, 0x31, //       Usage (Y)
	0x09, 0x32, //       Usage (Z)
	0x15, 0x81, //       Logical Minimum (-127)
	0x25, 0x7F, //       Logical Maximum (127)
	0x75, 0x08, //       Report Size (8)
	0x95, 0x03, //       Report Count (3)
	0x81, 0x02, //       Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)

	0xc0, //     End Collection
	0xc0  //   End Collection
};

static const uint8_t hori_pad_hid_report_descriptor[] PROGMEM = {
	0x05, 0x01,		  // Usage Page (Generic Desktop Ctrls)
	0x09, 0x05,		  // Usage (Game Pad)
	0xA1, 0x01,		  // Collection (Application)
	0x85, 0x01,		  // Report ID, not in the original descriptor, but the Arduino HID library can't seem to live without it
	0x15, 0x00,		  //   Logical Minimum (0)
	0x25, 0x01,		  //   Logical Maximum (1)
	0x35, 0x00,		  //   Physical Minimum (0)
	0x45, 0x01,		  //   Physical Maximum (1)
	0x75, 0x01,		  //   Report Size (1)
	0x95, 0x0D,		  //   Report Count (13)
	0x05, 0x09,		  //   Usage Page (Button)
	0x19, 0x01,		  //   Usage Minimum (0x01)
	0x29, 0x0D,		  //   Usage Maximum (0x0D)
	0x81, 0x02,		  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x95, 0x03,		  //   Report Count (3)
	0x81, 0x01,		  //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x05, 0x01,		  //   Usage Page (Generic Desktop Ctrls)
	0x25, 0x07,		  //   Logical Maximum (7)
	0x46, 0x3B, 0x01, //   Physical Maximum (315)
	0x75, 0x04,		  //   Report Size (4)
	0x95, 0x01,		  //   Report Count (1)
	0x65, 0x14,		  //   Unit (System: English Rotation, Length: Centimeter)
	0x09, 0x39,		  //   Usage (Hat switch)
	0x81, 0x42,		  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
	0x65, 0x00,		  //   Unit (None)
	0x95, 0x01,		  //   Report Count (1)
	0x81, 0x01,		  //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x26, 0xFF, 0x00, //   Logical Maximum (255)
	0x46, 0xFF, 0x00, //   Physical Maximum (255)
	0x09, 0x30,		  //   Usage (X)
	0x09, 0x31,		  //   Usage (Y)
	0x09, 0x32,		  //   Usage (Z)
	0x09, 0x35,		  //   Usage (Rz)
	0x75, 0x08,		  //   Report Size (8)
	0x95, 0x04,		  //   Report Count (4)
	0x81, 0x02,		  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x06, 0x00, 0xFF, //   Usage Page (Vendor Defined 0xFF00)
	0x09, 0x20,		  //   Usage (0x20)
	0x09, 0x21,		  //   Usage (0x21)
	0x09, 0x22,		  //   Usage (0x22)
	0x09, 0x23,		  //   Usage (0x23)
	0x09, 0x24,		  //   Usage (0x24)
	0x09, 0x25,		  //   Usage (0x25)
	0x09, 0x26,		  //   Usage (0x26)
	0x09, 0x27,		  //   Usage (0x27)
	0x09, 0x28,		  //   Usage (0x28)
	0x09, 0x29,		  //   Usage (0x29)
	0x09, 0x2A,		  //   Usage (0x2A)
	0x09, 0x2B,		  //   Usage (0x2B)
	0x95, 0x0C,		  //   Report Count (12)
	0x81, 0x02,		  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0x0A, 0x21, 0x26, //   Usage (0x2621)
	0x95, 0x08,		  //   Report Count (8)
	0xB1, 0x02,		  //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x0A, 0x21, 0x26, //   Usage (0x2621)
	0x91, 0x02,		  //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
	0x26, 0xFF, 0x03, //   Logical Maximum (1023)
	0x46, 0xFF, 0x03, //   Physical Maximum (1023)
	0x09, 0x2C,		  //   Usage (0x2C)
	0x09, 0x2D,		  //   Usage (0x2D)
	0x09, 0x2E,		  //   Usage (0x2E)
	0x09, 0x2F,		  //   Usage (0x2F)
	0x75, 0x10,		  //   Report Size (16)
	0x95, 0x04,		  //   Report Count (4)
	0x81, 0x02,		  //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
	0xC0,			  // End Collection
};

void setup() {
	static HIDSubDescriptor node(xbox_gamepad_hid_report_descriptor, sizeof(xbox_gamepad_hid_report_descriptor));
	// static HIDSubDescriptor node(hori_pad_hid_report_descriptor, sizeof(hori_pad_hid_report_descriptor));

	HID().AppendDescriptor(&node);

	// If using buttons for the triggers, use internal pull-up resistors
	if (UseTriggerButtons == true) {
		pinMode(Pin_TriggerL, INPUT_PULLUP);
		pinMode(Pin_TriggerR, INPUT_PULLUP);
	}
	// If using potentiometers for the triggers, set range
	else {
		XInput.setTriggerRange(0, ADC_Max);
	}

	// Set buttons as inputs, using internal pull-up resistors
	pinMode(Pin_ButtonA, INPUT_PULLUP);
	pinMode(Pin_ButtonB, INPUT_PULLUP);
	pinMode(Pin_ButtonX, INPUT_PULLUP);
	pinMode(Pin_ButtonY, INPUT_PULLUP);

	pinMode(Pin_ButtonLB, INPUT_PULLUP);
	pinMode(Pin_ButtonRB, INPUT_PULLUP);

	pinMode(Pin_ButtonBack, INPUT_PULLUP);
	pinMode(Pin_ButtonStart, INPUT_PULLUP);

	pinMode(Pin_ButtonL3, INPUT_PULLUP);
	pinMode(Pin_ButtonR3, INPUT_PULLUP);

	pinMode(Pin_DpadUp, INPUT_PULLUP);
	pinMode(Pin_DpadDown, INPUT_PULLUP);
	pinMode(Pin_DpadLeft, INPUT_PULLUP);
	pinMode(Pin_DpadRight, INPUT_PULLUP);

	pinMode(Pin_ButtonLogo, INPUT_PULLUP);

	XInput.setJoystickRange(0, ADC_Max);  // Set joystick range to the ADC
	XInput.setAutoSend(false);  // Wait for all controls before sending

	XInput.begin();
}

void loop() {
	// Read pin values and store in variables
	// (Note the "!" to invert the state, because LOW = pressed)
	boolean buttonA = !digitalRead(Pin_ButtonA);
	boolean buttonB = !digitalRead(Pin_ButtonB);
	boolean buttonX = !digitalRead(Pin_ButtonX);
	boolean buttonY = !digitalRead(Pin_ButtonY);

	boolean buttonLB = !digitalRead(Pin_ButtonLB);
	boolean buttonRB = !digitalRead(Pin_ButtonRB);

	boolean buttonBack  = !digitalRead(Pin_ButtonBack);
	boolean buttonStart = !digitalRead(Pin_ButtonStart);


	boolean dpadUp    = !digitalRead(Pin_DpadUp);
	boolean dpadDown  = !digitalRead(Pin_DpadDown);
	boolean dpadLeft  = !digitalRead(Pin_DpadLeft);
	boolean dpadRight = !digitalRead(Pin_DpadRight);

	boolean buttonLogo = !digitalRead(Pin_ButtonLogo);

	// Set XInput buttons
	XInput.setButton(BUTTON_A, buttonA);
	XInput.setButton(BUTTON_B, buttonB);
	XInput.setButton(BUTTON_X, buttonX);
	XInput.setButton(BUTTON_Y, buttonY);

	XInput.setButton(BUTTON_LB, buttonLB);
	XInput.setButton(BUTTON_RB, buttonRB);

	XInput.setButton(BUTTON_BACK, buttonBack);
	XInput.setButton(BUTTON_START, buttonStart);

	XInput.setButton(BUTTON_LOGO, buttonLogo);

	// Set XInput DPAD values
	XInput.setDpad(dpadUp, dpadDown, dpadLeft, dpadRight);

	// Set XInput trigger values
	if (UseTriggerButtons == true) {
		// Read trigger buttons
		boolean triggerLeft  = !digitalRead(Pin_TriggerL);
		boolean triggerRight = !digitalRead(Pin_TriggerR);

		// Set the triggers as if they were buttons
		XInput.setButton(TRIGGER_LEFT, triggerLeft);
		XInput.setButton(TRIGGER_RIGHT, triggerRight);
	}
	else {
		// Read trigger potentiometer values
		int triggerLeft  = analogRead(Pin_TriggerL);
		int triggerRight = analogRead(Pin_TriggerR);

		// Set the trigger values as analog
		XInput.setTrigger(TRIGGER_LEFT, triggerLeft);
		XInput.setTrigger(TRIGGER_RIGHT, triggerRight);
	}

	// Set left joystick
	if (UseLeftJoystick == true) {
		int leftJoyX = analogRead(Pin_LeftJoyX);
		int leftJoyY = analogRead(Pin_LeftJoyY);

		// White lie here... most generic joysticks are typically
		// inverted by default. If the "Invert" variable is false
		// then we'll take the opposite value with 'not' (!).
		boolean invert = !InvertLeftYAxis;

		XInput.setJoystickX(JOY_LEFT, leftJoyX);
		XInput.setJoystickY(JOY_LEFT, leftJoyY, invert);
	}

	// Set right joystick
	if (UseRightJoystick == true) {
		int rightJoyX = analogRead(Pin_RightJoyX);
		int rightJoyY = analogRead(Pin_RightJoyY);

		boolean invert = !InvertRightYAxis;

		XInput.setJoystickX(JOY_RIGHT, rightJoyX);
		XInput.setJoystickY(JOY_RIGHT, rightJoyY, invert);
	}

	if (UseL3R3Buttons)
	{
		boolean buttonL3 = !digitalRead(Pin_ButtonL3);
		boolean buttonR3 = !digitalRead(Pin_ButtonR3);

		XInput.setButton(BUTTON_L3, buttonL3);
		XInput.setButton(BUTTON_R3, buttonR3);
	}
	

	// Send control data to the computer
	XInput.send();
}
