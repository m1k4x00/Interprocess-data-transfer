all: compile make_log_dir

make_log_dir:
	mkdir Logs
compile:
	gcc -Wall -pedantic -g -std=c99 -D_POSIX_SOURCE ./main.c -o ./main
clean:
	rm -f main *~
	rm -r ./Logs
