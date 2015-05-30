# uppercaser - making everything LOUD

This application takes data on *stdin*, converts it to upper case and then prints the output to *stdout*

Note: The reason we can compare to 'z' and so forth is because ASCII is designed in such a way that
all of the letters have a fixed order and can be interchanged for known numbers.

## Doing it by hand

If you don't want to use clion or you want to do things manually:

    $ git clone https://github.com/pmembrey/uppercaser.git
    $ cd uppercaser
    $ mkdir build
    $ cd build
    $ cmake -G "Unix Makefiles" ../
    $ make
    $ echo "Hello! This is a silly example ;)" | ./uppercaser

Enjoy! :)
