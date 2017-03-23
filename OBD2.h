/*
 * OBD2.h
 *
 *  Created on: 22 de mar de 2017
 *      Author: DavidAlain
 */

#ifndef OBD2_H_
#define OBD2_H_

#include <Arduino.h>
#include <mcp_can.h>
#include <mcp_can_dfs.h>

class OBD2 {
public:

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
	}PID_Mode;

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

		//TODO: implement more PID codes
	}PID_Code;

#if 0
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

			byte a;
			byte b;
			byte c;
			byte d;
			byte notused;
		}query_sae_standard;

		union{
			byte additional_data_byte;	/** By default: 3. Length of next bytes of this message.**/
			byte mode;					/** OBD2 PID Mode. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Modes **/
			byte pid_code_byte1;		/** OBD2 PID Code byte 1. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01**/
			byte pid_code_byte2;		/** OBD2 PID Code byte 2. @see: https://en.wikipedia.org/wiki/OBD-II_PIDs#Mode_01**/

			byte a;
			byte b;
			byte c;
			byte d;
		}query_vehicle_specific;

		byte stmp_buf[8];

	}__attribute__((packed)) query_stmp;
#endif

	OBD2(MCP_CAN & can);
	virtual ~OBD2();

	/**
	 * Begin OBD2 driver.
	 *
	 * This method begins CAN with badurate as specified at 'can_baudrate'
	 *
	 * @param can_baudrate	Most cars uses CAN_500KBPS.
	 *
	 * 	But there are many possible values:
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
	byte begin(byte can_baudrate);

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
	byte readData(OBD2::PID_Mode mode,  OBD2::PID_Code pid_code, unsigned long * can_id, byte data_recv[8]);

	/**
	 * Read all PID supported by connected vehicle (from PID 0x01 to 0x1F)
	 */
	void showSupportedPIDs(void);

	/**
	 * Show current Engine RPM
	 */
	void showEngineRPM(void);

	/**
	 * Show current speed
	 */
	void showVehicleSpeed(void);

	/**
	 * Show current Throttle position
	 */
	void showThrottlePosition(void);

	/**
	 * Show current Fuel Tank Level Input
	 */
	void showFuelTankLevelInput(void);


private:
	MCP_CAN _CAN;
};

#endif /* OBD2_H_ */
