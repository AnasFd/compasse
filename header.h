#ifndef HEADER_H
#define HEADER_H

#define tolerance 1      // Tolerance pour eviter les micro corrections
#define errorHistorySize 10

// Régulation qui marche bien pour les deux modes (pour mon moteur)
#define P 10.0
#define D -0.02
#define I 0.001

// fonctions pour le mode fixe
task watchButtons();
void launchMotorSpeed(float speed);
float calculateIntegral();
task keepHeadingPID();
task IHM();

// fonctions pour le mode mobile
task watchButtons2();
void launchMotorSpeed2(float speed);
float calculateIntegral2();
task keepHeadingPID2();
task IHM2();

// fonctions pour le main
task interface(); // Interface principale du programme
void initialize(); // Menu d'initialisation et choix du mode
void startFix(); // Lancement du mode fixe
void startMobile(); // Lancement du mode mobile
void stopFix(); // Arret du mode fixe
void stopMobile(); // Arret du mode fixe

#endif
