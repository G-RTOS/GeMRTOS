/**
 *  \file
 *  \author Ricardo Cayssials
 *  \brief GeMRTOS trigger resource definitions and ISR-task API
 *  \details Defines the T_TRIGGER_RESOURCE structure, trigger state and
 *           timeout-type enumerations (GS_TRGStatus, GS_TRGTimeOutType), and
 *           the public trigger API: gu_TriggerCreate, gu_TriggerDestroy,
 *           gu_TriggerRegisterTask, gu_TriggerWait, gu_TriggerRelease,
 *           gu_TriggerEnable, gu_TriggerDisable, gu_TriggerEnableHook,
 *           gu_TriggerDisableHook, gu_TriggerSetTimeoutType.
 *  \version 0.9a
 *  \date      2015-2025
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


// Check https://stackoverflow.com/questions/1154709/how-can-i-hide-the-declaration-of-a-struct-in-c

#ifndef GEMRTOS_TRIGGER_H_
#define GEMRTOS_TRIGGER_H_


typedef enum {
	G_TRGStatus_not_valid = ENUM_CODE(GE_TRGStatus_HASH, 1),
	G_TRGStatus_waiting   = ENUM_CODE(GE_TRGStatus_HASH, 2),
	G_TRGStatus_triggered = ENUM_CODE(GE_TRGStatus_HASH, 3),
	G_TRGStatus_timeout   = ENUM_CODE(GE_TRGStatus_HASH, 4)
} GS_TRGStatus;


typedef enum  {
	GS_TRGTimeOutType_disabled             = ENUM_CODE(GE_TRGTimeOutType_HASH, 1),
	GS_TRGTimeOutType_restart_when_enable  = ENUM_CODE(GE_TRGTimeOutType_HASH, 2),
	GS_TRGTimeOutType_restart_when_timeout = ENUM_CODE(GE_TRGTimeOutType_HASH, 3),
	GS_TRGTimeOutType_no_restart           = ENUM_CODE(GE_TRGTimeOutType_HASH, 4)
} GS_TRGTimeOutType;


/**
 *  \brief The <b>T_TRIGGER_RESOURCE</b> is defined as the field <em>"trigger"</em> in a GS_RCB resource. So, the fields of the T_TRIGGER_RESOURCE structure should be addressed as: <em>(GS_RCB *)->trigger.\<field\></em>
 *  \relates Trigger
 */
struct T_TRIGGER_RESOURCE {
    G_UINT32 TRGIRQ_ID;              ///< \brief Number of IRQ associated with this trigger; set to G_TRIGGER_NO_IRQ (G_NUMBER_OF_IRQ_TCB + 1) if not associated with a hardware interrupt.
    G_UINT32 TRG_Enabled;            ///< \brief G_TRUE if trigger is enabled
	GS_TRGStatus TRGStatus;          ///< \brief Status of the trigger (enum GS_TRGStatus)
	GS_TRGTimeOutType TRGType;       ///< \brief Type of the trigger (enum GS_TRGTimeOutType)
	G_UINT64 TRGTimeout;             ///< \brief Trigger timeout in number of ticks
	GS_ECB *TRG_NextECB;             ///< \brief Pointer to the timeout ECB
    void  (*enable_code)(void *);    ///< \brief Pointer to the function to be executed to reenable trigger
    void  *enable_arg;               ///< \brief Pointer to the argument of the first call
    void  (*disable_code)(void *);   ///< \brief Pointer to the function to be executed to reenable trigger
    void  *disable_arg;              ///< \brief Pointer to the argument of the first call 
};
 
/**
 * \defgroup Trigger_API Trigger User API
 * \brief Public trigger functions exposed to application tasks.
 * \relates Trigger
 * @{
 */

/** \brief Creates a trigger resource and optionally associates it with a
 *         hardware interrupt.
 *  \param [in] irq_id        Hardware interrupt number to associate with this
 *                            trigger.  Pass -1 to create a software-only
 *                            trigger with no hardware interrupt binding.
 *  \param [in] ticks_to_wait Trigger timeout in system ticks.  Pass 0 to
 *                            disable the timeout.
 *  \return Pointer to the newly created GS_RCB trigger resource; must be
 *          used in all subsequent trigger operations.  Returns NULL if no
 *          resource is available.
 *  \sa gu_TriggerEnable, gu_TriggerDisable, gu_TriggerEnableHook,
 *      gu_TriggerDisableHook, gu_TriggerRegisterTask, gu_TriggerRelease,
 *      gu_TriggerWait
 *  \relates Trigger
 */
GS_RCB *gu_TriggerCreate(int irq_id, G_UINT64 ticks_to_wait);

/** \brief Destroys a trigger and returns its RCB and timeout event to the free
 *         pools.  Fails (returns G_FALSE) if ISR tasks are still registered or
 *         pending; on success it is disabled and detached from its IRQ line.
 *  \param [in] ptrigger Handle returned by gu_TriggerCreate().
 *  \return G_TRUE if destroyed; G_FALSE if tasks are still registered.
 *  \relates Trigger
 */
G_UINT32 gu_TriggerDestroy(GS_RCB *ptrigger);

