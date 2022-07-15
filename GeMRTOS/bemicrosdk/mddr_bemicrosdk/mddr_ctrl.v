/****************************************************************************/
/*                                                                          */
/*  Project:    BeMicro SDK                                                 */
/*  Module:     mddr_ctrl (mobile DDR SDRAM controller)                     */
/*  Author:     Harald Fluegel                                              */
/*              Arrow Central Europe GmbH                                   */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*  This module is a controller for the mobile DDR memory device mounted    */
/*  on the Arrow BeMirco SDK evaluation board. The device is a Micron       */
/*  MT46H32M16LFBF-5 low power DDR SDRAM with the properties listed below.  */
/*  o Configuration 8M x 16 x 4 banks                                       */
/*  o Refresh count 8K                                                      */
/*  o Row addressing A[12:0]                                                */
/*  o Column addressing A[9:0]                                              */
/*                                                                          */
/*  Note that this is a low-performance controller that runs the DRAM on    */
/*  a divided clock.                                                        */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*  History                                                                 */
/*  2011-08-29: initial release                                             */
/*                                                                          */
/****************************************************************************/

//----------------------------------------------------------------------------
// literals
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
// include
//----------------------------------------------------------------------------

// none

//----------------------------------------------------------------------------
// warnings that have been judged invalid
//----------------------------------------------------------------------------

// altera message_off 10230 10208 

//----------------------------------------------------------------------------
// module definition
//----------------------------------------------------------------------------

module mddr_ctrl
    (
                                    // global
    input           clk,
    input           rst,
                                    // Avalon slave
    input   [23:0]  address,            // 2^24 = 16M dwords = 64M bytes
    input           read,
    output  [31:0]  readdata,
    input           write,
    input   [31:0]  writedata,
    input   [3:0]   byteenable,
    input           begintransfer,
    output          waitrequest,
                                    // device
    output     RAM_A0,
    output     RAM_A1,
    output     RAM_A2,
    output     RAM_A3,
    output     RAM_A4,
    output     RAM_A5,
    output     RAM_A6,
    output     RAM_A7,
    output     RAM_A8,
    output     RAM_A9,
    output     RAM_A10,
    output     RAM_A11,
    output     RAM_A12,
    output     RAM_A13,
    output     RAM_BA0,
    output     RAM_BA1,
    output     RAM_CK_N,
    output     RAM_CK_P,
    output     RAM_CKE,
    output     RAM_CS_N,
    output     RAM_WS_N,
    output     RAM_RAS_N,
    output     RAM_CAS_N,
    output     RAM_LDM,
    output     RAM_UDM,	
    inout      RAM_D0,
    inout      RAM_D1,
    inout      RAM_D2,
    inout      RAM_D3,
    inout      RAM_D4,
    inout      RAM_D5,
    inout      RAM_D6,
    inout      RAM_D7,
    inout      RAM_D8,
    inout      RAM_D9,
    inout      RAM_D10,
    inout      RAM_D11,
    inout      RAM_D12,
    inout      RAM_D13,
    inout      RAM_D14,
    inout      RAM_D15,
    inout      RAM_LDQS,
    inout      RAM_UDQS
	
    );

//----------------------------------------------------------------------------
// local variables
//----------------------------------------------------------------------------
wire    [12:0]  ram_addr;
wire    [1:0]   ram_baddr;
wire    [15:0]  ram_dq_i;
wire    [15:0]  ram_dq_o;
wire            ram_ldqs_i;
wire            ram_ldqs_o;
wire            ram_udqs_i;
wire            ram_udqs_o;
wire            ram_oe;

wire          mddr_ck;
wire          mddr_ck_n;
wire          mddr_cke;
wire          mddr_cs_n;
wire          mddr_ras_n;
wire          mddr_cas_n;
wire          mddr_we_n;
wire          mddr_ldm;
wire          mddr_udm;
wire  [1:0]   mddr_ba;
wire  [12:0]  mddr_a;
wire  [15:0]  mddr_dq_o;
wire          mddr_ldqs_o;
wire          mddr_udqs_o;
wire          mddr_oe;
wire   [15:0]  mddr_dq_i;
wire           mddr_ldqs_i;
wire           mddr_udqs_i;

reg                 p1, p2, p3, p4;     // 4 phases of mddr clock
reg                 p23, p34            /* synthesis preserve */ ;            
reg                 clk_p, clk_n        /* synthesis preserve */ ;

