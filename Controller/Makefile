#
#  Controller Makefile
#

GIT_VERSION := $(shell git describe --always --tags)
SHOW_CHARS = 8

TARGET  = obdii
SOURCES = main.c dataLayer_USART.c physicalLayer_USART.c Settings.c ProtocolSelector.c SessionHandler_KWP2000.c Session_KWP2000.c dataLayer_KWP2000.c physicalLayer_KWP2000.c iso_tp.c

PROGRAMMER = arduino
MCU = atmega328p
PORT = COM4
BAUD = 115200
SERIAL_BAUD = 9600
F_CPU=16000000UL
CC = avr-gcc

CFLAGS = -c -idirafter'C:\Users\Simon\Documents\Projects\Programming\OBDII\commonFiles' -std=c99 -MD -DF_CPU=$(F_CPU) -DGIT_VERSION=$(GIT_VERSION) -DVERS_CHARS=$(SHOW_CHARS) -DSERIAL_BAUD=$(SERIAL_BAUD)
VPATH = $(shell find -mindepth 1 -maxdepth 10 -type d)
VPATH += ../commonFiles/USART/PhysicalLayer/src/ ../commonFiles/USART/DataLayer/src/
#$(info VPATH: $(VPATH))
BPATH = build

OBJECTS = $(SOURCES:.c=.o)
COBJECTS = $(addprefix $(BPATH)/,$(OBJECTS))
CTARGET = $(addprefix $(BPATH)/,$(TARGET))

all: $(CTARGET).hex

$(CTARGET).hex: $(CTARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@

$(CTARGET).elf: $(COBJECTS)
	$(CC) -mmcu=$(MCU) $^ -o $@

$(BPATH)/%.o:%.c
	$(CC) -Os -mmcu=$(MCU) $(CFLAGS) -o $@ $<

install: $(CTARGET).hex
	avrdude -F -V -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(CTARGET).hex

clean:
	rm -r build/
	mkdir build

-include $(COBJECTS:.o=.d)