/**
 *  \file 
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS freertos support
 *  \details This file contains the Message queue functions.
 *  \version 0.9a
 *  \date      2015-2020
 *  \bug  None known
 *  \copyright Ricardo Cayssials  (rcayssials@gemrtos.com)
 *  \copyright All rights reserved.
 *  \copyright Copyright (c) 2013-2020
 *  \warning This product was produced by Ricardo Cayssials in the hope that it 
 *  will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *  \warning DISCLAIMER OF WARRANTY. THIS PRODUCT IS LICENSED "AS IS." YOU BEAR THE RISK OF USING IT. 
 *  \warning RICARDO CAYSSIALS GIVES NO EXPRESS WARRANTIES, GUARANTEES, OR CONDITIONS. 
 *  TO THE EXTENT PERMITTED UNDER APPLICABLE LAWS, RICARDO CAYSSIALS EXCLUDES ALL IMPLIED WARRANTIES,
 *  INCLUDING MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
 */
  
/******************************************************************************
*                                                                             *
* License Agreement                                                           *
* Copyright (c) Ricardo L. Cayssials                                          *
* All rights reserved.                                                        *
*                                                                             *
******************************************************************************/

/** \class Trigger
The <b>trigger</b> resource can be seen as the generalization of interrupts. Tasks can be registered to a trigger resource in order to be resumed or restarted when the trigger resource is <em>triggered</em>. The <b>gu_trigger_create</b> functions creates a trigger resource and it allows associating an external interrupt with the trigger resource created.
The <b>gu_trigger_register_task</b> function registers a task to a trigger resource. 
When the trigger resource is triggered, it is disabled until all the tasks that have been resumed or restarted turn into <em>waiting for a new trigger</em> of the trigger resource. The <b>gu_trigger_wait</b> function puts the task into the <em>waiting for the trigger</em> state. A task associated with a trigger resource also waits for the trigger when finishes its execution.
Trigger resources can be associated with external events (meaning hardware interrupts); in this case, all the tasks registered to the trigger resource will behave as Interrupt Service Routines (ISRs). Because several tasks may be registered to a trigger resource, each one of these tasks will behave as an ISR independently of the others. 
On the other hand, trigger resources may be triggered from tasks' code. The <b>gu_trigger_release</b> function triggers a trigger resource. If enabled, all the registered tasks will be resumed or restarted.
The function <b>gu_trigger_enable</b> enables a trigger resource, while the function <b>gu_trigger_disable</b> is used to disable it.
<h3>Enable and disable hook functions</h3>
Hook functions can be configured to be executed when a trigger resource is either enabled or disabled. The <b>gu_trigger_enable_hook</b> may be used to configure a function to be executed prior to the trigger resource being enabled. The <b>gu_trigger_disable_hook</b> may be used to configure a function to be executed after the trigger resource is disabled. These hook functions are intended to be used to execute the special code associated with the devices associated with the trigger resource.
<h3>Trigger flexibility</h3>
Trigger resources are very flexible for handling system events. They overcome the limitations of interrupts in a generalized way. For instance, UART devices (as many other peripherals) group internal events, <em>input buffer full</em>, and <em>output buffer empty</em> into a single hardware interrupt signal. Therefore, the UART driver should check if an interrupt was produced either by the input or the output buffer and execute the corresponding code. Using trigger resources, a trigger resource may be associated with the UART interrupt signal, and the task code registered to it checks whether the interrupt was produced either by the input buffer, the output buffer, or both. If the input buffer is full, it triggers a trigger resource where all the reading tasks are registered. 
\image html "https://gemrtos.com/images/trigger_uart.gif"
If the output buffer is empty, then it triggers a trigger resource where all the writing tasks are registered. In this way, there may be as many reading tasks and as many writing tasks as required without interfering with one another.
The trigger resource associated with the input buffer should have defined hook functions to enable and disable the <em>input buffer full</em> event, while the trigger resource associated with the output buffer should have defined hook functions to enable and disable the <em>output buffer empty</em> event.
Consequently, trigger resources allow the virtualization and generalization of either internal or external events.
 */

#include <gemrtos.h>


OPTIMEZE_CODE(0)

OPTIMEZE_RESTORE

