
# tile

This is Tiling Window Manager for MS Windows.


## BUILD

> mingw32-make.exe -f makefile


## USAGE

> bin/tile.exe

    +--------------------+--------------------------------+
    | Alt + Ctrl + M     | active window to master erea.  |
    | Alt + Ctrl + Q     | exit tile.exe.                 |
    | Alt + Ctrl + C     | kill a active window.          |
    | Alt + Ctrl + K     | focus to a previous window.    |
    | Alt + Ctrl + J     | focus to a next window.        |
    | Alt + Ctrl + Space | change layout method.          |
    | Alt + Ctrl + Enter | run `powershell.exe`.          |
    +--------------------+--------------------------------+

You can edit `bin/tile.ini`, and customize this.


## LAYOUTS

### arrange

    +-------------------------+--------+
    |                         |        |
    |                         +--------+
    |                         |        |
    |      MASTER EREA        +--------+
    |                         |        |
    |                         +--------+
    |                         |        |
    |                         |...     |
    +-------------------------+--------+

### arrange\_maximal

    +----------------------------------+
    |                                  |
    |                                  |
    |                                  |
    |          MASTER EREA             |
    |                                  |
    |                                  |
    |                                  |
    |                                  |
    +----------------------------------+

## LICENSE

Distributed under MIT License. See LICENSE.txt

