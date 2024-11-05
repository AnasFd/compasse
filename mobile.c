//------------- Mode Mobile -------------//

// Proprietes angulaire
const float maxSpeed2 = 494.0;  // Vitesse maximale
const float minSpeed2 = 18.0;   // Vitesse minimale
const int maxPower2 = 48;       // Puissance maximale
const int minPower2 = 1;        // Puissance maximale
const float pente2 = maxSpeed2 / maxPower2; // pente pour convertir la vitesse en puissance

// Variables pour la consigne
TSemaphore semConsigne2; // Semaphore permettant une meilleur gestion de la consigne
int consigne2 = 0; // Initialisation de la consigne (le but a atteindre par le moteur)

// Variables pour le calcul d'integrale
float errorHistory2[errorHistorySize]; // Tableau pour stocker les erreurs
int historyIndex2 = 0;  // Index pour le tableau errorHistory

task watchButtons2() {
	//while(true) {
		// Au debut, cette fonction etait responsable de l'arret du programme
        // on cliquant sur le bouton du centre
		// Cette fonctionalite est geree maintenant par le touch sensor
	//}
}

void launchMotorSpeed2(float speed) {
	// Si vitesse null
	if (speed == 0) setMotorSpeed(moteur, 0);

	else {
		// Preserver le signe de la vitesse
		float absSpeed = fabs(speed);

		// Limiter la vitesse maximale
		absSpeed = (absSpeed > maxSpeed2) ? maxSpeed2 : minSpeed2;

		// calcul de la puissance en fonction de la vitesse
		int power = (int)(absSpeed / pente2);  // Convert speed to motor power

		// Verification que la puissance est ok
		if (power < minPower2) {
			power = minPower2;
		}

		// Apres les calculs, on reprond la direction initiale (droite ou gauche)
		if (speed < 0) {
			power = -power;
		}

		// Lancement du moteur a la puissance calculee
		setMotorSpeed(moteur, power);
	}
}

// Fonction pour calculer la somme des erreurs (integral)
float calculateIntegral2() {
	float sum = 0;
	for (int i = 0; i < 10; i++) {
		sum += errorHistory2[i];
	}
	return sum;
}

// Controle proportionnel integrale derivatif (PID)
task keepHeadingPID2() {
	semaphoreLock(semConsigne2);
	consigne2 = getGyroDegrees(gyro); // Reprednre la derniere position
	semaphoreUnlock(semConsigne2);
	while (true) {
		int capActual = getGyroDegrees(gyro);
		semaphoreLock(semConsigne2);
		int error = consigne2 - capActual;
		semaphoreUnlock(semConsigne2);
		int vitesseAngulaire = getGyroRate(gyro);

		// maj de l'historique des erreurs (pour le calcul d'integrale)
		errorHistory2[historyIndex2] = error;
		historyIndex2 = (historyIndex2 + 1) % errorHistorySize;

		// Calcul d'integrale
		float integral = calculateIntegral2();

		// Equation PID
		float speed = -P * error + I * integral + D * vitesseAngulaire;

		// Arret du moteur si l'erreur est toleree
		if (abs(error) < tolerance) { launchMotorSpeed2(0); }
		else { launchMotorSpeed2(speed); } // lancement du moteur a la vitesse calculee
	}
}

task IHM2() {
	while (true) {
		eraseDisplay();
		displayCenteredTextLine(1, "Mode Mobile");
		displayTextLine(3, "Consignes:");
		displayTextLine(4, "Btn Center: Menu Principal");

		displayTextLine(12, "DEBUG:");
		semaphoreLock(semConsigne2);
		displayTextLine(13, "Position initiale: %d", consigne2);
		semaphoreUnlock(semConsigne2);
		displayTextLine(14, "Position actuelle: %d", getGyroDegrees(gyro));

		delay(500); // Sans ce delai, l'affichage de debug ne foncionne pas
	}
}
