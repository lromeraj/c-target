all: clean main

main: ctarget
	./ctarget -c

clean:
	./ctarget --clean

ctarget:
	if [ ! -e "c-target" ]; then git clone https://github.com/lromeraj/c-target; fi
	cd c-target && make ctarget && cp ctarget ../ && make clean
