/**
 * \file lwip_main.h
 *
 * \brief This is the main interface API to initialize the LwIP stack.
 *
 * This is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License (version 2) as published by the
 * Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 * >>>NOTE<<< The modification to the GPL is included to allow you to
 * distribute a combined work that includes FreeRTOS without being obliged to
 * provide the source code for proprietary components outside of the FreeRTOS
 * kernel.  FreeRTOS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details. You should have received a copy of the GNU General Public
 * License and the FreeRTOS license exception along with FreeRTOS; if not it
 * can be viewed here: http://www.freertos.org/a00114.html and also obtained
 * by writing to Richard Barry, contact details for whom are available on the
 * FreeRTOS WEB site.
 *
 * Created by Engineering Spirit (c) 2012 http://engineering-spirit.nl/
 */

#ifndef __LWIP_MAIN_H_
#define __LWIP_MAIN_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <altera_msgdma.h>
#include <gemrtos_eth_tse_regs.h>


// include LwIP headers
#include <arch/cc.h>
#include <lwip/opt.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/dhcp.h>
#include <lwip/tcp.h>
#include <lwip/stats.h>
#include <lwip/sys.h>
#include <lwip/ip_addr.h>
#include <lwip/tcpip.h>
#include <lwip/sockets.h>
/* 2026-09-17: these two were the only backslash-separated #include paths in the
 * entire IP tree -- the other eight includes in this same file already used
 * '/'. A backslash is not a path separator to a conforming preprocessor:
 * Windows GCC tolerates it, Linux GCC treats it as part of the literal file
 * name and the include fails outright. Found by a parallel session building the
 * Atum A3 BSP on Linux for the first time; the defect had been invisible for as
 * long as every build was a Windows build. */
#include <lwip/timeouts.h>

#include <lwip/apps/httpd.h>

#include <lwip/snmp.h>

#include <netif/etharp.h>

#include <stdbool.h>

#include "gemrtos.h"


#define ETH_INTERFACE_UP			1
#define ETH_INTERFACE_DOWN			0


#define dprintf(x) do { /* printf x */ } while (0)



/**
 * \brief set status callback functions when network stats change.
 *
 * \return a pointer to the previous callback function
 */
extern netif_status_callback_fn lwip_set_status_callback(netif_status_callback_fn callback);
extern netif_status_callback_fn lwip_set_link_callback(netif_status_callback_fn callback);
err_t tse_mac_raw_send(struct netif *pnetif, struct pbuf *pkt);
void netif_status_callback(struct netif *pnetif);
/** \brief Forwards a link-state change to the callback registered via
 *         lwip_set_link_callback(), if any. An application must wire this in with
 *         netif_set_link_callback(pnetif, netif_link_callback_dispatch) -- lwIP core
 *         does not call it on its own. See the definition in lwip_main.c for why this
 *         exists (a wiring gap: lwip_set_link_callback() stored the
 *         callback but nothing ever read it). */
void netif_link_callback_dispatch(struct netif *pnetif);
int tse_mac_init(struct netif *netif);

/**
 * \brief Outputs a network byte orderd address to a static charachter array.
 *
 * \return the pointer to the statically allocated buffer.
 */
extern const char* print_ipad(alt_u32 ip, char* buf);




// Organizatioally Unique Identifier for PHY interface
typedef enum  {
	OUI_MARVELL = 0x5043,           // MARVELL
    OUI_TEXAS_INSTRUMENT = 0x80028 // Texas Instruments
} GE_OUI_lwip;

// Model Number for PHY interface
typedef enum  {
	ModelNumber_88E1111 = 0xC,  // MARVELL
    ModelNumber_DP83867 = 0x23, // Texas Instruments
} GE_ModelNumber_lwip;


// Organizatioally Unique Identifier for PHY interface
typedef enum  {
	GE_NOT_SUPPORTED,
	GE_SUPPORTED_PHY_MARVELL_88E1111, // MARVELL
    GE_SUPPORTED_PHY_TI_DP83867       // Texas Instruments
} GE_SUPPORTED_PHY;

// PHY link speed
typedef enum  {
	GE_PHY_NO_SPEED,
	GE_PHY_SPEED_10,
	GE_PHY_SPEED_100,
	GE_PHY_SPEED_1000
} GE_PHY_SPEED;

// PHY link duplex
typedef enum  {
	GE_PHY_NO_DUPLEX,
	GE_PHY_DUPLEX_HALF,
	GE_PHY_DUPLEX_FULL
} GE_PHY_DUPLEX;


