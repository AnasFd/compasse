//------------- Mode Mobile -------------//
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

task watchButtons2() {
	while(true) {
		if (getButtonPress(buttonEnter)) {
			// Stop the program on center button press
			stopAllTasks();
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
