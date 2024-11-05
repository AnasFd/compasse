//------------- Mode Fixe -------------//

// Proprietes lineaires
const float maxSpeed = 880.0;  // Vitesse maximale
const float minSpeed = 40.0;   // Vitesse minimale
const int maxPower = 92;       // Puissance maximale
const int minPower = 2;        // Puissance minimale
const float pente = maxSpeed / maxPower;  // pente pour convertir la vitesse en puissance

// Variables pour la consigne
TSemaphore semConsigne; // Semaphore permettant une meilleur gestion de la consigne
int consigne = 0; // Initialisation de la consigne (le but a atteindre par le moteur)

// Variables pour le calcul d'integrale
float errorHistory[errorHistorySize]; // Tableau pour stocker les erreurs
int historyIndex = 0;  // Index pour le tableau errorHistory


// Tâche pour surveiller les boutons et ajuster la consigne
task watchButtons() {
	while(true) {
		// Bouton haut (rotation de 90 deg a droite)
		if (getButtonPress(buttonUp)) {
			semaphoreLock(semConsigne);
			consigne += 90;
			semaphoreUnlock(semConsigne);
			while(getButtonPress(buttonUp)) {} // Attendre que le bouton soit relache
		}

		// Bouton bas (rotation de 90 deg a gauche)
		if (getButtonPress(buttonDown)) {
			semaphoreLock(semConsigne);
			consigne -= 90;
			semaphoreUnlock(semConsigne);
			while(getButtonPress(buttonDown)) {}
		}

		// Bouton droit (rotation de 10 deg a droite)
		if (getButtonPress(buttonRight)) {
			semaphoreLock(semConsigne);
			consigne += 10;
			semaphoreUnlock(semConsigne);
			while(getButtonPress(buttonRight)) {}
		}

		// Bouton gauche (rotation de 10 deg a gauche)
		if (getButtonPress(buttonLeft)) {
			semaphoreLock(semConsigne);
			consigne -= 10;
			semaphoreUnlock(semConsigne);
			while(getButtonPress(buttonLeft)) {}
		}
	}
}

void launchMotorSpeed(float speed) {
	// Si vitesse null
	if (speed == 0) setMotorSpeed(moteur, 0);

	else {
		// Preserver le signe de la vitesse
        float absSpeed = fabs(speed);

		// Limiter la vitesse maximale
		absSpeed = (absSpeed > maxSpeed) ? maxSpeed : minSpeed;

		// calcul de la puissance en fonction de la vitesse
		int power = (int)(absSpeed / pente);

		// Verification que la puissance est ok
		if (power < minPower) {
			power = minPower;
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
float calculateIntegral() {
	float sum = 0;
	for (int i = 0; i < errorHistorySize; i++) {
		sum += errorHistory[i];
	}
	return sum;
}

// Controle proportionnel integrale derivatif (PID)
task keepHeadingPID() {
	semaphoreLock(semConsigne);
	consigne = nMotorEncoder[moteur]; // Reprednre la derniere position
	semaphoreUnlock(semConsigne);
	while (true) {
		int capActual = nMotorEncoder[moteur];

		semaphoreLock(semConsigne);
		int error = consigne - capActual;
		semaphoreUnlock(semConsigne);

		int vitesseAngulaire = getMotorRPM(moteur) * 6; // rpm -> rps

		// maj de l'historique des erreurs (pour le calcul d'integrale)
		errorHistory[historyIndex] = error;
		historyIndex = (historyIndex + 1) % errorHistorySize;

		// Calcul d'integrale
		float integral = calculateIntegral();

		// Equation PID
		float speed = P * error + I * integral + D * vitesseAngulaire;

		// Arret du moteur si l'erreur est toleree
		if (abs(error) < tolerance) launchMotorSpeed(0);
		else launchMotorSpeed(speed); // lancement du moteur a la vitesse calculee
	}
}

task IHM() {
	while (true) {
		eraseDisplay();
		displayCenteredTextLine(1, "Mode Fixe");

		displayTextLine(3, "Consignes:");
		displayTextLine(4, "Btn droite: +10 deg");
		displayTextLine(5, "Btn Gauche: -10 deg");
		displayTextLine(6, "Btn Haut  : +90 deg");
		displayTextLine(7, "Btn Bas   : -90 deg");
		displayTextLine(8, "Btn Center: Menu Principal");

		displayTextLine(12, "DEBUG:");
		semaphoreLock(semConsigne);
		displayTextLine(13, "But en degrees: %d", consigne);
		semaphoreUnlock(semConsigne);
		displayTextLine(14, "Position actuelle: %d", nMotorEncoder[moteur]);

		delay(300); // Sans le delai, l'affichage de debug ne fonctionne pas
	}
}
