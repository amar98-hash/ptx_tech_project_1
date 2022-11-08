`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/07/2022 10:36:44 PM
// Design Name: 
// Module Name: spi_receiver
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


module spi_receiver(
    input axi_clk,
    input axi_reset_n,
    
    input i_sclk,
    input spi_trigger,
    
    output o_sclk,
    input MISO,
    output slave_select,
    
    output logic data_frame[1281:0]
    );
    
    
     
    reg [10:0] i; //5bit counter, 0-31
    
    
    //relayed spi clock line
    assign o_sclk=i_sclk;
    
    //relayed spi trigger line
    assign slave_select=spi_trigger;
        
    //axi interface reset.
    always@(posedge axi_clk)
    begin
    if(!axi_reset_n)
    begin
        i<=0; 
       // slave_select<=0;
    end
    end
 
    //address and data transfer
    always@(posedge i_sclk)
    begin
    if(axi_reset_n & spi_trigger)
    begin
    if(i<1282)
    begin
        data_frame[i]<=MISO;
    end
    
    i++;
    end
    end
    
endmodule