/* System Parameters for TSE System */
typedef struct gemrtos_tse_system_info_struct {
    _Alignas(4) G_UINT32 tse_mac_base;                   /* Base address of TSE MAC                               */
    _Alignas(4) G_UINT32 tse_tx_depth;                   /* TX Receive FIFO depth                                 */
    _Alignas(4) G_UINT32 tse_rx_depth;                   /* RX Receive FIFO depth                                 */
    _Alignas(4) G_UINT32 tse_use_mdio;                   /* is MDIO enabled                                       */
    _Alignas(4) G_UINT32 tse_en_maclite;                 /* is Small MAC                                          */
    _Alignas(4) G_UINT32 tse_maclite_gige;               /* is Small MAC 1000 Mbps                                */
    _Alignas(4) G_UINT32 tse_multichannel_mac;           /* MAC group together for MDIO block sharing             */
    _Alignas(4) G_UINT32 tse_num_of_channel;             /* Number of channel for Multi-channel MAC               */
    _Alignas(4) G_UINT32 tse_mdio_shared;                /* is MDIO block shared                                  */
    _Alignas(4) G_UINT32 tse_number_of_mac_mdio_shared;  /* Number of MAC sharing the MDIO block                  */
    _Alignas(4) G_UINT32 tse_pcs_ena;                    /* is MAC+PCS combination                                */
    _Alignas(4) G_UINT32 tse_pcs_sgmii;                  /* is SGMII mode of PCS enabled                          */

	_Alignas(4) alt_msgdma_dev * sgdma_tx_dev;           /* sgdma_tx_dev  CSR                                     */
	_Alignas(4) alt_msgdma_dev * sgdma_rx_dev;           /* sgdma_rx_dev  CSR                                     */
	_Alignas(4) char *		  tse_msgdma_tx_name;        /* MSGDMA TX CSR NAME                                    */
    _Alignas(4) void *        tse_msgdma_tx_csr_base;    /* MSGDMA TX CSR BASE                                    */
    _Alignas(4) G_UINT32      tse_msgdma_tx_irq;         /* MSGDMA TX CSR IRQ                                     */
	_Alignas(4) char *		  tse_msgdma_rx_name;        /* MSGDMA RX CSR NAME                                    */	
    _Alignas(4) void *        tse_msgdma_rx_csr_base;    /* MSGDMA RX CSR BASE                                    */
    _Alignas(4) G_UINT32      tse_msgdma_rx_irq;         /* MSGDMA TX CSR IRQ                                     */
    
    _Alignas(4) void *tse_msgdma_tx_descriptor_slave_base;  /* MSGDMA TX DESCRIPTOR SLAVE BASE                       */
    _Alignas(4) void *tse_msgdma_rx_descriptor_slave_base;  /* MSGDMA RX DESCRIPTOR SLAVE BASE                       */
    
    _Alignas(4) void *tse_msgdma_rx_response_base;          /* MSGDMA TX RESPONSE BASE                               */
    
    // _Alignas(4) alt_u8        ext_desc_mem;             /* is dedicated memory used for descriptor               */
    // G_UINT32        desc_mem_base;                   /* Base address of Descriptor Memory if ext_desc_mem = 1 */
      
    _Alignas(4) G_UINT32        tse_phy_mdio_address;   /* PHY's MDIO address                                    */
	_Alignas(4) GE_SUPPORTED_PHY phy_model;             /* PHY model                                             */
	_Alignas(4) char name[4];                           /* netif name (used 2 but defined 4 for alignment        */
	_Alignas(4) unsigned int phy_tse_mdio_register_space; /* TSE register address to access phy register 0x80/0xA0 */
	_Alignas(4) unsigned int phy_delay;                 /* 0 no delay in phy, 1 delay in phy                     */
	_Alignas(4) GE_PHY_SPEED phy_speed;                 /* PHY speed                                             */
	_Alignas(4) GE_PHY_DUPLEX phy_duplex;               /* PHY duplex                                            */
    _Alignas(4) G_UINT32 link_alive;                    /* G_TRUE or G_FALSE                                     */
	_Alignas(4) GS_RCB *sgdma_trigger_transmit;         /* Trigger for SGDMA transmit desciptor                  */
	_Alignas(4) GS_RCB *sgdma_trigger_receive;	        /* Trigger for SGDMA receive desciptor                   */
    _Alignas(4) struct netif *pnetif;                   /* pointer to the netif structure                        */
    _Alignas(4) GS_RCB  *p_tse_queue_out;               /* Output Message Queue for transmit                     */
    _Alignas(4) unsigned char * tx_frame;               /* pointer to buffer to transmit (not DMA buffer)        */
    _Alignas(4) unsigned char *buffer_tx_frame;         /* pointer to DMA buffer in on-chip memory to msgDMA     */
    _Alignas(4) unsigned char *buffer_rx_frame;         /* pointer to DMA buffer in on-chip memory to msgDMA     */    
} gemrtos_tse_system_info __attribute__((aligned(4)));

    // TSE_EXT_DESC_MEM,                                /* ext_desc_mem                         */ 
    
