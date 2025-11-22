#ifndef SAMD21E17A_INTERRUPTS_H
#define SAMD21E17A_INTERRUPTS_H

void push_PM_Handler(void (*isr_handler)(void));
void push_SYSCTRL_Handler(void (*isr_handler)(void));
void push_WDT_Handler(void (*isr_handler)(void));
void push_RTC_Handler(void (*isr_handler)(void));
void push_EIC_Handler(void (*isr_handler)(void));
void push_NVMCTRL_Handler(void (*isr_handler)(void));
void push_DMAC_Handler(void (*isr_handler)(void));
void push_USB_Handler(void (*isr_handler)(void));
void push_EVSYS_Handler(void (*isr_handler)(void));
void push_SERCOM0_Handler(void (*isr_handler)(void));
void push_SERCOM1_Handler(void (*isr_handler)(void));
void push_SERCOM2_Handler(void (*isr_handler)(void));
void push_SERCOM3_Handler(void (*isr_handler)(void));
void push_Reserved13(void (*isr_handler)(void));
void push_Reserved14(void (*isr_handler)(void));
void push_TCC0_Handler(void (*isr_handler)(void));
void push_TCC1_Handler(void (*isr_handler)(void));
void push_TCC2_Handler(void (*isr_handler)(void));
void push_TC3_Handler(void (*isr_handler)(void));
void push_TC4_Handler(void (*isr_handler)(void));
void push_TC5_Handler(void (*isr_handler)(void));
void push_Reserved21(void (*isr_handler)(void));
void push_Reserved22(void (*isr_handler)(void));
void push_ADC_Handler(void (*isr_handler)(void));
void push_AC_Handler(void (*isr_handler)(void));
void push_DAC_Handler(void (*isr_handler)(void));
void push_PTC_Handler(void (*isr_handler)(void));
void push_I2S_Handler(void (*isr_handler)(void));

#endif