reg     [24:0]      current_address;    // word address (16 bit)
wire    [12:0]      row_address;
wire    [9:0]       column_address;
reg     [1:0]       bank;
reg     [12:0]      muxed_address;
wire    [15:0]      muxed_data;
wire                data_enable;
wire                muxed_ldm;
wire                muxed_udm;

reg     [15:0]      delay_counter;
wire                reset_delay_counter;
reg                 delay_counter_is_4;
reg                 delay_counter_is_16;
reg                 init_delay_elapsed;

reg                 cs_cycle;           // combinational signal generated in state machine
reg                 cs;                 // registered signal for output
reg                 ras_cycle;          // dito
reg                 ras;
reg                 cas_cycle;
reg                 cas;
reg                 we_cycle;
reg                 we;
reg                 lsmr_cycle;         // load standard mode register cycle
reg                 lemr_cycle;         // load extended more register cycle
reg                 auto_precharge;

reg                 clear_delay_counter;
reg                 wait_state;
reg                 begin_write_slot;
reg                 write_slot;
reg                 read_slot;

reg                 ldqs_1, ldqs_2, ldqs_3;
reg                 udqs_1, udqs_2, udqs_3;
wire                take_ldq_1, take_ldq_2;
wire                take_udq_1, take_udq_2;
reg     [15:0]      captured_data_1;
reg     [31:0]      captured_data_2;

//----------------------------------------------------------------------------
// mode register settings
//----------------------------------------------------------------------------

wire    [14:0]      standard_mode_register;
wire    [14:0]      extended_mode_register;

// standard mode register, normal operation , CAS latency = 2, sequential burst, burst length = 2
assign  standard_mode_register = 15'b00_000000_010_0_001;

// extended mode register, normal AR operation , drive strength = 1/2, no temp. compensation, full array self refresh
assign  extended_mode_register = 15'b10_00000_001_00_000;

parameter           pRefreshCount = 390;    // for 50 MHz
reg     [15:0]      refresh_counter;
wire                reset_refresh_counter;
reg                 refresh_pending;
wire                reset_refresh_pending;

//----------------------------------------------------------------------------
// core logic
//----------------------------------------------------------------------------

// clock phases
//        _   _   _   _   _   _   _   _   _   _   _
// clk  _/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_/ \_
//         ___             ___             ___
// p1   __/   \___________/   \___________/   \______
//             ___             ___             ___
// p2   ______/   \___________/   \___________/   \__
//                 ___             ___             __
// p3   __________/   \___________/   \___________/  
//      __             ___             ___
// p4     \___________/   \___________/   \__________
//             _______         _______         ______
// p23  ______/       \_______/       \_______/       
//      __         _______         _______         __
// p34    \_______/       \_______/       \_______/       

//----------------------------------------------------------------------------
// outputs
//----------------------------------------------------------------------------

assign ram_addr    = mddr_a;
assign ram_baddr   = mddr_ba;
assign RAM_CK_P    = mddr_ck;
assign RAM_CK_N    = mddr_ck_n;
assign RAM_CKE     = mddr_cke;
assign RAM_CS_N    = mddr_cs_n;
assign RAM_RAS_N   = mddr_ras_n;
assign RAM_CAS_N   = mddr_cas_n;
assign RAM_WS_N    = mddr_we_n;
assign RAM_LDM     = mddr_ldm;
assign RAM_UDM     = mddr_udm;
assign ram_dq_o    = mddr_dq_o;
assign ram_ldqs_o  = mddr_ldqs_o;
assign ram_udqs_o  = mddr_udqs_o;
assign ram_oe      = mddr_oe;
assign mddr_dq_i   = ram_dq_i;
assign mddr_udqs_i = ram_udqs_i;
assign mddr_ldqs_i = ram_ldqs_i;

assign RAM_A0   = ram_addr[ 0];
assign RAM_A1   = ram_addr[ 1];
assign RAM_A2   = ram_addr[ 2];
assign RAM_A3   = ram_addr[ 3];
assign RAM_A4   = ram_addr[ 4];
assign RAM_A5   = ram_addr[ 5];
assign RAM_A6   = ram_addr[ 6];
assign RAM_A7   = ram_addr[ 7];
assign RAM_A8   = ram_addr[ 8];
assign RAM_A9   = ram_addr[ 9];
assign RAM_A10  = ram_addr[10];
assign RAM_A11  = ram_addr[11];
assign RAM_A12  = ram_addr[12];
assign RAM_A13  = 1'b0;

