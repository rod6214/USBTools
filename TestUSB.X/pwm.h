/* 
 * File:   pwm.h
 * Author: Nelson
 *
 * Created on March 6, 2022, 7:15 PM
 */

#ifndef PWM_H
#define	PWM_H

#include <xc.h>

#ifdef	__cplusplus
extern "C" {
#endif
    
#define PWM_FREQ_2K 375
#define PWM_FREQ_3K 250
#define PWM_FREQ_5K 148
#define PWM_FREQ_10K 73
#define PWM_FREQ_15K 48
#define PWM_FREQ_25K 28 // no
#define PWM_FREQ_30K 23 // no
#define PWM_FREQ_50K 13 // no
#define PWM_FREQ_75K 8 //no
#define PWM_FREQ_125K 4 
#define PWM_FREQ_150K 3
#define PWM_FREQ_250K 1
#define PWM_FREQ_375K 0
#define PWM_CPP2 2
#define PWM_CPP1 1

#define PWM_RC1 1
#define PWM_RC2 2
#define PWM_RB3 4


typedef struct _PWM 
{
    int DutyCicle;
    int Freq; // Given in Khz, Min Freq will be 2.9296875Khz, and Max freq 6Mhz
    int CPP_Port;
    int CPP_Modules;
    int IsActivated;
} PWM_t;

extern void pwm_init(PWM_t* conf);
extern void pwm_setPulse(PWM_t* conf, int percentage);
extern void pwm_finish(PWM_t* conf);
extern int pwm_getDutyCicle(PWM_t* p);
extern void pwm_setCCPPort(PWM_t* p, int port);
extern void pwm_setFreq(PWM_t* p, int freq);
extern void pwm_turnOnCPPModules(PWM_t* p, int modules);

#ifdef	__cplusplus
}
#endif

#endif	/* PWM_H */

