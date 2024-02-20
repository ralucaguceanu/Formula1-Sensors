#include "functions.h"

/* alocarea memoriei pentru o structura de tip power_management_unit */
power_management_unit *allocPMUSensor(void)
{
	power_management_unit *PMUSensor = malloc(sizeof(power_management_unit));
	if (PMUSensor == NULL) {
		printf("Malloc failed\n");
	}
	memset(PMUSensor, 0, sizeof(power_management_unit));
	return PMUSensor;
}

/* alocarea memoriei pentru o structura de tip tire_sensor */
tire_sensor *allocTireSensor(void)
{
	tire_sensor *TireSensor = (tire_sensor *)malloc(sizeof(tire_sensor));
	if (TireSensor == NULL) {
		printf("Malloc failed\n");
	}
	memset(TireSensor, 0, sizeof(tire_sensor));
	return TireSensor;
}

/* citirea din fisier a datelor si a operatiilor pentru un senzor PMU */
void readPMUSensor(sensor *sensorInfo, int countSensor, FILE *in)
{
	int countOp;
	sensorInfo[countSensor].sensor_type = PMU;

	sensorInfo[countSensor].sensor_data = (void *)allocPMUSensor();
	fread(&((power_management_unit *)sensorInfo[countSensor].sensor_data)
		->voltage, sizeof(float), 1, in);
	fread(&((power_management_unit *)sensorInfo[countSensor].sensor_data)
		->current, sizeof(float), 1, in);
	fread(&((power_management_unit *)sensorInfo[countSensor].sensor_data)
		->power_consumption, sizeof(float), 1, in);
	fread(&((power_management_unit *)sensorInfo[countSensor].sensor_data)
		->energy_regen, sizeof(int), 1, in);
	fread(&((power_management_unit *)sensorInfo[countSensor].sensor_data)
		->energy_storage, sizeof(int), 1, in);
	fread(&sensorInfo[countSensor].nr_operations, sizeof(int), 1, in);

	/* alocarea memoriei pentru vectorul cu indicii operatiilor */
	sensorInfo[countSensor].operations_idxs = (int *)malloc(
		sensorInfo[countSensor].nr_operations * sizeof(int));
	if (sensorInfo[countSensor].operations_idxs == NULL) {
		printf("Malloc failed\n");
	}

	for (countOp = 0; countOp < sensorInfo[countSensor].nr_operations;) {
		fread(&sensorInfo[countSensor].operations_idxs[countOp],
			sizeof(int), 1, in);
		countOp++;
	}
}

/* citirea din fisier a datelor si a operatiilor pentru un senzor Tire */
void readTireSensor(sensor *sensorInfo, int countSensor, FILE *in)
{
	int countOp;
	sensorInfo[countSensor].sensor_type = TIRE;
	sensorInfo[countSensor].sensor_data = (void *)allocTireSensor();
	fread(&((tire_sensor *)sensorInfo[countSensor].sensor_data)
		->pressure, sizeof(float), 1, in);
	fread(&((tire_sensor *)sensorInfo[countSensor].sensor_data)
		->temperature, sizeof(float), 1, in);
	fread(&((tire_sensor *)sensorInfo[countSensor].sensor_data)
		->wear_level, sizeof(int), 1, in);
	fread(&((tire_sensor *)sensorInfo[countSensor].sensor_data)
		->performace_score, sizeof(int), 1, in);
	fread(&sensorInfo[countSensor].nr_operations, sizeof(int), 1, in);

	/* alocarea memoriei pentru vectorul cu indicii operatiilor */
	sensorInfo[countSensor].operations_idxs = (int *)malloc(
			sensorInfo[countSensor].nr_operations * sizeof(int));
	if (sensorInfo[countSensor].operations_idxs == NULL) {
		printf("Malloc failed\n");
	}

	for (countOp = 0; countOp < sensorInfo[countSensor].nr_operations;) {
		fread(&sensorInfo[countSensor].operations_idxs[countOp],
			sizeof(int), 1, in);
		countOp++;
	}
}

