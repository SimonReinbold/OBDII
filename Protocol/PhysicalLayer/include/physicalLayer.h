#ifndef __OBDCOM__ 
#define __OBDCOM__ 

/*********************************************************************
** Outputs and Inputs
*********************************************************************/

// Uncomment if Logic is not to be negated
// For now IN and OUT are negated simultaneously
#define NEGATE_LOGIC

#define K_Line_REG DDRC
#define K_Line_PORT PORTC
#define K_Line_PIN PINC
#define K_Line_OUT PC5
#define K_Line_IN PC4

#define L_Line_REG DDRC
#define L_Line_PORT PORTC
#define L_Line_OUT PC2

/*********************************************************************
** Confirmation Values
*********************************************************************/

#define CODE_OK 0x00
#define CODE_DATA_ERROR 0x01
#define CODE_ERROR 0xEE

/*********************************************************************
** Prescalers to write inte uc register
** Atmega328p: TCCR0B, CS0[2:0]
*********************************************************************/

#define PRESCALER_1    1 
#define PRESCALER_8    2 
#define PRESCALER_64   3 	
#define PRESCALER_256  4 	
#define PRESCALER_1024 5 

/*********************************************************************
** TIMER0 is used for timing bit transmission - time for holding a bit
**	Baudrate up to 10400 baud/s -> 10400 bit/s -> 1 bit = 96us
**	8 bit timer -> max value = 256
** 
** TIMER1 is used for all other timings
**	tmax = 300ms (wait for bus idle time fo 5 baud init)
**	16 bit timer -> max value = 65536
*********************************************************************/

#define TIMER0_Prescaler PRESCALER_8	// leads to 0.5us resolution @ 16MHz clock
										// TIMER0 limit = 0.5us*256 = 128us

#define TIMER1_Prescaler PRESCALER_64	// leads to 4us resolution @ 16MHz clock
										// TIMER1 limit = 4us*65536 = 262.144ms

/*********************************************************************
** Transform time values to its TIMER1 tick value
** For use either load the timer with: max-X
** or run the timer from 0 and compare its value to the calculated number X
**
** In claculation 1 the value 64 stands for the Prescaler of TIMER1
**	hence TIMER1 has to be used for timer measuring
**
** In calculation 2 the TIMER0 load value for 10400 baud/s transmission is calculated
**	there the value 8 stands for the TIMER0 Prescaler
*********************************************************************/

#define ms2cnt(ms) (uint16_t)((uint32_t)ms/1000*((uint32_t)MCU_XTAL/64))

#define LOAD_TIMER0_10400BAUD (uint8_t)(265-((uint32_t)MCU_XTAL/((uint32_t)10400*(uint32_t)8)))

/*********************************************************************
** Define standardized time values extracted from ISO-14230 Part 2
** Already converted into TIMER1 values
** All values used are defaults from ISO-14230 Part 2
*********************************************************************/

/**** Normal Operation ****/

// Inter byte time for ECU response
#define TIME_P1min ms2cnt(0)		
#define TIME_P1max ms2cnt(20)
							
// Time between tester request and ECU response or two ECU responses
//	default used -> correct value has to be entered through keybytes
#define TIME_P2min ms2cnt(25)	
#define TIME_P2max ms2cnt(50)

// Time between end of ECU responses and start of new tester request
//	ISO-14230 Part 2 specifies:
//	P3min > P4min !
#define TIME_P3min ms2cnt(55)
#define TIME_P3max ms2cnt(5000)	// WARNING: NOT COUNTABLE WITH TIMER1 -> no use of this value

// Inter byte time for tester request
#define TIME_P4min ms2cnt(5)		
#define TIME_P4max ms2cnt(20)
	
/**** 5 Baud Init ****/

// Time from end of the address byte to start of synchronisation pattern
#define TIME_W1min ms2cnt(60)
#define TIME_W1max ms2cnt(300)	// WARNING: NOT COUNTABLE WITH TIMER1 -> no use of this value

// Time from end of the synchronisation pattern to the start of key byte 1
#define TIME_W2min ms2cnt(5)
#define TIME_W2max ms2cnt(20)	

