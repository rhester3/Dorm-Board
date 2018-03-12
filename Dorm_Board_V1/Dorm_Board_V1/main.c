/*
 * Dorm_Board_V1.c
 *
 * Created: 8/1/2017 12:41:45 PM
 * Author : Riley Hester
 */ 

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <stdbool.h>

/*	PORTS	*/

#define data_port PORTB		//Data Port
#define data_DDR DDRB		//Data DDR

#define signal_port PORTC	//Signal Port
#define signal_DDR	DDRC	//Signal DDR

#define RS PORTC0			//Register Select
#define RW PORTC1			//Read/Write
#define E PORTC2			//Enable
#define RST PORTC3			//Reset

/*	CONSTANTS	*/
//#define F_CPU 1000000UL
#define F_CPU 20000000UL
//#define FOSC 1843200		//Clock Speed
#define BAUD 9600
//#define MYUBRR FOSC/16/BAUD-1
#define BAUD_PRESCALER (((F_CPU / (BAUD * 20UL))) - 1)
#define Line1 0x80
#define Line2 0x90
#define Line3 0x88
#define Line4 0x98

/*	COMMANDS	*/
#define LCD_clear 0x01		//Clear LCD
#define LCD_8bit 0x30		//8-bit Mode
#define LCD_cursorOn 0x0f	//Cursor Blinking
#define LCD_home 0x80		//Cursor to Beginning
#define LCD_entry 0x06		//Entry Mode

void LCD(unsigned char ch);
void LCDstring(const char string[]);
void LCDcmd(int ch);
void LCDinit(void);
void USART_Init( void );
void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );
void CursorSet(char position);

char character_count = 0;
char address_counter = 0;
bool second_time = false;

/*	CODE BEGIN	*/
int main(void)
{
	const char string[] = "This is a testing message. But it needs to be much longer to test fully. Do you understand?";
	
	data_DDR=0xff;
	signal_DDR=0xff;
	_delay_ms(2000);
	signal_port=(1<<RST);
	LCDinit();
	USART_Init();
	//USART_Init(9600);
	
	_delay_ms(15000);
	while(1)
	{
		USART_Transmit('R');
		//_delay_ms(1000);
	}
	
	//LCDstring(string);
	
	/*
	LCD('H');
	LCD('e');
	LCD('l');
	LCD('l');
	LCD('o');
	*/
	return 0;
}

void LCD(unsigned char ch)
{
	data_port=ch;	//Put value in port
	signal_port=(0<<RW)|(1<<RS)|(1<<E);	//Send data
	_delay_ms(10);
	signal_port=(0<<RW)|(1<<RS)|(0<<E);
	_delay_ms(45);
}

void LCDstring(const char string[])
{
	unsigned char length = strlen(string);
	unsigned char i = 0;
	const char *ptr = string;
	character_count = 0;
	second_time = false;
	for(i = 0;i < length;i++)
	{
		if(i == 16)
		{
			CursorSet(Line2);
		}
		else if(i == 32)
		{
			CursorSet(Line3);
		}
		else if(i == 48)
		{
			CursorSet(Line4);
		}
		LCD(string[i]);
	}
}

void LCDcmd(int ch)
{ 
	data_port=ch;
	signal_port=(0<<RW)|(0<<RS)|(1<<E);		//Signaling to send commands
	_delay_ms(10);
	signal_port=(0<<RW)|(0<<RS)|(0<<E);
	_delay_ms(45);
	//if(ch==0x01||ch==0x02)			// If commands are LCD Clear or
	// LCDSeek00, delay 2mS
	_delay_ms(2);
}

void LCDinit(void)
{
	LCDcmd(LCD_8bit);
	LCDcmd(LCD_8bit);
	LCDcmd(LCD_cursorOn);
	LCDcmd(LCD_clear);
	_delay_ms(15);
	LCDcmd(LCD_entry);
}

void USART_Init( void )
{
	/*Set baud rate */
	UBRR0H = (unsigned char)(BAUD_PRESCALER>>8);
	UBRR0L = (unsigned char)(BAUD_PRESCALER);
	/*Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = 3<<UCSZ00;
	
	/* Set frame format: 8data, 2stop bit */
	//UCSR0C = (1<<USBS0)|(3<<UCSZ00);
}

void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}

void CursorSet(char position)
{
	data_port = position;
	signal_port=(0<<RW)|(0<<RS)|(1<<E);
	_delay_ms(10);
	signal_port=(0<<RW)|(0<<RS)|(0<<E);
	_delay_ms(45);
}