#ifndef _RPI_TIMER_REGISTERS_H
#define _RPI_TIMER_REGISTERS_H

#if 0

/* STC has a 1 MHz frequency */
#define STC_FREQ_HZ         1000000

#define RPI3_STC_BASE       0x3f003000
#define STC_CS              0x0
#define STC_CLO             0x4
#define STC_CHI             0x8
#define STC_C3              0x18

#endif

#define RPI3_QA7_BASE      0x40000000

#define QA7_CORE0TIMER     0x40
#define QA7_CORE0INT       0x60

#endif /* _RPI_TIMER_REGISTERS_H */
