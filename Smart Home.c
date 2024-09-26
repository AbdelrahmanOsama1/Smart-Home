
// Smart_Home_Cfg.h




#ifndef SMART_HOME_CFG_H_
#define SMART_HOME_CFG_H_


#define NUM_OF_INSTRUCTIONS 6
#define  MAX_NUM_OF_BUFFER_ELEMENTS 15
#define LED_BK 0
#define LED_R 3
#define LED_G 4
#define LED_B 5
#define LED_Y 6
#define LED_W 2



#endif /* SMART_HOME_CFG_H_ */




/************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************/


// Smart_Home_Int.h




#ifndef SMART_HOME_H_
#define SMART_HOME_H_


typedef struct{
	
	u8*instruction;
	void (*Pfunction)(u8);
}UARTtask_t;




#define  STRING_END '#'
#define T_FALSE 0
#define T_TRUE 1
#define T_WAIT 2

#define RED_SUM 46
#define GREEN_SUM 100
#define BLUE_SUM 199
#define YELLOW_SUM 157
#define BLACK_SUM 253

void RGB_LED(u8 num);
//void LED_ON(u8 num);
//void LED_OFF(u8 num);
void LCD_ON(u8 num);
void RELAY_ON(u8 num);
void RELAY_OFF(u8 num);
void MOTOR_ON(u8 num);
void MOTOR_OFF(u8 num);
void Text_Split(u8*text);
void Instruction_Compare_Setter(u8 value);
u8 Instruction_Compare_Getter(void);
void instruction_compare(void);
static void RX_Text(void);
void Receive_Text(void);
void Perform_Instruction(void);
void Smart_Home_Init(void);
void Smart_Home_Runnable(void);
//void UART_Receive_Instruction(void);



#endif /* SMART_HOME_H_ */



/************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************/


// Smart_Home_Prg.c


#include "Std_Types.h"
#include "Utils.h"
#include "UART_Int.h"
#include "UART_Services.h"
#include "Smart_Home_Cfg.h"
#include "Smart_Home_Int.h"
#include "DIO_Int.h"
#include "LCD_Int.h"
#include "ECU_Int.h"
#include "MOTOR_Int.h"
#include "RGB_Int.h"
#include "Timers.h"



static u8 buffer[MAX_NUM_OF_BUFFER_ELEMENTS];
static u8 Text_Done_Flag = T_FALSE , Number=0 , last_index , split_index , Compare_Flag=T_WAIT , instruction_index;
static u8 instruction_text[MAX_NUM_OF_BUFFER_ELEMENTS];



void RGB_LED(u8 num){
	
	
	if (num == RED_SUM)
	{
		num = LED_R;
	}
	else if (num == GREEN_SUM)
	{
		num = LED_G;
	}
	else if (num == BLUE_SUM)
	{
		num = LED_B;
	}
	else if (num == YELLOW_SUM)
	{
		num = LED_Y;
	}
	else if (num == BLACK_SUM)
	{
		num = LED_BK;
	}
	else{
		num = LED_W;
	}
	
	RGB_Change_Colors(num);
}


void LCD_ON(u8 num){
	
	LCD_WriteNumber(num);
	
}


void RELAY_ON(u8 num){
	
	RELAY_On(num);
	
}


void RELAY_OFF(u8 num){
	
	RELAY_Off(num);
	
}


void MOTOR_ON(u8 num){
	
	MOTOR_CW(num);
	
}


void MOTOR_OFF(u8 num){
	
	MOTOR_Stop(num);
	
}



UARTtask_t arr[NUM_OF_INSTRUCTIONS]= { {"@rgb" , RGB_LED },
									 {"@lcd" , LCD_ON },
									 {"@relayON" , RELAY_ON },
									 {"@relayOFF" , RELAY_OFF },
									 {"@motorON" , MOTOR_ON },
									 {"@motorOFF" , MOTOR_OFF }
};




void Text_Split(u8*text){
	
	u8 i=0;
		
	for (i=0; (text[i] != STRING_END) ;i++)
	{
		
		if (i<split_index)  // split element
		{
			instruction_text[i]=text[i];
		}
		else if (i==split_index){
			
			instruction_text[i]=0;   // null
			
		}
		else if (i>split_index){
			
			Number= (Number*10) + (text[i]-'0');
		}
		
	}
	

}



void Instruction_Compare_Setter(u8 value){
	Compare_Flag = value;
	Number=0;  // reset number
}



u8 Instruction_Compare_Getter(void){
	return Compare_Flag;
}



