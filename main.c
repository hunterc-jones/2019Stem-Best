#pragma config(Motor,  port2,           motorLeft,     tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           motorRight,    tmotorVex393_MC29, openLoop)


//a global boolean variable that represents the joystick that arcade-drive uses, if it is true, the arcade-drive uses the right joystick, if false, uses the left joystick.
bool arcadeDriveJoystickRight = true;
//a global boolean variable that represents which drive the robot is using, if it is true, then the robot is using the arcade drive mode, if it is false, then the robot is in tank drive mode.
bool arcadeDrive = false;
//global integer variables that allows the program to create, edit, and convert motor values without actually changing the motors until the end. This allows for a more module-based code, which is easier to manage and edit.
int motorLeftInput = 0;
int motorRightInput = 0;
//global integer variable that represents the range that the deadband affects
int deadBandRange = 2;
//global boolean variable that represents whether the precision mode has just been toggled
bool precisionRecentlyPressed = false;
//global boolean variable that represents whether the robot is in precision mode or not
bool precision = false;



/*NonLinearDrive: A function that takes in any linear motor value, and converts it into a non-linear motor value.
	Inputs: motorInput, an integer that represents a linear motor value
	Outputs: nonLinearMotor, an integer that represents the non-linear motor value of motorInput*/
int NonLinearDrive(int motorInput){
	//an integer variable that is the non-linear motor value output. Here it is defined.
	int nonLinearMotor;
	//an if/elif/else statement that checks the value of the linear motor value that is inputed. This is because the equation for conversion is different based on whether the linear motor value is positive, negative, or zero.
	if(motorInput > 0){
		//non-linear conversion equation for converting a positive linear motor value to a positive non-linear motor value
		nonLinearMotor = pow(2, (7 * motorInput / 127)) - 1;
	}else if(motorInput < 0){
		//non-linear conversion equation for converting a negative linear motor value to a negative non-linear motor value.
		//nonLinearMotor = pow(-2, (float)(-log(127)) / ((float)(126 * log(2)) * motorInput)) + 1;
		nonLinearMotor = pow(-2, (-7 * motorInput / 127)) + 1;
	}else{
		//equation for setting the non-linear motor value to zero, if the linear motor value is also zero. Without this, the motor value would be zero when the joystick isn't moving and the machine would move forward.
		nonLinearMotor = 0;
	}
	//returns the new non-linear motor value so that it can be sent to the robot
	return nonLinearMotor;
}


/*DeadBand: a function that takes in a motor value, and sets it to zero if it is close enough to zero. This allows for more precision and will hop around any calibration issues. A deadband is a "band" around the joystick center that sets the motor to zero, when the motor value is in this deadband, it is "dead".
	Inputs: motorInput, an integer that represents the input value that hasn't been deadbanded
	Outputs: an integer that represents the motor value after it has been deadbanded.*/
int DeadBand(int motorInput){
	//an if statement that checks whether the current motor input value is inside the deadband.
	if(abs(motorInput) < deadBandRange){
		//if the motor input value is within the deadband range, the function returns zero, making the motor value "dead"
		return 0;
	}
	//if the motor input value isn't in the deadband range, then this function does nothing and returns the same value.
	return motorInput;
}


/*ApplyPrecision: a function that takes in a motor/servo value, and checks if the robot is in a "precision mode". If the robot is currently in this "precision mode", then this function will slow down motor/servo values to half speed, allowing drivers to have easier and more efficient control over the robot.
	Inputs: motorInput, an integer that represents the motor/servo values at full speed, before they are converted to a more precise value.
	Outputs: an integer that represents the motor/servo values after the precision of them has been increased*/
int ApplyPrecision(int motorInput){
	//an if statement that detects if the robot is currently in precision mode
	if(precision){
		//returns the input value after it has been halfed, allowing drivers to be more precise with the robot's movements
		return motorInput/2;
	}
	//if the robot isn't in precise mode, it just returns the motor/servo value it was given
	return motorInput;
}


/*PrecisionToggle: a function that will check if a button is being pressed, and if it is, will toggle the robot's "percision mode" to be off or on.
	Inputs: None
	Outputs: None*/