assign RAM_BA0  = ram_baddr[0];
assign RAM_BA1  = ram_baddr[1];

assign RAM_D0   = ram_oe ? ram_dq_o[ 0] : 1'bz;
assign RAM_D1   = ram_oe ? ram_dq_o[ 1] : 1'bz;
assign RAM_D2   = ram_oe ? ram_dq_o[ 2] : 1'bz;
assign RAM_D3   = ram_oe ? ram_dq_o[ 3] : 1'bz;
assign RAM_D4   = ram_oe ? ram_dq_o[ 4] : 1'bz;
assign RAM_D5   = ram_oe ? ram_dq_o[ 5] : 1'bz;
assign RAM_D6   = ram_oe ? ram_dq_o[ 6] : 1'bz;
assign RAM_D7   = ram_oe ? ram_dq_o[ 7] : 1'bz;
assign RAM_D8   = ram_oe ? ram_dq_o[ 8] : 1'bz;
assign RAM_D9   = ram_oe ? ram_dq_o[ 9] : 1'bz;
assign RAM_D10  = ram_oe ? ram_dq_o[10] : 1'bz;
assign RAM_D11  = ram_oe ? ram_dq_o[11] : 1'bz;
assign RAM_D12  = ram_oe ? ram_dq_o[12] : 1'bz;
assign RAM_D13  = ram_oe ? ram_dq_o[13] : 1'bz;
assign RAM_D14  = ram_oe ? ram_dq_o[14] : 1'bz;
assign RAM_D15  = ram_oe ? ram_dq_o[15] : 1'bz;

assign ram_dq_i = {RAM_D15, RAM_D14, RAM_D13, RAM_D12, RAM_D11, RAM_D10, RAM_D9, RAM_D8,
                   RAM_D7,  RAM_D6,  RAM_D5,  RAM_D4,  RAM_D3,  RAM_D2,  RAM_D1, RAM_D0};

assign RAM_LDQS = ram_oe ? ram_ldqs_o : 1'bz;
assign RAM_UDQS = ram_oe ? ram_udqs_o : 1'bz;

assign ram_ldqs_i = RAM_LDQS;
assign ram_udqs_i = RAM_UDQS;


always @(posedge clk)
    begin
    if (rst)
        begin
        p1      <= 1'b0;
        p2      <= 1'b0;
        p3      <= 1'b0;
        p4      <= 1'b1;
        end
    else
        begin
        p1      <= p4;
        p2      <= p1;
        p3      <= p2;
        p4      <= p3;
        p23     <= p1 || p2;
        p34     <= p2 || p3;
        clk_p   <= p2 || p3;    // equivalent to p34 but located in output register
        clk_n   <= p4 || p1;    // equivalent to p12, located in output register
        end
    end

// delay counter

assign reset_delay_counter = rst || clear_delay_counter;

always @(posedge clk)
    begin
    if (reset_delay_counter)
        delay_counter <= 0;
    else if (!init_delay_elapsed)
        delay_counter <= delay_counter + 1;
    end

always @(posedge clk)
    begin
    delay_counter_is_4  <= delay_counter >= 3;
    delay_counter_is_16 <= delay_counter >= 15;
    init_delay_elapsed  <= delay_counter >= 21000;
    end

//----------------------------------------------------------------------------
// refresh counter
//----------------------------------------------------------------------------

always @(posedge clk)
    begin
    if (reset_refresh_counter)
        refresh_counter <= 0;
    else
        refresh_counter <= refresh_counter + 1;
    end

assign reset_refresh_counter = rst || (refresh_counter > pRefreshCount - 1);

always @(posedge clk)
    begin
    if (rst)
        refresh_pending <= 0;
    else if (reset_refresh_counter)
        refresh_pending <= 1;
    else if (reset_refresh_pending)
        refresh_pending <= 0;
    end

//----------------------------------------------------------------------------
// main state machine
//----------------------------------------------------------------------------

reg     [25:0]  main_state;
reg     [25:0]  main_next_state;

