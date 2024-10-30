//------------- Mode Fixe -------------//

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
	semaphoreInitialize(semConsigne);
	startTask(watchButtons);
	startTask(IHM);
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
