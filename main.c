#pragma config(Sensor, dgtl2,  LeftSensor,     sensorDigitalIn)
#pragma config(Sensor, dgtl6,  MidSensor,      sensorDigitalIn)
#pragma config(Sensor, dgtl10, AutoCheck,      sensorTouch)
#pragma config(Sensor, dgtl12, RightSensor,    sensorDigitalIn)
#pragma config(Motor,  port2,           left,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           arm,           tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           right,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           wrist,         tmotorServoStandard, openLoop)
#pragma config(Motor,  port7,           condAct,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           claw,          tmotorServoStandard, openLoop)
#pragma config(Motor,  port9,           pushRod,       tmotorServoStandard, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

long sensorLeft;
long sensorMid;
long sensorRight;
int driveMode = 0;
int wristServoPos = 1;
float deadband = 1.0;
bool clawOpen = false;
bool clawBtnPressed = false;
bool minus90WristDegBtnPressed = false;
bool plus90WristDegBtnPressed = false;

void ChangeWristServoPos() {
	if (wristServoPos < 0) {
		wristServoPos = 0;
	} else if (wristServoPos > 2) {
		wristServoPos = 2;
	}
	switch (wristServoPos) {
		case 0:
			motor[wrist] = -110;
			break;
		case 1:
			motor[wrist] = 0;
			break;
		case 2:
			motor[wrist] = 100;
			break;
	}
}

void Wrist() {
	if (vexRT[Btn7R] && !minus90WristDegBtnPressed) {
		wristServoPos--;
		ChangeWristServoPos();
		minus90WristDegBtnPressed = true;
	}
	if (vexRT[Btn7L] && !plus90WristDegBtnPressed) {
		wristServoPos++;
		ChangeWristServoPos();
		plus90WristDegBtnPressed = true;
	}
	if (!vexRT[Btn7L]) {
		plus90WristDegBtnPressed = false;
	}
	if (!vexRT[Btn7R]) {
		minus90WristDegBtnPressed = false;
	}
}

void Claw() {
	if (vexRT[Btn7U] && clawBtnPressed == false) {
		if (clawOpen) {
			motor[claw] = -127;
			clawOpen = false;
			clawBtnPressed = true;
		} else {
			motor[claw] = 127;
			clawOpen = true;
			clawBtnPressed = true;
		}
	} else if (!vexRT[Btn7U] && clawBtnPressed) {
		clawBtnPressed = false;
	}
}

void Arm() {
	if (vexRT[Btn6D]) {
		motor[arm] = 127.0;
	} else if (vexRT[Btn6U]) {
		motor[arm] = -127;
	} else {
		motor[arm] = 0;
	}
}

void ConstControls() {
	Claw();
	Arm();
	Wrist();
}

void EndConstControls() {

}

void Arcade() {
	if (abs(vexRT[Ch3] - vexRT[Ch4]) > deadband) {
		motor[left] = vexRT[Ch3] - vexRT[Ch4];
	} else {
		motor[left] = 0;
	}
	if (abs(vexRT[Ch3] + vexRT[Ch4]) > deadband) {
		motor[right] = vexRT[Ch3] + vexRT[Ch4];
	} else {
		motor[right] = 0;
	}
	ConstControls(); //Called at the end of arcade inorder to do functions like arm movements
}

void Tank() {
	if (abs(vexRT[Ch3]) > deadband) { //Checks value of left joystick to see if it's outside of deadband
		motor[left] = vexRT[Ch3];
	} else {
		motor[left] = 0;
	}
	if (abs(vexRT[Ch2]) > deadband) {
		motor[right] = vexRT[Ch2];
	} else {
		motor[right] = 0;
	}
	ConstControls(); //Called at the end of tank inorder to do functions like arm movements
}

void Auto() {
	sensorLeft = SensorValue[LeftSensor];
	sensorMid = SensorValue[MidSensor];
  sensorRight = SensorValue[RightSensor];
	EndConstControls(); //Called at the beginning of auto to set non drive motors to 0
}

task main() {
	while (true) {
		//Checks for button presses and sets drive mode accordingly
		if(vexRT[Btn8L]) driveMode = 0;
		if(vexRT[Btn8R]) driveMode = 1;
		if(vexRT[Btn8U]) driveMode = 2;

		switch (driveMode) { //Checks what drive mode is set to and runs drive function based on that
			case 0:
				Arcade();
				break;
			case 1:
				Tank();
				break;
			case 2:
				Auto();
				break;
		}
	}
}
