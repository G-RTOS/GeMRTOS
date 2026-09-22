# Third-Party Licenses

GeMRTOS contains, and its published files include, third-party software. This document lists it, states
under what terms each part is included, and reproduces the licence texts those terms require. It covers
what GeMRTOS **publishes**: the IP components under `GeMRTOS/ip/`, the copy of that tree inside every
`demos/<board>/ip/`, and the board project files in `demos/<board>/` (§7). Paths in §2 to §6 are relative to
`GeMRTOS/ip/`.

**The rule for files not listed here.** Inside `GeMRTOS/ip/`, every file that this document does not
list is GeMRTOS's own work, covered by the GeMRTOS licence (`LICENSE.md`; see the licence statement in the
repository `README.md`). Most carry a GeMRTOS header naming `PolyForm-Noncommercial-1.0.0`; the rest —
documentation, sample-output files (`verify.expect`), images, timing constraints — carry no header and are
covered in the same way as the component they accompany. The encrypted controller `grtos.vhd` is covered by
`GeMRTOS_controller/NOTICE.txt`. The rule does not extend to `demos/` beyond what §7 states.

**Each source file's own header remains the authority for that file.** This document does not replace
those notices; it summarises them and reproduces their texts so that a distribution of the software, in
source or binary form, can carry them. It was produced on 2026-09-19 by reading every published file under
`GeMRTOS_Bridge/HAL` and comparing it, byte for byte, with the lwIP 2.2.0 release. It is **not legal
advice**, and nothing in it has been reviewed by counsel.

GeMRTOS's own software is licensed separately (PolyForm Noncommercial 1.0.0, see `LICENSE.md`); the
GeMRTOS controller hardware IP is proprietary (see `GeMRTOS_controller/NOTICE.txt`).

---

## 1. Summary

| Component | Origin | Licence | Where |
|---|---|---|---|
| **lwIP 2.2.0** TCP/IP stack | Swedish Institute of Computer Science (Adam Dunkels) and many contributors | BSD-3-Clause (§2); a few files carry other notices (below) | `GeMRTOS_Bridge/HAL/{api,apps,core,inc,netif}` |
| PPP code inside lwIP | derived from `pppd` and BSD sources: Carnegie Mellon University, Paul Mackerras, Eric Rosenquist, Tommi Komulainen, Marc Boucher (MBSI), Google, Sun Microsystems, the University of California, others | BSD-style notices, **several with an acknowledgment clause** (§3) | `GeMRTOS_Bridge/HAL/netif/ppp`, `.../inc/netif/ppp` |
| PolarSSL / XySSL (MD4, MD5, SHA-1, DES, ARC4) | Christophe Devine, Paul Bakker | BSD-3-Clause variant | `GeMRTOS_Bridge/HAL/netif/ppp/polarssl`, `.../inc/netif/ppp/polarssl` |
| `tinydir.h` | tinydir authors (Cong Xu and others) | BSD-2-Clause | `GeMRTOS_Bridge/HAL/apps/http/makefsdata/` |
| lwIP contrib `ping` application | lwIP 2.2.0 `contrib/apps/ping` | BSD-3-Clause (`ping.c`; `ping.h` carries no notice) | `GeMRTOS_Bridge/HAL/apps/ping/` |
| `makefsdata.exe` | a build of lwIP 2.2.0's own `makefsdata` tool (source in the same folder) | as lwIP | `GeMRTOS_Bridge/HAL/apps/http/` |
| Altera Ethernet (TSE) driver headers and `alt_lwip_*` socket glue | Altera Corporation (2006, 2009) | MIT-style (§5) | `GeMRTOS_Bridge/HAL/inc`, `.../arch` |
| lwIP-to-GeMRTOS port `lwip_main.c` / `.h` | Engineering Spirit (2012), modified by GeMRTOS | **GNU GPL v2 with the FreeRTOS exception** (§6) | `GeMRTOS_Bridge/HAL/arch`, `.../inc` |
| Terasic `DE2_115.v` (DE2-115 board demo) | Terasic Technologies Inc. (2012) | Terasic's own permission, limited to Terasic boards (§7). **Withdrawn from the current publication** | `demos/DE2_115/` (not published) |
| Quartus project files (`.qpf`, `.qsf`) in the demos | written by Intel Quartus Prime / Altera Quartus II | carry Intel's / Altera's notice (§7) | `demos/<board>/` |
| Intel Quartus Prime, Platform Designer, Nios V toolchain | Intel | not distributed (§8) | — |

