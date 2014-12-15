/*
	Main program for the firefighting robot. Each subsystem on the robot has it's own class,
	split into a .h and .cpp file. For example, the swerve-style drive base has
 		SwerveDrive.h
 		SwerveDrive.cpp
 
 	The .h file contains the variables and objects (such as servos, I/O pins, etc.) and function
	prototypes. The .cpp is only to implement those prototypes.
 
	Each subsystem class is responsible for controlling that subsystem based on input given from
	this main class.
 
 */

#include <Wire.h>
#include <SwerveDrive.h>
#include <Turret.h>
#include <Constants.h>
#include <Map.h>
#include <LiquidCrystal.h>
#include <Fan.h>
#include <L3G.h>
#include <RunningMedian.h>
#include <Ultrasonic.h>
#include <Line.h>

LiquidCrystal lcd(53, 51, 49, 47, 45, 43);
SwerveDrive drive;
Turret turret;
Map fieldMap;
Fan fan;

byte currentState = START;

double robotX = 0;
double robotY = 0;

void setup() {
	Serial.begin(9600);
	lcd.begin(16, 2);

	lcd.clear();
	lcd.print("Setup!");
	turret.init();
	// fan.init();
	drive.init();

	lcd.clear();
	lcd.print("Initialized");
	attachInterrupt(FR_ENC_PIN, updateEncoderFR, CHANGE);
	attachInterrupt(FL_ENC_PIN, updateEncoderFL, CHANGE);
	attachInterrupt(RR_ENC_PIN, updateEncoderRR, CHANGE);
	attachInterrupt(RL_ENC_PIN, updateEncoderRL, CHANGE);

	Serial.println(MAP_WIDTH * 8);
	Serial.println(MAP_HEIGHT);
}

void loop() {
	// runStateMachine();
	// drive.pollGyro();

	turret.scan();

	// int input = analogRead(2);
	// input = map(input, 200, 800, 0, 360);
	// turret.setTurretAngle(input);
	// Serial.println(input);

	// int input = analogRead(2);
	// input = map(input, 200, 800, 0, 180);
	// input = constrain(input, 0, 180);
	// turret.setServoAngle(input);
	// Serial.println(input);
}


Point nextPoint(robotX, robotY);
void runStateMachine() {
	lcd.clear();
	switch(currentState) {
		case START: {
			lcd.clear();
			lcd.print("START");
			// Zero as many things as possible
			if(drive.rotatePods(0)) {
				currentState = SCANNING;
			}
			break;
		}
		case SCANNING: {
			lcd.clear();
			lcd.print("SCANNING");
			// scan for fire and obstacles
			if(turret.scan()) {
				currentState = CALCULATING;
			}		
			break;
		}
		case CALCULATING: {
			lcd.clear();
			lcd.print("CALCULATING");
			processObstacles();
			nextPoint = getTarget();
			currentState = MOVING;
			break;
		}
		case TURNING: { // Rotate pods
			lcd.clear();
			lcd.print("TURNING");
			break;
		}
		case MOVING: {
			lcd.clear();
			lcd.print("MOVING");
			lcd.setCursor(0, 1);
			lcd.print(nextPoint.x);
			lcd.setCursor(0, 7);
			lcd.print(nextPoint.y);
			
			moveToPoint(nextPoint.x, nextPoint.y);

			break;
		}
		case EXTINGUISHING: {
			break;
		}
		case RETURNING: {
			/*
				either backtrack exactly
				or use A* or Dijkstra's algorithm to go back to the start

				if(atStart) currentState = COMPLETE;
				else goBackToStart();

			 */
			break;
		}
		case COMPLETE: {
			/*
				Display the x, y, z coordinates of the flame on the LCD.
			 */
			break;
		}
		default:
			return;
	}
}

// Adds obstacles to the map
void processObstacles() {
	for(int i = 0; i < ((TURRET_MAX_ANGLE - TURRET_MIN_ANGLE) / TURRET_ANGLE_INCREMENT); i++) {
		double turretX = turret.obstacleXVals[i];
		double turretY = turret.obstacleYVals[i];
		double realX = robotX + turretX;
		double realY = robotY + turretY;
		fieldMap.set(realX, realY, true); // flag the point as having an obstacle
		turret.obstacleXVals[i] = 0; // reset the arrays
		turret.obstacleYVals[i] = 0;
	}
}

// Gets the next target point to drive to
Point getTarget() {
	double x = robotX, y = robotY; // target point
	// t1 sweeps CCW, t2 sweeps CW
	// t3 sweeps CW, t4 sweeps CCW
	int t1 = 270, t2 = 90; // front angles 
	int t3 = 270, t4 = 90; // rear angles
	int angleInc = 15;

	int lastAngle = 0;

	double x1 = 0, y1 = 0;
	double x2 = 0, y2 = 0;
	double x3 = 0, y3 = 0;
	double x4 = 0, y4 = 0;

	// Go 270->360->90
	for(; t1 != 90; t1 = (t1 + angleInc) % 360) {
		if(obstacleOnPath(&x1, &y1, t1)) {
			lastAngle = t1;
		} else if(t1 - angleInc == lastAngle) {
			t1 = lastAngle;
			break;
		}
	}


	Point p(x, y);
	return p;
}

boolean obstacleOnPath(double angle, double* distance) {
	double cosine = cos(toRad(angle));
	double sine = sin(toRad(angle));
	for(int dist = 9; dist <= 36; dist += 3) {
		double x = dist * cosine;
		double y = dist * sine;
		if(fieldMap.get(robotX + x, robotY + y)) {
			*distance = dist;
			return true;
		}
	}
	return false;	
}


byte moveToPointState = 0;
double moveToPointDist, moveToPointAngle;

boolean moveToPoint(double x, double y) {
	switch (moveToPointState) {
		case 0:
			// Calculate distance and angle
			moveToPointDist = sqrt(square(robotX - x) + square(robotY - y));
			moveToPointAngle = toDeg(atan(y / x));
			moveToPointState = 1;
			break;
	    case 1:
	    	// Rotate pods to the correct angle
			if(drive.rotatePods(moveToPointAngle)) {
				moveToPointState = 1;
			}
	    	break;
	    case 2:
	    	// Drive the calculated distance and return true once it happens
	    	if(drive.driveDistance(moveToPointDist) >= moveToPointDist) {
	    		moveToPointState = 0;
	    		return true;
	    	}
	    	break;
	    default:
	    	moveToPointState = 0;
	    	break;
	}
	return false;
}


// Glue for attaching encoder interrupts
void updateEncoderFR() {
	drive.frontRight.encoder.update();
}

void updateEncoderFL() {
	drive.frontLeft.encoder.update();
}

void updateEncoderRR() {
	drive.rearRight.encoder.update();	
}

void updateEncoderRL() {
	drive.rearLeft.encoder.update();
}