parameter       pMsReset        = 26'h0000001,
                pMsInit1        = 26'h0000002,
                pMsInit2        = 26'h0000004,
                pMsInit3        = 26'h0000008,
                pMsInit4        = 26'h0000010,
                pMsInit5        = 26'h0000020,
                pMsInit6        = 26'h0000040,
                pMsInit7        = 26'h0000080,
                pMsInit8        = 26'h0000100,
                pMsInit9        = 26'h0000200,
                pMsInit10       = 26'h0000400,
                pMsIdle         = 26'h0000800,
                pMsRead1        = 26'h0001000,
                pMsRead2        = 26'h0002000,
                pMsRead3        = 26'h0004000,
                pMsRead4        = 26'h0008000,
                pMsRead5        = 26'h0010000,
                pMsRead6        = 26'h0020000,
                pMsWrite1       = 26'h0040000,
                pMsWrite2       = 26'h0080000,
                pMsWrite3       = 26'h0100000,
                pMsWrite4       = 26'h0200000,
                pMsWrite5       = 26'h0400000,
                pMsWrite6       = 26'h0800000,
                pMsRefresh1     = 26'h1000000,
                pMsRefresh2     = 26'h2000000;

// state alias

assign reset_refresh_pending = main_state[24];

// state register

always @(posedge clk)
    begin
    if (rst)
        main_state <= pMsReset;
    else
        main_state <= main_next_state;
    end

// next state

always @*
    begin
    cs_cycle            = 0;
    ras_cycle           = 0;
    cas_cycle           = 0;
    we_cycle            = 0;
    lsmr_cycle          = 0;
    lemr_cycle          = 0;
    begin_write_slot    = 0;
    read_slot           = 0;
    auto_precharge      = 0;
    clear_delay_counter = 0;
    wait_state          = 1;
    main_next_state     = main_state;
    case (main_state)
        pMsReset:
            begin
            if (init_delay_elapsed && p4)
                begin
                cs_cycle        = 1;
                ras_cycle       = 1;
                we_cycle        = 1;
                main_next_state = pMsInit1;
                end
            end

//----------------------------------------------------------------------------
// MDDR device initialization
//----------------------------------------------------------------------------

        pMsInit1:
            begin
            cs_cycle            = 1;        // precharge all command
            ras_cycle           = 1;
            we_cycle            = 1;
            clear_delay_counter = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                ras_cycle       = 0;
                we_cycle        = 0;
                main_next_state = pMsInit2;
                end
            end
        pMsInit2:
            begin
            if (delay_counter_is_4 && p4)
                begin
                cs_cycle        = 1;
                ras_cycle       = 1;
                cas_cycle       = 1;
                main_next_state = pMsInit3;
                end
            end
        pMsInit3:
            begin
            cs_cycle            = 1;    // 1st auto refresh command
            ras_cycle           = 1;
            cas_cycle           = 1;
            clear_delay_counter = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                ras_cycle       = 0;
                cas_cycle       = 0;
                main_next_state = pMsInit4;
                end
            end
        pMsInit4:
            begin
            if (delay_counter_is_16 && p4)
                begin
                cs_cycle        = 1;
                ras_cycle       = 1;
                cas_cycle       = 1;
                main_next_state = pMsInit5;
                end
            end
        pMsInit5:
            begin
            cs_cycle            = 1;    // 2nd auto refresh command
            ras_cycle           = 1;
            cas_cycle           = 1;
            clear_delay_counter = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                ras_cycle       = 0;
                cas_cycle       = 0;
                main_next_state = pMsInit6;
                end
            end
        pMsInit6:
            begin
            if (delay_counter_is_16 && p4)
                begin
                cs_cycle        = 1;
                ras_cycle       = 1;
                cas_cycle       = 1;
                we_cycle        = 1;
                main_next_state = pMsInit7;
                end
            end
        pMsInit7:
            begin
            cs_cycle            = 1;    // load standard mode register
            ras_cycle           = 1;
            cas_cycle           = 1;
            we_cycle            = 1;
            lsmr_cycle          = 1;
            clear_delay_counter = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                ras_cycle       = 0;
                cas_cycle       = 0;
                we_cycle        = 0;
                main_next_state = pMsInit8;
                end
            end
        pMsInit8:
            begin
            if (delay_counter_is_16 && p4)
                begin
                cs_cycle        = 1;
                ras_cycle       = 1;
                cas_cycle       = 1;
                we_cycle        = 1;
                main_next_state = pMsInit9;
                end
            end
        pMsInit9:
            begin
            cs_cycle            = 1;    // load extended mode register
            ras_cycle           = 1;
            cas_cycle           = 1;
            we_cycle            = 1;
            lemr_cycle          = 1;
            clear_delay_counter = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                ras_cycle       = 0;
                cas_cycle       = 0;
                we_cycle        = 0;
                main_next_state = pMsInit10;
                end
            end
        pMsInit10:
            begin
            if (delay_counter_is_16 && p4)
                main_next_state   = pMsIdle;
            end