---

## 2. lwIP 2.2.0

**Origin.** lwIP, <https://savannah.nongnu.org/projects/lwip/>, version **2.2.0** (confirmed from
`GeMRTOS_Bridge/HAL/inc/lwip/init.h`: `LWIP_VERSION_MAJOR 2`, `MINOR 2`, `REVISION 0`). Copyright (c) 2001,
2002 Swedish Institute of Computer Science, and the contributors named in each file. The licence that
lwIP's own `COPYING` file states, reproduced from the release:

```text
Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
```

Most files carry that text (or its 2001–2004 variant) with their own copyright line; the holders named in
those files are listed in Appendix A, Notice 1. Some files carry other notices — see §3 to §5 and the
Appendix.

**What is the release and what was changed.** Of the 332 files published under `GeMRTOS_Bridge/HAL`, **302
are byte-identical to the lwIP 2.2.0 release** (including the contrib `ping` application and the release's
own `COPYING`, added to the tree on 2026-09-19 as `GeMRTOS_Bridge/HAL/COPYING`) and **11 were
modified by GeMRTOS**. The modified files keep their original notices; GeMRTOS's changes to them are not
separately licensed. They are, under `GeMRTOS_Bridge/HAL/`:

`apps/http/httpd.c` (SSI content-type handling and the `/index.html` SSI flag), `apps/http/fsdata.c`
(**generated** by `makefsdata` from `apps/http/fs/`), `apps/http/fs/index.html`, `core/init.c`, `core/sys.c`,
`inc/arch/cc.h`, `inc/arch/perf.h`, `inc/arch/sys_arch.h`, `inc/lwip/arch.h`, `inc/lwip/opt.h`,
`inc/lwipopts.h`.

**What GeMRTOS added to the tree** (not part of the lwIP release): the port layer — `arch/lwip_main.c`,
`arch/sys_arch.c` (lwIP 2.2.0's contrib FreeRTOS port by Simon Goldschmidt, BSD-3-Clause, with 4 lines
changed by GeMRTOS), `inc/lwip_main.h`, the
Altera files of §5, `inc/arch/bpstruct.h` and `epstruct.h` (one-line packing stubs with no notice) — and
GeMRTOS's own web pages and images under `apps/http/fs/` (`status.html`, `style.css`, the GeMRTOS logos),
which are covered by GeMRTOS's own licence.

**Files that carry no notice of their own** are listed at the end of Appendix A. Those that come from the
lwIP release (for example `apps/smtp/smtp.c`, `core/altcp.c`, `inc/lwip/priv/memp_std.h`) were distributed
without a per-file notice; the licence of the release is the text above.

**What ends up in a device image.** The shipped configuration (`inc/lwipopts.h`) sets `PPP_SUPPORT`,
`PPPOE_SUPPORT` and `PPPOS_SUPPORT` to `0`. Compiled with the RISC-V toolchain and the BSP's flags, the PPP
and PolarSSL sources produce **no code** in that configuration (checked 2026-09-19 for `ppp.c`, `auth.c`,
`lcp.c`, `eap.c`, `vj.c` and the `polarssl` files `md5.c` and `des.c`: 0 bytes), while the lwIP core, `igmp.c` and
`httpd.c` do. Which obligations apply to a given build or distribution is not decided here; every
notice text is reproduced in Appendix A so that it can be assessed.

---

## 3. PPP code (derived from `pppd`)

