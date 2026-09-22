/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief Web status page content for 09_network_httpd. See web_sources.h
 *         for the public surface; this file is extracted from
 *         examples/10_full_demo/hellogemrtos.c as a behaviour-preserving
 *         move -- every function body below is unchanged from the
 *         original, only its surrounding file changed. See README.md "Extraction
 *         notes" for exactly what moved where.
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

#include "web_sources.h"

GS_TCB  *g_periodic_task_tcb = (GS_TCB *) 0;
G_UINT32 g_infinite_loop_delay_ms;

/*****************************************************************************
 * Web status page -- generic SSI variable table mechanism
 *
 * Exposes live system values on the web status page (fs/status.html, tag
 * <!--#sysvars-->) as an HTML table, one row per value. To add a new value to
 * the page, add one "source" below (a row_count() + render_row() pair) to
 * gemrtos_web_sources[] -- no other code, and no change to status.html, is
 * needed. A source may contribute a fixed number of rows (e.g. 1, for a
 * single scalar) or a variable number determined at request time (e.g. one
 * row per item in a list); the dispatch logic in gemrtos_web_ssi_handler()
 * does not need to know which.
 *****************************************************************************/

typedef G_UINT32 (*gemrtos_web_row_count_fn)(void);
typedef u16_t (*gemrtos_web_render_row_fn)(G_UINT32 row_index, char *buf, u16_t buf_len);

typedef struct {
    gemrtos_web_row_count_fn  row_count;   /* how many rows this source has right now */
    gemrtos_web_render_row_fn render_row;  /* render row [0, row_count()) as "<tr>...</tr>" */
} gemrtos_web_source_t;

/* --- Source: number of tasks currently known to the kernel --- */

static G_UINT32 gemrtos_web_kernel_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_kernel_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    /* NOTE: g_kcb.KCB_NUMBER_OF_TCBs is NOT a live count of
     * active tasks -- it only increments when the static TCB pool is
     * exhausted and a TCB is allocated dynamically as a development-mode
     * fallback (see gk_TCB_GetFree(), gemrtos_core.c). In a correctly-sized
     * production build it is always 0, which is what made this row look
     * broken on real hardware. G_NUMBER_OF_TCB (the configured static pool
     * capacity) is reported instead -- a compile-time constant, always
     * correct, and safe to read without touching any live kernel list.
     * Reporting the true number of currently-active tasks would require
     * walking g_kcb.KCB_FREE_TCBs, which is mutated by other processors and
     * needs a synchronization answer first (the same open question as the
     * deferred task-name-table source). */
    (void) row_index;
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Configured task pool size</td><td>%lu</td></tr>",
        (unsigned long) G_NUMBER_OF_TCB);
}

/* --- Source: system uptime, in seconds --- */

static G_UINT32 gemrtos_web_uptime_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_uptime_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    /* NOTE: the first version of this row printed
     * milliseconds as a 64-bit value via snprintf(..., "%llu", (unsigned long
     * long) ...) and produced nonsensical, non-reproducible output on real
     * hardware. "%llu" is used elsewhere in this codebase (gemrtos_monitor.c,
     * gemrtos_kernel.c) and presumed safe by precedent, but always through
     * fprintf()/printf(), never snprintf() -- this row was the first place a
     * 64-bit value went through snprintf(), and evidence points at that
     * specific combination: lwIP's own sys_now() (lwip_main.c) does the exact
     * same gu_SystemTimeGet()/G_TICKS_PER_MSECOND division and is proven
     * correct indirectly (the whole TCP/IP stack depends on it and works), so
     * the division itself is not the suspect. Reporting whole seconds as a
     * 32-bit value computed before the snprintf() call sidesteps 64-bit
     * varargs entirely and is still a useful, correct value (32 bits of
     * seconds covers ~136 years of uptime). Before reintroducing a 64-bit
     * value into any snprintf() call on this target, verify the combination
     * actually works on real hardware first -- don't assume it from the
     * fprintf()-only precedent elsewhere in the codebase. */
    G_UINT32 uptime_s;

    (void) row_index;
    uptime_s = (G_UINT32) (gu_SystemTimeGet() / (G_UINT64) G_TICKS_PER_SECOND);
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Uptime (s)</td><td>%lu</td></tr>",
        (unsigned long) uptime_s);
}

