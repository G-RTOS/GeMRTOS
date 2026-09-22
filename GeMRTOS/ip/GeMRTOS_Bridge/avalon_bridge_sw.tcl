##
## \file
## \brief GeMRTOS Bridge software-package descriptor (_sw.tcl) for the lwIP TCP/IP stack
## \details Registers the lwIP 2.2.0 sources and headers under HAL/ and the GeMRTOS port layer
##          (arch/lwip_main.c, inc/lwip_main.h, ...) as the BSP software package named "lwip".
## \version 1.0
## \date 2026-09-19
##
## SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
## Copyright (c) 2013-2026 GeMRTOS Project Contributors
##
## Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
##
## This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
## License 1.0.0. You may use, copy, modify, and distribute this file for
## any noncommercial purpose. Commercial use requires a separate written
## license from GeMRTOS. Full terms:
## https://polyformproject.org/licenses/noncommercial/1.0.0
##

# Create a new software package named "lwip"
create_sw_package lwip

# Version of this package descriptor (the lwIP stack itself is 2.2.0: HAL/inc/lwip/init.h)
set_sw_property version 0.9

# (Don't) initialize the driver in alt_sys_init()
set_sw_property auto_initialize false

# Location in generated BSP that above sources will be copied into
set_sw_property bsp_subdirectory HAL

# Include paths
add_sw_property include_directory /
add_sw_property include_directory HAL/inc
add_sw_property include_directory HAL/inc/ipv4
add_sw_property include_directory HAL/apps/ping


# set up module instance callbacks
# set_sw_property callback_source_file callbacks.tcl
# set_sw_property initialization_callback initialize
# set_sw_property generation_callback generate
# set_sw_property validation_callback validate

#
# Source file listings...
#


# C/C++ source files

add_sw_property c_source HAL/arch/lwip_main.c
# add_sw_property c_source HAL/arch/sys_arch.c
# add_sw_property c_source HAL/arch/altera_tse_ethernetif.c

add_sw_property c_source HAL/api/api_lib.c
add_sw_property c_source HAL/api/api_msg.c
add_sw_property c_source HAL/api/err.c
add_sw_property c_source HAL/api/if_api.c
add_sw_property c_source HAL/api/netbuf.c
add_sw_property c_source HAL/api/netdb.c
add_sw_property c_source HAL/api/netifapi.c
add_sw_property c_source HAL/api/sockets.c
add_sw_property c_source HAL/api/tcpip.c
add_sw_property c_source HAL/apps/altcp_tls/altcp_tls_mbedtls_mem.c
add_sw_property c_source HAL/apps/http/altcp_proxyconnect.c
add_sw_property c_source HAL/apps/http/fs.c
add_sw_property c_source HAL/apps/http/fsdata.c
add_sw_property c_source HAL/apps/http/httpd.c
add_sw_property c_source HAL/apps/http/http_client.c
# lwIP 2.2.0 contrib ping application (BSD-3, byte-identical to upstream; replaces the
# unregistered Engineering Spirit ping.h, 2026-09-19). Raw-API build: NO_SYS=1.
add_sw_property c_source HAL/apps/ping/ping.c


# add_sw_property c_source HAL/apps/http/makefsdata/makefsdata.c

