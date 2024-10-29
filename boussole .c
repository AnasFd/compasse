#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro)
#pragma config(Sensor, S4,     touchSensor,    sensorEV3_Touch)
#pragma config(Motor,  motorA,          moteur,        tmotorEV3_Large, PIDControl, encoder)

#include "header.h"

#define tolerance 1       // Tolérance pour éviter les micro-corrections
#define errorHistorySize 10

// Régulation
#define P 10.0
#define D -0.02
#define I 0.001

// Linear properties
TSemaphore semConsigne;
const float maxSpeed = 880.0;  // Maximum allowed speed
const float minSpeed = 40.0;   // Minimum speed threshold
const int maxPower = 92;       // Maximum motor power
const int minPower = 2;        // Minimum motor power
const float pente = maxSpeed / maxPower;  // Slope to convert speed to power
int consigne = 0;
// Variables for the integral term
float errorHistory[errorHistorySize]; // Initialize history of errors with zeros
int historyIndex = 0;  // Index to track the history array

// Angular properties
TSemaphore semConsigne2;
const float minSpeed2 = 18.0;
const float maxSpeed2 = 494.0;
const int minPower2 = 1;
const int maxPower2 = 48;
const float pente2 = maxSpeed2 / maxPower2;
int consigne2 = 0;
// Define variables for the integral term
float errorHistory2[errorHistorySize]; // Initialize history of errors with zeros
int historyIndex2 = 0;  // Index to track the history array
int currentHistorySize2 = 0; // Track the actual number of values in history

bool inMenu = true;

//------------- Angular -------------//
task watchButtons2() {
	while(true) {
		if (getButtonPress(buttonEnter)) {
			// Stop the program on center button press
			stop_mobile();
			//stopAllTasks();
		}

		// Petite pause pour éviter la surconsommation CPU
		//delay(100);
	}
}


void launchMotorSpeed2(float speed) {
	// Handle null speed case
	if (speed == 0) setMotorSpeed(moteur, 0);

	else {
		// Preserve the sign of the speed
		float absSpeed = fabs(speed);  // Get absolute speed for comparison

		// Limit speed to maxSpeed if it exceeds the max limit
		absSpeed = (absSpeed > maxSpeed2) ? maxSpeed2 : minSpeed2;

		// Calculate the motor power from speed
		int power = (int)(absSpeed / pente2);  // Convert speed to motor power

		// Ensure the motor power is within the allowed range
		if (power < minPower2) {
			power = minPower2;
		}

		// Apply the correct sign back to the power (positive or negative)
		if (speed < 0) {
			power = -power;  // Negative power for reverse direction
		}

		// Set motor speed to the calculated power
		setMotorSpeed(moteur, power);
	}
}

// Function to calculate the sum of errors (Integral)
float calculateIntegral2() {
	float sum = 0;
	for (int i = 0; i < errorHistorySize; i++) {
		sum += errorHistory2[i];
	}
	return sum;
}

task keepHeadingPID2() {
	const float integralLimit = 1000;
	while (true) {
		int capActual = getGyroDegrees(gyro);
		semaphoreLock(semConsigne2);
		int error = consigne2 - capActual;
		semaphoreUnlock(semConsigne2);
		int vitesseAngulaire = getGyroRate(gyro);

		// Update the error history (for integral calculation)
		errorHistory2[historyIndex2] = error;
		historyIndex2 = (historyIndex2 + 1) % errorHistorySize;
		if (currentHistorySize2 < errorHistorySize) {
			currentHistorySize2++;
		}

		// Calculate the integral term
		float integral = calculateIntegral2();
		integral = (integral > integralLimit) ? integralLimit : -integralLimit;

		// PID control equation
		float speed = P * error + I * integral + D * vitesseAngulaire;

		// Stop the motor if the error is within tolerance
		if (abs(error) < tolerance) {
			launchMotorSpeed2(0);
			} else {
			launchMotorSpeed2(speed); // Launch motor with calculated PID speed
		}
	}
}

