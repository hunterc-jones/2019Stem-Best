#pragma config(Sensor, in2,    elbowPotSensor, sensorPotentiometer)
#pragma config(Sensor, dgtl2,  LeftSensor,     sensorDigitalIn)
#pragma config(Sensor, dgtl10, AutoCheck,      sensorTouch)
#pragma config(Sensor, dgtl12, RightSensor,    sensorDigitalIn)
#pragma config(Motor,  port2,           left,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           arm,           tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           right,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           claw,          tmotorServoStandard, openLoop)
#pragma config(Motor,  port7,           rake,          tmotorServoStandard, openLoop)
#pragma config(Motor,  port9,           pushRod,       tmotorServoStandard, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*
2019 Off The Grid Final Code
Neccesary Operations:
Arm Motor
-Motor port 3,
-Right side up(6U): Extend
-Right side down(6D): Retract
-Default off
-Motor with purpose of drawing cable into a spool to pull triple extension upwards



*/

// ARM control = up/down LEFT gamepad btns
// CLAW control = left/right RIGHT gamepad btns

// Autonomous Vars
bool StartingPathFound = false;
long sensorLeft;
long sensorRight;
int sensorAutoCheck;
float MaxSpeed = 1;					// CONFIG VAL		for setting highest speed robot can increment to during autonomous
float increment = 0.01;				// CONFIG VAL		for setting the speed robot makes each iteration during autonomou
float MinSpeed = -MaxSpeed;
float LeftSpeed;
float RightSpeed;
int pushRodSetupAngle = -7;
int pushRodReleaseAngle = 80;

// Basis Vars
int driveMode = 0;
int rememberWheelDrive;
bool arcadeToggle = true;		// DEFAULT = TANK
bool setupComplete = false;
bool arcadeBtnPressed = false;
bool mainautoBtnPressed = false;
bool AlternateAutoBtnPressed = false;
float deadband = 1.0;					// CONFIG VAL to reduce deadband

// Arm, Claw, Rake Vars
bool clawToggle;
bool plusThetaClawBtnPressed = false;
int clawAngleClose = -120;
int clawAngleOpen = -30;
/*
bool deployElbow = false;
float elbowPot = 0.0;
bool elbowCheck = true;
float elbowPercentIncrease = 2.3;
int elbowDownSpeed = 0.5;
*/
bool rakeBtnPressed = false;
bool rakeToggle = true;
int rakeAngle1 = 0;
int rakeAngle2 = 127;

// Random 'ShakeTheCrapOutOfRobot' Stuff
int leftMotorVal;
int rightMotorVal;
int armMotorVal;
int pushrodServoVal;
int rakeServoVal;
int clawServoVal;
bool shakeOutBtnPressed = false;
bool shakeToggle = false;





void InitRobot() {
	if (!setupComplete) {
		motor[claw] = clawAngleOpen;	// min 40, max -120
		motor[pushRod] = pushRodSetupAngle;
		setupComplete = true;
	}
}

void CheckDriveModes() {
	// UP btn on RIGHT gamepad
		// Activate arcade or tank
	if (vexRT[Btn8U] && !arcadeBtnPressed) {
		if (driveMode!=0&&driveMode!=1) {			// if not already in tank or arcade
			driveMode = rememberWheelDrive;
		} else if (driveMode==0||driveMode==1) {		// if already in tank or arcade
			if (arcadeToggle == true) {
				driveMode = 0;		// Arcade drivemode
				arcadeToggle = false;
			} else if (arcadeToggle == false) {
				driveMode = 1;		// Tank drivemode
				arcadeToggle = true;
			}
		}
		rememberWheelDrive = driveMode;
		arcadeBtnPressed = true;
	} else if (!vexRT[Btn8U]) {
		arcadeBtnPressed = false;
	}
	// LEFT/RIGHT btns on LEFT gamepad,	UP/DOWN btns on LEFT gamepad
		// Activate main auto or Ralsty auto
	if (    (vexRT[Btn7L]&&vexRT[Btn7R])    &&    (  (!mainautoBtnPressed && !vexRT[Btn7U])  &&  !vexRT[Btn7D])    ) {
		driveMode = 2;
		mainautoBtnPressed = true;
	} else if (!vexRT[Btn7L]&&!vexRT[Btn7R]) {
		mainautoBtnPressed = false;
	}
	if (	(vexRT[Btn7U]&&vexRT[Btn7D])	&&	(	(!mainautoBtnPressed && !vexRT[Btn7L])	&&!vexRT[Btn7R])	) {
		driveMode = 3;
		AlternateAutoBtnPressed = true;
	} else if (!vexRT[Btn7U]&&!vexRT[Btn7D]) {
		AlternateAutoBtnPressed = false;
	}
}