add_sw_property c_source HAL/core/altcp.c
add_sw_property c_source HAL/core/altcp_alloc.c
add_sw_property c_source HAL/core/altcp_tcp.c
add_sw_property c_source HAL/core/def.c
add_sw_property c_source HAL/core/dns.c
add_sw_property c_source HAL/core/inet_chksum.c
add_sw_property c_source HAL/core/init.c
add_sw_property c_source HAL/core/ip.c
add_sw_property c_source HAL/core/mem.c
add_sw_property c_source HAL/core/memp.c
add_sw_property c_source HAL/core/netif.c
add_sw_property c_source HAL/core/pbuf.c
add_sw_property c_source HAL/core/raw.c
add_sw_property c_source HAL/core/stats.c
add_sw_property c_source HAL/core/sys.c
add_sw_property c_source HAL/core/tcp.c
add_sw_property c_source HAL/core/tcp_in.c
add_sw_property c_source HAL/core/tcp_out.c
add_sw_property c_source HAL/core/timeouts.c
add_sw_property c_source HAL/core/udp.c
add_sw_property c_source HAL/core/ipv4/acd.c
add_sw_property c_source HAL/core/ipv4/autoip.c
add_sw_property c_source HAL/core/ipv4/dhcp.c
add_sw_property c_source HAL/core/ipv4/etharp.c
add_sw_property c_source HAL/core/ipv4/icmp.c
add_sw_property c_source HAL/core/ipv4/igmp.c
add_sw_property c_source HAL/core/ipv4/ip4.c
add_sw_property c_source HAL/core/ipv4/ip4_addr.c
add_sw_property c_source HAL/core/ipv4/ip4_frag.c
add_sw_property c_source HAL/core/ipv6/dhcp6.c
add_sw_property c_source HAL/core/ipv6/ethip6.c
add_sw_property c_source HAL/core/ipv6/icmp6.c
add_sw_property c_source HAL/core/ipv6/inet6.c
add_sw_property c_source HAL/core/ipv6/ip6.c
add_sw_property c_source HAL/core/ipv6/ip6_addr.c
add_sw_property c_source HAL/core/ipv6/ip6_frag.c
add_sw_property c_source HAL/core/ipv6/mld6.c
add_sw_property c_source HAL/core/ipv6/nd6.c

add_sw_property c_source HAL/netif/bridgeif.c
add_sw_property c_source HAL/netif/bridgeif_fdb.c
add_sw_property c_source HAL/netif/ethernet.c
add_sw_property c_source HAL/netif/lowpan6.c
add_sw_property c_source HAL/netif/lowpan6_ble.c
add_sw_property c_source HAL/netif/lowpan6_common.c
add_sw_property c_source HAL/netif/slipif.c
add_sw_property c_source HAL/netif/zepif.c
add_sw_property c_source HAL/netif/ppp/auth.c
add_sw_property c_source HAL/netif/ppp/ccp.c
add_sw_property c_source HAL/netif/ppp/chap-md5.c
add_sw_property c_source HAL/netif/ppp/chap-new.c
add_sw_property c_source HAL/netif/ppp/chap_ms.c
add_sw_property c_source HAL/netif/ppp/demand.c
add_sw_property c_source HAL/netif/ppp/eap.c
add_sw_property c_source HAL/netif/ppp/ecp.c
add_sw_property c_source HAL/netif/ppp/eui64.c
add_sw_property c_source HAL/netif/ppp/fsm.c
add_sw_property c_source HAL/netif/ppp/ipcp.c
add_sw_property c_source HAL/netif/ppp/ipv6cp.c
add_sw_property c_source HAL/netif/ppp/lcp.c
add_sw_property c_source HAL/netif/ppp/magic.c
add_sw_property c_source HAL/netif/ppp/mppe.c
add_sw_property c_source HAL/netif/ppp/multilink.c
add_sw_property c_source HAL/netif/ppp/ppp.c
add_sw_property c_source HAL/netif/ppp/pppapi.c
add_sw_property c_source HAL/netif/ppp/pppcrypt.c
add_sw_property c_source HAL/netif/ppp/pppoe.c
add_sw_property c_source HAL/netif/ppp/pppol2tp.c
add_sw_property c_source HAL/netif/ppp/pppos.c
add_sw_property c_source HAL/netif/ppp/upap.c
add_sw_property c_source HAL/netif/ppp/utils.c
add_sw_property c_source HAL/netif/ppp/vj.c
add_sw_property c_source HAL/netif/ppp/polarssl/arc4.c
add_sw_property c_source HAL/netif/ppp/polarssl/des.c
add_sw_property c_source HAL/netif/ppp/polarssl/md4.c
add_sw_property c_source HAL/netif/ppp/polarssl/md5.c
add_sw_property c_source HAL/netif/ppp/polarssl/sha1.c


# Include files
add_sw_property include_source HAL/apps/altcp_tls/altcp_tls_mbedtls_mem.h
add_sw_property include_source HAL/apps/http/httpd_structs.h
add_sw_property include_source HAL/apps/http/makefsdata/tinydir.h
add_sw_property include_source HAL/apps/ping/ping.h