task IHM2() {
	while (true) {
		eraseDisplay(); // Clear display
		displayTextLine(1, "Center: Exit");

		displayTextLine(12, "DEBUG:");
		semaphoreLock(semConsigne2);
		displayTextLine(13, "Consigne: %d", consigne2);
		semaphoreUnlock(semConsigne2);
		displayTextLine(14, "Degrés actuels: %d", getGyroDegrees(gyro));

		//delay(300);
	}
}

//------------- Linear -------------//

// Tâche pour surveiller les boutons et ajuster la consigne
task watchButtons() {
	while(true) {
		// Bouton haut (rotation de 90° à droite)
		if (getButtonPress(buttonUp)) {
			semaphoreLock(semConsigne);
			consigne += 90;
			semaphoreUnlock(semConsigne);
			setMotorTarget(moteur, consigne, 20);
			while(getButtonPress(buttonUp)) {} // Attendre que le bouton soit relâché
		}

		// Bouton bas (rotation de 90° à gauche)
		if (getButtonPress(buttonDown)) {
			semaphoreLock(semConsigne);
			consigne -= 90;
			semaphoreUnlock(semConsigne);
			setMotorTarget(moteur, consigne, 20);
			while(getButtonPress(buttonDown)) {}
		}

		// Bouton droit (rotation de 10° à droite)
		if (getButtonPress(buttonRight)) {
			semaphoreLock(semConsigne);
			consigne += 10;
			semaphoreUnlock(semConsigne);
			setMotorTarget(moteur, consigne, 20);
			while(getButtonPress(buttonRight)) {}
		}

		// Bouton gauche (rotation de 10° à gauche)
		if (getButtonPress(buttonLeft)) {
			semaphoreLock(semConsigne);
			consigne -= 10;
			semaphoreUnlock(semConsigne);
			setMotorTarget(moteur, consigne, 20);
			while(getButtonPress(buttonLeft)) {}
		}

		if (getButtonPress(buttonEnter)) {
			// Stop the program on center button press
			stop_fix();
			//stopAllTasks();
		}

		// Petite pause pour éviter la surconsommation CPU
		//delay(100);
	}
}

void launchMotorSpeed(float speed) {
	// Handle null speed case
	if (speed == 0) setMotorSpeed(moteur, 0);

	else {
		// Preserve the sign of the speed
		float absSpeed = fabs(speed);  // Get absolute speed for comparison

		// Limit speed to maxSpeed if it exceeds the max limit
		absSpeed = (absSpeed > maxSpeed) ? maxSpeed : minSpeed;

		// Calculate the motor power from speed
		int power = (int)(absSpeed / pente);  // Convert speed to motor power

		// Ensure the motor power is within the allowed range
		if (power < minPower) {
			power = minPower;
		}

		// Apply the correct sign back to the power (positive or negative)
		if (speed < 0) {
			power = -power;  // Negative power for reverse direction
		}

		// Set motor speed to the calculated power
		setMotorSpeed(moteur, power);
	}
}

// Function to calculate the sum of errors (Integral)
float calculateIntegral() {
	float sum = 0;
	for (int i = 0; i < errorHistorySize; i++) {
		sum += errorHistory[i];
	}
	return sum;
}

// Proportional Integral Derivative control
task keepHeadingPID() {
	while (true) {
		int capActual = nMotorEncoder[moteur];
		semaphoreLock(semConsigne);
		int error = consigne - capActual;
		semaphoreUnlock(semConsigne);
		int vitesseAngulaire = getMotorRPM(moteur) * 6;


		// Update the error history (for integral calculation)
		errorHistory[historyIndex] = error;
		historyIndex = (historyIndex + 1) % errorHistorySize;

		// Calculate the integral term
		float integral = calculateIntegral();

		// PID control equation
		float speed = P * error + I * integral + D * vitesseAngulaire;

		// Stop the motor if the error is within tolerance
		if (abs(error) < tolerance) {
			launchMotorSpeed(0);
			} else {
			launchMotorSpeed(speed); // Launch motor with calculated PID speed
		}
	}
}

