EXEC=../build/robotSem819
MMCU = atmega328p


all: main.c commands_parser.c commands_parser.h commands.c commands.h
	avr-gcc  -mmcu=$(MMCU) -c main.c -o main.o
	avr-gcc  -mmcu=$(MMCU) -c commands_parser.c -o commands_parser.o
	avr-gcc  -mmcu=$(MMCU) -c commands.c -o commands.o

	avr-gcc -mmcu=$(MMCU) main.o commands_parser.o commands.o -o $(EXEC).elf

	avr-objcopy -j .text -j .data -O ihex $(EXEC).elf $(EXEC).hex

clean:
	rm $(EXEC)