add_sw_property include_source HAL/inc/lwip_main.h
add_sw_property include_source HAL/inc/lwipopts.h
add_sw_property include_source HAL/inc/gemrtos_avalon_tse.h
add_sw_property include_source HAL/inc/gemrtos_avalon_tse_system_info.h
add_sw_property include_source HAL/inc/gemrtos_eth_tse.h
add_sw_property include_source HAL/inc/gemrtos_eth_tse_regs.h


add_sw_property include_source HAL/inc/arch/bpstruct.h
add_sw_property include_source HAL/inc/arch/cc.h
add_sw_property include_source HAL/inc/arch/epstruct.h
add_sw_property include_source HAL/inc/arch/perf.h
add_sw_property include_source HAL/inc/arch/sys_arch.h
add_sw_property include_source HAL/inc/compat/posix/netdb.h
add_sw_property include_source HAL/inc/compat/posix/arpa/inet.h
add_sw_property include_source HAL/inc/compat/posix/net/if.h
add_sw_property include_source HAL/inc/compat/posix/sys/socket.h
add_sw_property include_source HAL/inc/compat/stdc/errno.h
add_sw_property include_source HAL/inc/lwip/acd.h
add_sw_property include_source HAL/inc/lwip/altcp.h
add_sw_property include_source HAL/inc/lwip/altcp_tcp.h
add_sw_property include_source HAL/inc/lwip/altcp_tls.h
add_sw_property include_source HAL/inc/lwip/api.h
add_sw_property include_source HAL/inc/lwip/arch.h
add_sw_property include_source HAL/inc/lwip/autoip.h
add_sw_property include_source HAL/inc/lwip/debug.h
add_sw_property include_source HAL/inc/lwip/def.h
add_sw_property include_source HAL/inc/lwip/dhcp.h
add_sw_property include_source HAL/inc/lwip/dhcp6.h
add_sw_property include_source HAL/inc/lwip/dns.h
add_sw_property include_source HAL/inc/lwip/err.h
add_sw_property include_source HAL/inc/lwip/errno.h
add_sw_property include_source HAL/inc/lwip/etharp.h
add_sw_property include_source HAL/inc/lwip/ethip6.h
add_sw_property include_source HAL/inc/lwip/icmp.h
add_sw_property include_source HAL/inc/lwip/icmp6.h
add_sw_property include_source HAL/inc/lwip/if_api.h
add_sw_property include_source HAL/inc/lwip/igmp.h
add_sw_property include_source HAL/inc/lwip/inet.h
add_sw_property include_source HAL/inc/lwip/inet_chksum.h
add_sw_property include_source HAL/inc/lwip/init.h
add_sw_property include_source HAL/inc/lwip/ip.h
add_sw_property include_source HAL/inc/lwip/ip4.h
add_sw_property include_source HAL/inc/lwip/ip4_addr.h
add_sw_property include_source HAL/inc/lwip/ip4_frag.h
add_sw_property include_source HAL/inc/lwip/ip6.h
add_sw_property include_source HAL/inc/lwip/ip6_addr.h
add_sw_property include_source HAL/inc/lwip/ip6_frag.h
add_sw_property include_source HAL/inc/lwip/ip6_zone.h
add_sw_property include_source HAL/inc/lwip/ip_addr.h
add_sw_property include_source HAL/inc/lwip/mem.h
add_sw_property include_source HAL/inc/lwip/memp.h
add_sw_property include_source HAL/inc/lwip/mld6.h
add_sw_property include_source HAL/inc/lwip/nd6.h
add_sw_property include_source HAL/inc/lwip/netbuf.h
add_sw_property include_source HAL/inc/lwip/netdb.h
add_sw_property include_source HAL/inc/lwip/netif.h
add_sw_property include_source HAL/inc/lwip/netifapi.h
add_sw_property include_source HAL/inc/lwip/opt.h
add_sw_property include_source HAL/inc/lwip/pbuf.h
add_sw_property include_source HAL/inc/lwip/raw.h
add_sw_property include_source HAL/inc/lwip/sio.h
add_sw_property include_source HAL/inc/lwip/snmp.h
add_sw_property include_source HAL/inc/lwip/sockets.h
add_sw_property include_source HAL/inc/lwip/stats.h
add_sw_property include_source HAL/inc/lwip/sys.h
add_sw_property include_source HAL/inc/lwip/tcp.h
add_sw_property include_source HAL/inc/lwip/tcpbase.h
add_sw_property include_source HAL/inc/lwip/tcpip.h
add_sw_property include_source HAL/inc/lwip/timeouts.h
add_sw_property include_source HAL/inc/lwip/udp.h
add_sw_property include_source HAL/inc/lwip/apps/altcp_proxyconnect.h
add_sw_property include_source HAL/inc/lwip/apps/altcp_tls_mbedtls_opts.h
add_sw_property include_source HAL/inc/lwip/apps/fs.h
add_sw_property include_source HAL/inc/lwip/apps/httpd.h
add_sw_property include_source HAL/inc/lwip/apps/httpd_opts.h
add_sw_property include_source HAL/inc/lwip/apps/http_client.h
add_sw_property include_source HAL/inc/lwip/apps/lwiperf.h
add_sw_property include_source HAL/inc/lwip/apps/mdns.h
add_sw_property include_source HAL/inc/lwip/apps/mdns_domain.h
add_sw_property include_source HAL/inc/lwip/apps/mdns_opts.h
add_sw_property include_source HAL/inc/lwip/apps/mdns_out.h
add_sw_property include_source HAL/inc/lwip/apps/mdns_priv.h
add_sw_property include_source HAL/inc/lwip/apps/mqtt.h
add_sw_property include_source HAL/inc/lwip/apps/mqtt_opts.h
add_sw_property include_source HAL/inc/lwip/apps/mqtt_priv.h
add_sw_property include_source HAL/inc/lwip/apps/netbiosns.h
add_sw_property include_source HAL/inc/lwip/apps/netbiosns_opts.h
add_sw_property include_source HAL/inc/lwip/apps/smtp.h
add_sw_property include_source HAL/inc/lwip/apps/smtp_opts.h
# add_sw_property include_source HAL/inc/lwip/apps/snmp.h
add_sw_property include_source HAL/inc/lwip/apps/snmpv3.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_core.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_mib2.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_opts.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_scalar.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_snmpv2_framework.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_snmpv2_usm.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_table.h
add_sw_property include_source HAL/inc/lwip/apps/snmp_threadsync.h
add_sw_property include_source HAL/inc/lwip/apps/sntp.h
add_sw_property include_source HAL/inc/lwip/apps/sntp_opts.h
add_sw_property include_source HAL/inc/lwip/apps/tftp_client.h
add_sw_property include_source HAL/inc/lwip/apps/tftp_common.h
add_sw_property include_source HAL/inc/lwip/apps/tftp_opts.h
add_sw_property include_source HAL/inc/lwip/apps/tftp_server.h
add_sw_property include_source HAL/inc/lwip/priv/altcp_priv.h
add_sw_property include_source HAL/inc/lwip/priv/api_msg.h
add_sw_property include_source HAL/inc/lwip/priv/memp_priv.h
add_sw_property include_source HAL/inc/lwip/priv/memp_std.h
add_sw_property include_source HAL/inc/lwip/priv/mem_priv.h
add_sw_property include_source HAL/inc/lwip/priv/nd6_priv.h
add_sw_property include_source HAL/inc/lwip/priv/raw_priv.h
add_sw_property include_source HAL/inc/lwip/priv/sockets_priv.h
add_sw_property include_source HAL/inc/lwip/priv/tcpip_priv.h
add_sw_property include_source HAL/inc/lwip/priv/tcp_priv.h
add_sw_property include_source HAL/inc/lwip/prot/acd.h
add_sw_property include_source HAL/inc/lwip/prot/autoip.h
add_sw_property include_source HAL/inc/lwip/prot/dhcp.h
add_sw_property include_source HAL/inc/lwip/prot/dhcp6.h
add_sw_property include_source HAL/inc/lwip/prot/dns.h
add_sw_property include_source HAL/inc/lwip/prot/etharp.h
add_sw_property include_source HAL/inc/lwip/prot/ethernet.h
add_sw_property include_source HAL/inc/lwip/prot/iana.h
add_sw_property include_source HAL/inc/lwip/prot/icmp.h
add_sw_property include_source HAL/inc/lwip/prot/icmp6.h
add_sw_property include_source HAL/inc/lwip/prot/ieee.h
add_sw_property include_source HAL/inc/lwip/prot/igmp.h
add_sw_property include_source HAL/inc/lwip/prot/ip.h
add_sw_property include_source HAL/inc/lwip/prot/ip4.h
add_sw_property include_source HAL/inc/lwip/prot/ip6.h
add_sw_property include_source HAL/inc/lwip/prot/mld6.h
add_sw_property include_source HAL/inc/lwip/prot/nd6.h
add_sw_property include_source HAL/inc/lwip/prot/tcp.h
add_sw_property include_source HAL/inc/lwip/prot/udp.h
add_sw_property include_source HAL/inc/netif/bridgeif.h
add_sw_property include_source HAL/inc/netif/bridgeif_opts.h
add_sw_property include_source HAL/inc/netif/etharp.h
add_sw_property include_source HAL/inc/netif/ethernet.h
add_sw_property include_source HAL/inc/netif/ieee802154.h
add_sw_property include_source HAL/inc/netif/lowpan6.h
add_sw_property include_source HAL/inc/netif/lowpan6_ble.h
add_sw_property include_source HAL/inc/netif/lowpan6_common.h
add_sw_property include_source HAL/inc/netif/lowpan6_opts.h
add_sw_property include_source HAL/inc/netif/slipif.h
add_sw_property include_source HAL/inc/netif/zepif.h
add_sw_property include_source HAL/inc/netif/ppp/ccp.h
add_sw_property include_source HAL/inc/netif/ppp/chap-md5.h
add_sw_property include_source HAL/inc/netif/ppp/chap-new.h
add_sw_property include_source HAL/inc/netif/ppp/chap_ms.h
add_sw_property include_source HAL/inc/netif/ppp/eap.h
add_sw_property include_source HAL/inc/netif/ppp/ecp.h
add_sw_property include_source HAL/inc/netif/ppp/eui64.h
add_sw_property include_source HAL/inc/netif/ppp/fsm.h
add_sw_property include_source HAL/inc/netif/ppp/ipcp.h
add_sw_property include_source HAL/inc/netif/ppp/ipv6cp.h
add_sw_property include_source HAL/inc/netif/ppp/lcp.h
add_sw_property include_source HAL/inc/netif/ppp/magic.h
add_sw_property include_source HAL/inc/netif/ppp/mppe.h
add_sw_property include_source HAL/inc/netif/ppp/ppp.h
add_sw_property include_source HAL/inc/netif/ppp/pppapi.h
add_sw_property include_source HAL/inc/netif/ppp/pppcrypt.h
add_sw_property include_source HAL/inc/netif/ppp/pppdebug.h
add_sw_property include_source HAL/inc/netif/ppp/pppoe.h
add_sw_property include_source HAL/inc/netif/ppp/pppol2tp.h
add_sw_property include_source HAL/inc/netif/ppp/pppos.h
add_sw_property include_source HAL/inc/netif/ppp/ppp_impl.h
add_sw_property include_source HAL/inc/netif/ppp/ppp_opts.h
add_sw_property include_source HAL/inc/netif/ppp/upap.h
add_sw_property include_source HAL/inc/netif/ppp/vj.h
add_sw_property include_source HAL/inc/netif/ppp/polarssl/arc4.h
add_sw_property include_source HAL/inc/netif/ppp/polarssl/des.h
add_sw_property include_source HAL/inc/netif/ppp/polarssl/md4.h
add_sw_property include_source HAL/inc/netif/ppp/polarssl/md5.h
add_sw_property include_source HAL/inc/netif/ppp/polarssl/sha1.h


