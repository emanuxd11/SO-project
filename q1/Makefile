samples: samples.c
	gcc samples.c -o samples
debug: samples.c
	gcc samples.c -o samples -fsanitize=address -Wall -Wextra -pedantic
clean:
	rm samples
