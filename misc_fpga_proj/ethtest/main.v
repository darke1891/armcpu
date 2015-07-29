/*
 * $File: main.v
 * $Date: Fri Oct 25 15:05:51 2013 +0800
 */
module main(
    input clk11M,
	input clk50M,
    output [15:0] led,
    input rst_key,
    input [31:0] params,
	 
    output [0:6] segdisp0,
    output [0:6] segdisp1,
    
	// dm9000Aep eth
	inout [15:0] eth_data,
	output eth_cs,
	output reg eth_cmd,
	input eth_int,
	output reg eth_ior,
	output reg eth_iow,
	output eth_reset,
    output eth_clk
	 );

	wire [15:0] led0, led1;
	assign led[15:1] = led0[15:1] | led1[15:1];
	assign led[0] = 1'b1;

	led_looper looper_lclk(clk11M, led0, rst_key);
	led_looper looper_hclk(clk50M, led1, rst_key);
    
    reg [7:0] data;
    reg clk25M;
    
    digseg_driver digseg1(data[7:4], segdisp1);
    digseg_driver digseg2(data[3:0], segdisp0);
    
    reg [3:0] state;
    assign eth_reset = rst_key;
    assign eth_clk = clk25M;
    
    always @(posedge clk50M) begin
        clk25M = ~clk25M;
    end
    
	always @(posedge clk25M) begin
		state <= state + 1'b1;
		if (rst_key == 0) begin
			state <= {4{1'b0}};
            data <= 16'hffff;
        end
        if (state == 6)
            data <= params[31] ? eth_data[15:8] : eth_data[7:0];//data <= eth_data;
        if (params[30])
            data <= 8'haa;
        
	end
    
    reg [15:0] eth_data_reg;
    
    assign eth_cs = 1'b0;
    always @(*) begin
        if (params[30]) begin
            eth_cmd = ~(state == 2 || state == 3);
            eth_iow = ~(state == 2 || state == 6);
            eth_ior = 1;
            eth_data_reg = (eth_cmd) ? {8'b00000000, params[23:16]} : params[15:0];
        end
        else begin
            eth_cmd = ~(state == 2 || state == 3 );
            eth_iow = ~(state == 2);
            eth_ior = ~(state == 6 || state == 7);
            eth_data_reg = (eth_cmd) ? {16{1'bz}} : params[15:0];
        end
    end
    
    assign eth_data = eth_data_reg;

endmodule


module led_looper(
	input clk,
	output reg [15:0] led,
	input rst_key);

	reg [23:0] cnt;
	reg cnt_high_prev;
	wire cnt_changed = cnt[23] & ~cnt_high_prev;

	always @(posedge clk) begin
		cnt <= cnt + 1'b1;
		cnt_high_prev <= cnt[23];
		if (cnt_changed)
			led <= {led[14:0], ~ (|led[14:0])};
		//if (rst_key == 0)
		//	cnt <= {24{1'b0}};
	end
endmodule


