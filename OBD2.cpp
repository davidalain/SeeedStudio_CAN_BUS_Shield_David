/*
 * OBD2.cpp
 *
 *  Created on: 22 de mar de 2017
 *      Author: DavidAlain
 */

#include "OBD2.h"

uint16_t bytesToUint16(byte high, byte low)
{
	uint16_t out = 0;
	out = high;
	out <<= 8;
	return (out | low);
}

/**
 * Constructor
 *
 * @note: You need to begin CAN after to call this and before to use this class methods
 *
 * @param can
 */
OBD2::OBD2(MCP_CAN & can) : _CAN(can)
{

}

OBD2::~OBD2() {
	// TODO Auto-generated destructor stub
}

/**
 * @param can_baudrate	Most cars uses CAN_500KBPS.
 *
 * 	But there are may possible values:
 *   CAN_5KBPS
 *   CAN_10KBPS
 *   CAN_20KBPS
 *   CAN_25KBPS
 *   CAN_31K25BPS
 *   CAN_33KBPS
 *   CAN_40KBPS
 *   CAN_50KBPS
 *   CAN_80KBPS
 *   CAN_83K3BPS
 *   CAN_95KBPS
 *   CAN_100KBPS
 *   CAN_125KBPS
 *   CAN_200KBPS
 *   CAN_250KBPS
 *   CAN_500KBPS
 *   CAN_666KBPS
 *   CAN_1000KBPS
 *
 * @return An error code
 * 	CAN_OK 			If CAN driver was begin successfully.
 * 	CAN_FAILINIT	If there was some problem.
 *
 *
 *	 @see <mcp_can_dfs.h>
 */
byte OBD2::begin(byte can_baudrate){
	return _CAN.begin(can_baudrate);
}


/**
 * 	@see: http://wiki.seeed.cc/CAN-BUS_Shield_V1.2/
 *	@see: http://www.instructables.com/id/Hack-your-vehicle-CAN-BUS-with-Arduino-and-Seeed-C/?ALLSTEPS
 *	@see: https://en.wikipedia.org/wiki/CAN_bus#Data_frame
 *
 *	Read data
 *
 * @param mode			Standard PID mode, from 01 to 09. @see https://en.wikipedia.org/wiki/OBD-II_PIDs#Standard_PIDs
 * @param pid			Parameter ID. Identifier of parameter that you are getting data from
 * @param can_id		[out] CAN ID
 * @param data_recv		Received message data
 *
 * @return Length of response message
 */
byte OBD2::readData(OBD2::PID_Mode mode,  OBD2::PID_Code pid_code, unsigned long * can_id, byte data_recv[8]){

	// ================ send request ======================
	/** @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#CAN_.2811-bit.29_bus_format **/
	byte stmp[8] = {0x02 /* len*/, mode /* mode */, pid_code /* pid */, 0x00, 0x00, 0x00, 0x00, 0x00};

	/**
	 *	id:		represents where the data comes from. (The CAN id).
	 *	ext:	represents the status of the frame. '0' means standard frame (Identifier with 11 bit); '1' means extended frame (Identifier with 29 bit).
	 *	len:	represents the length of this frame (length of data).
	 *	buf:	is the content of this message (data).
	 */
#if 0
	_CAN.sendMsgBuf(0x00, 0, 8, stmp); //CAN id = 0x00, ext = 0 (standard fram), len = 8 (data with 8 bytes), buf = message data
#else
	_CAN.sendMsgBuf(0x7df, 0, 8, stmp); //CAN id = 0x7df, ext = 0 (standard fram), len = 8 (data with 8 bytes), buf = message data

	//TODO: There is a example using CAN id = 0x7df (https://github.com/Seeed-Studio/CANBUS_SHIELD_OBD_RECIPLE/blob/master/cbs_reciple_data.cpp).
	//TODO: Check what's correct.
#endif

	//================ do something ? =====================


	// ================ read response =====================
	byte len = 0;

	if(CAN_MSGAVAIL == _CAN.checkReceive())            // check if data coming
	{
		_CAN.readMsgBuf(&len, data_recv);    // read data,  len: data length, buf: data buf

		*can_id = _CAN.getCanId();	// get CAN ID
	}

	return len;
}

/**
 * Read all PID supported by connected vehicle (from PID 0x01 to 0x1F)
 */
void OBD2::showSupportedPIDs(void){

	byte data_recv[8];
	unsigned long can_id;

	byte len = readData(PID_Mode_Show_current_data, PID_Code_PIDs_supported_01_to_20, &can_id, data_recv); //mode=0x01, PID=0x00

	Serial.println("-----------------------------");
	Serial.print("Get data from ID: ");
	Serial.println(can_id, HEX);
	Serial.println(" ===== Supported PIDs ===== ");
	Serial.println("PID\tStatus");

	/**
	 * Prints as table doing bitwise operations as seen at https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_1_PID_00
	 */

	for(int i = 0; i<len; i++)    // print the data
	{
		for(int bit = 0 ; bit < 8 ; bit++){

			byte mask = (data_recv[i] & (1 << (7 - bit)));
			byte pid_value = (i * 8) + bit;

			Serial.print(pid_value, HEX);
			Serial.print("\t");
			Serial.println((mask != 0) ? "YES" : "NO");
		}
	}
	Serial.println();

}

/**
 * Show current Engine RPM
 */
void OBD2::showEngineRPM(void){

	byte data_recv[8];
	unsigned long can_id;

	//	readData(PID_Mode_Show_current_data, PID_Code_Engine_RPM, &can_id, data_recv);
	readData(OBD2::PID_Mode_Show_current_data, OBD2::PID_Code_Engine_RPM, &can_id, data_recv);

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
void OBD2::showVehicleSpeed(void){

	byte data_recv[8];
	unsigned long can_id;

	readData(PID_Mode_Show_current_data, PID_Code_Vehicle_speed, &can_id, data_recv);

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
void OBD2::showThrottlePosition(void){

	byte data_recv[8];
	unsigned long can_id;

	readData(PID_Mode_Show_current_data, PID_Code_Throttle_position, &can_id, data_recv);

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
void OBD2::showFuelTankLevelInput(void){

	byte data_recv[8];
	unsigned long can_id;

	readData(PID_Mode_Show_current_data, PID_Code_Fuel_Tank_Level_Input, &can_id, data_recv);

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