void instruction_compare(void){
	
	u8 i,j,local_compare_flag;
	
	local_compare_flag = T_TRUE;
	
	for (i=0 ; i<NUM_OF_INSTRUCTIONS ; i++)
	{
		
		for (j=0 ; (instruction_text[j] || (arr[i].instruction)[j]) ; j++)
		{
			
			if (instruction_text[j] != arr[i].instruction[j])
			{
				local_compare_flag = T_FALSE;
				break;
			}
			
			else if (instruction_text[j] == arr[i].instruction[j])
			{
				local_compare_flag = T_TRUE;
			}
			
		}
		
		
		if (local_compare_flag == T_TRUE)  // instruction found
		{
			instruction_index = i;
			break;
		}
	}
		Compare_Flag = local_compare_flag;
}




static void RX_Text(void){
	
	u8 static counter=0;
	u8 static enable_flag=0;
	
	
	buffer[counter] = UART_ReceiveNoBlock();
	
	if (buffer[counter] == '@')
	{
		enable_flag=1;
	}
	
	if (enable_flag==1)   // start character has come
	{
		
		if (buffer[counter] == '-')  // split element
		{
			split_index = counter;
		}
		
		counter++;


		if (counter == MAX_NUM_OF_BUFFER_ELEMENTS-1)   // close to exceed num of buffer elements (counter = last element index)
		{
			buffer[counter] = STRING_END;
			last_index = counter;
			Text_Done_Flag = T_TRUE ;
			counter=0;
			enable_flag=0;
		}
		
		else if (buffer[counter-1] == STRING_END)  // TEXT DONE
		{
			last_index = counter-1;
			Text_Done_Flag = T_TRUE ;
			counter=0;
			enable_flag=0;
		}
		
		
		
		if (Text_Done_Flag == T_TRUE )   // already receive text
		{
			Text_Done_Flag = T_FALSE;
			Text_Split(buffer);
			instruction_compare();
			
		}
		
	}
	
	
	
}



void Receive_Text(void){
	
	UART_RX_InterruptEnable();
	UART_RX_SetCallBack(RX_Text);
}


void Perform_Instruction(void){
	arr[instruction_index].Pfunction(Number);
}


void Smart_Home_Init(void){
	
	DIO_Init();
	LCD_Init();
	UART_Init();

	GLOBAL_INTERRUPT_ENABLE();
	
	
	
	TIMER0_Init(TIMER0_FASTPWM_MODE,TIMER0_SCALER_8);
	TIMER0_OC0Mode(OC0_INVERTING);

	Timer1_Init(TIMER1_FASTPWM_ICR_TOP_MODE,TIMER1_SCALER_8);
	Timer1_OCRA1Mode(OCRA_INVERTING);
	Timer1_OCRB1Mode(OCRB_INVERTING);
	RGB_Change_Colors(LED_W);
	ICR1 = 255;
	
	
	Receive_Text();
	
}



void Smart_Home_Runnable(void){
	
	
	if (Instruction_Compare_Getter() == T_TRUE)   // instruction found
	{
		LCD_SetCursor(2,1);
		Perform_Instruction();
		LCD_WriteString("   ");
		LCD_SetCursor(1,1);
		LCD_WriteString("DONE");
		LCD_WriteString("            ");
		Instruction_Compare_Setter(T_WAIT);
	}
	
	else if (Instruction_Compare_Getter() == T_FALSE)   // instruction not found
	{
		LCD_SetCursor(1,1);
		LCD_WriteString("NOT FOUND");
		LCD_WriteString("       ");
		LCD_SetCursor(2,1);
		LCD_WriteString("       ");
		Instruction_Compare_Setter(T_WAIT);
	}
	
	
	else{
		// do nothing
	}
	
	
}



/************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************/
/************************************************************************************************************************************************************************/


// main.c



#define F_CPU 8000000
#include <util/delay.h>

#include "Std_Types.h"
#include "Memory_Map.h"
#include "Utils.h"
#include "DIO_Int.h"
#include "MOTOR_Int.h"
#include "ECU_Int.h"
#include "MOVE_Int.h"
#include "STEPPER_Int.h"
#include "LCD_Int.h"
#include "KEYPAD_Int.h"
#include "ADC_Int.h"
#include "SENSORS_Int.h"
#include "MeanTempFilter_Int.h"
#include "FIRING_SYSTEM_Int.h"
#include "EXT_INTERRUPT_Int.h"
#include "Timers.h"
#include "RGB_Int.h"
#include "UART_Int.h"
#include "UART_Services.h"
#include "Smart_Home_Int.h"


int main(void)
{
	DIO_Init();
	LCD_Init();
	UART_Init();
	
	Smart_Home_Init();
	
	while (1)
	{

	Smart_Home_Runnable();
	
	}

	
}