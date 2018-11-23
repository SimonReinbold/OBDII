/*********************************************************************
** 
**
**
** Ths script defines the physical Layer of the KPW2000 Protocol
**
**
**
*********************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "../include/physicalLayer.h"

unsigned char send_byte(unsigned char data);
void wait(unsigned int TMP_TIME);

/*********************************************************************
** Initialise Hardware
**	Set Outputs and Inputs
*********************************************************************/
void obd_hardware_init(){
	K_Line_REG |= (1<<K_Line_OUT);
	// High on idle
	set_K_high();

	K_Line_REG &= ~(1<<K_Line_IN);

	L_Line_REG |= (1<<L_Line_OUT);
	// High on idle
	set_L_high();

	sei();
}

/*********************************************************************
** Initialise Communication using "Fast Init"
*********************************************************************/
unsigned char obd_fast_init(){
	// Wait min Idle Time
	wait(TIME_TidleFirst);

	// Wake up pattern
	set_K_low();
	set_L_low();
	timer1_start();
	while(timer1_get()<TIME_TiniL);
	timer1_stop();

	set_K_high();
	set_L_high();
	timer1_start();
	while(timer1_get()<TIME_TiniH);
	timer1_stop();

	// Send Start Communication Request
	//
	//**								Functinal Addressing | Target | Source | Service ID | Checksum
	//** Start Communication Pattern:           0xC1         |  0x33  |  0xF1  |   0x81     |   TBD
	
	unsigned char data[4];
	data[0] = 0xC1;
	data[1] = 0x33;
	data[2] = 0xF1;
	data[3] = 0x81;
	
	if(send_data(data,4) != CODE_OK){
		return CODE_ERROR;
	}
	
	// Receive Section
	// External interrupts are already activated at the end of the TIMER0 ISR

	// Maximum Waiting time is P2_MAX - Start TIMER1
	timer1_start();

	unsigned char byte_cnt = 0;
	unsigned char msg_buffer[10]; // Filled by INT0 interrupt
	unsigned char *msg_pointer;	// Used for parsing the received data
	
	return CODE_OK;
}

/*********************************************************************
** Send message
*********************************************************************/
char send_data(unsigned char *data, unsigned char n_bytes){
	if (n_bytes > 265) return CODE_DATA_ERROR;
	
	// Set status bit to Transmit (not Recieve)
	SETSTATUS(SENDING);
	// Calculate checksum while sending byte by byte
	unsigned char checksum;
	for (unsigned int i = 0; i < n_bytes; i++) {
		checksum += *data;
		send_byte(*data++);
		// Halt programm until byte is send
		while (CHECKSTATUS(BUSY));
	}
	// Send last byte - checksum
	send_byte(checksum);
	
	// Halt programm until byte is send
	while (CHECKSTATUS(BUSY));
	CLEARSTATUS(SENDING);
	
	return CODE_OK;
}

/*********************************************************************
** Send byte
** TIMER0 is used to time bit lengths
**
** First bit is send in this method
** Further bits are send in TIMER0 Interrupt
*********************************************************************/
unsigned char send_byte(unsigned char data){
	SETSTATUS(BUSY);

	helper.buffer = data; // Buffer data to handle in interrupt
	helper.bit_cnt = 10; // Send 1 start bit, 8 data bit, 1 stop bit
	helper.reload = LOAD_TIMER0_10400BAUD; // Loading value for TIMER0 to time bit length
	
	EIMSK = 0;  // disable INT0   
	TIFR0 = (1<<TOIE0);  // clear timer0 ovrf interrupt flag         
  	TIMSK0 = (1<<TOIE0);  // enable timer0 ovrf interrupt 
	
	timer0_set(helper.reload);

  	set_K_low(); // Start bit
}



/*********************************************************************
** Wait given time
** If Bus not idle the whole time, restart waiting
*********************************************************************/
void wait(unsigned int TMP_TIME){
	timer1_start();
	while(timer1_get()<TMP_TIME){
		if(!(is_K_high())){
			timer1_set(0);
		}
	}
	timer1_stop();
}

/*********************************************************************
** Interrupt Service Routine (ISR) for TIMER0
** Handle byte transmissions bit by bit
*********************************************************************/

ISR(TIMER0_OVF_vect){ 
	// Reset Timer to its loading value
	
	TCNT0 = helper.reload;

	// Reduce bit counter
	--helper.bit_cnt;

	if(CHECKSTATUS(SENDING)){
		// Send next bit
		if(helper.bit_cnt>0 && helper.bit_cnt<=9){
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
		// Disable Timer0 interrupts and enable external interrupts
		// Set free BUS communication
		if(helper.bit_cnt<=0){
			TIMSK0 = 0;  // disable timer0 interrupts 
			timer0_stop();
			EIFR = (1<<INTF0);  // clear INT0 flag   
          	EIMSK = (1<<INT0);  // enable INT0    
          	CLEARSTATUS(BUSY);
		}
	}
	
}