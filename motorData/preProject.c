float val[101];
long fileHandle;

//task writeData() {}

task motorCharacterization() {
	for (int i = 0; i <= 100; i++)
  {
    setMotorSpeed(motorA, i);
    wait1Msec(300);
    val[i] = getMotorRPM(motorA) * 6; // R*360/60 to get the value in rotations per second
  }

  string valString = "power,rpm\n";
  fileWriteData(fileHandle, valString, strlen(valString));
	for (int i = 0; i <= 100; i++)
  {
  	stringFormat(valString,"%d,%.2f\n",i,val[i]);
		fileWriteData(fileHandle, valString, strlen(valString));
		stringFormat(valString,"");
	}
	fileClose(fileHandle);
	stopAllTasks();
}

task main()
{
	string filename = "rps.csv";
	fileHandle = fileOpenWrite(filename);
	startTask(motorCharacterization);
	//startTask(writeData);
}
