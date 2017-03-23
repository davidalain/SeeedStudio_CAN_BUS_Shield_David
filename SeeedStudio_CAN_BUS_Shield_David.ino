#include <Arduino.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>
#include <SPI.h>

#include "OBD2.h"

//================= global variables ========================
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);     // Set CS pin

OBD2 obd2(CAN); //set CAN bus to ODB2 driver
//===========================================================



//================ Setup and loop functions ==================
void setup()
{
	Serial.begin(115200);

	while (CAN_OK != obd2.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
	{
		Serial.println("CAN BUS Shield init fail");
		Serial.println(" Init CAN BUS Shield again");
		delay(100);
	}
	Serial.println("CAN BUS Shield init ok!");
}

void loop()
{
	obd2.showSupportedPIDs();
	obd2.showEngineRPM();
	obd2.showVehicleSpeed();
	obd2.showThrottlePosition();
	obd2.showFuelTankLevelInput();

	delay(500);                       // send data per 100ms
}

/*********************************************************************************************************
  END FILE
 *********************************************************************************************************/
