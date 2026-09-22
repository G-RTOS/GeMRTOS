module  m10_rgmii (
        //Clock and Reset
        input  wire        clk_50_max10,
        input  wire        clk_ddr3_100_p,
        input  wire        fpga_resetn,

        //LED PB DIPSW
        output wire [4:0]  user_led,
        input  wire [3:0]  user_pb,
        input  wire [4:0]  user_dipsw,

        //Dual Ethernet
        output wire        enet_mdc,
        inout  wire        enet_mdio,
        

		
        output wire        enet_resetn,	
        input  wire        enet_rx_clk,
        input  wire        enet_tx_clk,
        output wire        enet_gtx_clk,
        input  wire [3:0]  enet_rx_d,
        output wire [3:0]  enet_tx_d,
        output wire        enet_tx_en,
        input  wire        enet_rx_dv,
        input  wire        enet_led_link100,
 
        output wire [13:0] mem_a,
        output wire [2:0]  mem_ba,
        inout  wire [0:0]  mem_ck,
        inout  wire [0:0]  mem_ck_n,
        output wire [0:0]  mem_cke,
        output wire [0:0]  mem_cs_n,
        output wire [0:0]  mem_dm,
        output wire [0:0]  mem_ras_n,
        output wire [0:0]  mem_cas_n,
        output wire [0:0]  mem_we_n,
        output wire        mem_reset_n,
        inout  wire [7:0]  mem_dq,
        inout  wire [0:0]  mem_dqs,
        inout  wire [0:0]  mem_dqs_n,
        output wire [0:0]  mem_odt,

        //QSPI
        output          qspi_clk,
        inout  [3:0]    qspi_io,
        output          qspi_csn
        );

//Heart-beat counter
reg   [25:0]  heart_beat_cnt;

// gemrtos phy ins and outs
wire    [31:0]   GEMRTOS_OUTS;
wire    [31:0]   GEMRTOS_INS;

//DDR3 interface assignments
wire          local_init_done;
wire          local_cal_success;
wire          local_cal_fail;

//Ethernet interface assignments
wire          phy_resetn;
wire          system_resetn;
wire          fpga_reset;

wire          gemrtos_reset_bridge_out_reset;

wire          mdio_oen_from_the_tse_mac;
wire          mdio_out_from_the_tse_mac;
wire          eth_mode_from_the_tse_mac;
wire          ena_10_from_the_tse_mac;
wire          enet_tx_125;
wire          enet_tx_25;
wire          enet_tx_2p5;
wire          locked_from_the_enet_pll;
wire          tx_clk_to_the_tse_mac;
wire          tx_clk_to_the_tse_mac_g;

wire          enet_tx_2p5_shift;
wire          locked_from_the_shift_pll;

//Ethernet interface assignments
wire [3:0]	  debounced_user_pb;
// Debounce logic to clean out glitches within 1ms
debounce debounce_inst (
.clk          (clk_50_max10),
.reset_n      (resetn),  
.data_in      (user_pb),
.data_out     (debounced_user_pb)
);
defparam debounce_inst.WIDTH = 4;
defparam debounce_inst.POLARITY = "LOW";
defparam debounce_inst.TIMEOUT = 50000;               // at 50Mhz this is a debounce time of 1ms
defparam debounce_inst.TIMEOUT_WIDTH = 16;            // ceil(log2(TIMEOUT))

assign phy_resetn    =  debounced_user_pb[0] & !epcount[MSB-1];
assign system_resetn = resetn;

////////////////////////////////////////////////////////////////////////////
//debounce and set correct polarity of reset signal
// fpga_resetn
reg				u_resetn_r1, u_resetn_r2, resetn ;
reg [19:0] db_count;	// Debounce counter

always @ (posedge clk_50_max10)  begin
   // This should create a long pulse clock 
   if (db_count[19]) begin
      db_count <= #1 20'd0;
   end else begin
      db_count <= #1 db_count + 20'd1;
   end
	
   // Debounce and double register
   if (db_count[19]) begin  // use db_count[19] as an enable
      u_resetn_r1 <= #1 fpga_resetn;
      u_resetn_r2 <= #1 u_resetn_r1;

      if (u_resetn_r2 == u_resetn_r1) begin
         resetn <= #1 u_resetn_r2;
      end else begin
         resetn <= #1 1'b0; // Keep in reset
      end	  
   end
