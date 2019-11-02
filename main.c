int driveMode = 0;

void constControls() {

}

void endConstControls() {

}

void arcade() {
	constControls(); //Called at the end of arcade inorder to do functions like arm movements
}

void tank() {
	constControls(); //Called at the end of tank inorder to do functions like arm movements
}

void auto() {
	endConstControls(); //Called at the beginning of auto to set non drive motors to 0
}

task main() {
	while (true) {
		if(vexRT[Btn8L]) driveMode = 0;
		if(vexRT[Btn8R]) driveMode = 1;
		if(vexRT[Btn8U]) driveMode = 2;

		switch (driveMode) {
			case 0:
				arcade();
				break;
			case 1:
				tank();
				break;
			case 2:
				auto();
				break;
		}
	}
}