task IHM() {
	while (true) {
		eraseDisplay(); // Clear display
		displayTextLine(1, "Motor Control");
		displayTextLine(2, "Right: +10 deg");
		displayTextLine(3, "Left: -10 deg");
		displayTextLine(4, "Up: +90 deg");
		displayTextLine(5, "Down: -90 deg");
		displayTextLine(6, "Center: Exit");

		displayTextLine(12, "DEBUG:");
		semaphoreLock(semConsigne);
		displayTextLine(13, "Consigne: %d", consigne);
		semaphoreUnlock(semConsigne);
		displayTextLine(14, "Motor Position: %d", nMotorEncoder[moteur]);

		//delay(100); // Refresh every 500ms
	}
}
//------------- Main -------------//

void initialize() {
	displayCenteredTextLine(1, "Gyroscope Init Mode");
	displayCenteredTextLine(5, "Bouton droite : Clockwise");
	displayCenteredTextLine(6, "Bouton gauche : Counterclockwise");
	displayCenteredTextLine(7, "Bouton central: Exit");

	// Set initial motor power to zero
	setMotorSpeed(moteur, 0);

	while (true) {
		// If the left button is pressed, turn counterclockwise
		if (getButtonPress(buttonLeft)) {
			setMotorSpeed(moteur, -20); // Negative power for counterclockwise rotation
			while(getButtonPress(buttonLeft)) {} // Wait until the button is released
		}
		// If the right button is pressed, turn clockwise
		else if (getButtonPress(buttonRight)) {
			setMotorSpeed(moteur, 20); // Positive power for clockwise rotation
			while(getButtonPress(buttonRight)) {} // Wait until the button is released
		}
		// If the center button is pressed, exit the initialization
		else if (getButtonPress(buttonEnter)) {
			setMotorSpeed(moteur, 0); // Stop the motor
			eraseDisplay(); // Clear the display before exiting
			break; // Exit the initialization mode
		}
		else {
			setMotorSpeed(moteur, 0); // Stop the motor if no button is pressed
		}
	}
}

void launch_fix() {
	startTask(watchButtons);
	startTask(keepHeadingPID);
	startTask(IHM);
}

void stop_fix() {
	stopTask(watchButtons);
	stopTask(keepHeadingPID);
	stopTask(IHM);
	inMenu = true;
}

void launch_mobile() {
	startTask(watchButtons2);
	startTask(keepHeadingPID2);
	startTask(IHM2);
}

void stop_mobile() {
	stopTask(watchButtons);
	stopTask(keepHeadingPID);
	stopTask(IHM);
	inMenu = true;
}

// Main menu function
void showMenu() {
	inMenu = true;
	eraseDisplay();
	displayCenteredTextLine(1, "Menu Principal");
	displayCenteredTextLine(3, "Bouton gauche: Robot Fixe");
	displayCenteredTextLine(4, "Bouton droite: Robot Mobile");
	displayCenteredTextLine(5, "Touch: Arret du programme");

	do {
		// Check for button presses
		if (getButtonPress(buttonLeft) == 1) {
			while(getButtonPress(buttonLeft) == 1) {}
			eraseDisplay();
			delay(200);

			launch_fix();
		}
		else if (getButtonPress(buttonRight) == 1) {
			while(getButtonPress(buttonRight) == 1) {}
			eraseDisplay();
			delay(200);

			launch_mobile();
		}
		//} while(getButtonPress(buttonLeft) == 0 && getButtonPress(buttonRight) == 0);
	} while(inMenu);
}

task main() {
	semaphoreInitialize(semConsigne);
	semaphoreInitialize(semConsigne2);
	resetMotorEncoder(moteur);

	showMenu(); // Start the main menu
}
