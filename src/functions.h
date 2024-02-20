#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

/* declararea unui tip de date drept pointer catre o functie de operatii */
typedef void (*op)(void *data);

power_management_unit *allocPMUSensor(void);
tire_sensor *allocTireSensor(void);
void readPMUSensor(sensor *sensorInfo, int countSensor, FILE *in);
void readTireSensor(sensor *sensorInfo, int countSensor, FILE *in);
void printPMUSensor(sensor *sensorInfo, int commandIndex);
void printTireSensor(sensor *sensorInfo, int commandIndex);
int compare(const void *a, const void *b);
void get_operations(void **operations);
void analyse(sensor *sensorInfo, int commandIndex, op *operations);