int genRandVal(int motorVal) {
	motorVal = random(255);		// because (127*2) + 1 to account for exclusion
	return (motorVal-127);		// to half range, since random() can only gen positive range
}

void ShakeParty() {
	// if LEFT AND RIGHT btns on RIGHT gamepad are pressed while all other RIGHT gamepad btns are not being pressed (in case by accident)
	if (vexRT[Btn8L]&&vexRT[Btn8R]&&!shakeOutBtnPressed&&!vexRT[Btn8U]&&!vexRT[Btn8D]) {
		// toggle to SHAKE or NOT SHAKE
		if (!shakeToggle) shakeToggle=true;
		if (shakeToggle) shakeToggle=false;
		shakeOutBtnPressed = true;
	} else if (!vexRT[Btn8L]&&!vexRT[Btn8R]) {
		shakeOutBtnPressed = false;
	}
	if (shakeToggle == true) {
		srand(2378);		// random value of BEST

		//Motors
		motor[left] = genRandVal(leftMotorVal);
		motor[arm] = genRandVal(rightMotorVal);
		motor[right] = genRandVal(armMotorVal);

		//Servos
		motor[claw] = genRandVal(pushrodServoVal);
		motor[rake] = genRandVal(rakeServoVal);
		motor[pushRod] = genRandVal(clawServoVal);
	}
}

void Claw() {
	if (vexRT[Btn5U] && !plusThetaClawBtnPressed) {
		if (clawToggle == true) {
			motor[claw] = clawAngleOpen;
			clawToggle = false;
		} else if (!clawToggle) {
			motor[claw] = clawAngleClose;
			clawToggle = true;
		}
		plusThetaClawBtnPressed = true;
	} else if (!vexRT[Btn5U]) {
		plusThetaClawBtnPressed = false;
	}
}
void Arm() {		// function for arm and elbow (elbow if configured for Corbin's design)
	if (vexRT[Btn6D]) {
		motor[arm] = 127.0;		// moves arm down
	} else if (vexRT[Btn6U]) {
		motor[arm] = -127;		// moves arm up
	} else {
		motor[arm] = 0;				// stops arm
	}
	/*
	if (vexRT[Btn7U]) deployElbow = true;
		if (vexRT[Btn7D]) deployElbow = false;

		elbowPot = SensorValue[elbowPotSensor];

		if (deployElbow) {
			elbowCheck = false;
			motor[elbow] = (elbowPot - 2716) / 12;
		} else {
			if (!elbowCheck) {
				if (elbowPot > 1000) {
					motor[elbow] = -15*elbowPercentIncrease;
				} else {
					motor[elbow] = 8*elbowPercentIncrease;
				}
				elbowCheck = elbowPot - 4094 > -20;
			} else {
				if (motor[elbow] > 0) {
					motor[elbow] -= elbowDownSpeed;
				} else {
					motor[elbow] = 0;
				}
			}
	}
	*/
}
void Rake() {
	if (vexRT[Btn8D] && !rakeBtnPressed) {
		if (rakeToggle == true) {
			rakeToggle = false;
		} else if (rakeToggle == false) {
			rakeToggle = true;
		}
		rakeBtnPressed = true;
	} else if (!vexRT[Btn8D]) {
		rakeBtnPressed = false;
	}
	if (rakeToggle) motor[rake] = rakeAngle1;
	if(!rakeToggle) motor[rake] = rakeAngle2;
}
void ManualControls() {
	Arm();
	Claw();
	Rake();
	ShakeParty();
}
void Arcade() {
	InitRobot();
	if (abs(vexRT[Ch3] - vexRT[Ch4]) > deadband) {
		motor[right] = vexRT[Ch3] - vexRT[Ch4];
	} else {
		motor[right] = 0;
	}
	if (abs(vexRT[Ch4] + vexRT[Ch3]) > deadband) {
		motor[left] = vexRT[Ch4] + vexRT[Ch3];
	} else {
		motor[left] = 0;
	}
	ManualControls();
}
void Tank() {
	InitRobot();
	if (abs(vexRT[Ch3]) > deadband) { // Checks value of left joystick to see if it's outside of deadband
		motor[left] = vexRT[Ch3];
		} else {
		motor[left] = 0;
	}
	if (abs(vexRT[Ch2]) > deadband) {
		motor[right] = vexRT[Ch2];
		} else {
		motor[right] = 0;
	}
	ManualControls();
}
bool sensorCheck(long sens) {
	return (sens > 0.5);
}
void GetSensorReadout() {
	sensorLeft = SensorValue[LeftSensor];
	sensorRight = SensorValue[RightSensor];
	sensorAutoCheck = SensorValue[AutoCheck];
}
float MotorCheck(float speed, float change) {
	speed += change;
	if (speed > MaxSpeed) {
		speed = MaxSpeed;
	} else if (speed < MinSpeed) {
		speed = MinSpeed;
	}
	return speed;
}
void PathFinishedReset() {
	motor[left] = 0;
	motor[right] = 0;
	motor[pushRod] = 	pushRodReleaseAngle;
	delay(2000);
	driveMode = 0;
	setupComplete = false;
}
void AlternateAuto() {
	GetSensorReadout();
	if (sensorAutoCheck == 1) {
		PathFinishedReset();
	} else {
		motor[left] = 127;
		motor[right] = 127;
	}
}

