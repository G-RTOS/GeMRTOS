--! \file GRTOS Controller 
--! \author Ricardo Cayssials
--! All rights reserved

LIBRARY ieee;
USE ieee.std_logic_1164.all; 

LIBRARY work;

ENTITY grtosproject IS 
	PORT
	(
		CLK_FPGA_50M    :  IN  STD_LOGIC;
		RAM_D0          :  INOUT  STD_LOGIC;
		RAM_D1          :  INOUT  STD_LOGIC;
		RAM_D2          :  INOUT  STD_LOGIC;
		RAM_D3          :  INOUT  STD_LOGIC;
		RAM_D4          :  INOUT  STD_LOGIC;
		RAM_D5          :  INOUT  STD_LOGIC;
		RAM_D6          :  INOUT  STD_LOGIC;
		RAM_D7          :  INOUT  STD_LOGIC;
		RAM_D8          :  INOUT  STD_LOGIC;
		RAM_D9          :  INOUT  STD_LOGIC;
		RAM_D10         :  INOUT  STD_LOGIC;
		RAM_D11         :  INOUT  STD_LOGIC;
		RAM_D12         :  INOUT  STD_LOGIC;
		RAM_D13         :  INOUT  STD_LOGIC;
		RAM_D14         :  INOUT  STD_LOGIC;
		RAM_D15         :  INOUT  STD_LOGIC;
		RAM_LDQS        :  INOUT  STD_LOGIC;
		RAM_UDQS        :  INOUT  STD_LOGIC;
		RAM_A0          :  OUT  STD_LOGIC;
		RAM_A1          :  OUT  STD_LOGIC;
		RAM_A2          :  OUT  STD_LOGIC;
		RAM_A3          :  OUT  STD_LOGIC;
		RAM_A4          :  OUT  STD_LOGIC;
		RAM_A5          :  OUT  STD_LOGIC;
		RAM_A6          :  OUT  STD_LOGIC;
		RAM_A7          :  OUT  STD_LOGIC;
		RAM_A8          :  OUT  STD_LOGIC;
		RAM_A9          :  OUT  STD_LOGIC;
		RAM_A10         :  OUT  STD_LOGIC;
		RAM_A11         :  OUT  STD_LOGIC;
		RAM_A12         :  OUT  STD_LOGIC;
		RAM_A13         :  OUT  STD_LOGIC;
		RAM_BA0         :  OUT  STD_LOGIC;
		RAM_BA1         :  OUT  STD_LOGIC;
		RAM_CK_N        :  OUT  STD_LOGIC;
		RAM_CK_P        :  OUT  STD_LOGIC;
		RAM_CKE         :  OUT  STD_LOGIC;
		RAM_CS_N        :  OUT  STD_LOGIC;
		RAM_WS_N        :  OUT  STD_LOGIC;
		RAM_RAS_N       :  OUT  STD_LOGIC;
		RAM_CAS_N       :  OUT  STD_LOGIC;
		RAM_LDM         :  OUT  STD_LOGIC;
		RAM_UDM         :  OUT  STD_LOGIC;
		RXD_0           :  IN STD_LOGIC;
		RXD_1           :  IN STD_LOGIC;
		RXD_2           :  IN STD_LOGIC;
		RXD_3           :  IN STD_LOGIC;
		RX_CLK          :  IN STD_LOGIC;
		RX_DV           :  IN STD_LOGIC;
		RX_ER           :  IN STD_LOGIC;
		TXD_0           :  OUT STD_LOGIC;
		TXD_1           :  OUT STD_LOGIC;
		TXD_2           :  OUT STD_LOGIC;
		TXD_3           :  OUT STD_LOGIC;
		TX_EN           :  OUT STD_LOGIC;
		TX_CLK          :  IN STD_LOGIC;
		ETH_COL         :  IN STD_LOGIC;
		ETH_CRS         :  IN STD_LOGIC;
		ETH_RESET_N     :  OUT STD_LOGIC;
		MDC             :  OUT STD_LOGIC;
		MDIO            :  INOUT STD_LOGIC;
		TX_ER           :  OUT STD_LOGIC;
		F_LED0          :  OUT std_logic;   
		F_LED1          :  OUT std_logic;   
		F_LED2          :  OUT std_logic;   
		F_LED3          :  OUT std_logic;   
		F_LED4          :  OUT std_logic;   
		F_LED5          :  OUT std_logic;   
		F_LED6          :  OUT std_logic;  
		F_LED7          :  OUT std_logic;  
		PBSW_N          :  IN STD_LOGIC
	);
END grtosproject;

ARCHITECTURE bdf_type OF grtosproject IS 

