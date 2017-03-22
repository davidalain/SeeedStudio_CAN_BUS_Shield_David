// demo: CAN-BUS Shield, send data
#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>


/**
 * Created by David Alain
 *
 * Standard PID as defined by SAE J1979.
 * Mode 01
 *
 * @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01
 *
 */
typedef enum{
	PID_Code_PIDs_supported_01_to_20 = 0x00,
	PID_Code_Fuel_system_status = 0x03,
	PID_Code_Calculated_engine_load = 0x04,
	PID_Code_Engine_coolant_temperature = 0x05,
	PID_Code_Fuel_pressure = 0x0A,
	PID_Code_Engine_RPM = 0x0C,
	PID_Code_Vehicle_speed = 0x0D,
	PID_Code_Throttle_position = 0x11,
	PID_Code_PIDs_supported_21_to_40 = 0x20,

	PID_Code_Fuel_Tank_Level_Input = 0x2F,
}ODB2_PID_Code;


/**
 * @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Modes
 */
typedef enum{
	PID_Mode_Show_current_data = 0x01,
	PID_Mode_Show_freeze_frame_data = 0x02,
	PID_Mode_Show_stored_Diagnostic_Trouble_Codes = 0x03,
	PID_Mode_Clear_Diagnostic_Trouble_Codes_and_stored_values = 0x04,
	//	PID_Mode_Test_results_oxygen_sensor_monitoring_non_CAN_only = 0x05,
	PID_Mode_Test_results_other_component_system_monitoring_Test_results_oxygen_sensor_monitoring_for_CAN_only = 0x06,
	PID_Mode_Show_pending_Diagnostic_Trouble_Codes_detected_during_current_or_last_driving_cycle = 0x07,
	PID_Mode_Control_operation_of_on_board_component_system = 0x08,
	PID_Mode_Request_vehicle_information = 0x09,
	PID_Mode_Permanent_Diagnostic_Trouble_Codes_DTCs_Cleared_DTCs = 0x0A,
}OBD2_PID_Mode;

/**
 * Created by David Alain
 *
 * @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#CAN_.2811-bit.29_bus_format
 */
typedef struct{
	union{
		byte additional_data_byte;	/** By default: 2. Length of next bytes of this message **/
		byte mode;					/** OBD2 PID Mode. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Modes **/
		byte pid_code;				/** OBD2 PID Code. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01**/
	}query_sae_standard;

	union{
		byte additional_data_byte;	/** By default: 3. Length of next bytes of this message.**/
		byte mode;					/** OBD2 PID Mode. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Modes **/
		byte pid_code_byte1;		/** OBD2 PID Code byte 1. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01**/
		byte pid_code_byte2;		/** OBD2 PID Code byte 2. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01**/
	}query_vehicle_specific;

	byte stmp_buf[8];

}__attribute__((packed)) query_stmp;

// ================ Functions declarations ==================
byte readData(OBD2_PID_Mode mode, ODB2_PID_Code pid_code, byte recv_data_buf[8]);

void showSupportedPIDs(void);
void showEngineRPM(void);
void showVehicleSpeed(void);
void showThrottlePosition(void);
void showFuelTankLevelInput(void);
//===========================================================

//================= global variables ========================
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;

MCP_CAN CAN(SPI_CS_PIN);     // Set CS pin
//===========================================================


uint16_t bytesToUint16(byte high, byte low)
{
	uint16_t out = 0;
	out = high;
	out <<= 8;
	return (out | low);
}


/**
 * 	http://wiki.seeed.cc/CAN-BUS_Shield_V1.2/
 *	http://www.instructables.com/id/Hack-your-vehicle-CAN-BUS-with-Arduino-and-Seeed-C/?ALLSTEPS
 *	https://en.wikipedia.org/wiki/CAN_bus#Data_frame
 *
 *	Read data
 *
 * @param mode			Standard PID mode, from 01 to 09. @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Standard_PIDs
 * @param pid			Parameter ID. Identifier of parameter that you are getting data from
 * @param data_recv		Received message data
 *
 * @return Length of response message
 */