void MainAuto() {
	GetSensorReadout();
	if (!StartingPathFound) {
		StartingPathFound = true;
	} else {
		if (sensorAutoCheck == 1) {
			PathFinishedReset();
		} else {
			if (sensorCheck(sensorLeft) && sensorCheck(sensorRight)) {		// If both are detecting black
				LeftSpeed = MotorCheck(LeftSpeed, increment);
				RightSpeed = MotorCheck(RightSpeed, increment);
			} else if (sensorCheck(sensorLeft) && !sensorCheck(sensorRight)){	// If only left is detcting black
				LeftSpeed = MotorCheck(LeftSpeed, increment);
				RightSpeed = MotorCheck(RightSpeed, -increment);
			} else if (!sensorCheck(sensorLeft) && sensorCheck(sensorRight)){	// If only right is detecting black
				LeftSpeed = MotorCheck(LeftSpeed, -increment);
				RightSpeed = MotorCheck(RightSpeed, increment);
			} else if (!sensorCheck(sensorLeft) && !sensorCheck(sensorRight)){		// If neither are detecting black
				LeftSpeed = MotorCheck(LeftSpeed, -increment);
				RightSpeed = MotorCheck(RightSpeed, -increment);
			}
			motor[left] = LeftSpeed * 127;
			motor[right] = RightSpeed * 127;
		}
	}
}


task main() {
	InitRobot();

	while (true) {
	/*
	Main Loop
	A continuosly running while loop inwhich contains all contunaly activating methods for our robot.
	All methods must be repeatedly run or else robot will be unable to react to changing stimuli.

	*/
		// Checks for button presses and sets drive mode accordingly
		CheckDriveModes();

		//Testing
		GetSensorReadout();

		switch (driveMode) { // Switch case to allow a varition of inputs to be switched between to fit driver needs and completion of autonomous challenges
		case 0:
			Arcade();//Arcade mode is where two analog inputs from one joystick is used
			break;
		case 1:
			Tank();//Tank mode is where two vertical inputs from two seperate joysticks are converted into motor values
			break;
		case 2:
			MainAuto();//Arcade mode is where the robot is controled by our sensors. This is the primary arcade mode where the robot is guided by IR drives
			break;
		case 3:
			AlternateAuto();//RalstonValley automatic
			break;
		}
	}
}