/* --- Source: current IP address of the default network interface --- */

static G_UINT32 gemrtos_web_ipaddr_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_ipaddr_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    /* print_ipad() internally hardcodes snprintf(buf, 17, ...) regardless of the
     * buffer size passed in, so this buffer must be at least 17 bytes or it
     * would overflow. */
    char ip_buf[17];

    (void) row_index;
    if (netif_default == NULL) {
        return (u16_t) snprintf(buf, buf_len, "<tr><td>IP address</td><td>(no interface)</td></tr>");
    }
    print_ipad(netif_default->ip_addr.addr, ip_buf);
    return (u16_t) snprintf(buf, buf_len, "<tr><td>IP address</td><td>%s</td></tr>", ip_buf);
}

/* --- Source: periodic task's current period, in ms (web-editable, see
 * gemrtos_web_cgi_set_handler() below) --- */

/**
 * \brief Returns the periodic task's current period in milliseconds, or 0 if
 *        the task was never created.
 *
 * Shared by the status-table row (gemrtos_web_period_render_row()) and the
 * bare-value SSI tag used to pre-fill the configuration form
 * (gemrtos_web_ssi_handler()) so the tick-to-ms conversion is written once.
 *
 * TCBPeriod is a G_UINT64 storing TICKS (gu_TaskPeriodSet() stores the result
 * of gu_ConvertTime(), not raw milliseconds) -- convert back to ms and
 * truncate to 32 bits, same rationale as the uptime row above: keep 64-bit
 * values out of snprintf()'s varargs on this target.
 */
static G_UINT32 gemrtos_web_current_period_ms(void)
{
    if (g_periodic_task_tcb == (GS_TCB *) 0) {
        return 0U;
    }
    return (G_UINT32) (g_periodic_task_tcb->TCBPeriod / (G_UINT64) G_TICKS_PER_MSECOND);
}

static G_UINT32 gemrtos_web_period_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_period_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    (void) row_index;
    if (g_periodic_task_tcb == (GS_TCB *) 0) {
        return (u16_t) snprintf(buf, buf_len, "<tr><td>Periodic task period (ms)</td><td>(not created)</td></tr>");
    }
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Periodic task period (ms)</td><td>%lu</td></tr>",
        (unsigned long) gemrtos_web_current_period_ms());
}

/* --- Source: infinite-loop task's current delay, in ms (web-editable) --- */

static G_UINT32 gemrtos_web_loopdelay_row_count(void)
{
    return 1U;
}

static u16_t gemrtos_web_loopdelay_render_row(G_UINT32 row_index, char *buf, u16_t buf_len)
{
    (void) row_index;
    return (u16_t) snprintf(buf, buf_len,
        "<tr><td>Infinite-loop delay (ms)</td><td>%lu</td></tr>",
        (unsigned long) g_infinite_loop_delay_ms);
}

/* Registry of all sources contributing rows to the <!--#sysvars--> table.
 * Add a new { row_count, render_row } pair here to expose a new value. */
static const gemrtos_web_source_t gemrtos_web_sources[] = {
    { gemrtos_web_kernel_row_count,     gemrtos_web_kernel_render_row     },
    { gemrtos_web_uptime_row_count,     gemrtos_web_uptime_render_row     },
    { gemrtos_web_ipaddr_row_count,     gemrtos_web_ipaddr_render_row     },
    { gemrtos_web_period_row_count,     gemrtos_web_period_render_row     },
    { gemrtos_web_loopdelay_row_count,  gemrtos_web_loopdelay_render_row  },
};
#define GEMRTOS_WEB_SOURCE_COUNT \
    (sizeof(gemrtos_web_sources) / sizeof(gemrtos_web_sources[0]))

enum {
    GEMRTOS_WEB_SSI_TAG_SYSVARS = 0,
    GEMRTOS_WEB_SSI_TAG_PERIOD_MS,
    GEMRTOS_WEB_SSI_TAG_LOOP_MS,
};

