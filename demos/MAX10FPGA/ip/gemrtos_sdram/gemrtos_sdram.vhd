--
--  \file
--  \author Ricardo Cayssials
--  \brief
--  \version 1.0
--  \date      2013-2026
--
--  SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
--  Copyright (c) 2013-2026 GeMRTOS Project Contributors
--
--  Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
--
--  This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
--  License 1.0.0. You may use, copy, modify, and distribute this file for
--  any noncommercial purpose. Commercial use requires a separate written
--  license from GeMRTOS. Full terms:
--  https://polyformproject.org/licenses/noncommercial/1.0.0
--


LIBRARY IEEE;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;
-- to get the log2 for number of bits requires to codify the number of processors
use ieee.math_real.all;

entity gemrtos_sdram is
    -- NO GENERIC to enable qxp compilation
	generic
	(
		AVALON_DATA_WIDTH       : integer := 32;    -- Avalon data width
        SDRAM_ROWS              : integer := 4096;  -- SDRAM rows
        SDRAM_COLS              : integer := 256;   -- SDRAM columns
        SDRAM_BANKS             : integer := 4;     -- SDRAM banks       
        SDRAM_ADDR_WIDTH        : integer := 12;    -- SDRAM address width
        SDRAM_DATA_WIDTH        : integer := 32;    -- SDRAM data width
        SDRAM_DATA_MASK_WIDTH   : integer := 4;     -- SDRAM data mask width
        SDRAM_INIT_TIME_US      : integer := 100;   -- SDRAM initialization time in us
        CLK_FREQUENCY_MHZ       : integer := 133;   -- Clock frequency in MHz units
        CAS_LATENCY             : integer := 3;     -- CAS latency cycles

        CYCLES_tAC              : integer := 1;     -- Access time from clock (ns)
        CYCLES_tHZ              : integer := 1;     -- Data-out High-Impedance time from CLK (ns)
        CYCLES_tOH              : integer := 1;     -- Data-out hold time (load) (ns)
        CYCLES_tMRD             : integer := 1;     -- MODE REGISTER to ACTIVE or REFRESH (clk)
        CYCLES_tRAS             : integer := 7;     -- Active to precharge comando (ns)
        CYCLES_tRC              : integer := 10;    -- Active bank a to ACTIVE bank a command (ns)
        CYCLES_tRCD             : integer := 3;     -- Active to READ or WRITE delay (ns)
        CYCLES_tRFC             : integer := 10;    -- Auto refresh period (ns)
        CYCLES_tRP              : integer := 3;     -- Precharge command period (ns)
        CYCLES_tRRD             : integer := 2;     -- ACTIVE bank a to ACTIVE bank b (ns)
        CYCLES_tWR              : integer := 2;     -- Write Recovery time (ns)        

        tAC                     : integer :=   5500; -- Access time from clock (ps)
        tHZ                     : integer :=   5500; -- Data-out High-Impedance time from CLK (ps)
        tOH                     : integer :=   2000; -- Data-out hold time (load) (ps)
        tMRD                    : integer :=      2; -- LOAD MODE REGISTER to ACTIVE or REFRESH (clk cycles)
        tRAS                    : integer :=  42000; -- Active to precharge comando (ps)
        tRC                     : integer :=  60000; -- Active bank a to ACTIVE bank a command (ps)
        tRCD                    : integer :=  18000; -- Active to READ or WRITE delay (ps)
        tRFC                    : integer :=  60000; -- Auto refresh period (ps)
        tRP                     : integer :=  18000; -- Precharge command period (ps)
        tRRD                    : integer :=  12000; -- ACTIVE bank a to ACTIVE bank b (ps)
        tWR                     : integer :=  15000;  -- Write Recovery time (ps)

        REFRESH_TIME_ms         : integer := 32;    -- How often refresh is needed
        REFRESH_COUNT           : integer := 8192   -- How many refreshes required per refresh time
	);
	port
	(
		-- Clock and Reset Inputs 
		clk	    : in std_logic;  -- Common clock for all processors from Avalon Bus
		reset   : in std_logic;  -- Common reset for all processors from Avalon Bus
        
		-- Avalon Slave port 
		avalon_address            : in std_logic_vector(integer(ceil(log2(real(SDRAM_BANKS)))) + integer(ceil(log2(real(SDRAM_ROWS)))) + integer(ceil(log2(real(SDRAM_COLS))))-1 downto 0);
		avalon_read               : in std_logic;
		avalon_write              : in std_logic;
		avalon_readdata           : out std_logic_vector(AVALON_DATA_WIDTH-1 downto 0);
        avalon_writedata          : in std_logic_vector(AVALON_DATA_WIDTH-1 downto 0);
        avalon_byteenable         : in std_logic_vector(3 downto 0);        
        avalon_waitrequest        : out std_logic;
        
        -- SDRAM ports
        sdram_bank_addr           : out std_logic_vector(integer(ceil(log2(real(SDRAM_BANKS))))-1 downto 0);
        sdram_data                : inout std_logic_vector(SDRAM_DATA_WIDTH-1 downto 0);
        sdram_clock_enable        : out std_logic;
        sdram_we_n                : out std_logic;         
        sdram_addr                : out std_logic_vector(SDRAM_ADDR_WIDTH-1 downto 0);
        sdram_cs_n                : out std_logic;
        sdram_ras_n               : out std_logic;
        sdram_cas_n               : out std_logic;
        sdram_data_mask           : out std_logic_vector(SDRAM_DATA_MASK_WIDTH-1 downto 0)
    );
