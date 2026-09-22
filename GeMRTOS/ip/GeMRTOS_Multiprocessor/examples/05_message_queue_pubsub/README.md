# 05 — `message_queue_pubsub`

GeMRTOS message queues used as a true publish/subscribe mechanism: one send
reaches every currently-subscribed task, not just one. Three independent
scenarios: fan-out broadcast, the guaranteed partial-delivery/timeout path,
and late-subscriber semantics. Task functions and message-queue creation
live in `pubsub_demo.c`/`pubsub_demo.h`, `main.c` only creates tasks.

## Why this is "pub/sub" and not just a queue

A GeMRTOS message queue tracks how many tasks are subscribed
(`gu_MessageQueueSubscribe()`), and `gu_MessageQueueSend()` /
`gu_MessageQueuePrintf()` block until the message has been delivered to
**every** one of them (or a timeout expires) — each subscriber gets its own
independent copy. This is different from a bounded resource pool
(`03_semaphores`' Scenario B), where taking a slot excludes other holders:
here, nobody's receipt of a message excludes anybody else's.

**Important implementation detail found while designing this example, not
assumed from the header doc:** `gu_MessageQueuePrintf()` always calls
`gu_MessageQueueSend()` with `timeout=0` internally (`gemrtos_mq.c`), which
blocks *until every current subscriber has received the message, with no
upper bound* — it can never demonstrate the timeout path. Scenario B and C
below therefore call `gu_MessageQueueSend()` directly with an explicit
non-zero timeout, so the returned delivered-subscriber count is also
visible (`gu_MessageQueuePrintf()` always returns `G_TRUE` and never
reports how many receivers were reached).

## Scenario A — fan-out broadcast

`sensor_publisher` publishes an incrementing "reading" every 2 s via
`gu_MessageQueuePrintf()`. Two independent subscribers, `logger_subscriber`
and `display_subscriber`, both subscribed to the same queue, each print
their *own* copy of every reading with a different prefix. One `Send()`,
two independent deliveries — the defining property of publish/subscribe.

## Scenario B — a slow subscriber forces a guaranteed partial delivery

Two subscribers on a second queue: `fast_subscriber` (always loops straight
back to `gu_MessageQueueReceive()`, never busy) and `slow_subscriber`
(after each receive, spends `SCENARIO_B_SLOW_BUSY_MS` = 2000 ms deliberately
busy before receiving again). `bursty_publisher` sends a `status <n>`
message every `SCENARIO_B_PUBLISH_PERIOD_MS` = 1000 ms with a
`SCENARIO_B_SEND_TIMEOUT_MS` = 300 ms timeout, then checks the return value:

- The **first** send is guaranteed delivered to both subscribers — both
  subscribed and were already blocked in `Receive()` well before it fires.
- The **second** send is guaranteed to arrive while `slow_subscriber` is
  still in its 2000 ms busy window (1000 ms < 2000 ms), so it can only ever
  reach `fast_subscriber` within the 300 ms timeout — a **guaranteed partial
  delivery, every run**, exactly the recoverable-error path an application
  must handle (compare `03_semaphores`' Scenario B, same discipline
  applied to message queues instead of semaphores).
- Later sends are **not** similarly guaranteed and will vary run to run,
  same caveat `03_semaphores`' Scenario C documents for its own timing.

A message that times out is **not** queued for later pickup — GeMRTOS drops
it for whichever subscriber(s) missed the delivery window (confirmed by
reading `gu_MessageQueueSend()`'s implementation, not assumed): the
publisher's return value is the only signal that some subscriber missed a
message, which is exactly why an application must check it.

## Scenario C — late subscriber, no retroactive delivery

`early_subscriber` subscribes at boot and immediately starts receiving.
`heartbeat_publisher` sends `heartbeat <n>` every
`SCENARIO_C_HEARTBEAT_PERIOD_S` = 1 s, starting immediately at t=0s (the
`gu_TaskDelay()` call runs *after* each send, not before the first one, so
heartbeat N goes out at t=(N-1)s). `late_subscriber` does not subscribe
until t=3.5s — strictly between the 4th heartbeat (t=3s) and the 5th
(t=4s) — so its first-ever received message is deterministically
`heartbeat 5`, **never** 1 through 4: subscribing is not retroactive, a
subscriber only receives what is sent *after* it joins. Watch
`heartbeat_publisher`'s own "delivered to N subscriber(s)" log line change
from 1 to 2 the moment `late_subscriber` joins — the audience size is
visible from the sender's side too.

**Note on `early_subscriber`'s first heartbeat, found during hardware
verification:** `early_subscriber` and `heartbeat_publisher`
both start at offset t=0s at the same priority, unlike Scenario A (which
deliberately starts its publisher 1s after its subscribers, precisely to
avoid this). Whether `early_subscriber`'s `gu_MessageQueueSubscribe()` call
runs before `heartbeat_publisher`'s first `gu_MessageQueueSend()` is
therefore a genuine scheduling race, not guaranteed: on one hardware run,
heartbeat 1 was delivered to 0 subscribers (`early_subscriber` lost the
race) and its own first received message was `heartbeat 2` instead of
`heartbeat 1`. This does not affect the scenario's actual point (`late_subscriber`
joining at t=3.5s deterministically misses heartbeats 1-4 regardless of
which heartbeat `early_subscriber` itself first catches).

## Building and running

```
new_script.bat -std -ex 05_message_queue_pubsub --board m10_rgmii_project_msgdma ...
```

or, once the board tree exists:

```
gemrtos_build.bat -std -ex 05_message_queue_pubsub -qpf top -qsys q_sys
```

## Expected output (JTAG UART, instance 0)

Interleaved, roughly (exact ordering/timing between scenarios will vary —
each is independent):

```
[sensor_publisher] published reading 1
[logger_subscriber] logged: reading 1
[display_subscriber] display: reading 1
[fast_subscriber] received: status 1
[slow_subscriber] received: status 1
[bursty_publisher] status 1 delivered to all 2 subscribers
[fast_subscriber] received: status 2
[bursty_publisher] status 2 PARTIAL delivery: 1 of 2 subscribers (slow_subscriber likely still busy)
[heartbeat_publisher] heartbeat 1 delivered to 0 or 1 subscriber(s)
[early_subscriber] received: heartbeat 1 or 2  (race with heartbeat_publisher's own start -- see note above)
...
[heartbeat_publisher] heartbeat 5 delivered to 2 subscriber(s)
[late_subscriber] first message ever seen: heartbeat 5 (notice it is not heartbeat 1)
...
```

## Things to try

- Lower `SCENARIO_B_SLOW_BUSY_MS` below `SCENARIO_B_PUBLISH_PERIOD_MS` and
  watch `slow_subscriber` keep up, turning the guaranteed partial delivery
  into a guaranteed full one instead.
- Add a third subscriber to Scenario A's queue and confirm all three see
  every reading — nothing about the mechanism is hardcoded to two.
- Lower `late_subscriber`'s start offset (`PUBSUB_START_OFFSET_C_LATE_S`,
  `main.c`) to 1.5s and confirm its first received message becomes
  `heartbeat 3` — showing the offset value, not some other mechanism, is
  what determines which heartbeat a late subscriber first sees.