end




//PHY power-on reset control
parameter MSB = 20; // PHY interface: need minimum 10ms delay for POR
reg [MSB:0] epcount;


assign fpga_reset = !gemrtos_reset_bridge_out_reset;

always @(posedge clk_50_max10 or negedge fpga_reset)
  if (!fpga_reset)
      epcount <= MSB + 1'b0;
  else if (epcount[MSB] == 1'b0)
      epcount <= epcount + 1;
  else
      epcount <= epcount;

assign enet_resetn   = phy_resetn;

// MDIO output control
assign enet_mdio = ( !mdio_oen_from_the_tse_mac ) ? ( mdio_out_from_the_tse_mac ) : ( 1'bz );

assign enet_tx_2p5_shift = !enet_tx_2p5;

//RGMII clock solution
assign tx_clk_to_the_tse_mac = ( eth_mode_from_the_tse_mac ) ? ( enet_tx_125 ) :  // GbE Mode = 125MHz clock
                               ( ena_10_from_the_tse_mac ) ? ( enet_tx_2p5 ) :    // 10Mb Mode = 2.5MHz clock
                               ( enet_tx_25 );                                    // 100Mb Mode = 25MHz clock
							   
										 
										 
clkctrl  clkctrl_inst0 (
   .inclk        (tx_clk_to_the_tse_mac),
   .outclk       (tx_clk_to_the_tse_mac_g)
);



enet_gtx_clk_ddio   enet_gtx_clk_ddio_inst (
                    .outclock              (tx_clk_to_the_tse_mac_g), 
                    .din                   (2'b01),
                    .pad_out               (enet_gtx_clk)
                    );



assign GEMRTOS_INS[0] = gemrtos_reset_bridge_out_reset;
assign GEMRTOS_INS[1] = eth_mode_from_the_tse_mac;
assign GEMRTOS_INS[2] = fpga_reset;

q_sys q_sys_inst (
        .sys_clk_clk                                 (clk_50_max10              ), //                             sys_clk.clk
        .refclk_clock_bridge_in_clk_clk              (clk_ddr3_100_p            ), //          refclk_clock_bridge_in_clk.clk
        .reset_reset_n                               (system_resetn             ), //                               reset.reset_n
        .mem_resetn_in_reset_reset_n                 (fpga_reset                ), //                 mem_resetn_in_reset.reset_n
        .gemrtos_reset_bridge_out_reset              (gemrtos_reset_bridge_out_reset), //       gemrtos_reset_bridge_out.reset

        .clock_bridge_0_in_clk_clk                   (enet_tx_25                ), //               clock_bridge_0_in_clk.clk
        .enet_pll_c0_clk                             (enet_tx_125               ), //                         enet_pll_c0.clk
        .enet_pll_c1_clk                             (enet_tx_25                ), //                         enet_pll_c1.clk
        .enet_pll_c2_clk                             (enet_tx_2p5               ), //                         enet_pll_c2.clk
        .enet_pll_locked_conduit_export              (locked_from_the_enet_pll  ), //             enet_pll_locked_conduit.export


        .eth_tse_mac_mdio_connection_mdc             (enet_mdc                  ), //         eth_tse_mac_mdio_connection.mdc
        .eth_tse_mac_mdio_connection_mdio_in         (enet_mdio                 ), //                                    .mdio_in
        .eth_tse_mac_mdio_connection_mdio_out        (mdio_out_from_the_tse_mac ), //                                    .mdio_out
        .eth_tse_mac_mdio_connection_mdio_oen        (mdio_oen_from_the_tse_mac ), //                                    .mdio_oen
        .eth_tse_mac_rgmii_connection_rgmii_in       (enet_rx_d                 ), //        eth_tse_mac_rgmii_connection.rgmii_in
        .eth_tse_mac_rgmii_connection_rgmii_out      (enet_tx_d                 ), //                                    .rgmii_out
        .eth_tse_mac_rgmii_connection_rx_control     (enet_rx_dv                ), //                                    .rx_control
        .eth_tse_mac_rgmii_connection_tx_control     (enet_tx_en                ), //                                    .tx_control
        .eth_tse_mac_status_connection_eth_mode      (eth_mode_from_the_tse_mac ), //                                    .eth_mode
        .eth_tse_mac_status_connection_ena_10        (ena_10_from_the_tse_mac   ), //                                    .ena_10
        .eth_tse_pcs_mac_rx_clock_connection_clk     (enet_rx_clk               ), // eth_tse_pcs_mac_rx_clock_connection.clk
        .eth_tse_pcs_mac_tx_clock_connection_clk     (tx_clk_to_the_tse_mac_g   ), // eth_tse_pcs_mac_tx_clock_connection.clk
        .ext_flash_flash_dataout_conduit_dataout     (qspi_io          ), //             ext_flash_flash_dataout.conduit_dataout
        .ext_flash_flash_dclk_out_conduit_dclk_out   (qspi_clk         ), //            ext_flash_flash_dclk_out.conduit_dclk_out
        .ext_flash_flash_ncs_conduit_ncs             (qspi_csn         ), //                 ext_flash_flash_ncs.conduit_ncs
        .memory_mem_a                                (mem_a[12:0]      ), //                              memory.mem_a
        .memory_mem_ba                               (mem_ba           ), //                                    .mem_ba
        .memory_mem_ck                               (mem_ck           ), //                                    .mem_ck
        .memory_mem_ck_n                             (mem_ck_n         ), //                                    .mem_ck_n
        .memory_mem_cke                              (mem_cke          ), //                                    .mem_cke
        .memory_mem_cs_n                             (mem_cs_n         ), //                                    .mem_cs_n
        .memory_mem_dm                               (mem_dm           ), //                                    .mem_dm
        .memory_mem_ras_n                            (mem_ras_n        ), //                                    .mem_ras_n
        .memory_mem_cas_n                            (mem_cas_n        ), //                                    .mem_cas_n
        .memory_mem_we_n                             (mem_we_n         ), //                                    .mem_we_n
        .memory_mem_reset_n                          (mem_reset_n      ), //                                    .mem_reset_n
        .memory_mem_dq                               (mem_dq           ), //                                    .mem_dq
        .memory_mem_dqs                              (mem_dqs          ), //                                    .mem_dqs
        .memory_mem_dqs_n                            (mem_dqs_n        ), //                                    .mem_dqs_n
        .memory_mem_odt                              (mem_odt          ), //                                    .mem_odt
        .led_pio_external_connection_export          (                 ), //         led_pio_external_connection.export
        .mem_if_ddr3_emif_0_status_local_init_done   (local_init_done  ), //           mem_if_ddr3_emif_0_status.local_init_done
        .mem_if_ddr3_emif_0_status_local_cal_success (local_cal_success), //                                    .local_cal_success
        .mem_if_ddr3_emif_0_status_local_cal_fail    (local_cal_fail   ), //                                    .local_cal_fail
        .gemrtos_phy_GEMRTOS_OUTS                    (GEMRTOS_OUTS     ), //                         gemrtos_phy.GEMRTOS_OUTS
        .gemrtos_phy_GEMRTOS_INS                     (GEMRTOS_INS      )  //                                    .GEMRTOS_INS
      );

	
		
//DDR3 Address Bit #13 is not available for DDR3 SDRAM A (64Mx16)
assign mem_a[13] = 1'b0;

//Heart beat by 50MHz clock
always @(posedge clk_50_max10 or posedge fpga_reset)
  if (fpga_reset)
      heart_beat_cnt <= 26'h0; //0x3FFFFFF
  else
      heart_beat_cnt <= heart_beat_cnt + 1'b1;


assign user_led[0] = GEMRTOS_OUTS[4];
assign user_led[1] = GEMRTOS_OUTS[5];
assign user_led[2] = GEMRTOS_OUTS[6];
assign user_led[3] = GEMRTOS_OUTS[7];

assign user_led[4] = !eth_mode_from_the_tse_mac; 

endmodule