end gemrtos_sdram;

architecture sdram_synth of gemrtos_sdram is

    function max_value (constant a, b: in integer) return integer is
    begin
      if a > b then
        return a;
      else
        return b;
      end if;
    end function max_value;

    constant MAX_STATE_COUNT : integer := max_value(CYCLES_tRP, 
                                          max_value(CYCLES_tRFC, 
                                          max_value(CYCLES_tMRD, 
                                          max_value(CYCLES_tRCD, 
                                          max_value(CYCLES_tWR, CAS_LATENCY)))));

    constant SDRAM_INIT_PER     : integer := SDRAM_INIT_TIME_US * CLK_FREQUENCY_MHZ; -- CLOCK cycles for memory initialization

    constant CYCLES_BETWEEN_REFRESH    : integer := (CLK_FREQUENCY_MHZ * 1000 * REFRESH_TIME_ms) / REFRESH_COUNT;

    constant SDRAM_BANK_WIDTH     : integer := integer(ceil(log2(real(SDRAM_BANKS))));   -- SDRAM bank address lines
    constant SDRAM_ROWS_WIDTH     : integer := integer(ceil(log2(real(SDRAM_ROWS))));
    constant SDRAM_COLS_WIDTH     : integer := integer(ceil(log2(real(SDRAM_COLS))));
    -- constant ADDRESS_WIDTH     : integer := SDRAM_BANK_WIDTH + SDRAM_ROWS_WIDTH + SDRAM_COLS_WIDTH;

    constant BURSTS_PER_CYCLE     : integer := AVALON_DATA_WIDTH / SDRAM_DATA_WIDTH;
    constant SDRAM_BITS           : integer := (SDRAM_ROWS * SDRAM_COLS * SDRAM_BANKS * SDRAM_DATA_WIDTH );
    constant AVALON_ADDRESS_WIDTH : integer :=  integer(ceil(log2(real(SDRAM_BITS / 32))));
    
    constant BURST_LENGTH         : integer := 1;
    constant BURST_TYPE           : integer := 1; -- 0 = sequential, 1 = interleave

    constant WRITE_BURST_MODE     : integer := 0;


    type states is (NOT_POWER_ON, IDLE, PRECHAGE_POWER_ON, REFRESH_POWER_ON1, REFRESH_POWER_ON2, MODE_POWER_ON, PRECHARGE, REFRESH_AUTO, WRITE_ACTIVATE, WRITING_STATE, READ_ACTIVATE, READING_STATE);
    attribute enum_encoding : string;
    attribute enum_encoding  of states : type is "one-hot";
    signal state, next_state           : states;

    type commands is (CMD_NOP, CMD_PALL, CMD_BACT, CMD_MRS, CMD_REF, CMD_WRIT, CMD_READ);
    signal command, next_command       : commands;
  
    signal state_cnt, next_state_cnt   : unsigned(integer(ceil(log2(real(MAX_STATE_COUNT))))-1 downto 0);
    signal init_timer, next_init_timer : unsigned(integer(ceil(log2(real(SDRAM_INIT_PER + 200))))-1 downto 0);
    
    signal tRC_count, next_tRC_count   : unsigned(integer(ceil(log2(real(CYCLES_tRC))))-1 downto 0);

	signal refresh_cnt, next_refresh_cnt : unsigned(integer(ceil(log2(real(CYCLES_BETWEEN_REFRESH + 3))))-1 downto 0);
    signal current_sdram_sigs, next_sdram_sigs   : std_logic_vector(5 downto 0); 

    signal busts_code : std_logic_vector(2 downto 0);
    signal busts_type_code : std_logic;
    signal cas_latency_code : std_logic_vector(2 downto 0);
    signal write_burst_mode_code : std_logic;
    signal op_code           : std_logic_vector(sdram_addr'left downto 0);
    signal active_code       : std_logic_vector(sdram_addr'left downto 0);
    signal read_write_code   : std_logic_vector(sdram_addr'left downto 0);

    signal next_sdram_cs_n      : std_logic;
    signal next_sdram_ras_n     : std_logic;
    signal next_sdram_cas_n     : std_logic;
    signal next_sdram_we_int_n, current_sdram_we_int_n      : std_logic;
    signal next_sdram_data_mask : std_logic_vector(SDRAM_DATA_MASK_WIDTH-1 downto 0);
    signal next_sdram_addr      : std_logic_vector(SDRAM_ADDR_WIDTH-1 downto 0);
    signal avalon_waitrequest_previous, avalon_waitrequest_int : std_logic;

    signal current_sdram_bank_addr, next_sdram_bank_addr : std_logic_vector(integer(ceil(log2(real(SDRAM_BANKS))))-1 downto 0);
    signal current_sdram_data, next_sdram_data           : std_logic_vector(SDRAM_DATA_WIDTH-1 downto 0);
    signal current_sdram_we_n                            : std_logic;         
    signal current_sdram_addr                            : std_logic_vector(SDRAM_ADDR_WIDTH-1 downto 0);
    signal current_sdram_cs_n                            : std_logic;
    signal current_sdram_ras_n                           : std_logic;
    signal current_sdram_cas_n                           : std_logic;
    signal current_sdram_data_mask                       : std_logic_vector(SDRAM_DATA_MASK_WIDTH-1 downto 0);

    signal bank    : std_logic_vector(SDRAM_BANK_WIDTH-1 downto 0);
    signal row     : std_logic_vector(SDRAM_ROWS_WIDTH-1 downto 0);
    signal col     : std_logic_vector(SDRAM_COLS_WIDTH-1 downto 0);
   
begin

    row     <= avalon_address(SDRAM_ROWS_WIDTH-1 downto 0);
    col     <= avalon_address(SDRAM_ROWS_WIDTH + SDRAM_COLS_WIDTH-1 downto SDRAM_ROWS_WIDTH);
    bank    <= avalon_address(SDRAM_COLS_WIDTH + SDRAM_ROWS_WIDTH + SDRAM_BANK_WIDTH-1 downto SDRAM_COLS_WIDTH + SDRAM_ROWS_WIDTH);    

-- AVALON MANAGEMENT
    avalon_waitrequest <= avalon_waitrequest_int;
    
    avalon_waitrequest_int <= '1' when avalon_waitrequest_previous = '0' else
                              '0' when avalon_read = '1' and state = READING_STATE and state_cnt =  to_unsigned(0,next_state_cnt'length) else
                              '0' when avalon_write = '1' and state = WRITING_STATE and next_state_cnt =  to_unsigned(0,next_state_cnt'length) else
                              '1';

    next_sdram_data <= avalon_writedata when next_sdram_we_int_n = '0' else (others => 'Z');

    -- 13-02-2026 current_sdram_data <= avalon_writedata when current_sdram_we_int_n = '0' else (others => 'Z');

    avalon_readdata    <= sdram_data;
 
    next_sdram_bank_addr <= bank;-- when state = WRITE_ACTIVATE else
                            -- bank when state = WRITING_STATE else
                            -- bank when state = READ_ACTIVATE else
                            -- bank when state = READING_STATE else
                            -- (others => '0');
                               
    current_sdram_bank_addr <= bank; -- when state = WRITE_ACTIVATE else
                               -- bank when state = WRITING_STATE else
                               -- bank when state = READ_ACTIVATE else
                               -- bank when state = READING_STATE else
                               -- (others => '0');

-- SDRAM MANAGEMENT
    sdram_bank_addr          <= next_sdram_bank_addr          ;
    sdram_data               <= next_sdram_data               ;
    sdram_we_n               <= next_sdram_we_int_n           ;
    sdram_addr               <= next_sdram_addr               ;
    sdram_cs_n               <= next_sdram_cs_n               ;
    sdram_ras_n              <= next_sdram_ras_n              ;
    sdram_cas_n              <= next_sdram_cas_n              ;
    sdram_data_mask          <= next_sdram_data_mask          ;

    sdram_clock_enable <= '1';
    
    busts_code <= "000" when BURST_LENGTH = 1 else
                  "001" when BURST_LENGTH = 2 else
                  "010" when BURST_LENGTH = 4 else
                  "011" when BURST_LENGTH = 8 else
                  "111";

    busts_type_code <= '0' when BURST_TYPE = 0 else '1';

    cas_latency_code <= "001" when CAS_LATENCY = 1 else
                        "010" when CAS_LATENCY = 2 else
                        "011" when CAS_LATENCY = 3 else
                        "111";

    write_burst_mode_code <= '0' when WRITE_BURST_MODE = 0 else '1';

op_code_process:
    process(write_burst_mode_code, cas_latency_code, busts_type_code, busts_code)
    begin
        op_code             <= (others => '0');
        op_code(9)          <= write_burst_mode_code;
        op_code(6 downto 4) <= cas_latency_code;
        op_code(3)          <= busts_type_code;
        op_code(2 downto 0) <= busts_code;
    end process;

active_process:
    process(row, bank)
    begin
        active_code                                          <= (others => '0');
        active_code(row'left downto 0)                       <= row;
 --       active_code(bank'left +row'left+1 downto row'left+1) <= bank;
    end process;

read_write_process:
    process(col, bank)
    begin
        read_write_code                                          <= (others => '0');
        read_write_code(col'left downto 0)                       <= col;
    end process;



    next_sdram_addr  <= (others => '-') when next_command = CMD_NOP else     -- No operation
                        (others => '1') when next_command = CMD_PALL else    -- Precharge All
                        (others => '-') when next_command = CMD_REF else     -- Refresh
                        op_code         when next_command = CMD_MRS else     -- Load mode register
                        active_code     when next_command = CMD_BACT else    -- Bank activate
                        read_write_code when next_command = CMD_WRIT else
                        read_write_code when next_command = CMD_READ else
                        (others => '-');  

    
    next_sdram_cs_n      <= next_sdram_sigs(4);
    next_sdram_ras_n     <= next_sdram_sigs(3);
    next_sdram_cas_n     <= next_sdram_sigs(2);
    next_sdram_we_int_n  <= next_sdram_sigs(1);
    
-- 11-02-2026 bytes_dqm: for i in next_sdram_data_mask'high downto  next_sdram_data_mask'low generate
-- 11-02-2026     next_sdram_data_mask(i) <= next_sdram_sigs(0);
-- 11-02-2026 end generate bytes_dqm;

    next_sdram_data_mask  <= not(avalon_byteenable);


                    --  A10  cs_n ras_n cas_n  we_n  dqm
    next_sdram_sigs  <= '-' & '0' & '1' & '1' & '1' & '0' when next_command = CMD_NOP else -- No operation - dqm in 0 to do not add CAS_Latency
                        '1' & '0' & '0' & '1' & '0' & '-' when next_command = CMD_PALL else -- Precharge All
                        '-' & '0' & '0' & '0' & '1' & '-' when next_command = CMD_REF else  -- Refresh
                        '-' & '0' & '0' & '0' & '0' & '-' when next_command = CMD_MRS else  -- Load mode register
                        '-' & '0' & '0' & '1' & '1' & '-' when next_command = CMD_BACT else -- Bank activate
                        '0' & '0' & '1' & '0' & '0' & '0' when next_command = CMD_WRIT else
                        '0' & '0' & '1' & '0' & '1' & '0' when next_command = CMD_READ else
                        '-' & '-' & '-' & '-' & '-' & '-';   

    next_command <= CMD_NOP   when next_state = NOT_POWER_ON else
                    CMD_PALL  when next_state = PRECHAGE_POWER_ON else
                    CMD_NOP   when next_state = REFRESH_POWER_ON1 and state = REFRESH_POWER_ON1 else
                    CMD_REF   when next_state = REFRESH_POWER_ON1 else
                    CMD_NOP   when next_state = REFRESH_POWER_ON2 and state = REFRESH_POWER_ON2 else
                    CMD_REF   when next_state = REFRESH_POWER_ON2 else
                    CMD_NOP   when next_state = REFRESH_AUTO and state = REFRESH_AUTO else
                    CMD_REF   when next_state = REFRESH_AUTO else                    
                    
                    CMD_NOP   when next_state = MODE_POWER_ON and state = MODE_POWER_ON else
                    CMD_MRS   when next_state = MODE_POWER_ON else
                    
                    CMD_PALL  when next_state = PRECHARGE else         
                    
                    CMD_BACT  when next_state = READ_ACTIVATE and state /= READ_ACTIVATE else
                    CMD_NOP   when next_state = READ_ACTIVATE else
                    
                    CMD_READ  when next_state = READING_STATE and state /= READING_STATE else
                    CMD_NOP   when next_state = READING_STATE else
                    
                    
                    CMD_BACT  when next_state = WRITE_ACTIVATE and state /= WRITE_ACTIVATE else
                    CMD_NOP   when next_state = WRITE_ACTIVATE else
                    
                    CMD_WRIT  when next_state = WRITING_STATE and state /= WRITING_STATE else
                    CMD_NOP   when next_state = WRITING_STATE else
                    
                    CMD_NOP   when next_state = IDLE else
                    CMD_PALL  when next_state = PRECHARGE else
                    
                    CMD_NOP; 


    next_init_timer <= init_timer + to_unsigned(1,init_timer'length) when init_timer <= to_unsigned(SDRAM_INIT_PER + 200, init_timer'length) else                   init_timer;

    next_refresh_cnt <= to_unsigned(0, next_refresh_cnt'length) when state = NOT_POWER_ON else
                        to_unsigned(0, next_refresh_cnt'length) when next_state = REFRESH_AUTO and state /= REFRESH_AUTO else
                        refresh_cnt + to_unsigned(1, next_refresh_cnt'length);

    next_state <= REFRESH_AUTO        when state = IDLE and refresh_cnt >= CYCLES_BETWEEN_REFRESH else
                  READ_ACTIVATE       when state = IDLE and avalon_read = '1' and tRC_count = to_unsigned(0, tRC_count'length) else
                  WRITE_ACTIVATE      when state = IDLE and avalon_write = '1' and tRC_count = to_unsigned(0, tRC_count'length) else

                  PRECHAGE_POWER_ON   when state = NOT_POWER_ON   and init_timer >= to_unsigned(SDRAM_INIT_PER + 10, init_timer'length) else
                  REFRESH_POWER_ON1   when state = PRECHAGE_POWER_ON and state_cnt = to_unsigned(0, state_cnt'length) else
                  REFRESH_POWER_ON2   when state = REFRESH_POWER_ON1 and state_cnt = to_unsigned(0, state_cnt'length) else
                  MODE_POWER_ON       when state = REFRESH_POWER_ON2 and state_cnt = to_unsigned(0, state_cnt'length) else                 
                  IDLE                when state = MODE_POWER_ON and state_cnt = to_unsigned(0, state_cnt'length) else
                  
                  PRECHARGE           when state = REFRESH_AUTO and state_cnt = to_unsigned(0, state_cnt'length) else                  
                  IDLE                when state = PRECHARGE and state_cnt = to_unsigned(0, state_cnt'length) else

                  READING_STATE       when state = READ_ACTIVATE and state_cnt = to_unsigned(0, state_cnt'length) else
                  PRECHARGE           when state = READING_STATE and state_cnt = to_unsigned(0, state_cnt'length) else

                  WRITING_STATE       when state = WRITE_ACTIVATE and state_cnt = to_unsigned(0, state_cnt'length) else
                  PRECHARGE           when state = WRITING_STATE and state_cnt = to_unsigned(0, state_cnt'length) else 

                  state;
                  

    next_state_cnt <= to_unsigned(CYCLES_tRP, next_state_cnt'length) when next_command = CMD_PALL and command /= CMD_PALL else
                      to_unsigned(CYCLES_tRFC, next_state_cnt'length) when next_command = CMD_REF and command /= CMD_REF else
                      to_unsigned(CYCLES_tMRD, next_state_cnt'length) when next_command = CMD_MRS and command /= CMD_MRS else
                      to_unsigned(CYCLES_tRCD, next_state_cnt'length) when next_command = CMD_BACT and command /= CMD_BACT else
                      to_unsigned(CYCLES_tWR, next_state_cnt'length) when next_command = CMD_WRIT and command /= CMD_WRIT else
                      to_unsigned(CAS_LATENCY, next_state_cnt'length) when next_command = CMD_READ and command /= CMD_READ else
                     
                      -- 13-02-2026 to_unsigned(1,next_state_cnt'length)  when next_state = WRITING_STATE   and state /= WRITING_STATE else
                      state_cnt - to_unsigned(1,next_state_cnt'length) when state_cnt > to_unsigned(0,next_state_cnt'length) else
                      to_unsigned(0,next_state_cnt'length);


    next_tRC_count <= to_unsigned(CYCLES_tRC, next_tRC_count'length) when next_command = CMD_BACT and command /= CMD_BACT else
                      tRC_count - 1 when tRC_count > to_unsigned(0, next_tRC_count'length) else
                      to_unsigned(0, next_tRC_count'length);


	process(reset, clk) is 
	begin 
		if (reset = '1') then
            state <= NOT_POWER_ON;
            command <= CMD_NOP;
            init_timer <= to_unsigned(0, init_timer'length);
            state_cnt <= to_unsigned(0, state_cnt'length);
            refresh_cnt <= to_unsigned(0, refresh_cnt'length);
            current_sdram_sigs <= (others => '0');
            avalon_waitrequest_previous <= '0';
		elsif (clk'event and clk = '1') then
            avalon_waitrequest_previous <= avalon_waitrequest_int;
            tRC_count <= next_tRC_count;
            
            state                       <= next_state;
            command                     <= next_command;
            init_timer                  <= next_init_timer;
            state_cnt                   <= next_state_cnt;
            refresh_cnt                 <= next_refresh_cnt;

            current_sdram_sigs          <= next_sdram_sigs;
            current_sdram_cs_n          <= next_sdram_cs_n;
            current_sdram_ras_n         <= next_sdram_ras_n;
            current_sdram_cas_n         <= next_sdram_cas_n;
            current_sdram_we_int_n      <= next_sdram_we_int_n;
            current_sdram_data_mask     <= next_sdram_data_mask;
            current_sdram_addr          <= next_sdram_addr;
        end if;
    end process;



end sdram_synth;
