/////////////////////////////////////////////////////////////////// 
//Student ID No.: 815006656
//Date: 30/10/2017
///////////////////////////////////////////////////////////////////
#include "includes.h" 
#include <timers.h> 
#include <xlcd.h>
#include <delays.h>
/* Write the appropriate code to set configuration bits: 
 * - set HS oscillator * - disable watchdog timer 
 * - disable low voltage programming */
/* Write LCD delay functions 
 */ 
/*Set configuration bits for use with ICD2*/
#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 4000000

//////////////////////////////////////////// GLOBAL VARIBLES /////////////////////////////////int stopped;

int stopped = 0;

// functions required for XLCD
// min of 18 Tcy
extern void DelayFor18TCY(void)
{
    Delay10TCYx(2);
}

// min of 15ms
extern void DelayPORXLCD(void)
{
    Delay1KTCYx(15);
}

// min of 5ms
extern void DelayXLCD(void)
{
    Delay1KTCYx(5);
}

/* Write the appropriate code to do the following: 
 * define the stack sizes for task1 and task2 in app_cfg.h file 
 * use the defines to assign stack sizes here. 
 */

OS_STK TaskAStk[256u]; 
OS_STK TaskBStk[256u];

/* Write the appropriate code to do the following: 
 * Configure PORTB pin 1 as an output 
 * TaskA will loop until the global variable stopped is set. 
 * Within the loop -- print string "Task 1 rocks! \n" to LCD top row 
 * -- toggle PORTB pin 1 * -- delay for 1 second using OSTimeDlyHMSM( ) 
 * TaskA will delete itself if the loop is exited. 
 */ 

void TaskA(void *pdata) 
{ 
    TRISBbits.RB0 = 0;                                                          //sets PORTB pin 0  as output
    
    while(stopped != 1)                                                         //loops until stopped is set
    {
        PORTBbits.RB0 = 0;                                                          //sets RB0 pin low
        PORTBbits.RB1 = 0;                                                          //sets RB1 pin low
        
        while(BusyXLCD());
        WriteCmdXLCD(0x01);                                                         //clears display
        
        while( BusyXLCD());                                                          //waits for the LCD to not be busy
        SetDDRamAddr (0x00);                                                         //sets the pointer at the ddaddr 
        while(BusyXLCD());
        putrsXLCD("Task 1 rocks!");
        PORTBbits.RB0 = 1;
        OSTimeDlyHMSM(0,0,1,0);
    } 
    OSTaskDel(OS_PRIO_SELF);   
}

/* Write the appropriate code to do the following: 
 * Configure PORTB pin 2 as an output 
 * TaskB will loop until the global variable stopped is set. 
 * Within the loop -- print string "Task 2 rules?\n" to LCD bottom row 
 * -- toggle PORTB pin 2 * -- delay of 200 ticks using OSTimeDly( ) 
 * TaskB will delete itself if the loop is exited. 
 */ 

void TaskB(void *pdata) 
{ 
    TRISBbits.RB1 = 0;                                                          //sets PORTB pin 0  as output
    
    while(stopped != 1)                                                         //loops until stopped is set
    {
        
        PORTBbits.RB0 = 0;                                                      //sets RB0 pin low
        PORTBbits.RB1 = 0;                                                      //sets RB1 pin low
        
        while(BusyXLCD());
        WriteCmdXLCD(0x01);                                                     //clears display
        
        while( BusyXLCD());                                                     //waits for the LCD to not be busy
        SetDDRamAddr (0x50);                                                    //sets the pointer at the bottom row
        while(BusyXLCD());
        putrsXLCD("Task 2 rocks!");
        PORTBbits.RB1 = 1;
        OSTimeDly(200);
        
    }
    
    OSTaskDel(OS_PRIO_SELF);
}

void main (void)
{ 
    // Write the appropriate code to disable all interrupts 
    INTCONbits.GIE = 0;
    
    // Write the appropriate code to initialize uC/OS II kernel
    OSInit();
    
/* Write the appropriate code to enable timer0 , 
 * using 16 bit mode on internal clk source and pre-scalar of 1. 
 */
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_1);
    
/* Write the appropriate code to set timer0 registers 
 * such that timer0 expires at 10ms using 4 Mhz oscillator. 
 * Do the same in vectors.c in CPUlowInterruptHook( ). 
 */
    WriteTimer0(-10000);                                                         // (required time)/(4 * 1/oscillator * pre scaler)
    
/* Write the appropriate code to define the priorities for taskA 
 * and taskB in app_cfg.h. Use these defines to assign priorities 
 * when creating taskA and taskB with OSTaskCreate( ) 
 */
  OSTaskCreate(TaskA, (void *)0, &TaskAStk[0], 0);
  OSTaskCreate(TaskB, (void *)0, &TaskBStk[0], 1);
    
/// Initialize the LCD display 
    while( BusyXLCD() ); 
    OpenXLCD( FOUR_BIT & LINES_5X7);                                            //configure LCD to 4 bit mode & 2 line display
    while( BusyXLCD() ); 
    WriteCmdXLCD( FOUR_BIT & LINES_5X7 ); 
    while( BusyXLCD() ); 
    WriteCmdXLCD( BLINK_ON ); 
    while( BusyXLCD() ); 
    WriteCmdXLCD( SHIFT_DISP_LEFT );
    while( BusyXLCD());                                                         //waits for the LCD to not be busy
    SetDDRamAddr (0x40);                                                        //sets the pointer at the top row 
    putrsXLCD ("LCD Init");                                                     //writes the sting of characters to the LCD
    //OSTimeDlyHMSM(0,2,0,0);                                                   //wait 3 seconds
    //Delay1KTCYx(200);
    SetDDRamAddr (0x10);                                                        //sets the pointer at the top row 
    while(BusyXLCD());
    putrsXLCD ("Completed");                                                    //writes the sting of characters to the LCD
    
    while(BusyXLCD());
    WriteCmdXLCD(0x01);                                                         //clears display
    
 // Write the appropriate code to start uC/OS II kernel
    OSStart();
}

