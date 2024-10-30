#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S4,     touchSensor,    sensorEV3_Touch)
#pragma config(Motor,  motorA,          moteur,        tmotorEV3_Large, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//------------- Main -------------//
#include "header.h"
#include "fix.c"
#include "mobile.c"

task interface(){

	eraseDisplay();
	displayCenteredTextLine(4, "***Robot***");
	displayCenteredTextLine(6, "Fixe -> Bouton gauche");
	displayCenteredTextLine(8, "Mobile -> Bouton Droit");

	while(getButtonPress(buttonLeft)==0 && getButtonPress(buttonRight)==0){}

	if(getButtonPress(buttonLeft)==1){
		while(getButtonPress(buttonLeft)==1){}

		startTask(keepHeadingPID);

	}
	else if(getButtonPress(buttonRight)==1){
		while(getButtonPress(buttonRight)==1){}

		initialize();
		startTask(keepHeadingPID2);

	}
}

void initialize() {
	eraseDisplay();
	displayCenteredTextLine(4, "***initialize***");
	displayCenteredTextLine(7, "Tourner a Gauche-> Bouton Gauche");
	displayCenteredTextLine(8, "Tourner a Droite-> Bouton Droit");
	displayCenteredTextLine(10, "Continuer-> Bouton Bas");

	int speed=20;

	while(getButtonPress(buttonDown)==0) {

		setMotorSpeed(moteur, 0);

		if(getButtonPress(buttonLeft)==1){

			setMotorSpeed(moteur, speed);
			while(getButtonPress(buttonLeft)==1){}

		}
		else if(getButtonPress(buttonRight)==1){

			setMotorSpeed(moteur, -speed);
			while(getButtonPress(buttonRight)==1){}

		}

	}

	while(getButtonPress(buttonDown)==1){}
	setMotorSpeed(moteur, 0);
}

task main() {
	startTask(interface);
	while(true){
		if(getButtonPress(buttonEnter) == 1){
			stopTask(keepHeadingPID);
			stopTask(watchButtons);
			stopTask(IHM);
			stopTask(keepHeadingPID2);
			stopTask(watchButtons2);
			stopTask(IHM2);
			setMotorSpeed(motorA, 0);
			startTask(interface);

			} else if(getTouchValue(touchSensor) == 1){
			stopAllTasks();
		}
	}
}
