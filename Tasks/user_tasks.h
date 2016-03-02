#pragma once

static const uint8_t TASK_TAPIS_ENTREE_PRIORITY = 10;
static const uint8_t TASK_ROBOT_PRIORITY = 20;
static const uint8_t TASK_TAPIS_SORTIE_PRIORITY = 30;
static const uint8_t TASK_SENSORS_PRIORITY = 40;
static const uint8_t TASK_ACTUATORS_PRIORITY = 50;

static const uint8_t SENSORS_QUEUE_LENGTH = 5;
static const uint8_t ACTUATORS_QUEUE_LENGTH = 5;

static const uint16_t SENSORS_UPDATE_PERIOD = 250; // millis
static const uint16_t ACTUATORS_UPDATE_PERIOD = 100; // millis

void vTaskAcquisitionCapteur( void *pvParameters );
void ReadSensors( uint16_t * sensors );

void vTaskEcritureActionneur( void *pvParameters );
void WriteActuators( uint16_t * actuators );

void vTaskTapisSortie( void *pvParameters );
void DeplacerTapisSortie( uint8_t etat );

void vTaskTapisEntree( void *pvParameters );
void DeplacerTapisEntree( uint8_t etat );

void vTaskRobot( void *pvParameters );
void deplacerRobot( uint8_t x , uint8_t y );
void resetRobot( void );
void prendrePiece( uint8_t prendre );
