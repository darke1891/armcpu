/*
 * $File: main.v
 * $Date: Fri Oct 25 15:05:51 2013 +0800
 */
module main(
    input clk11M,
	input clk50M,
    output [15:0] led,
    input rst_key,
	 
    output [0:6] segdisp0,
    output [0:6] segdisp1,
    
	// dm9000Aep eth
	inout [15:0] eth_data,
	output eth_cs,
	output eth_cmd,
	input eth_int,
	output eth_ior,
	output eth_iow,
	output eth_reset,
    output eth_clk
	 );

	wire [15:0] led0, led1;
	assign led[15:1] = led0[15:1] | led1[15:1];
	assign led[0] = 1'b1;

	led_looper looper_lclk(clk11M, led0, rst_key);
	led_looper looper_hclk(clk50M, led1, rst_key);
    
    reg [7:0] data;
    
    digseg_driver digseg1(data[7:4], segdisp1);
    digseg_driver digseg2(data[3:0], segdisp0);
    
    reg [3:0] state;
    assign eth_reset = rst_key;
    assign eth_clk = clk50M;
    
	always @(posedge clk50M) begin
		state <= state + 1'b1;
		if (rst_key == 0) begin
			state <= {4{1'b0}};
            data <= 16'hffff;
        end
        if (state == 6)
            data <= eth_data;
        
	end
    
    assign eth_cs = 1'b0;
    assign eth_cmd = ~(state == 2 || state == 3);
    assign eth_iow = ~(state == 2);
    assign eth_ior = ~(state == 6 || state == 7);
    assign eth_data = (eth_cmd) ? {16{1'bz}} : 16'h0028;

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


