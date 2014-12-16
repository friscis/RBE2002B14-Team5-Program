/*
 *  This file contains all the constants used in the program.
 */

#ifndef Constants_h
#define Constants_h

/*
 *  Helper macro function(s)
 */
#define sign(x) ((x) < 0 ? -1 : (x) > 0 ? 1 : 0)
#define toRad(deg) ((deg) * PI / 180.0)
#define toDeg(rad) ((rad) * 180.0 / PI)

/*
 *  Swerve drive constants. Change these!
 */
enum SwerveServoPins {
	CENTER 				= 6,	// motor that turns the pods
	FR_MOTOR_PIN 		= 5,
	FL_MOTOR_PIN 		= 8,
	RL_MOTOR_PIN 		= 7,
	RR_MOTOR_PIN 		= 4
};

enum SwerveEncoderPins {
	FR_ENC_PIN	= 0, // pin 3
	FL_ENC_PIN	= 1, // pin 2
	RR_ENC_PIN	= 4, // pin 19
	RL_ENC_PIN	= 5  // pin 18
};

const int SWERVE_POT_PIN = A11;

/*
 *  Encoder constants
 */
const double DEGREES_PER_ENC_TICK = 2.0; // 180 ticks per rev (counting rising and falling edge)

/*
 *  Turret constants.
 */
const int TURRET_MOTOR_PIN			= 9; // Change this to match the physical robot
const int TURRET_POT_PIN 			= A1; // analog
const int ULTRASONIC_TRIGGER_PIN	= 22;
const int ULTRASONIC_ECHO_PIN		= 24;
const int BOOM_LENGTH 				= 7; // inches
const int IR_SERVO_PIN 				= 11;
const int IR_SENSOR_PIN				= A2; //analog

/*
 * Fan constant(s)
 */
const int FAN_MOTOR_PIN = 10;//change this to match the physical robot

/*
 *  Map constants.
 */
const static int FIELD_WIDTH  = 96 * 2; // x4 because we keep a 2D array that's 2x width and 2x height
const static int FIELD_HEIGHT = 96 * 2;

// one bit per cell -> divide by 8

const static byte BITS_PER_CELL = 4;
const static byte MAP_WIDTH	 = FIELD_WIDTH / (3 * 8 / BITS_PER_CELL); // cells are 3 inches on a side
const static byte MAP_HEIGHT = FIELD_HEIGHT / 3;

/*
 *  Light sensor constants
 */
const static int FRONT_LS_PIN = A10;
const static int RIGHT_LS_PIN = A9;
const static int REAR_LS_PIN = A8;
const static int LEFT_LS_PIN = A7;

const static int LS_BLACK_VALUE = 700; // a light sensor reading less than this is "black"

// Helper macro that evaluates to true if the given light sensor value is "black"
#define isBlack(lsVal) ((lsVal) < LS_BLACK_VALUE)


/*
 *  State machine constants.
 */
enum State {
	START,
	SCANNING,
	TURNING,
	MOVING,
	CALCULATING,
	EXTINGUISHING,
	RETURNING,
	COMPLETE
};

#endif