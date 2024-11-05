-- $Id: //avalon_monitor.vhd#1 $
-- $Revision: #1 $
-- $Date: 2017/03/06 $
-- $Author: Ricardo Cayssials $
-- --------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

LIBRARY altera_mf;
USE altera_mf.all;



entity avalon_monitor is

	generic
		(
            NProcessors	             : integer;
			ENABLE_MONITORING        : integer := 0;
            DATA_WIDTH               : integer := 32;
            SYMBOL_WIDTH             : integer := 8;
            ADDRESS_WIDTH            : integer := 32;
            BURSTCOUNT_WIDTH         : integer := 1;
            BYTEEN_WIDTH             : integer := 4;
            AVALON_DATA_FIFO_DEPTH   : integer := 8; -- AVALON_DATA_FIFO_WIDTHU  : integer := 3;
            DEVICE_FAMILY            : string
		);
        
	port (
        clk         : in  std_logic;
		reset       : in  std_logic;

        -- Ouput indicating a processor is in waiting
		frozen_avalon_monitor  : out std_logic;
        
		-- Avalon Slave port to Access monitor Registers
	    slave_AvalonMonitor_chipselect    : in std_logic;		
		slave_AvalonMonitor_address       : in std_logic_vector(5 downto 0);
		slave_AvalonMonitor_read          : in std_logic;
		slave_AvalonMonitor_write         : in std_logic;
		slave_AvalonMonitor_readdata      : out std_logic_vector(31 downto 0);
        slave_AvalonMonitor_writedata     : in std_logic_vector(31 downto 0);        
        
        -- #####################################################################
        -- Ports for avalon sniffer
        -- #####################################################################        
        -- Processor 1
        s1_waitrequest    : out std_logic;                      
        s1_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s1_readdatavalid  : out std_logic;                      
        s1_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s1_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s1_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s1_write          : in  std_logic;                      
        s1_read           : in  std_logic;                      
        s1_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s1_debugaccess    : in  std_logic;                      
        m1_waitrequest    : in  std_logic;                      
        m1_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m1_readdatavalid  : in  std_logic;                      
        m1_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m1_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m1_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m1_write          : out std_logic;                      
        m1_read           : out std_logic;                      
        m1_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m1_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 2
        s2_waitrequest    : out std_logic;                      
        s2_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s2_readdatavalid  : out std_logic;                      
        s2_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s2_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s2_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s2_write          : in  std_logic;                      
        s2_read           : in  std_logic;                      
        s2_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s2_debugaccess    : in  std_logic;                      
        m2_waitrequest    : in  std_logic;                      
        m2_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m2_readdatavalid  : in  std_logic;                      
        m2_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m2_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m2_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m2_write          : out std_logic;                      
        m2_read           : out std_logic;                      
        m2_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m2_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 3
        s3_waitrequest    : out std_logic;                      
        s3_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s3_readdatavalid  : out std_logic;                      
        s3_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s3_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s3_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s3_write          : in  std_logic;                      
        s3_read           : in  std_logic;                      
        s3_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s3_debugaccess    : in  std_logic;                      
        m3_waitrequest    : in  std_logic;                      
        m3_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m3_readdatavalid  : in  std_logic;                      
        m3_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m3_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m3_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m3_write          : out std_logic;                      
        m3_read           : out std_logic;                      
        m3_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m3_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 4
        s4_waitrequest    : out std_logic;                      
        s4_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s4_readdatavalid  : out std_logic;                      
        s4_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s4_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s4_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s4_write          : in  std_logic;                      
        s4_read           : in  std_logic;                      
        s4_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s4_debugaccess    : in  std_logic;                      
        m4_waitrequest    : in  std_logic;                      
        m4_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m4_readdatavalid  : in  std_logic;                      
        m4_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m4_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m4_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m4_write          : out std_logic;                      
        m4_read           : out std_logic;                      
        m4_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m4_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 5
        s5_waitrequest    : out std_logic;                      
        s5_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s5_readdatavalid  : out std_logic;                      
        s5_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s5_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s5_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s5_write          : in  std_logic;                      
        s5_read           : in  std_logic;                      
        s5_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s5_debugaccess    : in  std_logic;                      
        m5_waitrequest    : in  std_logic;                      
        m5_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m5_readdatavalid  : in  std_logic;                      
        m5_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m5_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m5_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m5_write          : out std_logic;                      
        m5_read           : out std_logic;                      
        m5_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m5_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 6
        s6_waitrequest    : out std_logic;                      
        s6_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s6_readdatavalid  : out std_logic;                      
        s6_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s6_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s6_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s6_write          : in  std_logic;                      
        s6_read           : in  std_logic;                      
        s6_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s6_debugaccess    : in  std_logic;                      
        m6_waitrequest    : in  std_logic;                      
        m6_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m6_readdatavalid  : in  std_logic;                      
        m6_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m6_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m6_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m6_write          : out std_logic;                      
        m6_read           : out std_logic;                      
        m6_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m6_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 7
        s7_waitrequest    : out std_logic;                      
        s7_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s7_readdatavalid  : out std_logic;                      
        s7_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s7_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s7_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s7_write          : in  std_logic;                      
        s7_read           : in  std_logic;                      
        s7_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s7_debugaccess    : in  std_logic;                      
        m7_waitrequest    : in  std_logic;                      
        m7_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m7_readdatavalid  : in  std_logic;                      
        m7_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m7_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m7_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m7_write          : out std_logic;                      
        m7_read           : out std_logic;                      
        m7_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m7_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 8
        s8_waitrequest    : out std_logic;                      
        s8_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s8_readdatavalid  : out std_logic;                      
        s8_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s8_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s8_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s8_write          : in  std_logic;                      
        s8_read           : in  std_logic;                      
        s8_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s8_debugaccess    : in  std_logic;                      
        m8_waitrequest    : in  std_logic;                      
        m8_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m8_readdatavalid  : in  std_logic;                      
        m8_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m8_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m8_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m8_write          : out std_logic;                      
        m8_read           : out std_logic;                      
        m8_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m8_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 9
        s9_waitrequest    : out std_logic;                      
        s9_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s9_readdatavalid  : out std_logic;                      
        s9_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s9_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s9_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s9_write          : in  std_logic;                      
        s9_read           : in  std_logic;                      
        s9_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s9_debugaccess    : in  std_logic;                      
        m9_waitrequest    : in  std_logic;                      
        m9_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m9_readdatavalid  : in  std_logic;                      
        m9_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m9_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m9_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m9_write          : out std_logic;                      
        m9_read           : out std_logic;                      
        m9_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m9_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 10
        s10_waitrequest    : out std_logic;                      
        s10_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s10_readdatavalid  : out std_logic;                      
        s10_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s10_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s10_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s10_write          : in  std_logic;                      
        s10_read           : in  std_logic;                      
        s10_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s10_debugaccess    : in  std_logic;                      
        m10_waitrequest    : in  std_logic;                      
        m10_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m10_readdatavalid  : in  std_logic;                      
        m10_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m10_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m10_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m10_write          : out std_logic;                      
        m10_read           : out std_logic;                      
        m10_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m10_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 11
        s11_waitrequest    : out std_logic;                      
        s11_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s11_readdatavalid  : out std_logic;                      
        s11_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s11_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s11_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s11_write          : in  std_logic;                      
        s11_read           : in  std_logic;                      
        s11_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s11_debugaccess    : in  std_logic;                      
        m11_waitrequest    : in  std_logic;                      
        m11_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m11_readdatavalid  : in  std_logic;                      
        m11_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m11_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m11_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m11_write          : out std_logic;                      
        m11_read           : out std_logic;                      
        m11_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m11_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 12
        s12_waitrequest    : out std_logic;                      
        s12_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s12_readdatavalid  : out std_logic;                      
        s12_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s12_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s12_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s12_write          : in  std_logic;                      
        s12_read           : in  std_logic;                      
        s12_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s12_debugaccess    : in  std_logic;                      
        m12_waitrequest    : in  std_logic;                      
        m12_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m12_readdatavalid  : in  std_logic;                      
        m12_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m12_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m12_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m12_write          : out std_logic;                      
        m12_read           : out std_logic;                      
        m12_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m12_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 13
        s13_waitrequest    : out std_logic;                      
        s13_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s13_readdatavalid  : out std_logic;                      
        s13_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s13_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s13_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s13_write          : in  std_logic;                      
        s13_read           : in  std_logic;                      
        s13_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s13_debugaccess    : in  std_logic;                      
        m13_waitrequest    : in  std_logic;                      
        m13_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m13_readdatavalid  : in  std_logic;                      
        m13_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m13_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m13_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m13_write          : out std_logic;                      
        m13_read           : out std_logic;                      
        m13_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m13_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 13
        s14_waitrequest    : out std_logic;                      
        s14_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s14_readdatavalid  : out std_logic;                      
        s14_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s14_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s14_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s14_write          : in  std_logic;                      
        s14_read           : in  std_logic;                      
        s14_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s14_debugaccess    : in  std_logic;                      
        m14_waitrequest    : in  std_logic;                      
        m14_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m14_readdatavalid  : in  std_logic;                      
        m14_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m14_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m14_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m14_write          : out std_logic;                      
        m14_read           : out std_logic;                      
        m14_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m14_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 14
        s15_waitrequest    : out std_logic;                      
        s15_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s15_readdatavalid  : out std_logic;                      
        s15_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s15_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s15_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s15_write          : in  std_logic;                      
        s15_read           : in  std_logic;                      
        s15_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s15_debugaccess    : in  std_logic;                      
        m15_waitrequest    : in  std_logic;                      
        m15_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m15_readdatavalid  : in  std_logic;                      
        m15_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m15_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m15_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m15_write          : out std_logic;                      
        m15_read           : out std_logic;                      
        m15_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m15_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 16
        s16_waitrequest    : out std_logic;                      
        s16_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s16_readdatavalid  : out std_logic;                      
        s16_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s16_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s16_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s16_write          : in  std_logic;                      
        s16_read           : in  std_logic;                      
        s16_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s16_debugaccess    : in  std_logic;                      
        m16_waitrequest    : in  std_logic;                      
        m16_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m16_readdatavalid  : in  std_logic;                      
        m16_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m16_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m16_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m16_write          : out std_logic;                      
        m16_read           : out std_logic;                      
        m16_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m16_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 17
        s17_waitrequest    : out std_logic;                      
        s17_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s17_readdatavalid  : out std_logic;                      
        s17_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s17_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s17_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s17_write          : in  std_logic;                      
        s17_read           : in  std_logic;                      
        s17_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s17_debugaccess    : in  std_logic;                      
        m17_waitrequest    : in  std_logic;                      
        m17_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m17_readdatavalid  : in  std_logic;                      
        m17_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m17_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m17_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m17_write          : out std_logic;                      
        m17_read           : out std_logic;                      
        m17_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m17_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 18
        s18_waitrequest    : out std_logic;                      
        s18_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s18_readdatavalid  : out std_logic;                      
        s18_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s18_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s18_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s18_write          : in  std_logic;                      
        s18_read           : in  std_logic;                      
        s18_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s18_debugaccess    : in  std_logic;                      
        m18_waitrequest    : in  std_logic;                      
        m18_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m18_readdatavalid  : in  std_logic;                      
        m18_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m18_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m18_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m18_write          : out std_logic;                      
        m18_read           : out std_logic;                      
        m18_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m18_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 19
        s19_waitrequest    : out std_logic;                      
        s19_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s19_readdatavalid  : out std_logic;                      
        s19_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s19_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s19_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s19_write          : in  std_logic;                      
        s19_read           : in  std_logic;                      
        s19_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s19_debugaccess    : in  std_logic;                      
        m19_waitrequest    : in  std_logic;                      
        m19_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m19_readdatavalid  : in  std_logic;                      
        m19_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m19_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m19_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m19_write          : out std_logic;                      
        m19_read           : out std_logic;                      
        m19_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m19_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 20
        s20_waitrequest    : out std_logic;                      
        s20_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s20_readdatavalid  : out std_logic;                      
        s20_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s20_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s20_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s20_write          : in  std_logic;                      
        s20_read           : in  std_logic;                      
        s20_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s20_debugaccess    : in  std_logic;                      
        m20_waitrequest    : in  std_logic;                      
        m20_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m20_readdatavalid  : in  std_logic;                      
        m20_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m20_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m20_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m20_write          : out std_logic;                      
        m20_read           : out std_logic;                      
        m20_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m20_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 21
        s21_waitrequest    : out std_logic;                      
        s21_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s21_readdatavalid  : out std_logic;                      
        s21_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s21_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s21_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s21_write          : in  std_logic;                      
        s21_read           : in  std_logic;                      
        s21_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s21_debugaccess    : in  std_logic;                      
        m21_waitrequest    : in  std_logic;                      
        m21_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m21_readdatavalid  : in  std_logic;                      
        m21_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m21_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m21_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m21_write          : out std_logic;                      
        m21_read           : out std_logic;                      
        m21_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m21_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 22
        s22_waitrequest    : out std_logic;                      
        s22_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s22_readdatavalid  : out std_logic;                      
        s22_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s22_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s22_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s22_write          : in  std_logic;                      
        s22_read           : in  std_logic;                      
        s22_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s22_debugaccess    : in  std_logic;                      
        m22_waitrequest    : in  std_logic;                      
        m22_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m22_readdatavalid  : in  std_logic;                      
        m22_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m22_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m22_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m22_write          : out std_logic;                      
        m22_read           : out std_logic;                      
        m22_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m22_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 23
        s23_waitrequest    : out std_logic;                      
        s23_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s23_readdatavalid  : out std_logic;                      
        s23_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s23_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s23_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s23_write          : in  std_logic;                      
        s23_read           : in  std_logic;                      
        s23_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s23_debugaccess    : in  std_logic;                      
        m23_waitrequest    : in  std_logic;                      
        m23_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m23_readdatavalid  : in  std_logic;                      
        m23_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m23_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m23_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m23_write          : out std_logic;                      
        m23_read           : out std_logic;                      
        m23_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m23_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 24
        s24_waitrequest    : out std_logic;                      
        s24_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s24_readdatavalid  : out std_logic;                      
        s24_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s24_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s24_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s24_write          : in  std_logic;                      
        s24_read           : in  std_logic;                      
        s24_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s24_debugaccess    : in  std_logic;                      
        m24_waitrequest    : in  std_logic;                      
        m24_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m24_readdatavalid  : in  std_logic;                      
        m24_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m24_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m24_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m24_write          : out std_logic;                      
        m24_read           : out std_logic;                      
        m24_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m24_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 25
        s25_waitrequest    : out std_logic;                      
        s25_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s25_readdatavalid  : out std_logic;                      
        s25_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s25_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s25_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s25_write          : in  std_logic;                      
        s25_read           : in  std_logic;                      
        s25_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s25_debugaccess    : in  std_logic;                      
        m25_waitrequest    : in  std_logic;                      
        m25_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m25_readdatavalid  : in  std_logic;                      
        m25_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m25_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m25_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m25_write          : out std_logic;                      
        m25_read           : out std_logic;                      
        m25_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m25_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 26
        s26_waitrequest    : out std_logic;                      
        s26_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s26_readdatavalid  : out std_logic;                      
        s26_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s26_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s26_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s26_write          : in  std_logic;                      
        s26_read           : in  std_logic;                      
        s26_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s26_debugaccess    : in  std_logic;                      
        m26_waitrequest    : in  std_logic;                      
        m26_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m26_readdatavalid  : in  std_logic;                      
        m26_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m26_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m26_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m26_write          : out std_logic;                      
        m26_read           : out std_logic;                      
        m26_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m26_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 27
        s27_waitrequest    : out std_logic;                      
        s27_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s27_readdatavalid  : out std_logic;                      
        s27_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s27_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s27_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s27_write          : in  std_logic;                      
        s27_read           : in  std_logic;                      
        s27_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s27_debugaccess    : in  std_logic;                      
        m27_waitrequest    : in  std_logic;                      
        m27_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m27_readdatavalid  : in  std_logic;                      
        m27_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m27_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m27_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m27_write          : out std_logic;                      
        m27_read           : out std_logic;                      
        m27_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m27_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 28
        s28_waitrequest    : out std_logic;                      
        s28_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s28_readdatavalid  : out std_logic;                      
        s28_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s28_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s28_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s28_write          : in  std_logic;                      
        s28_read           : in  std_logic;                      
        s28_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s28_debugaccess    : in  std_logic;                      
        m28_waitrequest    : in  std_logic;                      
        m28_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m28_readdatavalid  : in  std_logic;                      
        m28_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m28_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m28_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m28_write          : out std_logic;                      
        m28_read           : out std_logic;                      
        m28_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m28_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 29
        s29_waitrequest    : out std_logic;                      
        s29_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s29_readdatavalid  : out std_logic;                      
        s29_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s29_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s29_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s29_write          : in  std_logic;                      
        s29_read           : in  std_logic;                      
        s29_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s29_debugaccess    : in  std_logic;                      
        m29_waitrequest    : in  std_logic;                      
        m29_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m29_readdatavalid  : in  std_logic;                      
        m29_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m29_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m29_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m29_write          : out std_logic;                      
        m29_read           : out std_logic;                      
        m29_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m29_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 30
        s30_waitrequest    : out std_logic;                      
        s30_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s30_readdatavalid  : out std_logic;                      
        s30_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s30_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s30_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s30_write          : in  std_logic;                      
        s30_read           : in  std_logic;                      
        s30_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s30_debugaccess    : in  std_logic;                      
        m30_waitrequest    : in  std_logic;                      
        m30_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m30_readdatavalid  : in  std_logic;                      
        m30_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m30_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m30_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m30_write          : out std_logic;                      
        m30_read           : out std_logic;                      
        m30_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m30_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 31
        s31_waitrequest    : out std_logic;                      
        s31_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s31_readdatavalid  : out std_logic;                      
        s31_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s31_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s31_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s31_write          : in  std_logic;                      
        s31_read           : in  std_logic;                      
        s31_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s31_debugaccess    : in  std_logic;                      
        m31_waitrequest    : in  std_logic;                      
        m31_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m31_readdatavalid  : in  std_logic;                      
        m31_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m31_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m31_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m31_write          : out std_logic;                      
        m31_read           : out std_logic;                      
        m31_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m31_debugaccess    : out std_logic;
        -- #####################################################################
        -- Processor 32
        s32_waitrequest    : out std_logic;                      
        s32_readdata       : out std_logic_vector(DATA_WIDTH-1 downto 0);      
        s32_readdatavalid  : out std_logic;                      
        s32_burstcount     : in  std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        s32_writedata      : in  std_logic_vector(DATA_WIDTH-1 downto 0);
        s32_address        : in  std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        s32_write          : in  std_logic;                      
        s32_read           : in  std_logic;                      
        s32_byteenable     : in  std_logic_vector(BYTEEN_WIDTH-1 downto 0);    
        s32_debugaccess    : in  std_logic;                      
        m32_waitrequest    : in  std_logic;                      
        m32_readdata       : in  std_logic_vector(DATA_WIDTH-1 downto 0);      
        m32_readdatavalid  : in  std_logic;                      
        m32_burstcount     : out std_logic_vector(BURSTCOUNT_WIDTH-1 downto 0);
        m32_writedata      : out std_logic_vector(DATA_WIDTH-1 downto 0);   
        m32_address        : out std_logic_vector(ADDRESS_WIDTH-1 downto 0);
        m32_write          : out std_logic;                      
        m32_read           : out std_logic;                      
        m32_byteenable     : out std_logic_vector(BYTEEN_WIDTH-1 downto 0);
        m32_debugaccess    : out std_logic        

    );
			