/* afisarea unui senzor PMU */
void printPMUSensor(sensor *sensorInfo, int commandIndex)
{
	printf("Power Management Unit\n");
	printf("Voltage: %.2f\n", ((power_management_unit *)
		sensorInfo[commandIndex].sensor_data)->voltage);
	printf("Current: %.2f\n", ((power_management_unit *)
		sensorInfo[commandIndex].sensor_data)->current);
	printf("Power Consumption: %.2f\n", ((power_management_unit *)
		sensorInfo[commandIndex].sensor_data)->power_consumption);
	printf("Energy Regen: %d%%\n", ((power_management_unit *)
		sensorInfo[commandIndex].sensor_data)->energy_regen);
	printf("Energy Storage: %d%%\n", ((power_management_unit *)
		sensorInfo[commandIndex].sensor_data)->energy_storage);
}

/* afisarea unui senzor Tire */
void printTireSensor(sensor *sensorInfo, int commandIndex)
{
	printf("Tire Sensor\n");
	printf("Pressure: %.2f\n", ((tire_sensor *)
		sensorInfo[commandIndex].sensor_data)->pressure);
	printf("Temperature: %.2f\n", ((tire_sensor *)
		sensorInfo[commandIndex].sensor_data)->temperature);
	printf("Wear Level: %d%%\n", ((tire_sensor *)
		sensorInfo[commandIndex].sensor_data)->wear_level);
	if (((tire_sensor *)sensorInfo[commandIndex].sensor_data)
		->performace_score != 0) {
		printf("Performance Score: %d\n", ((tire_sensor *)
			sensorInfo[commandIndex].sensor_data)->performace_score);
	} else {
		printf("Performance Score: Not Calculated\n");
	}
}

/* functie necesara la sortarea vectorului de senzori
in functie de prioritati */
int compare(const void *a, const void *b)
{
	sensor *firstSensor = (sensor *)a;
	sensor *secondSensor = (sensor *)b;
	if (firstSensor->sensor_type == TIRE && secondSensor->sensor_type == PMU) {
		return 1;
	}
	return -1;
}

/* apelarea functiilor de operatii prin vectorul operations */
void analyse(sensor *sensorInfo, int commandIndex, op *operations)
{
	int countOp;
	for (countOp = 0; countOp < sensorInfo[commandIndex].nr_operations;) {
		operations[sensorInfo[commandIndex].operations_idxs[countOp]](
			sensorInfo[commandIndex].sensor_data);
		countOp++;
	}
}

