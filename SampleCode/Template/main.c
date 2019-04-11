/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 14/06/30 4:51p $
 * @brief    Software Development Template.
 * @note
 * Copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC029xDE.h"

extern void SYS_Init(void); 


#define PWM_CLOCK 	(12000000)
#define PWM_FREQ 	(2000)

uint16_t PWM_DUTY_1000 = 103;	//test with 10.3%

void PWM0_DUTY_CHANGE(uint16_t PWM_DUTY_1000)	
{
    /* Assume PWM output frequency is 300Hz and duty ratio is 50%, user can calculate PWM settings by follows.
       duty ratio = (CMR+1)/(CNR+1)
       cycle time = CNR+1
       High level = CMR+1
       PWM clock source frequency = __HXT = 12000000
       (CNR+1) = PWM clock source frequency/prescaler/PWM output frequency
               = 12000000/2/300 = 20000
       (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
       CNR = 19999
       duty ratio = 50% ==> (CMR+1)/(CNR+1) = 50% ==> CMR = (CNR+1)*0.5-1 = 20000*50/100-1
       CMR = 9999
       Prescale value is 1 : prescaler= 2
    */

	
	uint16_t PWM_PSC = 1;		
	uint16_t CNR = 0;
	uint16_t CMR = 0;	

//	if (PWM_DUTY_1000 >= 1000)
//	{
//		PWM_DUTY_1000 = 0;
//	}

	CNR = PWM_CLOCK/(PWM_PSC+1)/PWM_FREQ - 1;	//299 , PWM clock source frequency/prescaler/PWM output frequency
	CMR = ((CNR+1)*PWM_DUTY_1000/1000) - 1;	//CMR = (CNR+1)*(PWM_DUTY_1000/1000)-1

	printf("CNR : %4d , CMR : %4d , TARGET = %4d\r\n" , CNR , CMR , PWM_DUTY_1000);

	PWM_SET_PRESCALER(PWM0, 0, PWM_PSC);
    PWM_SET_CNR(PWM0, 0, CNR);
    PWM_SET_CMR(PWM0, 0, CMR);


}

void PWM0_CH0_Init(void)
{
    SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA12_Msk);
    SYS->GPA_MFP |= SYS_GPA_MFP_PA12_PWM0_CH0;


    PWM_ConfigOutputChannel(PWM0, 0, PWM_FREQ, 0);
    PWM_EnableOutput(PWM0, PWM_CH_0_MASK);

    /* Start PWM module */
    PWM_Start(PWM0, PWM_CH_0_MASK);

}

void TMR3_IRQHandler(void)
{
	static uint32_t LOG = 0;
	static uint16_t CNT = 0;
	static uint16_t CNT_LED = 0;


    if(TIMER_GetIntFlag(TIMER3) == 1)
    {
        TIMER_ClearIntFlag(TIMER3);

		if (CNT_LED++ >= 500)
		{		
			CNT_LED = 0;
			PWM0_DUTY_CHANGE(PWM_DUTY_1000);

			if (PWM_DUTY_1000++ == 1000)
			{
				PWM_DUTY_1000 = 1000;	//stop at 100%
			}
		}

		if (CNT++ >= 1000)
		{		
			CNT = 0;
        	printf("%s : %4d\r\n",__FUNCTION__,LOG++);
		}
    }
}

void BasicTimer_TIMER3_Init(void)
{
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER3);
    NVIC_EnableIRQ(TMR3_IRQn);	
    TIMER_Start(TIMER3);
}


void GPIO_Init(void)
{
    GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);	//led TEST
	PA12 = 0;	
}


void UART0_Init(void)
{
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);

    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);

	/* Set UART receive time-out */
	UART_SetTimeoutCnt(UART0, 20);

	printf("\r\nCLK_GetCPUFreq : %8d\r\n",CLK_GetCPUFreq());
	printf("CLK_GetHCLKFreq : %8d\r\n",CLK_GetHCLKFreq());	
	printf("CLK_GetPCLK0Freq : %8d\r\n",CLK_GetHXTFreq());
	printf("CLK_GetPCLK1Freq : %8d\r\n",CLK_GetPCLKFreq());

	
}

int main()
{
    SYS_Init();

    UART0_Init();

	GPIO_Init();

	PWM0_CH0_Init();
			
	BasicTimer_TIMER3_Init();

    while(1)
    {

    }
}

/*** (C) COPYRIGHT 2018 Nuvoton Technology Corp. ***/
