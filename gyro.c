#pragma config(Sensor, S1,     gyro,           sensorEV3_Gyro)
#pragma config(Motor,  motorA,          moteur,        tmotorEV3_Large, PIDControl, encoder)

long fileHandle;

// Function to characterize the motor's speed based on gyrometer readings
void gyroRateCharacterization() {
    const int minPower = 0;
    const int maxPower = 100;
    const int step = 1;
    const int waitTime = 100; // Wait time for the motor to stabilize (in ms)
    const int measurementDuration = 100; // Time to collect speed data (in ms)

    string valString = "power,angularSpeed(degParSec)\n";
    fileWriteData(fileHandle, valString, strlen(valString));

    // Initialize the gyro sensor
    SensorType[gyro] = sensorEV3_Gyro;
    delay(500); // Let the gyro initialize
    resetGyro(gyro);
    int direction = 1;

    // Perform the characterization
    for (int power = minPower; power <= maxPower; power += step) {

        // Apply power to the motor and wait for stabilization
        setMotorSpeed(moteur, power * direction);
        delay(waitTime);

        // Reset gyro sensor and start recording speed
        resetGyro(gyro);
        delay(measurementDuration);

        // Measure angular velocity (gyro rate in degrees per second)
        int angularSpeed = getGyroRate(gyro); // Gyro rate in degrees/second

        // Log power and corresponding angular speed
        stringFormat(valString,"%d,%d\n",power,abs(angularSpeed));
        fileWriteData(fileHandle, valString, strlen(valString));
        stringFormat(valString,""); // Reset the string value to null

        // Reverse direction for the next iteration to avoid cable entanglement
        direction *= -1;

        // Stop motor between tests
        setMotorSpeed(moteur, 0);
        delay(300);  // Pause between measurements
    }

    // Stop the motor at the end of the test
    setMotorSpeed(moteur, 0);

    // Close the file
    fileClose(fileHandle);
}

task main() {
		string filename = "gyro_charac.csv";
		fileHandle = fileOpenWrite(filename);
    gyroRateCharacterization();  // Run the gyro characterization function
}