/* "period_ms" / "loop_ms" are bare-value tags (no <tr> wrapper) meant to sit
 * inside <input value="..."> in the configuration form, so the form
 * pre-fills with the live value -- see gemrtos_web_ssi_handler() and
 * LWIP_HTTPD_SSI_INCLUDE_TAG in lwipopts.h (must be 0 for this to render
 * correctly inside a quoted HTML attribute). */
const char *gemrtos_web_ssi_tags[3] = { "sysvars", "period_ms", "loop_ms" };

/**
 * \brief Total number of rows currently available across all registered sources.
 *
 * \returns the sum of row_count() over gemrtos_web_sources[]. Recomputed on
 *          every call since sources such as a future task-list source report a
 *          count that can change between requests.
 */
static G_UINT32 gemrtos_web_total_row_count(void)
{
    G_UINT32 total = 0U;
    G_UINT32 i;

    for (i = 0U; i < GEMRTOS_WEB_SOURCE_COUNT; i++) {
        total += gemrtos_web_sources[i].row_count();
    }
    return total;
}

/**
 * \brief SSI tag handler for the web status page's tags: <!--#sysvars-->
 *        (multi-row status table), and the bare-value <!--#period_ms-->/
 *        <!--#loop_ms--> tags used to pre-fill the configuration form.
 *
 * The <!--#sysvars--> case maps a linear row index (current_tag_part) onto
 * one row from one source in gemrtos_web_sources[], renders it, and tells
 * the httpd core (via *next_tag_part) whether another row follows so it can
 * call this handler again. The two bare-value tags are single-shot: they
 * write one number and leave *next_tag_part untouched (no further parts).
 * Registered with http_set_ssi_handler() in tse_receive_server() before
 * httpd_init() is called.
 *
 * \param [in]  tag_index        Index into gemrtos_web_ssi_tags[].
 * \param [out] insert           Buffer to write the tag's replacement text into.
 * \param [in]  insert_len       Size of \p insert, in bytes.
 * \param [in]  current_tag_part Zero-based row index for this call
 *                                (<!--#sysvars--> only; ignored otherwise).
 * \param [out] next_tag_part    Set to current_tag_part + 1 if another row
 *                                follows; left untouched (defaults to "no more
 *                                parts") otherwise.
 * \returns the number of bytes written into \p insert.
 */
u16_t gemrtos_web_ssi_handler(int tag_index, char *insert, int insert_len,
                               u16_t current_tag_part, u16_t *next_tag_part)
{
    G_UINT32 remaining_index;
    G_UINT32 source_idx;

    if (tag_index == GEMRTOS_WEB_SSI_TAG_PERIOD_MS) {
        return (u16_t) snprintf(insert, (u16_t) insert_len, "%lu",
                                (unsigned long) gemrtos_web_current_period_ms());
    }
    if (tag_index == GEMRTOS_WEB_SSI_TAG_LOOP_MS) {
        return (u16_t) snprintf(insert, (u16_t) insert_len, "%lu",
                                (unsigned long) g_infinite_loop_delay_ms);
    }
    if (tag_index != GEMRTOS_WEB_SSI_TAG_SYSVARS) {
        return 0;
    }

    remaining_index = (G_UINT32) current_tag_part;

    for (source_idx = 0U; source_idx < GEMRTOS_WEB_SOURCE_COUNT; source_idx++) {
        G_UINT32 rows = gemrtos_web_sources[source_idx].row_count();

        if (remaining_index < rows) {
            u16_t written = gemrtos_web_sources[source_idx].render_row(
                remaining_index, insert, (u16_t) insert_len);

            /* snprintf() returns the length it WOULD have written; clamp to what
             * actually landed in the buffer so we never report more bytes than
             * were written (see LWIP_HTTPD_MAX_TAG_INSERT_LEN in lwipopts.h). */
            if (written >= (u16_t) insert_len) {
                written = (u16_t) (insert_len - 1);
            }

            if ((G_UINT32) (current_tag_part + 1U) < gemrtos_web_total_row_count()) {
                *next_tag_part = (u16_t) (current_tag_part + 1U);
            }
            return written;
        }
        remaining_index -= rows;
    }

    /* current_tag_part points past the last available row -- nothing to emit. */
    return 0;
}

