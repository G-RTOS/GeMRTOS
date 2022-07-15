-- $Id: //avalon_bridge.vhd#1 $
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



entity avalon_bridge is

	generic
		(
            NProcessors              : integer;
            DATA_WIDTH               : integer := 32;
            ADDRESS_WIDTH            : integer := 32;
            BURSTCOUNT_WIDTH         : integer := 1;
            BYTEEN_WIDTH             : integer := 4;
            DEVICE_FAMILY            : string
		);
        
	port (
        clk         : in  std_logic;
		reset       : in  std_logic; 		
		      
        
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
			
end entity avalon_bridge;

architecture AM1 of avalon_bridge is
    

begin

    m1_burstcount     <= s1_burstcount;
    m1_writedata      <= s1_writedata;
    m1_address        <= s1_address;
    m1_write          <= s1_write;
    m1_read           <= s1_read;
    m1_byteenable     <= s1_byteenable;
    m1_debugaccess    <= s1_debugaccess;
    s1_waitrequest    <= m1_waitrequest;
    s1_readdatavalid  <= m1_readdatavalid;
    s1_readdata       <= m1_readdata;

    m2_burstcount     <= s2_burstcount;
    m2_writedata      <= s2_writedata;
    m2_address        <= s2_address;
    m2_write          <= s2_write;
    m2_read           <= s2_read;
    m2_byteenable     <= s2_byteenable;
    m2_debugaccess    <= s2_debugaccess;
    s2_waitrequest    <= m2_waitrequest;
    s2_readdatavalid  <= m2_readdatavalid;
    s2_readdata       <= m2_readdata;
    
    m3_burstcount     <= s3_burstcount;
    m3_writedata      <= s3_writedata;
    m3_address        <= s3_address;
    m3_write          <= s3_write;
    m3_read           <= s3_read;
    m3_byteenable     <= s3_byteenable;
    m3_debugaccess    <= s3_debugaccess;
    s3_waitrequest    <= m3_waitrequest;
    s3_readdatavalid  <= m3_readdatavalid;
    s3_readdata       <= m3_readdata;

    m4_burstcount     <= s4_burstcount;
    m4_writedata      <= s4_writedata;
    m4_address        <= s4_address;
    m4_write          <= s4_write;
    m4_read           <= s4_read;
    m4_byteenable     <= s4_byteenable;
    m4_debugaccess    <= s4_debugaccess;
    s4_waitrequest    <= m4_waitrequest;
    s4_readdatavalid  <= m4_readdatavalid;
    s4_readdata       <= m4_readdata;

    m5_burstcount     <= s5_burstcount;
    m5_writedata      <= s5_writedata;
    m5_address        <= s5_address;
    m5_write          <= s5_write;
    m5_read           <= s5_read;
    m5_byteenable     <= s5_byteenable;
    m5_debugaccess    <= s5_debugaccess;
    s5_waitrequest    <= m5_waitrequest;
    s5_readdatavalid  <= m5_readdatavalid;
    s5_readdata       <= m5_readdata;

    m6_burstcount     <= s6_burstcount;
    m6_writedata      <= s6_writedata;
    m6_address        <= s6_address;
    m6_write          <= s6_write;
    m6_read           <= s6_read;
    m6_byteenable     <= s6_byteenable;
    m6_debugaccess    <= s6_debugaccess;
    s6_waitrequest    <= m6_waitrequest;
    s6_readdatavalid  <= m6_readdatavalid;
    s6_readdata       <= m6_readdata;

    m7_burstcount     <= s7_burstcount;
    m7_writedata      <= s7_writedata;
    m7_address        <= s7_address;
    m7_write          <= s7_write;
    m7_read           <= s7_read;
    m7_byteenable     <= s7_byteenable;
    m7_debugaccess    <= s7_debugaccess;
    s7_waitrequest    <= m7_waitrequest;
    s7_readdatavalid  <= m7_readdatavalid;
    s7_readdata       <= m7_readdata;

    m8_burstcount     <= s8_burstcount;
    m8_writedata      <= s8_writedata;
    m8_address        <= s8_address;
    m8_write          <= s8_write;
    m8_read           <= s8_read;
    m8_byteenable     <= s8_byteenable;
    m8_debugaccess    <= s8_debugaccess;
    s8_waitrequest    <= m8_waitrequest;
    s8_readdatavalid  <= m8_readdatavalid;
    s8_readdata       <= m8_readdata;

    m9_burstcount     <= s9_burstcount;
    m9_writedata      <= s9_writedata;
    m9_address        <= s9_address;
    m9_write          <= s9_write;
    m9_read           <= s9_read;
    m9_byteenable     <= s9_byteenable;
    m9_debugaccess    <= s9_debugaccess;
    s9_waitrequest    <= m9_waitrequest;
    s9_readdatavalid  <= m9_readdatavalid;
    s9_readdata       <= m9_readdata;

    m10_burstcount     <= s10_burstcount;
    m10_writedata      <= s10_writedata;
    m10_address        <= s10_address;
    m10_write          <= s10_write;
    m10_read           <= s10_read;
    m10_byteenable     <= s10_byteenable;
    m10_debugaccess    <= s10_debugaccess;
    s10_waitrequest    <= m10_waitrequest;
    s10_readdatavalid  <= m10_readdatavalid;
    s10_readdata       <= m10_readdata;

    m11_burstcount     <= s11_burstcount;
    m11_writedata      <= s11_writedata;
    m11_address        <= s11_address;
    m11_write          <= s11_write;
    m11_read           <= s11_read;
    m11_byteenable     <= s11_byteenable;
    m11_debugaccess    <= s11_debugaccess;
    s11_waitrequest    <= m11_waitrequest;
    s11_readdatavalid  <= m11_readdatavalid;
    s11_readdata       <= m11_readdata;

    m12_burstcount     <= s12_burstcount;
    m12_writedata      <= s12_writedata;
    m12_address        <= s12_address;
    m12_write          <= s12_write;
    m12_read           <= s12_read;
    m12_byteenable     <= s12_byteenable;
    m12_debugaccess    <= s12_debugaccess;
    s12_waitrequest    <= m12_waitrequest;
    s12_readdatavalid  <= m12_readdatavalid;
    s12_readdata       <= m12_readdata;
    
    m13_burstcount     <= s13_burstcount;
    m13_writedata      <= s13_writedata;
    m13_address        <= s13_address;
    m13_write          <= s13_write;
    m13_read           <= s13_read;
    m13_byteenable     <= s13_byteenable;
    m13_debugaccess    <= s13_debugaccess;
    s13_waitrequest    <= m13_waitrequest;
    s13_readdatavalid  <= m13_readdatavalid;
    s13_readdata       <= m13_readdata;

    m14_burstcount     <= s14_burstcount;
    m14_writedata      <= s14_writedata;
    m14_address        <= s14_address;
    m14_write          <= s14_write;
    m14_read           <= s14_read;
    m14_byteenable     <= s14_byteenable;
    m14_debugaccess    <= s14_debugaccess;
    s14_waitrequest    <= m14_waitrequest;
    s14_readdatavalid  <= m14_readdatavalid;
    s14_readdata       <= m14_readdata;

    m15_burstcount     <= s15_burstcount;
    m15_writedata      <= s15_writedata;
    m15_address        <= s15_address;
    m15_write          <= s15_write;
    m15_read           <= s15_read;
    m15_byteenable     <= s15_byteenable;
    m15_debugaccess    <= s15_debugaccess;
    s15_waitrequest    <= m15_waitrequest;
    s15_readdatavalid  <= m15_readdatavalid;
    s15_readdata       <= m15_readdata;

    m16_burstcount     <= s16_burstcount;
    m16_writedata      <= s16_writedata;
    m16_address        <= s16_address;
    m16_write          <= s16_write;
    m16_read           <= s16_read;
    m16_byteenable     <= s16_byteenable;
    m16_debugaccess    <= s16_debugaccess;
    s16_waitrequest    <= m16_waitrequest;
    s16_readdatavalid  <= m16_readdatavalid;
    s16_readdata       <= m16_readdata;

    m17_burstcount     <= s17_burstcount;
    m17_writedata      <= s17_writedata;
    m17_address        <= s17_address;
    m17_write          <= s17_write;
    m17_read           <= s17_read;
    m17_byteenable     <= s17_byteenable;
    m17_debugaccess    <= s17_debugaccess;
    s17_waitrequest    <= m17_waitrequest;
    s17_readdatavalid  <= m17_readdatavalid;
    s17_readdata       <= m17_readdata;

    m18_burstcount     <= s18_burstcount;
    m18_writedata      <= s18_writedata;
    m18_address        <= s18_address;
    m18_write          <= s18_write;
    m18_read           <= s18_read;
    m18_byteenable     <= s18_byteenable;
    m18_debugaccess    <= s18_debugaccess;
    s18_waitrequest    <= m18_waitrequest;
    s18_readdatavalid  <= m18_readdatavalid;
    s18_readdata       <= m18_readdata;

    m19_burstcount     <= s19_burstcount;
    m19_writedata      <= s19_writedata;
    m19_address        <= s19_address;
    m19_write          <= s19_write;
    m19_read           <= s19_read;
    m19_byteenable     <= s19_byteenable;
    m19_debugaccess    <= s19_debugaccess;
    s19_waitrequest    <= m19_waitrequest;
    s19_readdatavalid  <= m19_readdatavalid;
    s19_readdata       <= m19_readdata;

    m20_burstcount     <= s20_burstcount;
    m20_writedata      <= s20_writedata;
    m20_address        <= s20_address;
    m20_write          <= s20_write;
    m20_read           <= s20_read;
    m20_byteenable     <= s20_byteenable;
    m20_debugaccess    <= s20_debugaccess;
    s20_waitrequest    <= m20_waitrequest;
    s20_readdatavalid  <= m20_readdatavalid;
    s20_readdata       <= m20_readdata;

    m21_burstcount     <= s21_burstcount;
    m21_writedata      <= s21_writedata;
    m21_address        <= s21_address;
    m21_write          <= s21_write;
    m21_read           <= s21_read;
    m21_byteenable     <= s21_byteenable;
    m21_debugaccess    <= s21_debugaccess;
    s21_waitrequest    <= m21_waitrequest;
    s21_readdatavalid  <= m21_readdatavalid;
    s21_readdata       <= m21_readdata;

    m22_burstcount     <= s22_burstcount;
    m22_writedata      <= s22_writedata;
    m22_address        <= s22_address;
    m22_write          <= s22_write;
    m22_read           <= s22_read;
    m22_byteenable     <= s22_byteenable;
    m22_debugaccess    <= s22_debugaccess;
    s22_waitrequest    <= m22_waitrequest;
    s22_readdatavalid  <= m22_readdatavalid;
    s22_readdata       <= m22_readdata;
    
    m23_burstcount     <= s23_burstcount;
    m23_writedata      <= s23_writedata;
    m23_address        <= s23_address;
    m23_write          <= s23_write;
    m23_read           <= s23_read;
    m23_byteenable     <= s23_byteenable;
    m23_debugaccess    <= s23_debugaccess;
    s23_waitrequest    <= m23_waitrequest;
    s23_readdatavalid  <= m23_readdatavalid;
    s23_readdata       <= m23_readdata;

    m24_burstcount     <= s24_burstcount;
    m24_writedata      <= s24_writedata;
    m24_address        <= s24_address;
    m24_write          <= s24_write;
    m24_read           <= s24_read;
    m24_byteenable     <= s24_byteenable;
    m24_debugaccess    <= s24_debugaccess;
    s24_waitrequest    <= m24_waitrequest;
    s24_readdatavalid  <= m24_readdatavalid;
    s24_readdata       <= m24_readdata;

    m25_burstcount     <= s25_burstcount;
    m25_writedata      <= s25_writedata;
    m25_address        <= s25_address;
    m25_write          <= s25_write;
    m25_read           <= s25_read;
    m25_byteenable     <= s25_byteenable;
    m25_debugaccess    <= s25_debugaccess;
    s25_waitrequest    <= m25_waitrequest;
    s25_readdatavalid  <= m25_readdatavalid;
    s25_readdata       <= m25_readdata;

    m26_burstcount     <= s26_burstcount;
    m26_writedata      <= s26_writedata;
    m26_address        <= s26_address;
    m26_write          <= s26_write;
    m26_read           <= s26_read;
    m26_byteenable     <= s26_byteenable;
    m26_debugaccess    <= s26_debugaccess;
    s26_waitrequest    <= m26_waitrequest;
    s26_readdatavalid  <= m26_readdatavalid;
    s26_readdata       <= m26_readdata;

    m27_burstcount     <= s27_burstcount;
    m27_writedata      <= s27_writedata;
    m27_address        <= s27_address;
    m27_write          <= s27_write;
    m27_read           <= s27_read;
    m27_byteenable     <= s27_byteenable;
    m27_debugaccess    <= s27_debugaccess;
    s27_waitrequest    <= m27_waitrequest;
    s27_readdatavalid  <= m27_readdatavalid;
    s27_readdata       <= m27_readdata;

    m28_burstcount     <= s28_burstcount;
    m28_writedata      <= s28_writedata;
    m28_address        <= s28_address;
    m28_write          <= s28_write;
    m28_read           <= s28_read;
    m28_byteenable     <= s28_byteenable;
    m28_debugaccess    <= s28_debugaccess;
    s28_waitrequest    <= m28_waitrequest;
    s28_readdatavalid  <= m28_readdatavalid;
    s28_readdata       <= m28_readdata;

    m29_burstcount     <= s29_burstcount;
    m29_writedata      <= s29_writedata;
    m29_address        <= s29_address;
    m29_write          <= s29_write;
    m29_read           <= s29_read;
    m29_byteenable     <= s29_byteenable;
    m29_debugaccess    <= s29_debugaccess;
    s29_waitrequest    <= m29_waitrequest;
    s29_readdatavalid  <= m29_readdatavalid;
    s29_readdata       <= m29_readdata;

    m30_burstcount     <= s30_burstcount;
    m30_writedata      <= s30_writedata;
    m30_address        <= s30_address;
    m30_write          <= s30_write;
    m30_read           <= s30_read;
    m30_byteenable     <= s30_byteenable;
    m30_debugaccess    <= s30_debugaccess;
    s30_waitrequest    <= m30_waitrequest;
    s30_readdatavalid  <= m30_readdatavalid;
    s30_readdata       <= m30_readdata;

    m31_burstcount     <= s31_burstcount;
    m31_writedata      <= s31_writedata;
    m31_address        <= s31_address;
    m31_write          <= s31_write;
    m31_read           <= s31_read;
    m31_byteenable     <= s31_byteenable;
    m31_debugaccess    <= s31_debugaccess;
    s31_waitrequest    <= m31_waitrequest;
    s31_readdatavalid  <= m31_readdatavalid;
    s31_readdata       <= m31_readdata;

    m32_burstcount     <= s32_burstcount;
    m32_writedata      <= s32_writedata;
    m32_address        <= s32_address;
    m32_write          <= s32_write;
    m32_read           <= s32_read;
    m32_byteenable     <= s32_byteenable;
    m32_debugaccess    <= s32_debugaccess;
    s32_waitrequest    <= m32_waitrequest;
    s32_readdatavalid  <= m32_readdatavalid;
    s32_readdata       <= m32_readdata;

end architecture AM1;	
