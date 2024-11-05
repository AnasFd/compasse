// Tache principale pour gerer le suivi de la consigne
task keepHeading() {
    while(fabs(consigne - getMotorEncoder(moteur)) > tolerance) {
    		semaphoreLock(semConsigne);
    		setMotorTarget(moteur, consigne, 20);
    		semaphoreUnlock(semConsigne);
    }
}

task keepHeadingPD() {
    while (true) {
    		semaphoreLock(semConsigne);
        int capActual = nMotorEncoder[moteur];
        int error = consigne - capActual;
        int vitesseAngulaire = getMotorRPM(moteur) * 6;
        semaphoreUnlock(semConsigne);
        float speed = P * error + D * vitesseAngulaire;
        if (abs(error) < tolerance) {
            launchMotorSpeed(0);
        } else {
            launchMotorSpeed(speed);
        }

        delay(50);
    }
}

    // Start test: first question
  //while (true) {
  //    // Test by calling the function with different speeds
      //launchMotorSpeed(10080.0);  // Test with a speed value
      //wait1Msec(1000);  // Wait for 1 second before updating speed
  //}
	// End test
