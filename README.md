# Nuzzle (N-curses p-UZZLE)

Nuzzle is a terminal based puzzle game collection. It is implemented with 
ncurses and requires color and mouse support.

The games can be played with the mouse or the terminal. 
See the [man page](https://github.com/dead-end/nuzzle/tree/master/man) for 
details.

![Example](res/nuzzle-example.gif)

# Installation

## Debian Package

The [latest release](https://github.com/dead-end/nuzzle/releases) of nuzzle 
contains a debian package. Download the package and verify the result with 
md5sum:

```
# md5sum nuzzle_0.3.0_amd64.deb 
72ef2180a9c12ffbf471e8dbc4b936c6  nuzzle_0.3.0_amd64.de
```

Install the package with:

```
# sudo dpkg -i nuzzle_0.3.0_amd64.deb
```

Uninstall the package with:

```
# sudo dpkg -r nuzzle
```

## Build from sources

The [latest release](https://github.com/dead-end/nuzzle/releases) of nuzzle 
contains a zip / tar file with the sources. Download the sources and build the
program with the following commands:

```
tar xvzf nuzzle-0.3.tar.gz

cd nuzzle-0.3/

make

sudo make install
```

To uninstall nuzzle simply remove the installation directory and the directory 
which nuzzle uses for its data: 

```
sudo make uninstall
```

## Completely uninstall nuzzle

Additionally there is a user specific directory that has to be removed to 
completely uninstall nuzzle

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

Nuzzle has several configuration files that can be overwritten to customize the
games.
See the [man page](https://github.com/dead-end/nuzzle/tree/master/man) for 
details.

