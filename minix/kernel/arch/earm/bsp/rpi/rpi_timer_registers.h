#ifndef _RPI_TIMER_REGISTERS_H
#define _RPI_TIMER_REGISTERS_H

/* STC has a 1 MHz frequency */
#define STC_FREQ_HZ         1000000

#define RPI3_STC_BASE       0x3f003000
#define STC_CS              0x0
#define STC_CLO             0x4
#define STC_CHI             0x8
#define STC_C3              0x18

#endif /* _RPI_TIMER_REGISTERS_H */
