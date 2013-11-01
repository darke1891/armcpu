/*
 * $File: ram_driver.v
 * $Date: Fri Nov 01 13:28:17 2013 +0800
 * $Author: jiakai <jia.kai66@gmail.com>
 */

module ram_driver(
	input clk,
	input enable,
	input enable_read,
	input enable_write,
	// note: addr and data would not be latched
	input [20:0] addr,
	input [31:0] data_in,
	output [31:0] data_out,

	// assert for one cycle when finished previous writing
	output reg write_finished,	

	// assert when data could be continuously read; can not issue commands
	// when this is asserted
	output read_ready,

	output [19:0] baseram_addr,
	inout [31:0] baseram_data,
	output baseram_ce,
	output baseram_oe,
	output baseram_we,
	output [19:0] extram_addr,
	inout [31:0] extram_data,
	output extram_ce,
	output extram_oe,
	output extram_we);

	assign ram_selector = addr[20];

	reg ram_oe = 1, ram_we = 1;

	assign baseram_ce = ~(enable & ~ram_selector),
		extram_ce = ~(enable & ram_selector),
		baseram_oe = ~(enable & ~ram_selector & ~ram_oe),
		extram_oe = ~(enable & ram_selector & ~ram_oe),
		baseram_we = ~(enable & ~ram_selector & ~ram_we),
		extram_we = ~(enable & ram_selector & ~ram_we);

	assign data_out = ram_selector ? extram_data : baseram_data;

	assign baseram_data = baseram_oe ? data_in : {32{1'bz}},
		extram_data = extram_oe ? data_in : {32{1'bz}},
		baseram_addr = addr[19:0],
		extram_addr = addr[19:0];

	
	reg [1:0] state;
	localparam IDLE = 2'b00, READ = 2'b01, WRITE0 = 2'b11, WRITE1 = 2'b10;

	assign read_ready = (state == READ);

	always @(posedge clk) begin
		case (state)

			IDLE: begin
				write_finished <= 0;
				if (enable & enable_read) begin
					ram_oe <= 0;
					state <= READ;
				end else if (enable & enable_write) begin
					ram_oe <= 1;
					state <= WRITE0;
				end else
					ram_oe <= 1;
			end

			READ: begin
				if (!enable_read) begin
					state <= IDLE;
					ram_oe <= 1;
				end
			end

			WRITE0:
				state <= WRITE1;

			WRITE1: begin
				write_finished <= 1;
				state <= IDLE;
			end

		endcase
	end

	always @(negedge clk)
		ram_we <= (state != WRITE0);

endmodule


