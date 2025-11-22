#include <interrupts/samd21e/interrupts.h>
#include <common.h>

#define ISR_HANDLER(name)                           \
    void name(void)                                 \
    {                                               \
        if (peripheral_isr_handlers.name)           \
            peripheral_isr_handlers.name();         \
        else                                        \
            while(1);                               \
    }

#define ENUM_ISR_HANDLER(name, n)                   \
    void name(void)                                 \
    {                                               \
        if (peripheral_isr_handlers.name)           \
            peripheral_isr_handlers.name(n);        \
        else                                        \
            while(1);                               \
    }

#define PUSH_ISR(name)                              \
    void push_##name(void (*isr_handler)(void)) {   \
        peripheral_isr_handlers.name = isr_handler; \
    }

#define ENUM_PUSH_ISR(name)                          \
    void push_##name(void (*isr_handler)(uint8_t)) { \
        peripheral_isr_handlers.name = isr_handler;  \
    }

/* Peripherals handlers */
struct Peripheral_Interrupt_Handler_Table {
    void (*PM_Handler)(void);
    void (*SYSCTRL_Handler)(void);
    void (*WDT_Handler)(void);
    void (*RTC_Handler)(void);
    void (*EIC_Handler)(void);
    void (*NVMCTRL_Handler)(void);
    void (*DMAC_Handler)(void);
    void (*USB_Handler)(void);
    void (*EVSYS_Handler)(void);
    void (*SERCOM0_Handler)(uint8_t n);
    void (*SERCOM1_Handler)(uint8_t n);
    void (*SERCOM2_Handler)(uint8_t n);
    void (*SERCOM3_Handler)(uint8_t n);
    void (*Reserved13)(void);
    void (*Reserved14)(void);
    void (*TCC0_Handler)(uint8_t n);
    void (*TCC1_Handler)(uint8_t n);
    void (*TCC2_Handler)(uint8_t n);
    void (*TC3_Handler)(uint8_t n);
    void (*TC4_Handler)(uint8_t n);
    void (*TC5_Handler)(uint8_t n);
    void (*Reserved21)(void);
    void (*Reserved22)(void);
    void (*ADC_Handler)(void);
    void (*AC_Handler)(void);
    void (*DAC_Handler)(void);
    void (*PTC_Handler)(void);
    void (*I2S_Handler)(void);
} peripheral_isr_handlers;

PUSH_ISR(PM_Handler);
PUSH_ISR(SYSCTRL_Handler);
PUSH_ISR(WDT_Handler);
PUSH_ISR(RTC_Handler);
PUSH_ISR(EIC_Handler);
PUSH_ISR(NVMCTRL_Handler);
PUSH_ISR(DMAC_Handler);
PUSH_ISR(USB_Handler);
PUSH_ISR(EVSYS_Handler);
ENUM_PUSH_ISR(SERCOM0_Handler);
ENUM_PUSH_ISR(SERCOM1_Handler);
ENUM_PUSH_ISR(SERCOM2_Handler);
ENUM_PUSH_ISR(SERCOM3_Handler);
PUSH_ISR(Reserved13);
PUSH_ISR(Reserved14);
ENUM_PUSH_ISR(TCC0_Handler);
ENUM_PUSH_ISR(TCC1_Handler);
ENUM_PUSH_ISR(TCC2_Handler);
ENUM_PUSH_ISR(TC3_Handler);
ENUM_PUSH_ISR(TC4_Handler);
ENUM_PUSH_ISR(TC5_Handler);
PUSH_ISR(Reserved21);
PUSH_ISR(Reserved22);
PUSH_ISR(ADC_Handler);
PUSH_ISR(AC_Handler);
PUSH_ISR(DAC_Handler);
PUSH_ISR(PTC_Handler);
PUSH_ISR(I2S_Handler);

ISR_HANDLER(PM_Handler);
ISR_HANDLER(SYSCTRL_Handler);
ISR_HANDLER(WDT_Handler);
ISR_HANDLER(RTC_Handler);
ISR_HANDLER(EIC_Handler);
ISR_HANDLER(NVMCTRL_Handler);
ISR_HANDLER(DMAC_Handler);
ISR_HANDLER(USB_Handler);
ISR_HANDLER(EVSYS_Handler);
ENUM_ISR_HANDLER(SERCOM0_Handler, 0);
ENUM_ISR_HANDLER(SERCOM1_Handler, 1);
ENUM_ISR_HANDLER(SERCOM2_Handler, 2);
ENUM_ISR_HANDLER(SERCOM3_Handler, 3);
ISR_HANDLER(Reserved13);
ISR_HANDLER(Reserved14);
ENUM_ISR_HANDLER(TCC0_Handler, 0);
ENUM_ISR_HANDLER(TCC1_Handler, 1);
ENUM_ISR_HANDLER(TCC2_Handler, 2);
ENUM_ISR_HANDLER(TC3_Handler, 3);
ENUM_ISR_HANDLER(TC4_Handler, 4);
ENUM_ISR_HANDLER(TC5_Handler, 5);
ISR_HANDLER(Reserved21);
ISR_HANDLER(Reserved22);
ISR_HANDLER(ADC_Handler);
ISR_HANDLER(AC_Handler);
ISR_HANDLER(DAC_Handler);
ISR_HANDLER(PTC_Handler);
ISR_HANDLER(I2S_Handler);

// TODO: add exception handlers
#if 0
/* Exceptions */
void NMI_Handler            (void);
// void HardFault_Handler      (void) __attribute__ ((weak));
void MemManage_Handler      (void);
void BusFault_Handler       (void);
void UsageFault_Handler     (void);
void SecureFault_Handler    (void);
void SVC_Handler            (void);
void DebugMon_Handler       (void);
void PendSV_Handler         (void);
void SysTick_Handler        (void);
#endif
