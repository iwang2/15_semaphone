all: write.c control.c
	gcc -o client write.c
	gcc -o control control.c

clean:
	rm -rf client
	rm -rf control
	rm -rf story.txt
