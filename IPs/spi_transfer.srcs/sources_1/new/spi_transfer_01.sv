`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/07/2022 08:07:14 PM
// Design Name: 
// Module Name: spi_transfer_01
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


module spi_transfer_01(
    input axi_clk,
    input axi_reset_n,
    
    //spi_sensor interface
    output logic sclk,
    input MISO,
    output logic MOSI,
    output logic slave_select,
    
    //another module to spi sensor,
    input logic [7:0] addr,
    input logic [7:0] config_data,
    input spi_trigger      //trigger to start communication
    

    );
    
    reg [4:0] i; //5bit counter, 0-31
    reg [2:0]done;//3bit state  
    
    
    //axi interface reset.
    always@(posedge axi_clk)
    begin
    if(!axi_reset_n)
    begin
        i<=0; 
        slave_select<=0;
    end
    end
 
    //address and data transfer
    always@(posedge sclk)
    begin
    if(axi_reset_n)
    begin
    if(spi_trigger==1)
    begin
        slave_select<=1;
    end
    
    if(i<8)
        MOSI<=addr[i];
    else if(i<16)
        MOSI<=config_data[i-8];
    else if(i<24)
        MOSI<=1;   //end of message
    else 
        slave_select<=0; //should set sclk off.
          
    i++;
    end
    end
  
    
endmodule