int main(int argc, char const *argv[])
{
	int sensorNumber = 0;
	FILE *in = fopen(argv[1], "rb");
	fread(&sensorNumber, sizeof(int), 1, in);

	/* alocarea memoriei pentru vectorul de senzori */
	sensor *sensorInfo = (sensor *)malloc(sensorNumber * sizeof(sensor));
	if (sensorInfo == NULL) {
		printf("Malloc failed\n");
	}

	int countSensor = 0, sensorType;

	/* parcurgerea fisierului pana la ultimul senzor si stocarea informatiilor
	despre fiecare senzor in functie de tipul acestuia */
	for (countSensor = 0; countSensor < sensorNumber; countSensor++) {
		fread(&sensorType, sizeof(int), 1, in);
		switch (sensorType) {
		case 1:
			readPMUSensor(sensorInfo, countSensor, in);
			break;

		case 0:
			readTireSensor(sensorInfo, countSensor, in);
			break;

		default:
			break;
		}
	}
	fclose(in);

	/* sortarea vectorului de senzori */
	qsort(sensorInfo, sensorNumber, sizeof(sensor), compare);

	char command[100], whiteSpace;
	int commandIndex;
	fscanf(stdin, "%s", command);

	/* alocarea memoriei pentru vectorul de operatii */
	op *operations = (op *)malloc(8 * sizeof(op));
	if (operations == NULL) {
		printf("Malloc failed\n");
	}

	/* alcatuirea vectorului de operatii */
	get_operations((void **)(operations));


	/* citirea si executarea comenzilor */
	while (strcmp(command, "exit") != 0) {
		if (strcmp(command, "clear") != 0) {
			fscanf(stdin, "%c", &whiteSpace);
			fscanf(stdin, "%d", &commandIndex);
			if (commandIndex >= sensorNumber || commandIndex < 0) {
				printf("Index not in range!\n");
				fscanf(stdin, "%s", command);
				continue;
			}
			if (strcmp(command, "print") == 0) {
				if (sensorInfo[commandIndex].sensor_type == PMU) {
					printPMUSensor(sensorInfo, commandIndex);
				} else {
					printTireSensor(sensorInfo, commandIndex);
				}
			} else if (strcmp(command, "analyze") == 0) {
				analyse(sensorInfo, commandIndex, operations);
			}
		} else {
			int newCountSensor;

			/* parcurgerea vectorului de senzori si verificarea
			validitatii acestora */
			for (countSensor = 0; countSensor < sensorNumber; countSensor++) {
				if (sensorInfo[countSensor].sensor_type == PMU) {
					if (((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->voltage < 10
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->voltage > 20
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->current < -100
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->current > 100
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->power_consumption < 0
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->power_consumption > 1000
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->energy_regen < 0
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->energy_regen > 100
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->energy_storage < 0
					|| ((power_management_unit *)
						sensorInfo[countSensor].sensor_data)
							->energy_storage > 100) {

						/* eliberarea memoriei pentru senzorul PMU invalid */
						free((power_management_unit *)
							sensorInfo[countSensor].sensor_data);
						free((power_management_unit *)
							sensorInfo[countSensor].operations_idxs);

						/* stergerea senzorului PMU invalid */
						newCountSensor = countSensor;
						while (newCountSensor < sensorNumber - 1) {
							sensorInfo[newCountSensor]
								= sensorInfo[newCountSensor + 1];
							newCountSensor++;
						}

						/* actualizarea numarului de senzori */
						sensorNumber--;

						/* realocarea vectorului pentru noua dimensiune */
						sensor *auxRealloc;
						auxRealloc = (sensor *)realloc(
							sensorInfo, (sensorNumber) * sizeof(sensor));
						if (auxRealloc) {
							sensorInfo = auxRealloc;
						} else {
							printf("Realloc failed\n");
						}

						countSensor--;
					}
				} else {
					if (((tire_sensor *)
						sensorInfo[countSensor].sensor_data)
							->pressure < 19
					|| ((tire_sensor *)
						sensorInfo[countSensor].sensor_data)
							->pressure > 28
					|| ((tire_sensor *)
						sensorInfo[countSensor].sensor_data)
							->temperature < 0
					|| ((tire_sensor *)
						sensorInfo[countSensor].sensor_data)
							->temperature > 120
					|| ((tire_sensor *)
						sensorInfo[countSensor].sensor_data)
							->wear_level < 0
					|| ((tire_sensor *)
						sensorInfo[countSensor].sensor_data)
							->wear_level > 100) {

						/* eliberarea memoriei pentru senzorul Tire invalid */
						free((tire_sensor *)
							sensorInfo[countSensor].sensor_data);
						free((tire_sensor *)
							sensorInfo[countSensor].operations_idxs);

						/* stergerea senzorului Tire invalid */
						newCountSensor = countSensor;
						while (newCountSensor < sensorNumber - 1) {
							sensorInfo[newCountSensor]
								= sensorInfo[newCountSensor + 1];
							newCountSensor++;
						}

						/* actualizarea numarului de senzori */
						sensorNumber--;

						/* realocarea vectorului pentru noua dimensiune */
						sensor *auxRealloc;
						auxRealloc = (sensor *)realloc(
							sensorInfo, (sensorNumber) * sizeof(sensor));
						if (auxRealloc) {
							sensorInfo = auxRealloc;
						} else {
							printf("Realloc failed\n");
						}

						countSensor--;
					}
				}
			}
		}

		fscanf(stdin, "%s", command);
	}

	/* eliberarea memoriei alocate */
	if (strcmp(command, "exit") == 0) {
		for (countSensor = 0; countSensor < sensorNumber; countSensor++) {
			free(sensorInfo[countSensor].sensor_data);
			free(sensorInfo[countSensor].operations_idxs);
		}
		free(sensorInfo);
		free(operations);
	}

	return 0;
}