byte readData(OBD2_PID_Mode mode, ODB2_PID_Code pid_code, byte recv_data_buf[8]){

	// ================ send request ======================
	query_stmp data;

	//zerofill buffer
	memset(data.stmp_buf, 0x00, 8);

	data.query_sae_standard.additional_data_byte = 2;
	data.query_sae_standard.mode = mode;
	data.query_sae_standard.pid_code = pid_code;

	/**
	 *	id:		represents where the data comes from. (The CAN id).
	 *	ext:	represents the status of the frame. ‘0’ means standard frame (Identifier with 11 bit); ‘1’ means extended frame (Identifier with 29 bit).
	 *	len:	represents the length of this frame (length of data).
	 *	buf:	is the content of this message (data).
	 */
#if 1
	CAN.sendMsgBuf(0x00, 0, 8, data.stmp_buf); //CAN id = 0x00, ext = 0 (standard fram), len = 8 (data with 8 bytes), buf = message data
#else
	CAN.sendMsgBuf(0x7df, 0, 8, data.stmp_buf); //CAN id = 0x7df, ext = 0 (standard fram), len = 8 (data with 8 bytes), buf = message data

	//TODO: There is a example using CAN id = 0x7df (https://github.com/Seeed-Studio/CANBUS_SHIELD_OBD_RECIPLE/blob/master/cbs_reciple_data.cpp).
	//TODO: Check what's correct.
#endif

	//================ do something =======================

	//TODO: Check what's correct
#if 0
	delay(500); //Wait
#else
	/**
	 *	As seen at function sendOrder()
	 *	@see https://github.com/Seeed-Studio/CANBUS_SHIELD_OBD_RECIPLE/blob/master/cbs_reciple_data.cpp
	 */
	while(CAN_NOMSG == CAN.checkReceive());
	while(CAN_NOMSG == CAN.checkReceive());
	while(CAN_NOMSG == CAN.checkReceive());
#endif

	// ================ read response =====================
	byte len = 0;

	if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
	{
		CAN.readMsgBuf(&len, recv_data_buf);    // read data,  len: data length, buf: data buf
	}

	return len;

}

/**
 * Read all PID supported by connected vehicle (from PID 0x01 to 0x1F)
 */
void showSupportedPIDs(void){

	byte data_recv[8];

	byte len = readData(PID_Mode_Show_current_data, PID_Code_PIDs_supported_01_to_20, data_recv); //mode=0x01, PID=0x00

	unsigned long canId = CAN.getCanId(); //TODO: Check if this can be called here

	Serial.println("-----------------------------");
	Serial.print("Get data from ID: ");
	Serial.println(canId, HEX);
	Serial.println(" ===== Supported PIDs ===== ");
	Serial.println("PID\tStatus");

	/**
	 * Prints as table doing bitwise operations as seen at https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_1_PID_00
	 */

	for(int i = 0; i<len; i++)    // print the data
	{
		for(int bit = 0 ; bit < 8 ; bit++){

			byte mask = (data_recv[i] & (1 << (7 - bit)));
			byte value = (i * 8) + bit;

			Serial.print(value, HEX);
			Serial.print("\t");
			Serial.println((mask != 0) ? "YES" : "NO");
		}
	}
	Serial.println();

}

/**
 * Show current Engine RPM
 */
void showEngineRPM(void){

	byte data_recv[8];

	readData(PID_Mode_Show_current_data, PID_Code_Engine_RPM, data_recv);

	/**
	 * Formula of PID 0Ch in Mode 01
	 * @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_1_PID_00
	 */
	uint16_t rpm = bytesToUint16(data_recv[3], data_recv[4]) / 4;

	Serial.print("RPM: ");
	Serial.println(rpm);

}

/**
 * Show current speed
 */
void showVehicleSpeed(void){

	byte data_recv[8];

	readData(PID_Mode_Show_current_data, PID_Code_Vehicle_speed, data_recv);

	/**
	 * Formula of PID 0Dh in Mode 01
	 * @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_1_PID_00
	 */
	byte speed = data_recv[3];

	Serial.print("Speed: ");
	Serial.print(speed);
	Serial.println(" km/h");

}

/**
 * Show current Throttle position
 */
void showThrottlePosition(void){

	byte data_recv[8];

	readData(PID_Mode_Show_current_data, PID_Code_Throttle_position, data_recv);

	/**
	 * Formula of PID 11h in Mode 01
	 * @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_1_PID_00
	 */
	uint16_t position = data_recv[3];

	position = (position * 100) / 255;

	Serial.print("Throttle position: ");
	Serial.print(position);
	Serial.println(" %");

}

/**
 * Show current Fuel Tank Level Input
 */
void showFuelTankLevelInput(void){

	byte data_recv[8];

	readData(PID_Mode_Show_current_data, PID_Code_Fuel_Tank_Level_Input, data_recv);

	/**
	 * Formula of PID 2Fh in Mode 01
	 * @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_1_PID_00
	 */
	uint16_t level = data_recv[3];

	level = (level * 100) / 255;

	Serial.print("Fuel Tank Level Input: ");
	Serial.print(level);
	Serial.println(" %");

}

//================ Setup and loop functions ========================

void setup()
{
	Serial.begin(115200);

	while (CAN_OK != CAN.begin(CAN_500KBPS))              // init can bus : baudrate = 500k
	{
		Serial.println("CAN BUS Shield init fail");
		Serial.println(" Init CAN BUS Shield again");
		delay(100);
	}
	Serial.println("CAN BUS Shield init ok!");
}

void loop()
{
	showSupportedPIDs();
	showEngineRPM();
	showVehicleSpeed();
	showThrottlePosition();
	showFuelTankLevelInput();

	delay(100);                       // send data per 100ms
}

/*********************************************************************************************************
  END FILE
 *********************************************************************************************************/