add_sw_property include_source HAL/inc/lwip/apps/httpd_opts.h
add_sw_property include_source HAL/inc/lwip/apps/http_client.h


# Overriden HAL files
# add_sw_property excluded_hal_source HAL/inc/os/alt_syscall.h
# add_sw_property excluded_hal_source drivers/inc/altera_avalon_tse.h
# add_sw_property excluded_hal_source drivers/inc/altera_avalon_tse.c


# This driver supports only FreeRTOS BSP (OS) type
add_sw_property supported_bsp_type HAL

# Add preprocessor definitions to public makefile: ALT_LWIP
add_sw_property alt_cppflags_addition "-DALT_LWIP"


# LwIP configuration options
add_sw_setting boolean system_h_define checksum.enable_generate_ip CONF_LWIP_CHECKSUM_GEN_IP true "Enable generation of checksums for IP frames"
add_sw_setting boolean system_h_define checksum.enable_generate_udp CONF_LWIP_CHECKSUM_GEN_UDP true "Enable generation of checksums for UDP frames"
add_sw_setting boolean system_h_define checksum.enable_generate_tcp CONF_LWIP_CHECKSUM_GEN_TCP true "Enable generation of checksums for TCP frames"

add_sw_setting boolean system_h_define checksum.enable_check_ip CONF_LWIP_CHECKSUM_CHECK_IP true "Enable checking of checksums from IP frames"
add_sw_setting boolean system_h_define checksum.enable_check_udp CONF_LWIP_CHECKSUM_CHECK_UDP true "Enable checking of checksums from UDP frames"
add_sw_setting boolean system_h_define checksum.enable_check_tcp CONF_LWIP_CHECKSUM_CHECK_TCP true "Enable checking of checksums from TCP frames"