#if LWIP_HTTPD_CGI

/* Web-editable parameters are clamped to this range rather than rejected outright,
 * so a stray/malformed form submission cannot set the infinite-loop delay to 0
 * (which would busy-loop the task) or the periodic task's period to 0 (which
 * would trip PRINT_ASSERT((ptcb->TCBPeriod > 0), ...) inside gu_TaskPeriodSet()
 * and halt the system via G_DEBUG_WHILEFOREVER). */
#define GEMRTOS_WEB_MIN_PERIOD_MS    10U
#define GEMRTOS_WEB_MAX_PERIOD_MS  60000U

/**
 * \brief Parses a non-negative decimal string, clamped to
 *        [GEMRTOS_WEB_MIN_PERIOD_MS, GEMRTOS_WEB_MAX_PERIOD_MS].
 *
 * A small hand-rolled parser is used instead of atol()/strtoul() to avoid
 * pulling in <stdlib.h>, which this codebase deliberately does not include
 * project-wide (see the <stdlib.h> note at the top of gemrtos.h).
 *
 * \param [in] value  NUL-terminated decimal string (from the CGI parameter value).
 * \returns the parsed value, clamped to the valid range. Non-digit characters
 *          (including an empty string) parse as 0, which then clamps up to
 *          GEMRTOS_WEB_MIN_PERIOD_MS.
 */
static G_UINT32 gemrtos_web_parse_clamped_ms(const char *value)
{
    G_UINT32 result = 0U;

    while ((*value >= '0') && (*value <= '9')) {
        result = (result * 10U) + (G_UINT32) (*value - '0');
        value++;
    }

    if (result < GEMRTOS_WEB_MIN_PERIOD_MS) {
        result = GEMRTOS_WEB_MIN_PERIOD_MS;
    } else if (result > GEMRTOS_WEB_MAX_PERIOD_MS) {
        result = GEMRTOS_WEB_MAX_PERIOD_MS;
    }
    return result;
}

/**
 * \brief CGI handler for "/set.cgi" -- applies the web status page's editable
 *        runtime parameters (periodic task period, infinite-loop delay).
 *
 * Registered with http_set_cgi_handlers() in tse_receive_server() before
 * httpd_init(). Invoked for GET requests to "/set.cgi?param=value&..." (see
 * the form in fs/status.html). Recognises "period_ms" (periodic task period)
 * and "loop_delay_ms" (infinite-loop task delay); unrecognised parameters are
 * ignored. Always redirects back to the status page so the applied values are
 * visible immediately via the existing <!--#sysvars--> rows.
 *
 * \param [in] cgi_index   Index into gemrtos_web_cgi_table[] (unused, only one entry).
 * \param [in] num_params  Number of parameters in \p params / \p values.
 * \param [in] params      Array of parameter names.
 * \param [in] values      Array of parameter values (parallel to \p params).
 * \returns the URI to serve as the response ("/status.html").
 */
static const char *gemrtos_web_cgi_set_handler(int cgi_index, int num_params,
                                                char *params[], char *values[])
{
    int i;

    (void) cgi_index;

    for (i = 0; i < num_params; i++) {
        if (strcmp(params[i], "period_ms") == 0) {
            G_UINT32 new_period_ms = gemrtos_web_parse_clamped_ms(values[i]);
            if (g_periodic_task_tcb != (GS_TCB *) 0) {
                gu_TaskPeriodSet(g_periodic_task_tcb, 0, 0, 0, new_period_ms);
                GU_FPRINTF(stderr, "[web] Periodic task period set to %lu ms\n",
                           (unsigned long) new_period_ms);
            }
        } else if (strcmp(params[i], "loop_delay_ms") == 0) {
            g_infinite_loop_delay_ms = gemrtos_web_parse_clamped_ms(values[i]);
            GU_FPRINTF(stderr, "[web] Infinite-loop delay set to %lu ms\n",
                       (unsigned long) g_infinite_loop_delay_ms);
        }
    }

    return "/status.html";
}

const tCGI gemrtos_web_cgi_table[1] = {
    { "/set.cgi", gemrtos_web_cgi_set_handler },
};

#endif /* LWIP_HTTPD_CGI */