/* Define whole TSE system (dedicated descriptor memory, no shared fifo) */
#define GEMRTOS_TSE_SYSTEM(netif_name, tse_name, phy_delay, msgdma_tx_name, msgdma_rx_name, phy_address) {  \
    tse_name##_BASE,                                 /* tse_mac_base                         */ \
    tse_name##_TRANSMIT_FIFO_DEPTH,                  /* tse_tx_depth                         */ \
    tse_name##_RECEIVE_FIFO_DEPTH,                   /* tse_rx_depth                         */ \
    tse_name##_USE_MDIO,                             /* tse_use_mdio                         */ \
    tse_name##_ENABLE_MACLITE,                       /* tse_en_maclite                       */ \
    tse_name##_MACLITE_GIGE,                         /* tse_maclite_gige                     */ \
    tse_name##_IS_MULTICHANNEL_MAC,                  /* tse_multichannel_mac                 */ \
    tse_name##_NUMBER_OF_CHANNEL,                    /* tse_num_of_channel                   */ \
    tse_name##_MDIO_SHARED,                          /* tse_mdio_shared                      */ \
    tse_name##_NUMBER_OF_MAC_MDIO_SHARED,            /* tse_number_of_mac_mdio_shared        */ \
    tse_name##_PCS,                                  /* tse_pcs_ena                          */ \
    tse_name##_PCS_SGMII,                            /* tse_pcs_sgmii                        */ \
	(alt_msgdma_dev *) 0,                            /* sgdma_tx_dev                         */ \
	(alt_msgdma_dev *) 0,                            /* sgdma_rx_dev                         */ \
    msgdma_tx_name##_CSR_NAME,                       /* tse_msgdma_tx_name                   */ \
    (void *) msgdma_tx_name##_CSR_BASE,              /* tse_msgdma_tx_csr_base               */ \
    (G_UINT32) msgdma_tx_name##_CSR_IRQ,             /* tse_msgdma_tx_irq                    */ \
    msgdma_rx_name##_CSR_NAME,                       /* tse_msgdma_rx_name                   */ \
    (void *) msgdma_rx_name##_CSR_BASE,              /* tse_msgdma_rx_csr_base               */ \
    (G_UINT32) msgdma_rx_name##_CSR_IRQ,             /* tse_msgdma_rx_irq                    */ \
    (void *) msgdma_tx_name##_DESCRIPTOR_SLAVE_BASE, /* tse_msgdma_tx_descriptor_slave_base  */ \
    (void *) msgdma_rx_name##_DESCRIPTOR_SLAVE_BASE, /* tse_msgdma_rx_descriptor_slave_base  */ \
    (void *) msgdma_rx_name##_RESPONSE_BASE,         /* tse_msgdma_rx_response_base          */ \
	phy_address,                                     /* tse_phy_mdio_address                 */ \
	GE_NOT_SUPPORTED,                                /* PHY model                            */ \
	{(char) netif_name[0],(char) netif_name[1]},     /* netid name                           */ \
	/* tse_mdio_space,                                mdio space = 0 or 1                  */ \
	0,                                               /* phy_tse_mdio_register_space          */ \
	phy_delay,                                       /* 0 no delay, 1 delay in phy           */ \
	GE_PHY_NO_SPEED,                                 /* PHY speed                            */ \
	GE_PHY_NO_DUPLEX,                                /* PHY duplex                           */ \
	G_FALSE,                                         /* link_alive                           */ \
	(GS_RCB *) 0,                                    /* sgdma_trigger_transmit               */ \
	(GS_RCB *) 0,                                    /* sgdma_trigger_receive                */ \
    (struct netif *) 0,                              /* pnetif                               */ \
    (GS_RCB  *) 0,                                   /* p_tse_queue_out                      */ \
    (unsigned char *) 0,                             /* tx_frame                             */ \
    (unsigned char *) 0,                             /* pointer to DMA buffer in on-chip memory to msgDMA */ \
    (unsigned char *) 0                              /* pointer to DMA buffer in on-chip memory to msgDMA */ \
}


//    (unsigned char *) 0,                             /* pointer to DMA buffer in on-chip memory to msgDMA */ 
//    (unsigned char *) 0                              /* pointer to DMA buffer in on-chip memory to msgDMA */ 


extern gemrtos_tse_system_info gemrtos_tse_mac_device[];

int netif_ip_is_up(struct netif *netif);
err_t netif_tse_init(struct netif *pnetif);
gemrtos_tse_system_info *gemrtos_GetTSEInfo (gemrtos_tse_system_info *tse_info);
G_UINT32 mdio_read_register(void *tse_base, unsigned int phy_addr, unsigned int reg_addr);
G_UINT32 mdio_write_register(void *tse_base, unsigned int phy_addr, unsigned int reg_addr, unsigned int value);
G_UINT32 gemrtos_UpdateTSEInfo (struct netif *netif);
G_UINT32 gemrtos_InitialiseTSE (struct netif *netif);
int lwip_is_interface_up(struct netif *netif);

void low_level_init(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif // __LWIP_MAIN_H_