//----------------------------------------------------------------------------
// idle
//----------------------------------------------------------------------------

        pMsIdle:
            begin
            if (p4)
                begin
                if (refresh_pending)
                    begin
                    cs_cycle        = 1;
                    ras_cycle       = 1;
                    cas_cycle       = 1;
                    main_next_state = pMsRefresh1;
                    end
                else
                    begin
                    if (read)
                        begin
                        cs_cycle        = 1;
                        ras_cycle       = 1;
                        main_next_state = pMsRead1;
                        end
                    if (write)
                        begin
                        cs_cycle        = 1;
                        ras_cycle       = 1;
                        main_next_state = pMsWrite1;
                        end
                    end
                end
            end

//----------------------------------------------------------------------------
// single read cycle
//----------------------------------------------------------------------------

        pMsRead1:                          // active
            begin
            cs_cycle            = 1;
            ras_cycle           = 1;
            if (p4)
                begin
                ras_cycle       = 0;
                main_next_state = pMsRead2;
                end
            end
        pMsRead2:                           // nop
            begin
            cs_cycle            = 1;
            if (p4)
                begin
                cas_cycle       = 1;
                main_next_state = pMsRead3;
                end
            end
        pMsRead3:
            begin
            cs_cycle            = 1;
            cas_cycle           = 1;
            auto_precharge      = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                cas_cycle       = 0;
                main_next_state = pMsRead4;
                end
            end
        pMsRead4:
            begin
            if (p4)
                read_slot       = 1;
            if (p4)
                main_next_state = pMsRead5;
            end
        pMsRead5:
            begin
            if (!p4)
                read_slot       = 1;
            if (p4)
                main_next_state = pMsRead6;
            end
        pMsRead6:
            begin
            wait_state          = 0;
            main_next_state     = pMsIdle;
            end

//----------------------------------------------------------------------------
// single write cycle
//----------------------------------------------------------------------------

        pMsWrite1:                          // active
            begin
            cs_cycle            = 1;
            ras_cycle           = 1;
            if (p4)
                begin
                ras_cycle       = 0;
                main_next_state = pMsWrite2;
                end
            end
        pMsWrite2:                          // nop
            begin
            cs_cycle            = 1;
            if (p4)
                begin
                cas_cycle       = 1;
                we_cycle        = 1;
                main_next_state = pMsWrite3;
                end
            end
        pMsWrite3:                          // write
            begin
            cs_cycle            = 1;
            cas_cycle           = 1;
            we_cycle            = 1;
            auto_precharge      = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                cas_cycle       = 0;
                we_cycle        = 0;
                main_next_state = pMsWrite4;
                end
            end
        pMsWrite4:
            begin
            begin_write_slot    = 1;
            if (p4)
                main_next_state = pMsWrite5;
            end
        pMsWrite5:
            begin
            if (p4)
                main_next_state = pMsWrite6;
            end
        pMsWrite6:
            begin
            wait_state          = 0;
            main_next_state     = pMsIdle;
            end

//----------------------------------------------------------------------------
// auto refresh
//----------------------------------------------------------------------------

        pMsRefresh1:
            begin
            cs_cycle            = 1;
            ras_cycle           = 1;
            cas_cycle           = 1;
            if (p4)
                begin
                cs_cycle        = 0;
                ras_cycle       = 0;
                cas_cycle       = 0;
                main_next_state = pMsRefresh2;
                end
            end
        pMsRefresh2:
            begin
            if (p4)
                main_next_state = pMsIdle;
            end

//----------------------------------------------------------------------------

        default:
            begin
            main_next_state = main_state;
            end
        endcase
    end

//----------------------------------------------------------------------------
// address and data output
//----------------------------------------------------------------------------

