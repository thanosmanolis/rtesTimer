CC = gcc

all: rtesTimer.c
	gcc -Wall rtesTimer.c -o rtesTimer
	gcc -Wall rtesTimerNo.c -o rtesTimerNo

clean:
	$(RM) rtesTimer