void PrecisionToggle(){
	//an if/elif statement that checks whether the assigned toggle button is being pressed and if the button has been pressed recently, which prevents the code from rapidly toggling the precision mode on and off again.
	if(vexRT[Btn7D] == 1 && precisionRecentlyPressed == false){
		//toggles the precision mode from on to off it is was originally on, or off to on if it was originally off
		precision = !precision;
		//tells the robot that the precision mode has already been toggled during this button press and disabes the toggle system
		precisionRecentlyPressed = true;
	//checks if the button isn't being pressed
	}else if(vexRT[Btn7D] == 0){
		//as soon as the button is not being pressed anymore, the toggle system is re-enabled and ready to be toggled again.
		precisionRecentlyPressed = false;
	}
}


/* TankDrive: a function that takes in two joystick values and sets the motor input values to a linear motor value that is ready to be edited and converted for the robot to use.
	Inputs: None
	Outputs: None*/
void TankDrive(){
	//sets the motor values for the left motor to the left joystick values.
	motorLeftInput = vexRT[Ch3];
	//sets the motor values for the right motor to the right joystick values.
	motorRightInput = vexRT[Ch2];
}


/*MotorRight: This function is in charge of all of the arcade drive equations, it takes in the x and y of a single joystick and converts them to arcade style motor values, based on the quadrants.
	Inputs: JoyX, the left and right value of a joystick
					JoyY, the up and down value of a joystick
	Outputs: an integer, which is the value of the right side motor*/
int MotorRight(int JoyX, int JoyY){
	//if/elif/elif/else statement that checks which quadrant the joystick is in, and outputs a different equation based on that
		//(return statements under if statements:) takes in an x and y value, runs it through an equation that derives the power of the motor based on the quadrant. Returns this value as the linear motor value.
	if(JoyY > 0 && JoyX > 0){
		return  sqrt((JoyX * JoyX) + (JoyY * JoyY)) * cos(2 * atan(JoyX / JoyY));
	}else if(JoyY < 0 && JoyX < 0){
		return -sqrt((JoyX * JoyX) + (JoyY * JoyY)) * cos(2 * atan(JoyX / -JoyY));
	}else if(JoyY >= 0 && JoyX <=0){
		return sqrt((JoyX * JoyX) + (JoyY * JoyY));
	}else{
		return -sqrt((JoyX * JoyX) + (JoyY * JoyY));
	}
}


/*MotorLeft:the ArcadeDrive function for the left motor instead of the right. Because the left motor is a mirror image to the right motor, this function just runs the motorRight function with the x-axis flipped.
	Inputs: JoyX, an integer that represents the x-axis on the left joystick
					JoyY, an integer that represents the y-axis on the right joystick
	Outputs: an integer that represents the linear arcade drive motor value for the left motor*/
int MotorLeft(int JoyX, int JoyY){
	//returns the linear arcade drive motor value
	return MotorRight(-JoyX, JoyY);
}


/*ArcadeDrive: The ArcadeDrive functon that will take joytick inputs and run them through either the MotorLeft function or the MotorRight function to create linear arcade motor value
	Inputs: None
	Outputs: None*/
void ArcadeDrive(){
	//An if statement that determines which joystick is being used for the arcade controls, and will use different joystick channels depending on which one
	if(arcadeDriveJoystickRight == true){
		//sets the left motor input value to the output of MotorLeft using the right joystick channels
		motorLeftInput = MotorLeft(vexRT[Ch1],vexRT[Ch2]);
		//sets the right motor input value to the output of MotorRight using the right joystick channels
		motorRightInput = MotorRight(vexRT[Ch1],vexRT[Ch2]);
	}else{
		//sets the left motor input value to the output of MotorLeft using the left joystick channels
		motorLeftInput = MotorLeft(vexRT[Ch4], vexRT[Ch3]);
		//sets the right motor input value to the output of MotorRight using the right joystick channels
		motorRightInput = MotorRight(vexRT[Ch4], vexRT[Ch3]);
	}
}


/*Drive: the Drive function is run in the main function and contains all of the functions for the drive controls. Depending on the value of a drive-type variable, it will run either TankDrive or ArcadeDrive.
	Inputs: None
	Outputs: None*/
void Drive(){
	//An if/else statement that detects wether the robot is going to use ArcadeDrive or TankDrive.
	if(arcadeDrive == true){
		//Runs the ArcadeDrive function
		ArcadeDrive();
	}else{
		//Runs the TankDrive function
		TankDrive();
	}
}


task main() //This is the task that is called when the robot starts the program.
{
	while(true) { //This is the main loop, everything that is updated continuously will go in here.
		Drive();
	}
}
