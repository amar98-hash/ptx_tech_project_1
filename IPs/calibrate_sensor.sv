`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/07/2022 09:09:25 AM
// Design Name: 
// Module Name: calibrate_sensor
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

/********************************************************************
   Function:         write_calibration_settings_to_sensor

   Description:     write calibration data (from eeprom) to trim registers (sensor)

   Dependencies:
 *******************************************************************/

module calibrate_sensor#(parameter mbit_calib=0,parameter bias_calib=0,
                         parameter clk_calib=0,parameter bpa_calib=0,parameter pu_calib=0)(
     
       
    
    input logic  perform_calibration,   
    output logic trigger_spi_transfer
    );
    // `include "sensor_defs.vh"
    
    //trigers calibration of the sensor upon user trigger. 
    always@(*)
    begin
    if(perform_calibration)
        trigger_spi_transfer<=1;
    else
        trigger_spi_transfer<=0;
    end
    
endmodule


