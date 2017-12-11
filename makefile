all: write.c control.c
	gcc -o write write.c
	gcc -o control control.c

clean:
	rm write
	rm control
	rm story.txt
