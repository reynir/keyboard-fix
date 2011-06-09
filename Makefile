NAME=keyboard-fix

all:
	cc -std=c99 -lX11 -o $(NAME) $(NAME).c