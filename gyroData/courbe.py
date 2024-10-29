import matplotlib.pyplot as plt

# Lire les données à partir du fichier
powers = []
speeds = []

with open('gyro_charac.csv', 'r') as file:
    next(file)  # Skip the header line
    for line in file:
        power, speed = line.split(',')
        powers.append(int(power))
        speeds.append(float(speed))

# Find the maximum speed and the corresponding minimum power
max_speed = max(speeds)
min_power_for_max_speed = min([powers[i] for i in range(len(speeds)) if speeds[i] == max_speed])
min_power = min([powers[i] for i in range(len(speeds)) if speeds[i] != 0])

# Print the result
print(f"The smallest power where the speed is the biggest ({max_speed}) is: {min_power_for_max_speed}")
print(f"The smallest power where the motor starts spinning is: {min_power}")

# Tracer les données
plt.plot(powers, speeds, marker='.')
plt.title('Vitesse angulaire du moteur en fonction de la puissance')
plt.xlabel('Puissance (%)')
plt.ylabel('Vitesse (degrés par seconde)')
plt.grid()
plt.xlim(0, 100)
plt.ylim(0, max(speeds) + 10)
plt.axhline(0, color='black', linewidth=0.5, linestyle='--')
plt.axvline(0, color='black', linewidth=0.5, linestyle='--')

# Afficher le graphique
plt.show()
