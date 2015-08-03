`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   11:33:19 07/28/2015
// Design Name:   armcpu
// Module Name:   /home/cjld/summer_project/armcpu/armcpu/test.v
// Project Name:  armcpu
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: armcpu
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module test;

	// Inputs
	reg clk50M;
	reg rst_key;
	reg clk_manual;
	reg [31:0] params;
	reg com_RxD;
	reg kbd_enb_hi;
	reg kbd_enb_lo;
	reg [3:0] kbd_data;
	reg eth_int;
	reg [15:0] eth_data_m;

	// Outputs
	wire [0:6] segdisp0;
	wire [0:6] segdisp1;
	wire [15:0] led;
	wire [19:0] baseram_addr;
	wire baseram_ce;
	wire baseram_oe;
	wire baseram_we;
	wire [19:0] extram_addr;
	wire extram_ce;
	wire extram_oe;
	wire extram_we;
	wire com_TxD;
	wire [22:0] flash_addr;
	wire [7:0] flash_ctl;
	wire [8:0] vga_color_out;
	wire vga_hsync;
	wire vga_vsync;
	wire eth_cs;
	wire eth_cmd;
	wire eth_ior;
	wire eth_iow;
	wire eth_reset;

	// Bidirs
	wire [31:0] baseram_data;
	wire [31:0] extram_data;
	wire [15:0] flash_data;
	wire [15:0] eth_data;

	// Instantiate the Unit Under Test (UUT)
	armcpu uut (
		.clk50M(clk50M), 
		.rst_key(rst_key), 
		.clk_manual(clk_manual), 
		.segdisp0(segdisp0), 
		.segdisp1(segdisp1), 
		.led(led), 
		.params(params), 
		.baseram_addr(baseram_addr), 
		.baseram_data(baseram_data), 
		.baseram_ce(baseram_ce), 
		.baseram_oe(baseram_oe), 
		.baseram_we(baseram_we), 
		.extram_addr(extram_addr), 
		.extram_data(extram_data), 
		.extram_ce(extram_ce), 
		.extram_oe(extram_oe), 
		.extram_we(extram_we), 
		.com_TxD(com_TxD), 
		.com_RxD(com_RxD), 
		.flash_addr(flash_addr), 
		.flash_data(flash_data), 
		.flash_ctl(flash_ctl), 
		.vga_color_out(vga_color_out), 
		.vga_hsync(vga_hsync), 
		.vga_vsync(vga_vsync), 
		.kbd_enb_hi(kbd_enb_hi), 
		.kbd_enb_lo(kbd_enb_lo), 
		.kbd_data(kbd_data), 
		.eth_data(eth_data), 
		.eth_cs(eth_cs), 
		.eth_cmd(eth_cmd), 
		.eth_int(eth_int), 
		.eth_ior(eth_ior), 
		.eth_iow(eth_iow), 
		.eth_reset(eth_reset)
	);
	assign eth_data = eth_data_m;

	initial begin
		// Initialize Inputs
		clk50M = 0;
		rst_key = 0;
		clk_manual = 0;
		params = 0;
		com_RxD = 0;
		kbd_enb_hi = 0;
		kbd_enb_lo = 0;
		kbd_data = 0;
		eth_int = 0;
		params[0] = 1;
		params[1] = 1;
		eth_data_m = 16'hzzzz;

		// Wait 100 ns for global reset to finish
		#100;
        
		// Add stimulus here
		
		rst_key = 1;
		
	end
	
	always
		#10 clk50M = ~clk50M;
      
endmodule