COMPONENT qsysgrtos
	PORT(clk_clk : IN STD_LOGIC;
        reset_reset_n : IN    STD_LOGIC;
        ram_D0 : INOUT STD_LOGIC;
        ram_D1 : INOUT STD_LOGIC;
        ram_D2 : INOUT STD_LOGIC;
        ram_D3 : INOUT STD_LOGIC;
        ram_D4 : INOUT STD_LOGIC;
        ram_D5 : INOUT STD_LOGIC;
        ram_D6 : INOUT STD_LOGIC;
        ram_D7 : INOUT STD_LOGIC;
        ram_D8 : INOUT STD_LOGIC;
        ram_D9 : INOUT STD_LOGIC;
        ram_D10 : INOUT STD_LOGIC;
        ram_D11 : INOUT STD_LOGIC;
        ram_D12 : INOUT STD_LOGIC;
        ram_D13 : INOUT STD_LOGIC;
        ram_D14 : INOUT STD_LOGIC;
        ram_D15 : INOUT STD_LOGIC;
        ram_LDQS : INOUT STD_LOGIC;
        ram_UDQS : INOUT STD_LOGIC;
        ram_A0 : OUT STD_LOGIC;
        ram_A1 : OUT STD_LOGIC;
        ram_A2 : OUT STD_LOGIC;
        ram_A3 : OUT STD_LOGIC;
        ram_A4 : OUT STD_LOGIC;
        ram_A5 : OUT STD_LOGIC;
        ram_A6 : OUT STD_LOGIC;
        ram_A7 : OUT STD_LOGIC;
        ram_A8 : OUT STD_LOGIC;
        ram_A9 : OUT STD_LOGIC;
        ram_A10 : OUT STD_LOGIC;
        ram_A11 : OUT STD_LOGIC;
        ram_A12 : OUT STD_LOGIC;
        ram_A13 : OUT STD_LOGIC;
        ram_BA0 : OUT STD_LOGIC;
        ram_BA1 : OUT STD_LOGIC;
        ram_CK_N : OUT STD_LOGIC;
        ram_CK_P : OUT STD_LOGIC;
        ram_CKE : OUT STD_LOGIC;
        ram_CS_N : OUT STD_LOGIC;
        ram_WS_N : OUT STD_LOGIC;
        ram_RAS_N : OUT STD_LOGIC;
        ram_CAS_N : OUT STD_LOGIC;
        ram_LDM : OUT STD_LOGIC;
        ram_UDM : OUT STD_LOGIC;
        reset_grtos_reset_n : OUT std_logic;
        gemrtos_phy_GEMRTOS_OUTS : out std_logic_vector(31 downto 0);
        gemrtos_phy_GEMRTOS_INS  : in  std_logic_vector(31 downto 0)     
	);
END COMPONENT;


 signal RXD_I : std_logic_vector(3 downto 0);
 signal TXD_I : std_logic_vector(3 downto 0);
 signal PORT_LEDS : std_logic_vector(31 downto 0);
 signal PORT_SWITCHES : std_logic_vector(31 downto 0); 
 signal reset_internal : std_logic;

BEGIN 
  
    RXD_I <= RXD_3 & RXD_2 & RXD_1 & RXD_0;
    TXD_0 <= TXD_I(0);
    TXD_1 <= TXD_I(1);
    TXD_2 <= TXD_I(2);
    TXD_3 <= TXD_I(3);

    -- F_LED2 <= '1';                               -- !!!!D
    -- F_LED1 <= PBSW_N;                            -- !!!!D
    -- F_LED0 <= reset_internal;                     -- !!!!D

F_LED0 <= PORT_LEDS(0);
F_LED1 <= PORT_LEDS(1);
F_LED2 <= PORT_LEDS(2);
F_LED3 <= PORT_LEDS(3);
F_LED4 <= PORT_LEDS(4);
F_LED5 <= PORT_LEDS(5);
F_LED6 <= PORT_LEDS(6);
F_LED7 <= PORT_LEDS(7);


b2v_inst : qsysgrtos
PORT MAP(clk_clk     => CLK_FPGA_50M,
        reset_reset_n => PBSW_N and reset_internal,
        ram_D0 => RAM_D0,
        ram_D1 => RAM_D1,
        ram_D2 => RAM_D2,
        ram_D3 => RAM_D3,
        ram_D4 => RAM_D4,
        ram_D5 => RAM_D5,
        ram_D6 => RAM_D6,
        ram_D7 => RAM_D7,
        ram_D8 => RAM_D8,
        ram_D9 => RAM_D9,
        ram_D10 => RAM_D10,
        ram_D11 => RAM_D11,
        ram_D12 => RAM_D12,
        ram_D13 => RAM_D13,
        ram_D14 => RAM_D14,
        ram_D15 => RAM_D15,
        ram_LDQS => RAM_LDQS,
        ram_UDQS => RAM_UDQS,
        ram_A0 => RAM_A0,
        ram_A1 => RAM_A1,
        ram_A2 => RAM_A2,
        ram_A3 => RAM_A3,
        ram_A4 => RAM_A4,
        ram_A5 => RAM_A5,
        ram_A6 => RAM_A6,
        ram_A7 => RAM_A7,
        ram_A8 => RAM_A8,
        ram_A9 => RAM_A9,
        ram_A10 => RAM_A10,
        ram_A11 => RAM_A11,
        ram_A12 => RAM_A12,
        ram_A13 => RAM_A13,
        ram_BA0 => RAM_BA0,
        ram_BA1 => RAM_BA1,
        ram_CK_N => RAM_CK_N,
        ram_CK_P => RAM_CK_P,
        ram_CKE => RAM_CKE,
        ram_CS_N => RAM_CS_N,
        ram_WS_N => RAM_WS_N,
        ram_RAS_N => RAM_RAS_N,
        ram_CAS_N => RAM_CAS_N,
        ram_LDM => RAM_LDM,
        ram_UDM => RAM_UDM,
        reset_grtos_reset_n => reset_internal,
		gemrtos_phy_GEMRTOS_OUTS => PORT_LEDS,
        gemrtos_phy_GEMRTOS_INS  => PORT_SWITCHES
       
    );


END bdf_type;