always @(posedge clk)
    begin
    if (rst)
        current_address <= 'h100000;    // A[10] must be 1 for precharge command during device init
    else if (begintransfer)
        current_address <= {address, 1'b0};
    end

assign row_address      = current_address[22:10];
assign column_address   = current_address[9:0];

always @*
    begin
    casex({lsmr_cycle, lemr_cycle, cas_cycle, ras_cycle})   // synthesis full_case, parallel_case
        4'b10xx:
            begin
            muxed_address   = standard_mode_register[12:0];
            bank            = standard_mode_register[14:13];
            end
        4'b01xx:
            begin
            muxed_address   = extended_mode_register[12:0];
            bank            = extended_mode_register[14:13];
            end
        4'b0010:      // active
            begin
            muxed_address   = {2'h0, auto_precharge, column_address};
            bank            = current_address[24:23];
            end
        4'b0001:
            begin
            muxed_address   = row_address;
            bank            = current_address[24:23];
            end
        endcase
    end
   
assign muxed_data   = p23 ? writedata[15:0] : writedata[31:16];
assign muxed_ldm    = p23 ? !byteenable[0]  : !byteenable[2];
assign muxed_udm    = p23 ? !byteenable[1]  : !byteenable[3];
assign data_enable  = write_slot;

//----------------------------------------------------------------------------
// write slot
//----------------------------------------------------------------------------

always @(posedge clk)
    begin
    if (rst)
        write_slot <= 1'b0;
    else
        write_slot <= begin_write_slot
                    || write_slot && !p1;
    end

//----------------------------------------------------------------------------
// data capture
//----------------------------------------------------------------------------

always @(posedge clk)
    begin
    if (rst)
        begin
        ldqs_1  <= 1'b0;
        ldqs_3  <= 1'b0;
        udqs_1  <= 1'b0;
        udqs_3  <= 1'b0;
        end
    else
        begin
        ldqs_1  <= mddr_ldqs_i;
        ldqs_3  <= ldqs_2;
        udqs_1  <= mddr_udqs_i;
        udqs_3  <= udqs_2;
        end
    end

always @(negedge clk)
    begin
    if (rst)
        begin
        ldqs_2  <= 1'b0;
        udqs_2  <= 1'b0;
        end
    else
        begin
        ldqs_2  <= ldqs_1;
        udqs_2  <= udqs_1;
        end
    end

assign take_ldq_1 =  ldqs_2 && !ldqs_3 && read_slot;
assign take_udq_1 =  udqs_2 && !udqs_3 && read_slot;
assign take_ldq_2 = !ldqs_2 &&  ldqs_3 && read_slot;
assign take_udq_2 = !udqs_2 &&  udqs_3 && read_slot;

always @(posedge clk)
    begin
    captured_data_1 <= mddr_dq_i;
    end

always @(posedge clk)
    begin
    if (take_ldq_1)
        captured_data_2[7:0]   <= captured_data_1[7:0];
    if (take_udq_1)
        captured_data_2[15:8]  <= captured_data_1[15:8];
    if (take_ldq_2)
        captured_data_2[23:16] <= captured_data_1[7:0];
    if (take_udq_2)
        captured_data_2[31:24] <= captured_data_1[15:8];
    end

//----------------------------------------------------------------------------
// output
//----------------------------------------------------------------------------

always @(posedge clk)
    begin
    cs  <= cs_cycle;
    ras <= ras_cycle;
    cas <= cas_cycle;
    we  <= we_cycle;
    end

assign mddr_cs_n    = !cs;
assign mddr_ras_n   = !ras;
assign mddr_cas_n   = !cas;
assign mddr_we_n    = !we;

assign mddr_ck      =  clk_p;
assign mddr_ck_n    =  clk_n;
assign mddr_cke     =  1'b1;
assign mddr_a       =  muxed_address;
assign mddr_ba      =  bank;
assign mddr_ldm     =  muxed_ldm;
assign mddr_udm     =  muxed_udm;

assign mddr_dq_o    =  muxed_data;
assign mddr_ldqs_o  =  p34;
assign mddr_udqs_o  =  p34;
assign mddr_oe      =  data_enable;

//----------------------------------------------------------------------------

assign readdata     = captured_data_2;
assign waitrequest  = wait_state;

//----------------------------------------------------------------------------

endmodule
