txt2epub: txt2epub.c
	gcc txt2epub.c -o txt2epub
debug: txt2epub.c
	gcc txt2epub.c -o txt2epub -fsanitize=address -Wall -Wextra -pedantic
clean:
	rm txt2epub *.epub *.zip
