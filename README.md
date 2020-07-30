# Nuzzle (N-curses p-UZZLE)

Nuzzle is a terminal based puzzle game collection. It is implemented with 
ncurses and requires color and mouse support.

The games can be played with the mouse or the terminal. See the 
[https://github.com/dead-end/nuzzle/tree/master/man](man page)
for details.

![Example](res/nuzzle-example.gif)

# Installation

To install nuzzle download the latest released sources:

[https://github.com/dead-end/nuzzle/releases](https://github.com/dead-end/nuzzle/releases)

Then build the program with the following commands:

```
tar xvzf nuzzle-0.3.tar.gz

cd nuzzle-0.3/

make

sudo make install
```

Now you can start the program with:

```
nuzzle
```

# Uninstall

To uninstall nuzzle simply remove the installation directory and the directory which nuzzle uses for its data: 

```
sudo make uninstall
```

Additionally there is a user specific directory that has to be removed:

```
rm -rf ${HOME}/.nuzzle/
```

# Dependencies

Nuzzle uses ncursesw which is the wide character version of ncurses. To build 
nuzzle, the following list of dependencies is relevant for ubuntu 18.0.4:

- libc6 (>=2.27)
- libtinfo5 (>=6.1)
- libncursesw5 (>=6.1)

# Customize nuzzle

Nuzzle has several configuration files that can be customized by overwriting 
them. The default configurations are located in:

       /usr/share/games/nuzzle/

Copy the configuration file that you want to customize to the directory:       

       ${HOME}/.nuzzle/

