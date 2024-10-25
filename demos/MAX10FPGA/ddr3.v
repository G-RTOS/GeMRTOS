// ddr3.v

`timescale 1 ps / 1 ps
module  ddr3 (
        //Reset and Clocks
        input    wire        fpga_resetn    , // Device Resetn
        input    wire        clk_ddr3_100_p , // DDR3 dedicated reference clock, not used currently 
        input    wire        clk_50_max10   , // System Clock 50MHz
        input    wire        clk_25_max10   , // System Clock 25MHz
		
        //GPIOs
        output   wire [4:0]  user_led       , // LEDs
        input    wire [3:0]  user_pb        , // Push buttons
        input    wire [4:0]  user_dipsw     , // DIP switches
		
        //EMIF interface
        output   wire [13:0] ddr3_a         , // mem_a
        output   wire [2:0]  ddr3_ba        , // mem_ba
        inout    wire [0:0]  ddr3_ck        , // mem_ck
        inout    wire [0:0]  ddr3_ck_n      , // mem_ck_n
        output   wire [0:0]  ddr3_cke       , // mem_cke
        output   wire [0:0]  ddr3_cs_n      , // mem_cs_n
        output   wire [2:0]  ddr3_dm        , // mem_dm
        output   wire [0:0]  ddr3_ras_n     , // mem_ras_n
        output   wire [0:0]  ddr3_cas_n     , // mem_cas_n
        output   wire [0:0]  ddr3_we_n      , // mem_we_n
        output   wire        ddr3_reset_n   , // mem_reset_n
        inout    wire [23:0] ddr3_dq        , // mem_dq
        inout    wire [2:0]  ddr3_dqs       , // mem_dqs
        inout    wire [2:0]  ddr3_dqs_n     , // mem_dqs_n
        output   wire [0:0]  ddr3_odt         // mem_odt
        );

reg     [25:0]   heart_beat_cnt;

wire             local_cal_fail;
wire             local_cal_success;
wire             local_init_done;
wire             local_error_mon;
wire             local_status_mon;

assign ddr3_a[13] = 1'b0;


q_sys           u0_q_sys (
                .reset_reset_n                                 (fpga_resetn       ), //                               reset.reset_n
                .sys_clk_clk                                   (clk_50_max10      ), //                             sys_clk.clk
                .refclk_clock_bridge_in_clk_clk                (clk_ddr3_100_p    ), //          refclk_clock_bridge_in_clk.clk
                .memory_mem_a                                  (ddr3_a[12:0]      ), //                              memory.mem_a
                .memory_mem_ba                                 (ddr3_ba           ), //                                    .mem_ba
                .memory_mem_ck                                 (ddr3_ck           ), //                                    .mem_ck
                .memory_mem_ck_n                               (ddr3_ck_n         ), //                                    .mem_ck_n
                .memory_mem_cke                                (ddr3_cke          ), //                                    .mem_cke
                .memory_mem_cs_n                               (ddr3_cs_n         ), //                                    .mem_cs_n
                .memory_mem_dm                                 (ddr3_dm           ), //                                    .mem_dm
                .memory_mem_ras_n                              (ddr3_ras_n        ), //                                    .mem_ras_n
                .memory_mem_cas_n                              (ddr3_cas_n        ), //                                    .mem_cas_n
                .memory_mem_we_n                               (ddr3_we_n         ), //                                    .mem_we_n
                .memory_mem_reset_n                            (ddr3_reset_n      ), //                                    .mem_reset_n
                .memory_mem_dq                                 (ddr3_dq           ), //                                    .mem_dq
                .memory_mem_dqs                                (ddr3_dqs          ), //                                    .mem_dqs
                .memory_mem_dqs_n                              (ddr3_dqs_n        ), //                                    .mem_dqs_n
                .memory_mem_odt                                (ddr3_odt          ) //                                    .mem_odt
                // .msgdma_0_cal_fail_mon_export                  (local_cal_fail    ), //               msgdma_0_cal_fail_mon.export
                // .msgdma_0_cal_success_mon_export               (local_cal_success ), //            msgdma_0_cal_success_mon.export
                // .msgdma_0_status_mon_0_init_done_mon_export    (local_init_done   ), // msgdma_0_status_mon_0_init_done_mon.export
                // .master_driver_msgdma_0_conduit_end_error_mon  (local_error_mon   ), //  master_driver_msgdma_0_conduit_end.error_mon
                // .master_driver_msgdma_0_conduit_end_status_mon (local_status_mon  )  //                                    .status_mon
                );

//Heart beat by 50MHz clock
always @(posedge clk_50_max10 or negedge fpga_resetn)
  if (!fpga_resetn)
      heart_beat_cnt <= 26'h0; //0x3FFFFFF
  else
      heart_beat_cnt <= heart_beat_cnt + 1'b1;

assign user_led[0] = !local_init_done;
assign user_led[1] = !local_cal_success;
assign user_led[2] = local_error_mon;
assign user_led[3] = !local_status_mon;
assign user_led[4] = heart_beat_cnt[25];

endmodule
