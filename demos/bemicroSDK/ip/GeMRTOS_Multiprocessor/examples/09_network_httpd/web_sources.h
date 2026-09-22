/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Web status page content for 09_network_httpd: the generic SSI
 *         variable-source registry, the SSI tag handler, and the "/set.cgi"
 *         form handler for the page's two editable runtime parameters.
 *  \version 1.0
 *  \date      2026-08-03
 *
 *  SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
 *  Copyright (c) 2013-2026 GeMRTOS Project Contributors
 *
 *  Required Notice: Copyright GeMRTOS Project Contributors (https://gemrtos.com)
 *
 *  This file is part of GeMRTOS, licensed under the PolyForm Noncommercial
 *  License 1.0.0. You may use, copy, modify, and distribute this file for
 *  any noncommercial purpose. Commercial use requires a separate written
 *  license from GeMRTOS. Full terms:
 *  https://polyformproject.org/licenses/noncommercial/1.0.0
 */

#ifndef WEB_SOURCES_H_
#define WEB_SOURCES_H_

#include <gemrtos.h>
#include <system.h>
#include "lwip_main.h"

/* Owned by main.c (task_periodic/task_infinite_loop's live, CGI-editable
 * state); read by web_sources.c's row-render functions and SSI/CGI
 * handlers. g_periodic_task_tcb is NULL until task_periodic is created. */
extern GS_TCB  *g_periodic_task_tcb;
extern G_UINT32 g_infinite_loop_delay_ms;

/** \brief SSI tag handler for the web status page's tags: <!--#sysvars-->
 *         (multi-row status table, driven by the source registry in
 *         web_sources.c), and the bare-value <!--#period_ms-->/
 *         <!--#loop_ms--> tags used to pre-fill the configuration form.
 *         Registered with http_set_ssi_handler() at the start of
 *         tse_receive_server(), before httpd_init(). */
u16_t gemrtos_web_ssi_handler(int tag_index, char *insert, int insert_len,
                               u16_t current_tag_part, u16_t *next_tag_part);

/* SSI tag names, indexed by GEMRTOS_WEB_SSI_TAG_* (web_sources.c). Passed
 * to http_set_ssi_handler() together with gemrtos_web_ssi_handler(). */
extern const char *gemrtos_web_ssi_tags[3];

#if LWIP_HTTPD_CGI
/* "/set.cgi" table, passed to http_set_cgi_handlers(). Applies the web
 * status page's editable runtime parameters (periodic task period,
 * infinite-loop delay) -- see gemrtos_web_cgi_set_handler() (web_sources.c). */
extern const tCGI gemrtos_web_cgi_table[1];
#endif /* LWIP_HTTPD_CGI */

#endif /* WEB_SOURCES_H_ */