add_sw_setting boolean system_h_define proto.enable_vlan CONF_LWIP_PROTO_VLAN false "Enable VLAN support"
add_sw_setting boolean system_h_define proto.enable_icmp CONF_LWIP_PROTO_ICMP true "Enable ICMP support"
add_sw_setting boolean system_h_define proto.enable_autoip CONF_LWIP_PROTO_AUTOIP false "Enable AutoIP support"
add_sw_setting boolean system_h_define proto.enable_snmp CONF_LWIP_PROTO_SNMP false "Enable SNMP support"
add_sw_setting boolean system_h_define proto.enable_igmp CONF_LWIP_PROTO_IGMP false "Enable IGMP support"
add_sw_setting boolean system_h_define proto.enable_dns CONF_LWIP_PROTO_DNS false "Enable DNS support"
add_sw_setting boolean system_h_define proto.enable_udp CONF_LWIP_PROTO_UDP true "Enable UDP support"
add_sw_setting boolean system_h_define proto.enable_tcp CONF_LWIP_PROTO_TCP true "Enable TCP support"
add_sw_setting boolean system_h_define proto.enable_dhcp CONF_LWIP_PROTO_DHCP true "Enable DHCP support"

add_sw_setting decimal_number system_h_define connections.raw_pcb_count CONF_LWIP_RAW_PCB 2 "Number of raw sockets supported (Used by ICMP for example)"
add_sw_setting decimal_number system_h_define connections.udp_pcb_count CONF_LWIP_UDP_PCB 15 "Number of UDP sockets supported"
add_sw_setting decimal_number system_h_define connections.tcp_pcb_count CONF_LWIP_TCP_PCB 15 "Number of TCP sockets supported"
add_sw_setting decimal_number system_h_define connections.tcp_listen_pcb_count CONF_LWIP_TCP_PCB_LISTEN 10 "Number of TCP Listening sockets supported"

add_sw_setting decimal_number system_h_define memory.mem_size CONF_LWIP_MEM_SIZE 32768 "Size of the memory poll"
add_sw_setting boolean system_h_define memory.overflow_check CONF_LWIP_OVERFLOW_CHECK false "Do memory overflow checking"
add_sw_setting decimal_number system_h_define memory.pbuf_count CONF_LWIP_PBUF_COUNT 32 "Number of packet buffers supported"

add_sw_setting boolean system_h_define loopback CONF_LWIP_LOOPBACK false "Loopback outgoing traffic to our selfs"
add_sw_setting boolean system_h_define loopback_if CONF_LWIP_LOOPBACKIF false "Create a loopback interface 'lo'"
add_sw_setting boolean system_h_define stats CONF_LWIP_STATS true "Keep track of the LwIP traffic statistics"
add_sw_setting boolean system_h_define ip_forward CONF_LWIP_IP_FORWARD false "Keep track of the LwIP traffic statistics"
add_sw_setting decimal_number system_h_define ttl CONF_LWIP_DEFAULT_TTL 64 "The default TTL (Time To Life) set to a packet"





# End of file