/** \brief Associates a task with a trigger resource, making it an ISR task.
 *         When the trigger fires, all associated tasks are resumed or
 *         restarted.
 *  \param [in] ptcb     Pointer to the GS_TCB of the task to associate.
 *  \param [in] ptrigger Handle of the trigger resource.
 *  \return G_TRUE if the task was successfully registered; G_FALSE otherwise.
 *  \sa gu_TriggerCreate, gu_TriggerWait, gu_TriggerRelease
 *  \relates Trigger
 */
G_UINT32 gu_TriggerRegisterTask(struct gs_tcb *ptcb, GS_RCB *ptrigger);

/** \brief Places the calling task into a waiting state for the trigger
 *         resource it is registered to.  May be called anywhere in the
 *         task's code; the same effect occurs automatically when the task
 *         function returns (for non-infinite-loop tasks).
 *  \return GS_TRGStatus value: G_TRGStatus_triggered if the trigger fired,
 *          G_TRGStatus_timeout if a timeout occurred.
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerRelease
 *  \relates Trigger
 */
GS_TRGStatus gu_TriggerWait(void);

/* gk_TRG_Wait prototype lives here (not in gemrtos_core.h) because its return
   type GS_TRGStatus is defined in this file and gemrtos_core.h is parsed before
   trigger.h is in scope — a circular include would result if moved. */
GS_TRGStatus gk_TRG_Wait(GS_TCB *ptcb);

/** \brief Activates a trigger resource.  If the trigger is enabled and all
 *         associated tasks are in a waiting state, they are resumed or
 *         restarted.
 *  \param [in] ptrigger Handle of the trigger resource.
 *  \return The trigger status after activation (GS_TRGStatus).
 *  \sa gu_TriggerCreate, gu_TriggerRegisterTask, gu_TriggerWait
 *  \relates Trigger
 */
GS_TRGStatus gu_TriggerRelease(GS_RCB *ptrigger); /* return type changed from G_UINT32 to GS_TRGStatus */

/** \brief Enables the trigger resource, allowing it to be activated by
 *         gu_TriggerRelease() or the associated hardware interrupt.
 *  \param [in] ptrigger Handle of the trigger resource.
 *  \return G_TRUE if the operation was successful.
 *  \sa gu_TriggerCreate, gu_TriggerDisable, gu_TriggerEnableHook,
 *      gu_TriggerRelease, gu_TriggerRegisterTask, gu_TriggerWait
 *  \relates Trigger
 */
G_UINT32 gu_TriggerEnable(GS_RCB *ptrigger);

/** \brief Disables the trigger resource, preventing it from being activated.
 *  \param [in] ptrigger Handle of the trigger resource.
 *  \return G_TRUE if the operation was successful.
 *  \sa gu_TriggerCreate, gu_TriggerEnable, gu_TriggerDisableHook,
 *      gu_TriggerRelease
 *  \relates Trigger
 */
G_UINT32 gu_TriggerDisable(GS_RCB *ptrigger);

/** \brief Sets the hook function called before the trigger resource is
 *         enabled.  Typically used to re-arm a hardware peripheral interrupt.
 *  \param [in] ptrigger       Handle of the trigger resource.
 *  \param [in] code_callback  Function pointer executed when the trigger is
 *                             enabled.
 *  \param [in] p_arg          Argument passed to \p code_callback.
 *  \return G_TRUE if the hook was successfully configured; G_FALSE otherwise.
 *  \sa gu_TriggerCreate, gu_TriggerEnable, gu_TriggerDisableHook
 *  \relates Trigger
 */
G_UINT32 gu_TriggerEnableHook(GS_RCB *ptrigger,
                               void (*code_callback)(void *), void *p_arg);

/** \brief Sets the hook function called after the trigger resource is
 *         disabled.  Typically used to mask a hardware peripheral interrupt.
 *  \param [in] ptrigger       Handle of the trigger resource.
 *  \param [in] code_callback  Function pointer executed when the trigger is
 *                             disabled.
 *  \param [in] p_arg          Argument passed to \p code_callback.
 *  \return G_TRUE if the hook was successfully configured; G_FALSE otherwise.
 *  \sa gu_TriggerCreate, gu_TriggerDisable, gu_TriggerEnable,
 *      gu_TriggerEnableHook, gu_TriggerRelease
 *  \relates Trigger
 */
G_UINT32 gu_TriggerDisableHook(GS_RCB *ptrigger,
                                void (*code_callback)(void *), void *p_arg);

/** \brief Sets the timeout type that controls trigger re-arming behavior.
 *  \param [in] ptrigger Handle of the trigger resource.
 *  \param [in] type     One of: GS_TRGTimeOutType_disabled,
 *                       GS_TRGTimeOutType_restart_when_enable,
 *                       GS_TRGTimeOutType_restart_when_timeout,
 *                       GS_TRGTimeOutType_no_restart.
 *  \return G_TRUE to indicate success.
 *  \relates Trigger
 */
G_UINT32 gu_TriggerSetTimeoutType(GS_RCB *ptrigger, GS_TRGTimeOutType type);

/** @} */ /* end Trigger_API */

#endif /* GEMRTOS_TRIGGER_H_ */