end entity avalon_monitor;

architecture AM1 of avalon_monitor is

component STD_FIFO is
	Generic (
		constant DATA_WIDTH  : positive := 8;
		constant FIFO_DEPTH	: positive := 256
	);
	Port ( 
		CLK		: in  STD_LOGIC;
		RST		: in  STD_LOGIC;
		WriteEn	: in  STD_LOGIC;
		DataIn	: in  STD_LOGIC_VECTOR (DATA_WIDTH - 1 downto 0);
		ReadEn	: in  STD_LOGIC;
		DataOut	: out STD_LOGIC_VECTOR (DATA_WIDTH - 1 downto 0);
		Empty	: out STD_LOGIC;
        Almost_Full	: out STD_LOGIC;
		Full	: out STD_LOGIC
	);
end component;


    constant ADDR_FIFO_0 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(0, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_1 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(1, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_2 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(2, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_3 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(3, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_4 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(4, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_5 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(5, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_6 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(6, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_7 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(7, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_8 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(8, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_9 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(9, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_10 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(10, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_11 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(11, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_12 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(12, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_13 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(13, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_14 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(14, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_15 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(15, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_16 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(16, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_17 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(17, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_18 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(18, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_19 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(19, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_20 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(20, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_21 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(21, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_22 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(22, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_23 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(23, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_24 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(24, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_25 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(25, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_26 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(26, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_27 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(27, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_28 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(28, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_29 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(29, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_30 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(30, slave_AvalonMonitor_address'length));
    constant ADDR_FIFO_31 : std_logic_vector(slave_AvalonMonitor_address'left downto 0) := std_logic_vector(to_unsigned(31, slave_AvalonMonitor_address'length));



    signal i1_address_prev , i1_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i2_address_prev , i2_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i3_address_prev , i3_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i4_address_prev , i4_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i5_address_prev , i5_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i6_address_prev , i6_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i7_address_prev , i7_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i8_address_prev , i8_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i9_address_prev , i9_data_out  : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i10_address_prev, i10_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);    
    signal i11_address_prev, i11_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i12_address_prev, i12_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i13_address_prev, i13_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i14_address_prev, i14_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i15_address_prev, i15_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i16_address_prev, i16_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i17_address_prev, i17_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i18_address_prev, i18_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i19_address_prev, i19_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i20_address_prev, i20_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);    
    signal i21_address_prev, i21_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i22_address_prev, i22_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i23_address_prev, i23_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i24_address_prev, i24_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i25_address_prev, i25_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i26_address_prev, i26_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i27_address_prev, i27_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i28_address_prev, i28_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i29_address_prev, i29_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i30_address_prev, i30_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);    
    signal i31_address_prev, i31_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    signal i32_address_prev, i32_data_out : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);    
    
    signal slave_AvalonMonitor_readdata_int : STD_LOGIC_VECTOR (ADDRESS_WIDTH - 1 downto 0);
    
    signal i_frozzen : STD_LOGIC_VECTOR(32 downto 1);
    
    signal i1_empty, i1_full, i1_fifo_store, i1_read_fifo,      i1_almost_full    : STD_LOGIC;
    signal i2_empty, i2_full, i2_fifo_store, i2_read_fifo,      i2_almost_full    : STD_LOGIC;
    signal i3_empty, i3_full, i3_fifo_store, i3_read_fifo,      i3_almost_full    : STD_LOGIC;
    signal i4_empty, i4_full, i4_fifo_store, i4_read_fifo,      i4_almost_full    : STD_LOGIC;
    signal i5_empty, i5_full, i5_fifo_store, i5_read_fifo,      i5_almost_full    : STD_LOGIC;
    signal i6_empty, i6_full, i6_fifo_store, i6_read_fifo,      i6_almost_full    : STD_LOGIC;
    signal i7_empty, i7_full, i7_fifo_store, i7_read_fifo,      i7_almost_full    : STD_LOGIC;
    signal i8_empty, i8_full, i8_fifo_store, i8_read_fifo,      i8_almost_full    : STD_LOGIC;
    signal i9_empty, i9_full, i9_fifo_store, i9_read_fifo,      i9_almost_full    : STD_LOGIC;
    signal i10_empty, i10_full, i10_fifo_store, i10_read_fifo, i10_almost_full        : STD_LOGIC; 
    signal i11_empty, i11_full, i11_fifo_store, i11_read_fifo, i11_almost_full        : STD_LOGIC;
    signal i12_empty, i12_full, i12_fifo_store, i12_read_fifo, i12_almost_full        : STD_LOGIC;
    signal i13_empty, i13_full, i13_fifo_store, i13_read_fifo, i13_almost_full        : STD_LOGIC;
    signal i14_empty, i14_full, i14_fifo_store, i14_read_fifo, i14_almost_full        : STD_LOGIC;
    signal i15_empty, i15_full, i15_fifo_store, i15_read_fifo, i15_almost_full        : STD_LOGIC;
    signal i16_empty, i16_full, i16_fifo_store, i16_read_fifo, i16_almost_full        : STD_LOGIC;
    signal i17_empty, i17_full, i17_fifo_store, i17_read_fifo, i17_almost_full        : STD_LOGIC;
    signal i18_empty, i18_full, i18_fifo_store, i18_read_fifo, i18_almost_full        : STD_LOGIC;
    signal i19_empty, i19_full, i19_fifo_store, i19_read_fifo, i19_almost_full        : STD_LOGIC;
    signal i20_empty, i20_full, i20_fifo_store, i20_read_fifo, i20_almost_full        : STD_LOGIC; 
    signal i21_empty, i21_full, i21_fifo_store, i21_read_fifo, i21_almost_full        : STD_LOGIC;
    signal i22_empty, i22_full, i22_fifo_store, i22_read_fifo, i22_almost_full        : STD_LOGIC;
    signal i23_empty, i23_full, i23_fifo_store, i23_read_fifo, i23_almost_full        : STD_LOGIC;
    signal i24_empty, i24_full, i24_fifo_store, i24_read_fifo, i24_almost_full        : STD_LOGIC;
    signal i25_empty, i25_full, i25_fifo_store, i25_read_fifo, i25_almost_full        : STD_LOGIC;
    signal i26_empty, i26_full, i26_fifo_store, i26_read_fifo, i26_almost_full        : STD_LOGIC;
    signal i27_empty, i27_full, i27_fifo_store, i27_read_fifo, i27_almost_full        : STD_LOGIC;
    signal i28_empty, i28_full, i28_fifo_store, i28_read_fifo, i28_almost_full        : STD_LOGIC;
    signal i29_empty, i29_full, i29_fifo_store, i29_read_fifo, i29_almost_full        : STD_LOGIC;
    signal i30_empty, i30_full, i30_fifo_store, i30_read_fifo, i30_almost_full        : STD_LOGIC; 
    signal i31_empty, i31_full, i31_fifo_store, i31_read_fifo, i31_almost_full        : STD_LOGIC;
    signal i32_empty, i32_full, i32_fifo_store, i32_read_fifo, i32_almost_full        : STD_LOGIC;    

    
    -- signal counter                    : signed(15 downto 0);
    signal prev_AvalonMonitor_read    : std_logic;
    signal prev_AvalonMonitor_address : std_logic_vector(slave_AvalonMonitor_address'left downto 0);

    -- Control registers
    -- Bit 0: if 1 then frozen when fifo is full, 0 works normally
    -- Bit 1: if 1 then frozen
    -- Bit 2: if 1 then read fifo until it is empty
    signal R_CTRL       : STD_LOGIC_VECTOR(31 downto 0);
    

begin
    process(reset, clk) is
        variable frozen_out_i : STD_LOGIC;
	begin 
        frozen_out_i := '0';
        for i in 1 to NProcessors loop
            frozen_out_i := frozen_out_i or i_frozzen(i);
        end loop;
        frozen_avalon_monitor <= frozen_out_i;
		if ENABLE_MONITORING = 0 then
			frozen_avalon_monitor <= '0';
		end if;
    end process;
    -- #######################################################################################################################


ENABLE_MONITORING_if: if ENABLE_MONITORING = 1 generate

    u1: STD_FIFO 
      generic map ( 
        DATA_WIDTH => ADDRESS_WIDTH, 
        FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) 
      port map ( 
        CLK => clk, 
        RST => reset, 
        WriteEn => i1_fifo_store, 
        DataIn => s1_address, 
        ReadEn => i1_read_fifo, 
        DataOut => i1_data_out, 
        Empty => i1_empty,        
        Almost_Full => i1_almost_full , 
        Full => i1_full );
    
    i1_fifo_store <= '0' when i1_full = '1'  or R_CTRL(2) = '1' else 
                     '1' when  i1_address_prev /=  s1_address and  s1_read = '1' else 
                     '0';
    i1_read_fifo <=  '1' when i1_empty = '0' and R_CTRL(2) = '1' else 
                     '1' when  i1_almost_full = '1' and R_CTRL(0) = '0' else 
                     '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_0 else 
                     '0';

    i_frozzen(1)    <= '1' when ((i1_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i1_address_prev /=  s1_address and  s1_read = '1' else 
                     '0';
    
    -- #######################################################################################################################
    
    u2: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i2_fifo_store, DataIn => s2_address, ReadEn => i2_read_fifo, DataOut => i2_data_out, Empty => i2_empty,        Almost_Full => i2_almost_full , Full => i2_full );
    u3: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i3_fifo_store, DataIn => s3_address, ReadEn => i3_read_fifo, DataOut => i3_data_out, Empty => i3_empty,        Almost_Full => i3_almost_full , Full => i3_full );
    u4: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i4_fifo_store, DataIn => s4_address, ReadEn => i4_read_fifo, DataOut => i4_data_out, Empty => i4_empty,        Almost_Full => i4_almost_full , Full => i4_full );
    u5: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i5_fifo_store, DataIn => s5_address, ReadEn => i5_read_fifo, DataOut => i5_data_out, Empty => i5_empty,        Almost_Full => i5_almost_full , Full => i5_full );
    u6: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i6_fifo_store, DataIn => s6_address, ReadEn => i6_read_fifo, DataOut => i6_data_out, Empty => i6_empty,        Almost_Full => i6_almost_full , Full => i6_full );
    u7: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i7_fifo_store, DataIn => s7_address, ReadEn => i7_read_fifo, DataOut => i7_data_out, Empty => i7_empty,        Almost_Full => i7_almost_full , Full => i7_full );
    u8: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i8_fifo_store, DataIn => s8_address, ReadEn => i8_read_fifo, DataOut => i8_data_out, Empty => i8_empty,        Almost_Full => i8_almost_full , Full => i8_full );
    u9: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i9_fifo_store, DataIn => s9_address, ReadEn => i9_read_fifo, DataOut => i9_data_out, Empty => i9_empty,        Almost_Full => i9_almost_full , Full => i9_full );
    u10: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i10_fifo_store, DataIn => s10_address, ReadEn => i10_read_fifo, DataOut => i10_data_out, Empty => i10_empty,  Almost_Full => i10_almost_full, Full => i10_full );    
    u11: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i11_fifo_store, DataIn => s11_address, ReadEn => i11_read_fifo, DataOut => i11_data_out, Empty => i11_empty,  Almost_Full => i11_almost_full, Full => i11_full );
    u12: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i12_fifo_store, DataIn => s12_address, ReadEn => i12_read_fifo, DataOut => i12_data_out, Empty => i12_empty,  Almost_Full => i12_almost_full, Full => i12_full );
    u13: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i13_fifo_store, DataIn => s13_address, ReadEn => i13_read_fifo, DataOut => i13_data_out, Empty => i13_empty,  Almost_Full => i13_almost_full, Full => i13_full );
    u14: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i14_fifo_store, DataIn => s14_address, ReadEn => i14_read_fifo, DataOut => i14_data_out, Empty => i14_empty,  Almost_Full => i14_almost_full, Full => i14_full );
    u15: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i15_fifo_store, DataIn => s15_address, ReadEn => i15_read_fifo, DataOut => i15_data_out, Empty => i15_empty,  Almost_Full => i15_almost_full, Full => i15_full );
    u16: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i16_fifo_store, DataIn => s16_address, ReadEn => i16_read_fifo, DataOut => i16_data_out, Empty => i16_empty,  Almost_Full => i16_almost_full, Full => i16_full );
    u17: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i17_fifo_store, DataIn => s17_address, ReadEn => i17_read_fifo, DataOut => i17_data_out, Empty => i17_empty,  Almost_Full => i17_almost_full, Full => i17_full );
    u18: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i18_fifo_store, DataIn => s18_address, ReadEn => i18_read_fifo, DataOut => i18_data_out, Empty => i18_empty,  Almost_Full => i18_almost_full, Full => i18_full );
    u19: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i19_fifo_store, DataIn => s19_address, ReadEn => i19_read_fifo, DataOut => i19_data_out, Empty => i19_empty,  Almost_Full => i19_almost_full, Full => i19_full );
    u20: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i20_fifo_store, DataIn => s20_address, ReadEn => i20_read_fifo, DataOut => i20_data_out, Empty => i20_empty,  Almost_Full => i20_almost_full, Full => i20_full );    
    u21: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i21_fifo_store, DataIn => s21_address, ReadEn => i21_read_fifo, DataOut => i21_data_out, Empty => i21_empty,  Almost_Full => i21_almost_full, Full => i21_full );
    u22: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i22_fifo_store, DataIn => s22_address, ReadEn => i22_read_fifo, DataOut => i22_data_out, Empty => i22_empty,  Almost_Full => i22_almost_full, Full => i22_full );
    u23: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i23_fifo_store, DataIn => s23_address, ReadEn => i23_read_fifo, DataOut => i23_data_out, Empty => i23_empty,  Almost_Full => i23_almost_full, Full => i23_full );
    u24: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i24_fifo_store, DataIn => s24_address, ReadEn => i24_read_fifo, DataOut => i24_data_out, Empty => i24_empty,  Almost_Full => i24_almost_full, Full => i24_full );
    u25: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i25_fifo_store, DataIn => s25_address, ReadEn => i25_read_fifo, DataOut => i25_data_out, Empty => i25_empty,  Almost_Full => i25_almost_full, Full => i25_full );
    u26: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i26_fifo_store, DataIn => s26_address, ReadEn => i26_read_fifo, DataOut => i26_data_out, Empty => i26_empty,  Almost_Full => i26_almost_full, Full => i26_full );
    u27: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i27_fifo_store, DataIn => s27_address, ReadEn => i27_read_fifo, DataOut => i27_data_out, Empty => i27_empty,  Almost_Full => i27_almost_full, Full => i27_full );
    u28: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i28_fifo_store, DataIn => s28_address, ReadEn => i28_read_fifo, DataOut => i28_data_out, Empty => i28_empty,  Almost_Full => i28_almost_full, Full => i28_full );
    u29: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i29_fifo_store, DataIn => s29_address, ReadEn => i29_read_fifo, DataOut => i29_data_out, Empty => i29_empty,  Almost_Full => i29_almost_full, Full => i29_full );
    u30: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i30_fifo_store, DataIn => s30_address, ReadEn => i30_read_fifo, DataOut => i30_data_out, Empty => i30_empty,  Almost_Full => i30_almost_full, Full => i30_full );    
    u31: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i31_fifo_store, DataIn => s31_address, ReadEn => i31_read_fifo, DataOut => i31_data_out, Empty => i31_empty,  Almost_Full => i31_almost_full, Full => i31_full );    
    u32: STD_FIFO generic map ( DATA_WIDTH => ADDRESS_WIDTH, FIFO_DEPTH => AVALON_DATA_FIFO_DEPTH ) port map ( CLK => clk, RST => reset, WriteEn => i32_fifo_store, DataIn => s32_address, ReadEn => i32_read_fifo, DataOut => i32_data_out, Empty => i32_empty,  Almost_Full => i32_almost_full, Full => i32_full );        


    i2_fifo_store <= '0'  when i2_full = '1'  or R_CTRL(2) = '1' else '1' when  i2_address_prev /=  s2_address and  s2_read = '1' else '0';
    i3_fifo_store <= '0'  when i3_full = '1'  or R_CTRL(2) = '1' else '1' when  i3_address_prev /=  s3_address and  s3_read = '1' else '0';
    i4_fifo_store <= '0'  when i4_full = '1'  or R_CTRL(2) = '1' else '1' when  i4_address_prev /=  s4_address and  s4_read = '1' else '0';
    i5_fifo_store <= '0'  when i5_full = '1'  or R_CTRL(2) = '1' else '1' when  i5_address_prev /=  s5_address and  s5_read = '1' else '0';
    i6_fifo_store <= '0'  when i6_full = '1'  or R_CTRL(2) = '1' else '1' when  i6_address_prev /=  s6_address and  s6_read = '1' else '0';
    i7_fifo_store <= '0'  when i7_full = '1'  or R_CTRL(2) = '1' else '1' when  i7_address_prev /=  s7_address and  s7_read = '1' else '0';
    i8_fifo_store <= '0'  when i8_full = '1'  or R_CTRL(2) = '1' else '1' when  i8_address_prev /=  s8_address and  s8_read = '1' else '0';
    i9_fifo_store <= '0'  when i9_full = '1'  or R_CTRL(2) = '1' else '1' when  i9_address_prev /=  s9_address and  s9_read = '1' else '0';
    i10_fifo_store <= '0' when i10_full = '1' or R_CTRL(2) = '1' else '1' when i10_address_prev /= s10_address and s10_read = '1' else '0'; 
    i11_fifo_store <= '0' when i11_full = '1' or R_CTRL(2) = '1' else '1' when i11_address_prev /= s11_address and s11_read = '1' else '0';
    i12_fifo_store <= '0' when i12_full = '1' or R_CTRL(2) = '1' else '1' when i12_address_prev /= s12_address and s12_read = '1' else '0';
    i13_fifo_store <= '0' when i13_full = '1' or R_CTRL(2) = '1' else '1' when i13_address_prev /= s13_address and s13_read = '1' else '0';
    i14_fifo_store <= '0' when i14_full = '1' or R_CTRL(2) = '1' else '1' when i14_address_prev /= s14_address and s14_read = '1' else '0';
    i15_fifo_store <= '0' when i15_full = '1' or R_CTRL(2) = '1' else '1' when i15_address_prev /= s15_address and s15_read = '1' else '0';
    i16_fifo_store <= '0' when i16_full = '1' or R_CTRL(2) = '1' else '1' when i16_address_prev /= s16_address and s16_read = '1' else '0';
    i17_fifo_store <= '0' when i17_full = '1' or R_CTRL(2) = '1' else '1' when i17_address_prev /= s17_address and s17_read = '1' else '0';
    i18_fifo_store <= '0' when i18_full = '1' or R_CTRL(2) = '1' else '1' when i18_address_prev /= s18_address and s18_read = '1' else '0';
    i19_fifo_store <= '0' when i19_full = '1' or R_CTRL(2) = '1' else '1' when i19_address_prev /= s19_address and s19_read = '1' else '0';  
    i20_fifo_store <= '0' when i20_full = '1' or R_CTRL(2) = '1' else '1' when i20_address_prev /= s20_address and s20_read = '1' else '0'; 
    i21_fifo_store <= '0' when i21_full = '1' or R_CTRL(2) = '1' else '1' when i21_address_prev /= s21_address and s21_read = '1' else '0';
    i22_fifo_store <= '0' when i22_full = '1' or R_CTRL(2) = '1' else '1' when i22_address_prev /= s22_address and s22_read = '1' else '0';
    i23_fifo_store <= '0' when i23_full = '1' or R_CTRL(2) = '1' else '1' when i23_address_prev /= s23_address and s23_read = '1' else '0';
    i24_fifo_store <= '0' when i24_full = '1' or R_CTRL(2) = '1' else '1' when i24_address_prev /= s24_address and s24_read = '1' else '0';
    i25_fifo_store <= '0' when i25_full = '1' or R_CTRL(2) = '1' else '1' when i25_address_prev /= s25_address and s25_read = '1' else '0';
    i26_fifo_store <= '0' when i26_full = '1' or R_CTRL(2) = '1' else '1' when i26_address_prev /= s26_address and s26_read = '1' else '0';
    i27_fifo_store <= '0' when i27_full = '1' or R_CTRL(2) = '1' else '1' when i27_address_prev /= s27_address and s27_read = '1' else '0';
    i28_fifo_store <= '0' when i28_full = '1' or R_CTRL(2) = '1' else '1' when i28_address_prev /= s28_address and s28_read = '1' else '0';
    i29_fifo_store <= '0' when i29_full = '1' or R_CTRL(2) = '1' else '1' when i29_address_prev /= s29_address and s29_read = '1' else '0';
    i30_fifo_store <= '0' when i30_full = '1' or R_CTRL(2) = '1' else '1' when i30_address_prev /= s30_address and s30_read = '1' else '0'; 
    i31_fifo_store <= '0' when i31_full = '1' or R_CTRL(2) = '1' else '1' when i31_address_prev /= s31_address and s31_read = '1' else '0';
    i32_fifo_store <= '0' when i32_full = '1' or R_CTRL(2) = '1' else '1' when i32_address_prev /= s32_address and s32_read = '1' else '0';        
                    
    Edge_logic:
        process(reset, clk) is 
        begin 
            if (reset = '1') then    
                i1_address_prev <= (others => '0'); 
                i2_address_prev <= (others => '0'); 
                i3_address_prev <= (others => '0'); 
                i4_address_prev <= (others => '0'); 
                i5_address_prev <= (others => '0'); 
                i6_address_prev <= (others => '0'); 
                i7_address_prev <= (others => '0'); 
                i8_address_prev <= (others => '0'); 
                i9_address_prev <= (others => '0'); 
                i10_address_prev <= (others => '0');                 
                i11_address_prev <= (others => '0'); 
                i12_address_prev <= (others => '0'); 
                i13_address_prev <= (others => '0'); 
                i14_address_prev <= (others => '0'); 
                i15_address_prev <= (others => '0'); 
                i16_address_prev <= (others => '0'); 
                i17_address_prev <= (others => '0'); 
                i18_address_prev <= (others => '0'); 
                i19_address_prev <= (others => '0');
                i20_address_prev <= (others => '0');                                 
                i21_address_prev <= (others => '0'); 
                i22_address_prev <= (others => '0'); 
                i23_address_prev <= (others => '0'); 
                i24_address_prev <= (others => '0'); 
                i25_address_prev <= (others => '0'); 
                i26_address_prev <= (others => '0'); 
                i27_address_prev <= (others => '0'); 
                i28_address_prev <= (others => '0'); 
                i29_address_prev <= (others => '0');    
                i30_address_prev <= (others => '0');                 
                i31_address_prev <= (others => '0');                                 
                i32_address_prev <= (others => '0');                                                 
                
                prev_AvalonMonitor_address <= (others => '0'); 
                prev_AvalonMonitor_read <= '0';
            elsif (clk'event and clk = '1') then
                prev_AvalonMonitor_read <= slave_AvalonMonitor_read;
                prev_AvalonMonitor_address <= slave_AvalonMonitor_address;
                if  i1_full = '0' and  s1_read = '1' then i1_address_prev <= s1_address; end if;
                if  i2_full = '0' and  s2_read = '1' then i2_address_prev <= s2_address; end if;
                if  i3_full = '0' and  s3_read = '1' then i3_address_prev <= s3_address; end if;
                if  i4_full = '0' and  s4_read = '1' then i4_address_prev <= s4_address; end if;
                if  i5_full = '0' and  s5_read = '1' then i5_address_prev <= s5_address; end if;
                if  i6_full = '0' and  s6_read = '1' then i6_address_prev <= s6_address; end if;
                if  i7_full = '0' and  s7_read = '1' then i7_address_prev <= s7_address; end if;
                if  i8_full = '0' and  s8_read = '1' then i8_address_prev <= s8_address; end if;
                if  i9_full = '0' and  s9_read = '1' then i9_address_prev <= s9_address; end if;
                if i10_full = '0' and s10_read = '1' then i10_address_prev <= s10_address; end if;
                if i11_full = '0' and s11_read = '1' then i11_address_prev <= s11_address; end if;
                if i12_full = '0' and s12_read = '1' then i12_address_prev <= s12_address; end if;
                if i13_full = '0' and s13_read = '1' then i13_address_prev <= s13_address; end if;
                if i14_full = '0' and s14_read = '1' then i14_address_prev <= s14_address; end if;
                if i15_full = '0' and s15_read = '1' then i15_address_prev <= s15_address; end if;
                if i16_full = '0' and s16_read = '1' then i16_address_prev <= s16_address; end if;
                if i17_full = '0' and s17_read = '1' then i17_address_prev <= s17_address; end if;
                if i18_full = '0' and s18_read = '1' then i18_address_prev <= s18_address; end if;
                if i19_full = '0' and s19_read = '1' then i19_address_prev <= s19_address; end if;
                if i20_full = '0' and s20_read = '1' then i20_address_prev <= s20_address; end if;                
                if i21_full = '0' and s21_read = '1' then i21_address_prev <= s21_address; end if;
                if i22_full = '0' and s22_read = '1' then i22_address_prev <= s22_address; end if;
                if i23_full = '0' and s23_read = '1' then i23_address_prev <= s23_address; end if;
                if i24_full = '0' and s24_read = '1' then i24_address_prev <= s24_address; end if;
                if i25_full = '0' and s25_read = '1' then i25_address_prev <= s25_address; end if;
                if i26_full = '0' and s26_read = '1' then i26_address_prev <= s26_address; end if;
                if i27_full = '0' and s27_read = '1' then i27_address_prev <= s27_address; end if;
                if i28_full = '0' and s28_read = '1' then i28_address_prev <= s28_address; end if;
                if i29_full = '0' and s29_read = '1' then i29_address_prev <= s29_address; end if;
                if i30_full = '0' and s30_read = '1' then i30_address_prev <= s30_address; end if;                
                if i31_full = '0' and s31_read = '1' then i31_address_prev <= s31_address; end if;                
                if i32_full = '0' and s32_read = '1' then i32_address_prev <= s32_address; end if;                                
            end if;
        end process;                    


    i2_read_fifo <=  '1' when i2_empty = '0' and R_CTRL(2) = '1' else '1' when  i2_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_1 else '0';
    i3_read_fifo <=  '1' when i3_empty = '0' and R_CTRL(2) = '1' else '1' when  i3_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_2 else '0';
    i4_read_fifo <=  '1' when i4_empty = '0' and R_CTRL(2) = '1' else '1' when  i4_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_3 else '0';
    i5_read_fifo <=  '1' when i5_empty = '0' and R_CTRL(2) = '1' else '1' when  i5_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_4 else '0';
    i6_read_fifo <=  '1' when i6_empty = '0' and R_CTRL(2) = '1' else '1' when  i6_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_5 else '0';
    i7_read_fifo <=  '1' when i7_empty = '0' and R_CTRL(2) = '1' else '1' when  i7_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_6 else '0';
    i8_read_fifo <=  '1' when i8_empty = '0' and R_CTRL(2) = '1' else '1' when  i8_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_7 else '0';
    i9_read_fifo <=  '1' when i9_empty = '0' and R_CTRL(2) = '1' else '1' when  i9_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_8 else '0';
    i10_read_fifo <= '1' when i10_empty = '0' and R_CTRL(2) = '1' else '1' when i10_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_9 else '0';    
    i11_read_fifo <= '1' when i11_empty = '0' and R_CTRL(2) = '1' else '1' when i11_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_10 else '0';
    i12_read_fifo <= '1' when i12_empty = '0' and R_CTRL(2) = '1' else '1' when i12_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_11 else '0';
    i13_read_fifo <= '1' when i13_empty = '0' and R_CTRL(2) = '1' else '1' when i13_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_12 else '0';
    i14_read_fifo <= '1' when i14_empty = '0' and R_CTRL(2) = '1' else '1' when i14_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_13 else '0';
    i15_read_fifo <= '1' when i15_empty = '0' and R_CTRL(2) = '1' else '1' when i15_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_14 else '0';
    i16_read_fifo <= '1' when i16_empty = '0' and R_CTRL(2) = '1' else '1' when i16_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_15 else '0';
    i17_read_fifo <= '1' when i17_empty = '0' and R_CTRL(2) = '1' else '1' when i17_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_16 else '0';
    i18_read_fifo <= '1' when i18_empty = '0' and R_CTRL(2) = '1' else '1' when i18_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_17 else '0';
    i19_read_fifo <= '1' when i19_empty = '0' and R_CTRL(2) = '1' else '1' when i19_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_18 else '0';
    i20_read_fifo <= '1' when i20_empty = '0' and R_CTRL(2) = '1' else '1' when i20_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_19 else '0';    
    i21_read_fifo <= '1' when i21_empty = '0' and R_CTRL(2) = '1' else '1' when i21_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_10 else '0';
    i22_read_fifo <= '1' when i22_empty = '0' and R_CTRL(2) = '1' else '1' when i22_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_21 else '0';
    i23_read_fifo <= '1' when i23_empty = '0' and R_CTRL(2) = '1' else '1' when i23_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_22 else '0';
    i24_read_fifo <= '1' when i24_empty = '0' and R_CTRL(2) = '1' else '1' when i24_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_23 else '0';
    i25_read_fifo <= '1' when i25_empty = '0' and R_CTRL(2) = '1' else '1' when i25_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_24 else '0';
    i26_read_fifo <= '1' when i26_empty = '0' and R_CTRL(2) = '1' else '1' when i26_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_25 else '0';
    i27_read_fifo <= '1' when i27_empty = '0' and R_CTRL(2) = '1' else '1' when i27_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_26 else '0';
    i28_read_fifo <= '1' when i28_empty = '0' and R_CTRL(2) = '1' else '1' when i28_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_27 else '0';
    i29_read_fifo <= '1' when i29_empty = '0' and R_CTRL(2) = '1' else '1' when i29_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_28 else '0';
    i30_read_fifo <= '1' when i30_empty = '0' and R_CTRL(2) = '1' else '1' when i30_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_29 else '0';    
    i31_read_fifo <= '1' when i31_empty = '0' and R_CTRL(2) = '1' else '1' when i31_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_30 else '0';
    i32_read_fifo <= '1' when i32_empty = '0' and R_CTRL(2) = '1' else '1' when i32_almost_full = '1' and R_CTRL(0) = '0' else '1' when slave_AvalonMonitor_read = '0' and prev_AvalonMonitor_read = '1' and prev_AvalonMonitor_address = ADDR_FIFO_31 else '0';

    i_frozzen(2)    <= '1' when ((i2_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i2_address_prev /=  s2_address and  s2_read = '1' else '0';
    i_frozzen(3)    <= '1' when ((i3_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i3_address_prev /=  s3_address and  s3_read = '1' else '0';
    i_frozzen(4)    <= '1' when ((i4_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i4_address_prev /=  s4_address and  s4_read = '1' else '0';
    i_frozzen(5)    <= '1' when ((i5_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i5_address_prev /=  s5_address and  s5_read = '1' else '0';
    i_frozzen(6)    <= '1' when ((i6_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i6_address_prev /=  s6_address and  s6_read = '1' else '0';
    i_frozzen(7)    <= '1' when ((i7_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i7_address_prev /=  s7_address and  s7_read = '1' else '0';
    i_frozzen(8)    <= '1' when ((i8_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i8_address_prev /=  s8_address and  s8_read = '1' else '0';
    i_frozzen(9)    <= '1' when ((i9_full = '1'  and R_CTRL(0) = '1') or R_CTRL(1) = '1') and  i9_address_prev /=  s9_address and  s9_read = '1' else '0';
    i_frozzen(10)   <= '1' when ((i10_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i10_address_prev /= s10_address and s10_read = '1' else '0';    
    i_frozzen(11)   <= '1' when ((i11_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i11_address_prev /= s11_address and s11_read = '1' else '0';
    i_frozzen(12)   <= '1' when ((i12_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i12_address_prev /= s12_address and s12_read = '1' else '0';
    i_frozzen(13)   <= '1' when ((i13_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i13_address_prev /= s13_address and s13_read = '1' else '0';
    i_frozzen(14)   <= '1' when ((i14_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i14_address_prev /= s14_address and s14_read = '1' else '0';
    i_frozzen(15)   <= '1' when ((i15_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i15_address_prev /= s15_address and s15_read = '1' else '0';
    i_frozzen(16)   <= '1' when ((i16_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i16_address_prev /= s16_address and s16_read = '1' else '0';
    i_frozzen(17)   <= '1' when ((i17_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i17_address_prev /= s17_address and s17_read = '1' else '0';
    i_frozzen(18)   <= '1' when ((i18_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i18_address_prev /= s18_address and s18_read = '1' else '0';
    i_frozzen(19)   <= '1' when ((i19_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i19_address_prev /= s19_address and s19_read = '1' else '0';
    i_frozzen(20)   <= '1' when ((i20_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i20_address_prev /= s20_address and s20_read = '1' else '0';    
    i_frozzen(21)   <= '1' when ((i21_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i21_address_prev /= s21_address and s21_read = '1' else '0';
    i_frozzen(22)   <= '1' when ((i22_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i22_address_prev /= s22_address and s22_read = '1' else '0';
    i_frozzen(23)   <= '1' when ((i23_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i23_address_prev /= s23_address and s23_read = '1' else '0';
    i_frozzen(24)   <= '1' when ((i24_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i24_address_prev /= s24_address and s24_read = '1' else '0';
    i_frozzen(25)   <= '1' when ((i25_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i25_address_prev /= s25_address and s25_read = '1' else '0';
    i_frozzen(26)   <= '1' when ((i26_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i26_address_prev /= s26_address and s26_read = '1' else '0';
    i_frozzen(27)   <= '1' when ((i27_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i27_address_prev /= s27_address and s27_read = '1' else '0';
    i_frozzen(28)   <= '1' when ((i28_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i28_address_prev /= s28_address and s28_read = '1' else '0';
    i_frozzen(29)   <= '1' when ((i29_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i29_address_prev /= s29_address and s29_read = '1' else '0';    
    i_frozzen(30)   <= '1' when ((i30_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i30_address_prev /= s30_address and s30_read = '1' else '0';    
    i_frozzen(31)   <= '1' when ((i31_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i31_address_prev /= s31_address and s31_read = '1' else '0';
    i_frozzen(32)   <= '1' when ((i32_full = '1' and R_CTRL(0) = '1') or R_CTRL(1) = '1') and i32_address_prev /= s32_address and s32_read = '1' else '0';


Slave_Avalon_Read:
	WITH slave_AvalonMonitor_address SELECT
	    slave_AvalonMonitor_readdata_int <= i1_data_out   when ADDR_FIFO_0,
                                        i2_data_out   when ADDR_FIFO_1,
                                        i3_data_out   when ADDR_FIFO_2,
                                        i4_data_out   when ADDR_FIFO_3,
                                        i5_data_out   when ADDR_FIFO_4,
                                        i6_data_out   when ADDR_FIFO_5,
                                        i7_data_out   when ADDR_FIFO_6,
                                        i8_data_out   when ADDR_FIFO_7,
                                        i9_data_out   when ADDR_FIFO_8,
                                        i10_data_out   when ADDR_FIFO_9,                                        
                                        i11_data_out   when ADDR_FIFO_10,
                                        i12_data_out   when ADDR_FIFO_11,
                                        i13_data_out   when ADDR_FIFO_12,
                                        i14_data_out   when ADDR_FIFO_13,
                                        i15_data_out   when ADDR_FIFO_14,
                                        i16_data_out   when ADDR_FIFO_15,
                                        i17_data_out   when ADDR_FIFO_16,
                                        i18_data_out   when ADDR_FIFO_17,
                                        i19_data_out   when ADDR_FIFO_18,
                                        i20_data_out   when ADDR_FIFO_19,
                                        i21_data_out   when ADDR_FIFO_20,
                                        i22_data_out   when ADDR_FIFO_21,
                                        i23_data_out   when ADDR_FIFO_22,
                                        i24_data_out   when ADDR_FIFO_23,
                                        i25_data_out   when ADDR_FIFO_24,
                                        i26_data_out   when ADDR_FIFO_25,
                                        i27_data_out   when ADDR_FIFO_26,
                                        i28_data_out   when ADDR_FIFO_27,
                                        i29_data_out   when ADDR_FIFO_28,
                                        i30_data_out   when ADDR_FIFO_29,
                                        i31_data_out   when ADDR_FIFO_30,
                                        i32_data_out   when ADDR_FIFO_31,                                        
                                        (others => '0') when others;

Read_data: process (slave_AvalonMonitor_readdata_int) is
    begin
        slave_AvalonMonitor_readdata <= (others => '0');
        slave_AvalonMonitor_readdata(ADDRESS_WIDTH - 1 downto 0) <= slave_AvalonMonitor_readdata_int;
    end process;     

Control_register:
    process(reset, clk) is 
    begin 
        if (reset = '1') then    
            R_CTRL <= (others => '0'); 
        elsif (clk'event and clk = '1') then
            if (slave_AvalonMonitor_write = '1' and slave_AvalonMonitor_address = ADDR_FIFO_0) then
                R_CTRL <= slave_AvalonMonitor_writedata;
            end if;
        end if;
    end process; 
        
end generate ENABLE_MONITORING_if;

ENABLE_MONITORING_if0: if ENABLE_MONITORING = 0 generate
	i_frozzen <= (others => '0');
end generate ENABLE_MONITORING_if0;

    m1_burstcount     <= s1_burstcount;
    m1_writedata      <= s1_writedata;
    m1_address        <= s1_address;
    m1_write          <= s1_write;
    m1_read           <= s1_read when i_frozzen(1) = '0' else '0';
    m1_byteenable     <= s1_byteenable;
    m1_debugaccess    <= s1_debugaccess;
    s1_waitrequest    <= m1_waitrequest when i_frozzen(1) = '0' else '1';
    s1_readdatavalid  <= m1_readdatavalid;
    s1_readdata       <= m1_readdata;

    m2_burstcount     <= s2_burstcount;
    m2_writedata      <= s2_writedata;
    m2_address        <= s2_address;
    m2_write          <= s2_write;
    m2_read           <= s2_read when i_frozzen(2) = '0' else '0';
    m2_byteenable     <= s2_byteenable;
    m2_debugaccess    <= s2_debugaccess;
    s2_waitrequest    <= m2_waitrequest when i_frozzen(2) = '0' else '1';
    s2_readdatavalid  <= m2_readdatavalid;
    s2_readdata       <= m2_readdata;
    
    m3_burstcount     <= s3_burstcount;
    m3_writedata      <= s3_writedata;
    m3_address        <= s3_address;
    m3_write          <= s3_write;
    m3_read           <= s3_read when i_frozzen(3) = '0' else '0';
    m3_byteenable     <= s3_byteenable;
    m3_debugaccess    <= s3_debugaccess;
    s3_waitrequest    <= m3_waitrequest when i_frozzen(3) = '0' else '1';
    s3_readdatavalid  <= m3_readdatavalid;
    s3_readdata       <= m3_readdata;

    m4_burstcount     <= s4_burstcount;
    m4_writedata      <= s4_writedata;
    m4_address        <= s4_address;
    m4_write          <= s4_write;
    m4_read           <= s4_read when i_frozzen(4) = '0' else '0';
    m4_byteenable     <= s4_byteenable;
    m4_debugaccess    <= s4_debugaccess;
    s4_waitrequest    <= m4_waitrequest when i_frozzen(4) = '0' else '1';
    s4_readdatavalid  <= m4_readdatavalid;
    s4_readdata       <= m4_readdata;

    m5_burstcount     <= s5_burstcount;
    m5_writedata      <= s5_writedata;
    m5_address        <= s5_address;
    m5_write          <= s5_write;
    m5_read           <= s5_read when i_frozzen(5) = '0' else '0';
    m5_byteenable     <= s5_byteenable;
    m5_debugaccess    <= s5_debugaccess;
    s5_waitrequest    <= m5_waitrequest when i_frozzen(5) = '0' else '1';
    s5_readdatavalid  <= m5_readdatavalid;
    s5_readdata       <= m5_readdata;

    m6_burstcount     <= s6_burstcount;
    m6_writedata      <= s6_writedata;
    m6_address        <= s6_address;
    m6_write          <= s6_write;
    m6_read           <= s6_read when i_frozzen(6) = '0' else '0';
    m6_byteenable     <= s6_byteenable;
    m6_debugaccess    <= s6_debugaccess;
    s6_waitrequest    <= m6_waitrequest when i_frozzen(6) = '0' else '1';
    s6_readdatavalid  <= m6_readdatavalid;
    s6_readdata       <= m6_readdata;

    m7_burstcount     <= s7_burstcount;
    m7_writedata      <= s7_writedata;
    m7_address        <= s7_address;
    m7_write          <= s7_write;
    m7_read           <= s7_read when i_frozzen(7) = '0' else '0';
    m7_byteenable     <= s7_byteenable;
    m7_debugaccess    <= s7_debugaccess;
    s7_waitrequest    <= m7_waitrequest when i_frozzen(7) = '0' else '1';
    s7_readdatavalid  <= m7_readdatavalid;
    s7_readdata       <= m7_readdata;

    m8_burstcount     <= s8_burstcount;
    m8_writedata      <= s8_writedata;
    m8_address        <= s8_address;
    m8_write          <= s8_write;
    m8_read           <= s8_read when i_frozzen(8) = '0' else '0';
    m8_byteenable     <= s8_byteenable;
    m8_debugaccess    <= s8_debugaccess;
    s8_waitrequest    <= m8_waitrequest when i_frozzen(8) = '0' else '1';
    s8_readdatavalid  <= m8_readdatavalid;
    s8_readdata       <= m8_readdata;

    m9_burstcount     <= s9_burstcount;
    m9_writedata      <= s9_writedata;
    m9_address        <= s9_address;
    m9_write          <= s9_write;
    m9_read           <= s9_read when i_frozzen(9) = '0' else '0';
    m9_byteenable     <= s9_byteenable;
    m9_debugaccess    <= s9_debugaccess;
    s9_waitrequest    <= m9_waitrequest when i_frozzen(9) = '0' else '1';
    s9_readdatavalid  <= m9_readdatavalid;
    s9_readdata       <= m9_readdata;

    m10_burstcount     <= s10_burstcount;
    m10_writedata      <= s10_writedata;
    m10_address        <= s10_address;
    m10_write          <= s10_write;
    m10_read           <= s10_read when i_frozzen(10) = '0' else '0';
    m10_byteenable     <= s10_byteenable;
    m10_debugaccess    <= s10_debugaccess;
    s10_waitrequest    <= m10_waitrequest when i_frozzen(10) = '0' else '1';
    s10_readdatavalid  <= m10_readdatavalid;
    s10_readdata       <= m10_readdata;

    m11_burstcount     <= s11_burstcount;
    m11_writedata      <= s11_writedata;
    m11_address        <= s11_address;
    m11_write          <= s11_write;
    m11_read           <= s11_read when i_frozzen(11) = '0' else '0';
    m11_byteenable     <= s11_byteenable;
    m11_debugaccess    <= s11_debugaccess;
    s11_waitrequest    <= m11_waitrequest when i_frozzen(11) = '0' else '1';
    s11_readdatavalid  <= m11_readdatavalid;
    s11_readdata       <= m11_readdata;

    m12_burstcount     <= s12_burstcount;
    m12_writedata      <= s12_writedata;
    m12_address        <= s12_address;
    m12_write          <= s12_write;
    m12_read           <= s12_read when i_frozzen(12) = '0' else '0';
    m12_byteenable     <= s12_byteenable;
    m12_debugaccess    <= s12_debugaccess;
    s12_waitrequest    <= m12_waitrequest when i_frozzen(12) = '0' else '1';
    s12_readdatavalid  <= m12_readdatavalid;
    s12_readdata       <= m12_readdata;
    
    m13_burstcount     <= s13_burstcount;
    m13_writedata      <= s13_writedata;
    m13_address        <= s13_address;
    m13_write          <= s13_write;
    m13_read           <= s13_read when i_frozzen(13) = '0' else '0';
    m13_byteenable     <= s13_byteenable;
    m13_debugaccess    <= s13_debugaccess;
    s13_waitrequest    <= m13_waitrequest when i_frozzen(13) = '0' else '1';
    s13_readdatavalid  <= m13_readdatavalid;
    s13_readdata       <= m13_readdata;

    m14_burstcount     <= s14_burstcount;
    m14_writedata      <= s14_writedata;
    m14_address        <= s14_address;
    m14_write          <= s14_write;
    m14_read           <= s14_read when i_frozzen(14) = '0' else '0';
    m14_byteenable     <= s14_byteenable;
    m14_debugaccess    <= s14_debugaccess;
    s14_waitrequest    <= m14_waitrequest when i_frozzen(14) = '0' else '1';
    s14_readdatavalid  <= m14_readdatavalid;
    s14_readdata       <= m14_readdata;

    m15_burstcount     <= s15_burstcount;
    m15_writedata      <= s15_writedata;
    m15_address        <= s15_address;
    m15_write          <= s15_write;
    m15_read           <= s15_read when i_frozzen(15) = '0' else '0';
    m15_byteenable     <= s15_byteenable;
    m15_debugaccess    <= s15_debugaccess;
    s15_waitrequest    <= m15_waitrequest when i_frozzen(15) = '0' else '1';
    s15_readdatavalid  <= m15_readdatavalid;
    s15_readdata       <= m15_readdata;

    m16_burstcount     <= s16_burstcount;
    m16_writedata      <= s16_writedata;
    m16_address        <= s16_address;
    m16_write          <= s16_write;
    m16_read           <= s16_read when i_frozzen(16) = '0' else '0';
    m16_byteenable     <= s16_byteenable;
    m16_debugaccess    <= s16_debugaccess;
    s16_waitrequest    <= m16_waitrequest when i_frozzen(16) = '0' else '1';
    s16_readdatavalid  <= m16_readdatavalid;
    s16_readdata       <= m16_readdata;

    m17_burstcount     <= s17_burstcount;
    m17_writedata      <= s17_writedata;
    m17_address        <= s17_address;
    m17_write          <= s17_write;
    m17_read           <= s17_read when i_frozzen(17) = '0' else '0';
    m17_byteenable     <= s17_byteenable;
    m17_debugaccess    <= s17_debugaccess;
    s17_waitrequest    <= m17_waitrequest when i_frozzen(17) = '0' else '1';
    s17_readdatavalid  <= m17_readdatavalid;
    s17_readdata       <= m17_readdata;

    m18_burstcount     <= s18_burstcount;
    m18_writedata      <= s18_writedata;
    m18_address        <= s18_address;
    m18_write          <= s18_write;
    m18_read           <= s18_read when i_frozzen(18) = '0' else '0';
    m18_byteenable     <= s18_byteenable;
    m18_debugaccess    <= s18_debugaccess;
    s18_waitrequest    <= m18_waitrequest when i_frozzen(18) = '0' else '1';
    s18_readdatavalid  <= m18_readdatavalid;
    s18_readdata       <= m18_readdata;

    m19_burstcount     <= s19_burstcount;
    m19_writedata      <= s19_writedata;
    m19_address        <= s19_address;
    m19_write          <= s19_write;
    m19_read           <= s19_read when i_frozzen(19) = '0' else '0';
    m19_byteenable     <= s19_byteenable;
    m19_debugaccess    <= s19_debugaccess;
    s19_waitrequest    <= m19_waitrequest when i_frozzen(19) = '0' else '1';
    s19_readdatavalid  <= m19_readdatavalid;
    s19_readdata       <= m19_readdata;

    m20_burstcount     <= s20_burstcount;
    m20_writedata      <= s20_writedata;
    m20_address        <= s20_address;
    m20_write          <= s20_write;
    m20_read           <= s20_read when i_frozzen(20) = '0' else '0';
    m20_byteenable     <= s20_byteenable;
    m20_debugaccess    <= s20_debugaccess;
    s20_waitrequest    <= m20_waitrequest when i_frozzen(20) = '0' else '1';
    s20_readdatavalid  <= m20_readdatavalid;
    s20_readdata       <= m20_readdata;

    m21_burstcount     <= s21_burstcount;
    m21_writedata      <= s21_writedata;
    m21_address        <= s21_address;
    m21_write          <= s21_write;
    m21_read           <= s21_read when i_frozzen(21) = '0' else '0';
    m21_byteenable     <= s21_byteenable;
    m21_debugaccess    <= s21_debugaccess;
    s21_waitrequest    <= m21_waitrequest when i_frozzen(21) = '0' else '1';
    s21_readdatavalid  <= m21_readdatavalid;
    s21_readdata       <= m21_readdata;

    m22_burstcount     <= s22_burstcount;
    m22_writedata      <= s22_writedata;
    m22_address        <= s22_address;
    m22_write          <= s22_write;
    m22_read           <= s22_read when i_frozzen(22) = '0' else '0';
    m22_byteenable     <= s22_byteenable;
    m22_debugaccess    <= s22_debugaccess;
    s22_waitrequest    <= m22_waitrequest when i_frozzen(22) = '0' else '1';
    s22_readdatavalid  <= m22_readdatavalid;
    s22_readdata       <= m22_readdata;
    
    m23_burstcount     <= s23_burstcount;
    m23_writedata      <= s23_writedata;
    m23_address        <= s23_address;
    m23_write          <= s23_write;
    m23_read           <= s23_read when i_frozzen(23) = '0' else '0';
    m23_byteenable     <= s23_byteenable;
    m23_debugaccess    <= s23_debugaccess;
    s23_waitrequest    <= m23_waitrequest when i_frozzen(23) = '0' else '1';
    s23_readdatavalid  <= m23_readdatavalid;
    s23_readdata       <= m23_readdata;

    m24_burstcount     <= s24_burstcount;
    m24_writedata      <= s24_writedata;
    m24_address        <= s24_address;
    m24_write          <= s24_write;
    m24_read           <= s24_read when i_frozzen(24) = '0' else '0';
    m24_byteenable     <= s24_byteenable;
    m24_debugaccess    <= s24_debugaccess;
    s24_waitrequest    <= m24_waitrequest when i_frozzen(24) = '0' else '1';
    s24_readdatavalid  <= m24_readdatavalid;
    s24_readdata       <= m24_readdata;

    m25_burstcount     <= s25_burstcount;
    m25_writedata      <= s25_writedata;
    m25_address        <= s25_address;
    m25_write          <= s25_write;
    m25_read           <= s25_read when i_frozzen(25) = '0' else '0';
    m25_byteenable     <= s25_byteenable;
    m25_debugaccess    <= s25_debugaccess;
    s25_waitrequest    <= m25_waitrequest when i_frozzen(25) = '0' else '1';
    s25_readdatavalid  <= m25_readdatavalid;
    s25_readdata       <= m25_readdata;

    m26_burstcount     <= s26_burstcount;
    m26_writedata      <= s26_writedata;
    m26_address        <= s26_address;
    m26_write          <= s26_write;
    m26_read           <= s26_read when i_frozzen(26) = '0' else '0';
    m26_byteenable     <= s26_byteenable;
    m26_debugaccess    <= s26_debugaccess;
    s26_waitrequest    <= m26_waitrequest when i_frozzen(26) = '0' else '1';
    s26_readdatavalid  <= m26_readdatavalid;
    s26_readdata       <= m26_readdata;

    m27_burstcount     <= s27_burstcount;
    m27_writedata      <= s27_writedata;
    m27_address        <= s27_address;
    m27_write          <= s27_write;
    m27_read           <= s27_read when i_frozzen(27) = '0' else '0';
    m27_byteenable     <= s27_byteenable;
    m27_debugaccess    <= s27_debugaccess;
    s27_waitrequest    <= m27_waitrequest when i_frozzen(27) = '0' else '1';
    s27_readdatavalid  <= m27_readdatavalid;
    s27_readdata       <= m27_readdata;

    m28_burstcount     <= s28_burstcount;
    m28_writedata      <= s28_writedata;
    m28_address        <= s28_address;
    m28_write          <= s28_write;
    m28_read           <= s28_read when i_frozzen(28) = '0' else '0';
    m28_byteenable     <= s28_byteenable;
    m28_debugaccess    <= s28_debugaccess;
    s28_waitrequest    <= m28_waitrequest when i_frozzen(28) = '0' else '1';
    s28_readdatavalid  <= m28_readdatavalid;
    s28_readdata       <= m28_readdata;

    m29_burstcount     <= s29_burstcount;
    m29_writedata      <= s29_writedata;
    m29_address        <= s29_address;
    m29_write          <= s29_write;
    m29_read           <= s29_read when i_frozzen(29) = '0' else '0';
    m29_byteenable     <= s29_byteenable;
    m29_debugaccess    <= s29_debugaccess;
    s29_waitrequest    <= m29_waitrequest when i_frozzen(29) = '0' else '1';
    s29_readdatavalid  <= m29_readdatavalid;
    s29_readdata       <= m29_readdata;

    m30_burstcount     <= s30_burstcount;
    m30_writedata      <= s30_writedata;
    m30_address        <= s30_address;
    m30_write          <= s30_write;
    m30_read           <= s30_read when i_frozzen(30) = '0' else '0';
    m30_byteenable     <= s30_byteenable;
    m30_debugaccess    <= s30_debugaccess;
    s30_waitrequest    <= m30_waitrequest when i_frozzen(30) = '0' else '1';
    s30_readdatavalid  <= m30_readdatavalid;
    s30_readdata       <= m30_readdata;

    m31_burstcount     <= s31_burstcount;
    m31_writedata      <= s31_writedata;
    m31_address        <= s31_address;
    m31_write          <= s31_write;
    m31_read           <= s31_read when i_frozzen(31) = '0' else '0';
    m31_byteenable     <= s31_byteenable;
    m31_debugaccess    <= s31_debugaccess;
    s31_waitrequest    <= m31_waitrequest when i_frozzen(31) = '0' else '1';
    s31_readdatavalid  <= m31_readdatavalid;
    s31_readdata       <= m31_readdata;

    m32_burstcount     <= s32_burstcount;
    m32_writedata      <= s32_writedata;
    m32_address        <= s32_address;
    m32_write          <= s32_write;
    m32_read           <= s32_read when i_frozzen(32) = '0' else '0';
    m32_byteenable     <= s32_byteenable;
    m32_debugaccess    <= s32_debugaccess;
    s32_waitrequest    <= m32_waitrequest when i_frozzen(32) = '0' else '1';
    s32_readdatavalid  <= m32_readdatavalid;
    s32_readdata       <= m32_readdata;

 
end architecture AM1;	
