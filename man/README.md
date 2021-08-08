```
NUZZLE(6)                            Game                            NUZZLE(6)

NAME
       nuzzle - terminal based puzzle game

SYNOPSIS
       nuzzle

DESCRIPTION
       Nuzzle  is  a  terminal based puzzle game collection. It is implemented
       with ncurses and requires color and mouse support.

       The target of the game is to place as much blocks as possible,  from  a
       home  area to the game area. If the blocks on the game area form a cer‐
       tain pattern, they will be removed and the player gets  one  point  for
       each block that was removed.

       There are 3 games that are supported:

       Lines  The  game  is played with one color. If a complete row or column
              is filled with blocks, they will be removed.

       Lines-squares
              The game is played with one color. The game area consists of 9x9
              blocks  with  9  squares, that consists of 3x3 blocks. If a com‐
              plete row or column is filled with blocks or if  one  of  the  9
              squares is filled with blocks the blocks will be removed.

       4-Colors
              The  game  is played with 4 colors. If 4 or more adjacent blocks
              with the same color exist, then they will be removed.

       The game can be played with the mouse and/or the keyboard. The  follow‐
       ing keys are supported.

       <ESC>  Show the main menu during the game.

       <TAB>  Pick  up a block in the home area or loop through the home area,
              if a block is already picked up.

       <ARROW-KEY>
              If the blocks from the home area were not inside the game  area,
              they are moved to the first possible drop area on the game area.
              If the blocks are inside the game area, the blocks can be  moved
              with the arrow keys.

       <ENTER>
              Drop to blocks on the game area if it is possible.

       <q>    Quit the game.

       To  play  nuzzle  with the mouse, you can simply left click on the home
       area to pick up the blocks. Once the blocks are picked up you can  move
       the  blocks  with  the mouse. The mouse needs not to be pressed to move
       the blocks. A right click moves the blocks back to the  home  area.  If
       blocks  are  picked  up  and moved to the game area, another left click
       drops the blocks on the game area, if this is possible.

FILES
       Nuzzle uses the following configuration files:

       nuzzle.cfg
              The file contains the configurations for the three games.

       color.cfg
              The file contains the definitions of the colors  that  are  used
              within the nuzzle.

       5-shapes.cfg shapes-lines.cfg
              The two file contain the definitions of the various block struc‐
              tures that are used in the Lines and Squares-Lines games.

       Nuzzle references the configuration files with their names. The default
       files from the installation can be overwritten. Each configuration file
       is resolved by looking in the following directories in the given order.
       To  overwrite  one or more of the files, copy a the default file to one
       of the directies.

       ${PWD}/cfg/
              Nuzzle first looks in the cfg directory inside the  working  di‐
              rectory  of  the  user.  This is necessary to ensure that nuzzle
              finds configurations after a build with make.

       ${HOME}/.nuzzle/
              Nuzzle uses this directory to store user specific files like the
              score  files for the games. This is the recommended directory to
              place customized configuration files.

       /usr/share/games/nuzzle/
              The last directory contains the  default  configurations,  which
              are provided with the installation.

SEE ALSO
       nuzzle(6)

BUGS
       No known bugs.

AUTHOR
       Dead End

V 0.3.2                           August 2021                        NUZZLE(6)
```
