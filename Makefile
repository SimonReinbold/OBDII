GIT_VERSION := $(shell git describe --abbrev=0 --always --tags)

TARGET  = obdii
SOURCES = main.c applicationLayer.c dataLayer.c physicalLayer.c lcd-routines.c 

PROGRAMMER = arduino
MCU = atmega328p
PORT = COM4
BAUD = 115200
F_CPU=16000000UL
CC = avr-gcc

CFLAGS = -c -std=c99 -MD -DF_CPU=$(F_CPU) -DGIT_VERSION=$(GIT_VERSION)

VPATH = Protocol/ApplicationLayer/src;Protocol/DataLayer/src;Protocol/PhysicalLayer/src;Application/src/;LCD/
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