// Time between key byte 1 and key byte 2
#define TIME_W3min ms2cnt(0)
#define TIME_W3max ms2cnt(20)	

// Time between key byte 2 (from the ECU) and its inversion from the tester. Also the time
//	from the inverted key byte 2 from the tester and the inverted address from the ECU
#define TIME_W4min ms2cnt(25)
#define TIME_W4max ms2cnt(50)	

// Time before the tester starts to transmit the address byte
#define TIME_W5max ms2cnt(300)	// WARNING: NOT COUNTABLE WITH TIMER1 -> no use of this value

/**** Fast Init ****/

// Min and Max values are neglected
#define TIME_TiniL ms2cnt(25)				// Time for Wakeup Pattern Low
#define TIME_TiniH ms2cnt(25)				// Time for Wakeup Pattern High

#define TIME_TidleFirst ms2cnt(TIME_W5max)	// Idel time after power on
#define TIME_TidleStop ms2cnt(TIME_P3min)	// Idel time after completion of StopCommunication service
#define TIME_TidleStop2 ms2cnt(TIME_P3max)			// Idel time after stopping communication by time-out P3max

/*********************************************************************
** Functions
*********************************************************************/

void obd_hardware_init(void);
unsigned char obd_fast_init(void);
char obd_5_baud_init(void);
char send_data(unsigned char *data, unsigned char n_bytes);

// General
static inline void set_high(unsigned int TMP_PORT, unsigned int TMP_PIN){
    TMP_PORT |= (1<<TMP_PIN);
}

static inline void set_low(unsigned int TMP_PORT, unsigned int TMP_PIN){
    TMP_PORT &= ~(1<<TMP_PIN);
}

// Timers 
static inline void timer0_start() 
{ 
    TCCR0B = TIMER0_Prescaler; 
    TCNT0 = 0; 
} 
 
static inline void timer0_stop(void) 
{ 
    TCCR0B = 0x00; 
} 
 
static inline void timer0_set(unsigned char val) 
{ 
	TCCR0B = TIMER0_Prescaler;
	TCNT0 = val;
} 

#define timer0_get() TCNT0

static inline void timer1_start() 
{ 
    TCCR1B = TIMER1_Prescaler; 
    TCNT1 = 0; 
} 
 
static inline void timer1_stop(void) 
{ 
    TCCR1B = 0x00; 
} 
 
static inline void timer1_set(unsigned int val) 
{ 
	TCCR1B = TIMER1_Prescaler;
    TCNT1 = val; 
} 

#define timer1_get() TCNT1 
 
// Communication
#ifndef NEGATE_LOGIC
    // High equals High
    #define set_K_high() K_Line_PORT |= (1<<K_Line_OUT)
    #define set_L_high() L_Line_PORT |= (1<<L_Line_OUT)

	#define set_K_low() K_Line_PORT &= ~(1<<K_Line_OUT)
	#define set_L_low() L_Line_PORT &= ~(1<<L_Line_OUT)

    #define is_K_high() bit_is_set(K_Line_PIN,K_Line_IN)
#else
    // High equals Low
    #define set_K_high() K_Line_PORT &= ~(1<<K_Line_OUT)
    #define set_L_high() L_Line_PORT &= ~(1<<L_Line_OUT)

	#define set_K_low() K_Line_PORT |= (1<<K_Line_OUT)
	#define set_L_low() L_Line_PORT |= (1<<L_Line_OUT)

    #define is_K_high() bit_is_clear(K_Line_PIN,K_Line_IN)
#endif

/*********************************************************************
** Helper for KWP2000-Handling
*********************************************************************/

volatile struct {
    unsigned char buffer;
    unsigned char reload;
    unsigned char bit_cnt;
    unsigned char status;
	unsigned char recieve;
}helper;

//#define CHECKSTATUS(status_bit) (helper.status & status_bit)
#define CHECKSTATUS(status_bit) (helper.status & (1<<(status_bit)))
#define SETSTATUS(status_bit) (helper.status |= status_bit)
#define CLEARSTATUS(status_bit) (helper.status &= ~status_bit)

#define SENDING 1
#define BUSY 2

#endif // obdCOM