The `netif/ppp` and `inc/netif/ppp` directories are derived from the BSD/`pppd` code base and carry the
notices of their original authors, which are **not** identical to lwIP's. Several are BSD-style licences
with an **acknowledgment clause** — for example, the Carnegie Mellon University text requires that
*"Redistributions of any form whatsoever must retain the following acknowledgment: 'This product includes
software developed by Computing Services at Carnegie Mellon University (http://www.cmu.edu/computing/)'"*,
the Paul Mackerras text requires the acknowledgment *"This product includes software developed by Paul
Mackerras <paulus@samba.org>"*, and the University of California text requires that documentation and
advertising materials related to the distribution acknowledge that the software was developed by the
University of California, Berkeley. The full texts are in Appendix A (Notices 3, 5 and 12 among them).
The source files are published with these notices intact. Whether an acknowledgment must also appear in
documentation or product literature, for a build in which this code is not compiled (§2), is not decided
here.

## 4. PolarSSL / XySSL and tinydir

`GeMRTOS_Bridge/HAL/netif/ppp/polarssl/*` (MD4, MD5, SHA-1, DES, ARC4) are by Christophe Devine (XySSL)
and Paul Bakker (PolarSSL), under a BSD-3-Clause variant whose third clause protects the names *PolarSSL*
and *XySSL* (Appendix A, Notice 4). `apps/http/makefsdata/tinydir.h` is by the tinydir authors, under
BSD-2-Clause (Appendix A). Neither is used by an application built from the shipped configuration;
`tinydir.h` is used only by the `makefsdata` host tool.

## 5. Altera Corporation — MIT-style

Eight files are by Altera Corporation and carry an MIT-style licence: `arch/alt_lwip_close.c`,
`alt_lwip_fcntl.c`, `alt_lwip_read.c`, `alt_lwip_write.c` (© 2006) and `inc/gemrtos_avalon_tse.h`,
`gemrtos_avalon_tse_system_info.h`, `gemrtos_eth_tse.h`, `gemrtos_eth_tse_regs.h` (© 2009). The four
`gemrtos_`-prefixed headers are Altera's TSE driver headers **renamed** for the GeMRTOS build; the
prefix records the rename, not authorship, and each keeps its notice. The texts are in Appendix A
(Notices 6 and 9).

## 6. Engineering Spirit — GNU GPL v2 with the FreeRTOS exception

**Files:** `GeMRTOS_Bridge/HAL/arch/lwip_main.c` and `GeMRTOS_Bridge/HAL/inc/lwip_main.h`, the lwIP
bring-up and Ethernet (TSE) initialisation layer. Created by Engineering Spirit (c) 2012
(<http://engineering-spirit.nl/>) and since modified by GeMRTOS.

**Notice, reproduced verbatim from `inc/lwip_main.h`:**

```text
This is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License (version 2) as published by the
Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
>>>NOTE<<< The modification to the GPL is included to allow you to
distribute a combined work that includes FreeRTOS without being obliged to
provide the source code for proprietary components outside of the FreeRTOS
kernel.  FreeRTOS is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details. You should have received a copy of the GNU General Public
License and the FreeRTOS license exception along with FreeRTOS; if not it
can be viewed here: http://www.freertos.org/a00114.html and also obtained
by writing to Richard Barry, contact details for whom are available on the
FreeRTOS WEB site.

Created by Engineering Spirit (c) 2012 http://engineering-spirit.nl/
```

**GPL v2 full text:** <https://www.gnu.org/licenses/old-licenses/gpl-2.0.html>
**FreeRTOS exception full text:** <http://www.freertos.org/a00114.html>

The exception is worded for a combined work that includes **FreeRTOS**; GeMRTOS is a different work. How
these terms apply to the way GeMRTOS uses these two files **has not been legally reviewed**, and this
document does not state a conclusion.

## 7. Board demo projects (`demos/<board>/`)

Each demo is a Quartus project: a `.qpf`, a `.qsf`, a top-level HDL file and a Platform Designer `.qsys`
system, plus the `ip/` tree of §2 to §6. Reading those project files on 2026-09-19:

**Terasic — the DE2-115 demo (withdrawn).** `DE2_115.v` is by Terasic Technologies Inc., and `DE2_115.qsf`
states *"Build by Terasic System Builder"*. The notice at the top of `DE2_115.v`, verbatim:

```text
// ============================================================================
// Copyright (c) 2012 by Terasic Technologies Inc.
// ============================================================================
//
// Permission:
//
//   Terasic grants permission to use and modify this code for use
//   in synthesis for all Terasic Development Boards and Altera Development 
//   Kits made by Terasic.  Other use of this code, including the selling 
//   ,duplication, or modification of any portion is strictly prohibited.
//
// Disclaimer:
//
//   This VHDL/Verilog or C/C++ source code is intended as a design reference
//   which illustrates how these types of functions can be implemented.
//   It is the user's responsibility to verify their design for
//   consistency and functionality through the use of formal
//   verification methods.  Terasic provides no warranty regarding the use 
//   or functionality of this code.
```

Its permission is limited to use in synthesis for Terasic boards, and it prohibits other use of any portion.
The DE2-115 demo **is not part of the current publication** (`demos/DE2_115/` and `DE2_115.zip` were
withdrawn on 2026-09-19); it was published earlier, and remains in the history of the public repository.
It will be restored only with Terasic's permission or with a top-level and pin assignments that are
GeMRTOS's own work.

**Intel / Altera notices in Quartus project files.** `ddr3.qpf`/`ddr3.qsf` (MAX 10 demo),
`grtosproject.qpf`/`grtosproject.qsf` (BeMicro SDK demo) and `top.qpf`/`top.qsf` (MAX10_RGMII demo) begin
with the notice that Quartus wrote into them. They are the tool's own output, kept as generated, and their
notice applies to them; the three texts in use are:

```text
# -------------------------------------------------------------------------- #
#
# Copyright (C) 2023  Intel Corporation. All rights reserved.
# Your use of Intel Corporation's design tools, logic functions 
# and other software and tools, and any partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Intel Program License 
# Subscription Agreement, the Intel Quartus Prime License Agreement,
# the Intel FPGA IP License Agreement, or other applicable license
# agreement, including, without limitation, that your use is for
# the sole purpose of programming logic devices manufactured by
# Intel and sold by Intel or its authorized distributors.  Please
# refer to the applicable agreement for further details, at
# https://fpgasoftware.intel.com/eula.
```

```text
# -------------------------------------------------------------------------- #
#
# Copyright (C) 1991-2014 Altera Corporation. All rights reserved.
# Your use of Altera Corporation's design tools, logic functions 
# and other software and tools, and its AMPP partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License 
# Subscription Agreement, the Altera Quartus II License Agreement,
# the Altera MegaCore Function License Agreement, or other 
# applicable license agreement, including, without limitation, 
# that your use is for the sole purpose of programming logic 
# devices manufactured by Altera and sold by Altera or its 
# authorized distributors.  Please refer to the applicable 
# agreement for further details.
#
```

```text
# -------------------------------------------------------------------------- #
#
# Copyright (C) 1991-2013 Altera Corporation
# Your use of Altera Corporation's design tools, logic functions 
# and other software and tools, and its AMPP partner logic 
# functions, and any output files from any of the foregoing 
# (including device programming or simulation files), and any 
# associated documentation or information are expressly subject 
# to the terms and conditions of the Altera Program License 
# Subscription Agreement, Altera MegaCore Function License 
# Agreement, or other applicable license agreement, including, 
# without limitation, that your use is for the sole purpose of 
# programming logic devices manufactured by Altera and sold by 
# Altera or its authorized distributors.  Please refer to the 
# applicable agreement for further details.
#
```

**No third-party notice.** `ddr3.v`, `m10_rgmii.v`, `grtosproject.vhd` and the three Platform Designer
`.qsys` system files carry no notice of a third party.

## 8. Intel / Altera design tools

GeMRTOS does **not** distribute Quartus Prime, Platform Designer, the Nios V or Nios II toolchains, or the
files those tools generate (board support packages, simulation models); the only tool-written files it
publishes are the Quartus project files of §7. Your use of Intel's tools
is governed by your own agreement with Intel. The encrypted GeMRTOS controller (`grtos.vhd`) is used with
a FlexLM licence file supplied by GeMRTOS; see `GeMRTOS_controller/NOTICE.txt`.

---

## Appendix A — Licence texts

Every distinct licence notice found in the published files under `GeMRTOS_Bridge/HAL`, reproduced as it
appears in the files, with the copyright holders each names and where those files are. Produced by
scanning the files on 2026-09-19; re-run the scan whenever lwIP or this component is updated.

300 files carry a licence notice, in 22 distinct texts; 27 carry none.

#### Notice 1 — 207 file(s)

Copyright holders named in these files: Swedish Institute of Computer Science (104); Simon Goldschmidt (26); Inico Technologies Ltd (23); Axon Digital Design B (22); Verisure Innovation AB (8); Leon Woestenberg <leon (8); Elias Oenal (6); Dominik Spies <kontakt@dspies (6); Joel Cunningham, Garmin International, Inc (4); Simon Goldschmidt <goldsimon@gmx (3); Frédéric Bernon, Simon Goldschmidt (3); Jasper Verschueren <jasper (3); Erik Andersson (3); Dirk Ziegelmeier (3).

Located in: `inc/lwip` (50), `apps/snmp` (25), `inc/lwip/apps` (25), `core` (19), `inc/lwip/prot` (18), `core/ipv6` (9), `inc/lwip/priv` (9), `inc/netif` (9).

```text
Copyright (c) 2001, 2002 Swedish Institute of Computer Science.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
```

#### Notice 2 — 24 file(s)

Copyright holders named in these files: see the text.

Located in: `inc/lwip/apps` (6), `inc/netif/ppp` (4), `netif/ppp` (3), `api` (2), `inc/lwip` (2), `apps/netbiosns` (1), `apps/ping` (1), `apps/tftp` (1).

```text
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
```

#### Notice 3 — 10 file(s)

Copyright holders named in these files: Paul Mackerras (10).

Located in: `netif/ppp` (7), `inc/netif/ppp` (3).

```text
Copyright (c) 1994-2002 Paul Mackerras. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. The name(s) of the authors of this software must not be used to
   endorse or promote products derived from this software without
   prior written permission.

3. Redistributions of any form whatsoever must retain the following
   acknowledgment:
   "This product includes software developed by Paul Mackerras
    <paulus@samba.org>".

THE AUTHORS OF THIS SOFTWARE DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

#### Notice 4 — 10 file(s)

Copyright holders named in these files: Christophe Devine (10); Paul Bakker <polarssl_maintainer at polarssl dot org> (10).

Located in: `inc/netif/ppp/polarssl` (5), `netif/ppp/polarssl` (5).

```text
 Based on XySSL: Copyright (C) 2006-2008  Christophe Devine

 Copyright (C) 2009  Paul Bakker <polarssl_maintainer at polarssl dot org>

 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   * Neither the names of PolarSSL or XySSL nor the names of its contributors
     may be used to endorse or promote products derived from this software
     without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Notice 5 — 10 file(s)

Copyright holders named in these files: Carnegie Mellon University (10).

Located in: `inc/netif/ppp` (5), `netif/ppp` (5).

```text
Copyright (c) 1984-2000 Carnegie Mellon University. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The name "Carnegie Mellon University" must not be used to
   endorse or promote products derived from this software without
   prior written permission. For permission or any legal
   details, please contact
     Office of Technology Transfer
     Carnegie Mellon University
     5000 Forbes Avenue
     Pittsburgh, PA  15213-3890
     (412) 268-4387, fax: (412) 268-7395
     tech-transfer@andrew.cmu.edu

4. Redistributions of any form whatsoever must retain the following
   acknowledgment:
   "This product includes software developed by Computing Services
    at Carnegie Mellon University (http://www.cmu.edu/computing/)."

CARNEGIE MELLON UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

#### Notice 6 — 4 file(s)

Copyright holders named in these files: Altera Corporation, San Jose, California, USA (4).

Located in: `arch` (4).

```text
Copyright (c) 2006 Altera Corporation, San Jose, California, USA.           *
All rights reserved.                                                        *
*
Permission is hereby granted, free of charge, to any person obtaining a     *
copy of this software and associated documentation files (the "Software"),  *
to deal in the Software without restriction, including without limitation   *
the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
and/or sell copies of the Software, and to permit persons to whom the       *
Software is furnished to do so, subject to the following conditions:        *
*
The above copyright notice and this permission notice shall be included in  *
all copies or substantial portions of the Software.                         *
*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
DEALINGS IN THE SOFTWARE.                                                   *
*
This agreement shall be governed in all respects by the laws of the State   *
of California and by the laws of the United States of America.              *
*
Altera does not recommend, suggest or require that this reference design    *
file be used in conjunction or combination with any other product.          *
```

#### Notice 7 — 4 file(s)

Copyright holders named in these files: Tommi Komulainen (4).

Located in: `inc/netif/ppp` (2), `netif/ppp` (2).

```text
Copyright (c) 1999 Tommi Komulainen.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The name(s) of the authors of this software must not be used to
   endorse or promote products derived from this software without
   prior written permission.

4. Redistributions of any form whatsoever must retain the following
   acknowledgment:
   "This product includes software developed by Tommi Komulainen
    <Tommi.Komulainen@iki.fi>".

THE AUTHORS OF THIS SOFTWARE DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

#### Notice 8 — 4 file(s)

Copyright holders named in these files: Eric Rosenquist (4).

Located in: `inc/netif/ppp` (2), `netif/ppp` (2).

```text
Copyright (c) 1995 Eric Rosenquist.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The name(s) of the authors of this software must not be used to
   endorse or promote products derived from this software without
   prior written permission.

THE AUTHORS OF THIS SOFTWARE DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

#### Notice 9 — 4 file(s)

Copyright holders named in these files: Altera Corporation, San Jose, California, USA (4).

Located in: `inc` (4).

```text
Copyright (c) 2009 Altera Corporation, San Jose, California, USA.           *
All rights reserved.                                                        *
*
Permission is hereby granted, free of charge, to any person obtaining a     *
copy of this software and associated documentation files (the "Software"),  *
to deal in the Software without restriction, including without limitation   *
the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
and/or sell copies of the Software, and to permit persons to whom the       *
Software is furnished to do so, subject to the following conditions:        *
*
The above copyright notice and this permission notice shall be included in  *
all copies or substantial portions of the Software.                         *
*
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
DEALINGS IN THE SOFTWARE.                                                   *
*
This agreement shall be governed in all respects by the laws of the State   *
of California and by the laws of the United States of America.              *
*
```

#### Notice 10 — 3 file(s)

Copyright holders named in these files: Swedish Institute of Computer Science (3).

Located in: `inc/arch` (1), `inc/netif` (1), `netif` (1).

```text
Copyright (c) 2001, Swedish Institute of Computer Science.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the Institute nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
```

#### Notice 11 — 3 file(s)

Copyright holders named in these files: Marc Boucher, Services Informatiques (MBSI) inc (3); Global Election Systems Inc (3).

Located in: `inc/netif/ppp` (2), `netif/ppp` (1).

```text
Copyright (c) 2003 by Marc Boucher, Services Informatiques (MBSI) inc.
portions Copyright (c) 1997 Global Election Systems Inc.

The authors hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice and the following disclaimer are included verbatim in any
distributions. No written agreement, license, or royalty fee is required
for any of the authorized uses.

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Notice 12 — 2 file(s)

Copyright holders named in these files: Regents of the University of California (2).

Located in: `inc/netif/ppp` (1), `netif/ppp` (1).

```text
Copyright (c) 1989 Regents of the University of California.
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by the University of California, Berkeley.  The name of the
University may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
```

#### Notice 13 — 2 file(s)

Copyright holders named in these files: Adam Dunkels (2).

Located in: `core` (1), `inc/lwip/prot` (1).

```text
uIP version Copyright (c) 2002-2003, Adam Dunkels.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Notice 14 — 2 file(s)

Copyright holders named in these files: CITEL Technologies Ltd (2).

Located in: `core/ipv4` (1), `inc/lwip` (1).

```text
Copyright (c) 2002 CITEL Technologies Ltd.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of CITEL Technologies Ltd nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY CITEL TECHNOLOGIES AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL CITEL TECHNOLOGIES OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
```

#### Notice 15 — 2 file(s)

Copyright holders named in these files: Sun Microsystems, Inc (2).

Located in: `inc/netif/ppp` (1), `netif/ppp` (1).

```text
Copyright (c) 2001 by Sun Microsystems, Inc.
All rights reserved.

Non-exclusive rights to redistribute, modify, translate, and use
this software in source and binary forms, in whole or in part, is
hereby granted, provided that the above copyright notice is
duplicated in any source form, and that neither the name of the
copyright holder nor the author is used to endorse or promote
products derived from this software.

THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
```

#### Notice 16 — 2 file(s)

Copyright holders named in these files: Marc Boucher, Services Informatiques (MBSI) inc (2).

Located in: `inc/netif/ppp` (1), `netif/ppp` (1).

```text
Copyright (c) 2006 by Marc Boucher, Services Informatiques (MBSI) inc.

The authors hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice and the following disclaimer are included verbatim in any
distributions. No written agreement, license, or royalty fee is required
for any of the authorized uses.

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Notice 17 — 2 file(s)

Copyright holders named in these files: Google, Inc (2).

Located in: `inc/netif/ppp` (1), `netif/ppp` (1).

```text
Copyright (c) 2002 Google, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The name(s) of the authors of this software must not be used to
   endorse or promote products derived from this software without
   prior written permission.

THE AUTHORS OF THIS SOFTWARE DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

#### Notice 18 — 1 file(s)

Copyright holders named in these files: Paul Mackerras (1).

Located in: `inc/netif/ppp` (1).

```text
Copyright (c) 2008 Paul Mackerras. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The name(s) of the authors of this software must not be used to
   endorse or promote products derived from this software without
   prior written permission.

4. Redistributions of any form whatsoever must retain the following
   acknowledgment:
   "This product includes software developed by Paul Mackerras
    <paulus@samba.org>".

THE AUTHORS OF THIS SOFTWARE DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
```

#### Notice 19 — 1 file(s)

Copyright holders named in these files: tinydir authors: (1).

Located in: `apps/http/makefsdata` (1).

```text
Copyright (c) 2013-2019, tinydir authors:
- Cong Xu
- Lautis Sun
- Baudouin Feildel
- Andargor <andargor@yahoo.com>
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Notice 20 — 1 file(s)

Copyright holders named in these files: Google, Inc (1).

Located in: `netif/ppp` (1).

```text
Copyright (c) 2002,2003,2004 Google, Inc.
All rights reserved.

License:
Permission to use, copy, modify, and distribute this software and its
documentation is hereby granted, provided that the above copyright
notice appears in all copies.  This software is provided without any
warranty, express or implied.
```

#### Notice 21 — 1 file(s)

Copyright holders named in these files: Adam Dunkels (1).

Located in: `inc/lwip` (1).

```text
ported from uIP resolv.c Copyright (c) 2002-2003, Adam Dunkels.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote
   products derived from this software without specific prior
   written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Notice 22 — 1 file(s)

Copyright holders named in these files: Marc Boucher, Services Informatiques (MBSI) inc (1); Global Election Systems Inc (1); Cognizant Pty Ltd (1).

Located in: `inc/netif/ppp` (1).

```text
Copyright (c) 2003 by Marc Boucher, Services Informatiques (MBSI) inc.
portions Copyright (c) 1998 Global Election Systems Inc.
portions Copyright (c) 2001 by Cognizant Pty Ltd.

The authors hereby grant permission to use, copy, modify, distribute,
and license this software and its documentation for any purpose, provided
that existing copyright notices are retained in all copies and that this
notice and the following disclaimer are included verbatim in any
distributions. No written agreement, license, or royalty fee is required
for any of the authorized uses.

THIS SOFTWARE IS PROVIDED BY THE CONTRIBUTORS *AS IS* AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

#### Files with no licence notice of their own

- `apps/http/fs/404.html`
- `apps/http/fs/index.html`
- `apps/http/fs/status.html`
- `apps/http/fs/style.css`
- `apps/http/fsdata.c`
- `apps/http/httpd_structs.h`
- `apps/http/makefsdata/makefsdata`
- `apps/http/makefsdata/makefsdata.c`
- `apps/http/makefsdata/readme.txt`
- `apps/ping/ping.h`
- `apps/smtp/smtp.c`
- `apps/snmp/snmp_snmpv2_framework.c`
- `apps/snmp/snmp_snmpv2_usm.c`
- `arch/lwip_main.c`
- `inc/arch/bpstruct.h`
- `inc/arch/epstruct.h`
- `inc/lwip/apps/FILES`
- `inc/lwip/apps/smtp.h`
- `inc/lwip/apps/smtp_opts.h`
- `inc/lwip/apps/snmp_snmpv2_framework.h`
- `inc/lwip/apps/snmp_snmpv2_usm.h`
- `inc/lwip/priv/memp_std.h`
- `inc/lwip_main.h`
- `inc/netif/etharp.h`
- `netif/FILES`
- `netif/ppp/PPPD_FOLLOWUP`
- `netif/ppp/polarssl/README`

---

*Last updated: 2026-09-19.*
