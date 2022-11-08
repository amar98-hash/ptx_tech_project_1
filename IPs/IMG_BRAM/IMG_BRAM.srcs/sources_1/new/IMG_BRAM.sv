`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 11/08/2022 06:39:44 AM
// Design Name: 
// Module Name: IMG_BRAM
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


module IMG_URAM(

    input axi_clk,
    input axi_reset_n,
    
    input i_sclk,
    input spi_MISO_data,
    
    input spi_receive_start

    
    );
    
    
    reg [7:0] R_data;
    reg [7:0] B_data;
        
    reg [4:0] i; //counter
    
    always@(posedge i_sclk)
    begin
    if(axi_reset_n & spi_receive_start)
    begin
       
    if(i>1 & i<10) //i=2,3,4..9, st. i-2=0,1,..,7
    begin
        R_data<=spi_MISO_data<<(i-2); 
        
    end 
    else if(i>9 & i<18)
    begin
       B_data<=spi_MISO_data<<(i-10);  
    end
    i++; 
    if(i==18)
       i=0;   
    end
    end
        
    
    
    
endmodule
