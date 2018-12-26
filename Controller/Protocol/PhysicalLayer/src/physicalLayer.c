/*********************************************************************
** 
**
**
** Ths script defines the physical Layer of the KPW2000 Protocol
**
**
**
*********************************************************************/

#include <avr/interrupt.h>
#include <stdint.h>
#include <util/delay.h>

#include <error_defs.h>
#include "../include/physicalLayer.h"

void start_Transmit();
void start_Receive();
void init_timer0();
void init_timer1();


volatile struct {
	unsigned char buffer;
	unsigned char load;
	unsigned char bit_cnt;
	unsigned char status;
	unsigned char recieve;
	unsigned char error;
	unsigned char byte_buffer;
	int bitRate;
}helper;

void wake_up() {
	// Wait min Idle Time
	wait_idle(TIME_TidleFirst);

	// Wake up pattern
	set_K_low();
	set_L_low();
	timer1_start();
	while (timer1_get() < TIME_TiniL);
	timer1_stop();

	set_K_high();
	set_L_high();
	timer1_start();
	while (timer1_get() < TIME_TiniH);
	timer1_stop();
}

void start_Transmit() {
	SETSTATUS(SENDING);
	disable_INT0();

	helper.error = CODE_OK;
}

void start_Receive() {
	CLEARSTATUS(SENDING);
	SETSTATUS(BUSY);

	helper.error = CODE_OK;

	enable_INT0();
}

/*********************************************************************
** Initialise Hardware
**	Set Outputs and Inputs
*********************************************************************/
void init_physicalLayer(){
	K_Line_REG |= (1<<K_Line_OUT);
	// High on idle
	set_K_high();

	K_Line_REG &= ~(1<<K_Line_IN);

	L_Line_REG |= (1<<L_Line_OUT);
	// High on idle
	set_L_high();

	sei();

	init_timer0();
	init_timer1();

	DDRD &= ~(1 << DDD2);
	PORTD |= (1 << PD2); // Activate pull up
	EICRA = (1 << ISC00);	//Set INT0 to occur on either edge
	disable_INT0();
}

void init_timer0() {
	TCCR0A = 0x00;
	// Prescaler will start and change TCCR0B
	TCCR0B = 0x00;

	// Disable Interrupts
	TIMSK0 = 0x00;
}

void init_timer1() {

	TCCR1A = 0x00;

	// Prescaler will start and change TCCR1B
	TCCR1B = 0x00;

	TCCR1C = 0x00;

	// Disablke Interrupts
	TIMSK1 = 0x00;
}
/*********************************************************************
** Send byte
** TIMER0 is used to time bit lengths
**
** First bit is send in this method
** Further bits are send in TIMER0 Interrupt
*********************************************************************/
unsigned char send_byte(unsigned char byte, int bitRate){	
	// Set status bit to Transmit (not Recieve)
	start_Transmit();

	SETSTATUS(BUSY);

	helper.load = CALC_TIMER0_LOAD(bitRate); // Loading value for TIMER0 to time bit length
	helper.buffer = byte; // Buffer data to handle in interrupt
	helper.bit_cnt = 10; // Send 1 start bit, 8 data bit, 1 stop bit
	
	EIMSK = 0;  // disable INT0   	
	timer0_set(helper.load);

  	set_K_low(); // Start bit
	
	while (CHECKSTATUS(BUSY));

	return helper.error;
}

/*********************************************************************
** Receive byte
** TIMER0 is used to time bit lengths
** TIMER1 checks for timeouts
**
** First bit is send in this method
** Further bits are send in TIMER0 Interrupt
*********************************************************************/
unsigned char receive_byte(int bitRate) {
	helper.bitRate = bitRate;
	helper.load = CALC_TIMER0_LOAD(bitRate); // Loading value for TIMER0 to time bit length
	
	start_Receive();
	// Maximum Waiting time is P2_MAX - Start TIMER1
	timer1_set(TIME_P2max);
	while (CHECKSTATUS(BUSY)); // Wait for received byte
	
	incoming_byte = helper.byte_buffer;

	return helper.error;
}

/*********************************************************************
** Wait given time
** If Bus not idle the whole time, restart waiting
*********************************************************************/
void wait_idle(unsigned int TMP_TIME){
	timer1_start();
	while(timer1_get()<TMP_TIME){
		if(!(is_K_high())){
			timer1_set(0);
		}
	}
	timer1_stop();
}

/*********************************************************************
** Wait given time
*********************************************************************/
void wait(unsigned int TMP_TIME) {
	timer1_start();
	while (timer1_get() < TMP_TIME);
	timer1_stop();
}

/*********************************************************************
** Interrupt Service Routine (ISR) for TIMER0
** Handle byte transmissions bit by bit
*********************************************************************/

ISR(TIMER0_OVF_vect){ 
	// Reset Timer to its loading value
	TCNT0 = helper.load;

	// Reduce bit counter
	--helper.bit_cnt;

	if(CHECKSTATUS(SENDING)){
		
		// Send next bit
		if(helper.bit_cnt>1 && helper.bit_cnt<=9){
			if( helper.buffer&0x01 ) // check LSB status   
			{   
				set_K_high();  // send 1 bit   
			}   
			else   
			{    
				set_K_low();  // send 0 bit   
			}   
			helper.buffer = helper.buffer>>1;  // next bit 
		}
		else{
			// Stop bit - high
			set_K_high();
		}
		// After stop bit is send:
		// Disable Timer0 interrupts
		// Set free BUS communication
		if(helper.bit_cnt<1){
			 
			timer0_stop();
			
			CLEARSTATUS(BUSY);
		}
	}
	
	if (!CHECKSTATUS(SENDING)) {
		
		if (helper.bit_cnt >= 10 && is_K_high()){
			// Start bit wrong: Cancel receiving and set error
			CLEARSTATUS(BUSY);
			
			timer0_stop();
			
			helper.error = CODE_BUS_ERROR_START;
		}
		else if (helper.bit_cnt < 10 && helper.bit_cnt >= 2) {
			// Shift to receive next bit
			helper.byte_buffer >>= 1;
			// Read each bit
			if (is_K_high()) {
				helper.byte_buffer |= 0x80;
			}
		}
		else if (helper.bit_cnt == 1) {
			CLEARSTATUS(BUSY);
			timer0_stop();
			if (!is_K_high()) {
				// Stop bit wrong: Set error
				helper.error = CODE_BUS_ERROR_STOP;
			}
		}
	}
}


ISR(TIMER1_OVF_vect) {
	if (!CHECKSTATUS(SENDING)) {
		// If message was about to be received -> time is up
		timer1_stop();
		helper.error = CODE_NO_DATA;
		CLEARSTATUS(BUSY);
	}
}


ISR(INT0_vect) {
	timer1_stop();

	timer0_set(CALC_TIMER0_LOAD(FAST_INIT_BITRATE * 2)); // Set timer to half a bit time
	// Reset incoming struct
	helper.bit_cnt = 11; // One more to compensate for double reading of start bit
	helper.byte_buffer = 0x00;

	// Disable INT0 interrupt
	disable_INT0();
}