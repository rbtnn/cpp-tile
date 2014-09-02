
# tile

This is Tiling Window Manager for MS Windows.


## BUILD

> mingw32-make.exe -f makefile


## USAGE

> bin/tile.exe

Default key mappings of `tile.exe` is written in following figure.
You can edit `bin/tile.ini` and customize this.

    +--------------------+--------------------------------+-----------------------------+
    | KeyStroke          | Description                    | keyname of tile.ini         |
    +--------------------+--------------------------------+-----------------------------+
    | Alt + Ctrl + M     | active window to master erea.  | focus_window_to_master      |
    | Alt + Ctrl + Q     | exit tile.exe.                 | exit_tile                   |
    | Alt + Ctrl + C     | kill a active window.          | kill_client                 |
    | Alt + Ctrl + K     | focus to a previous window.    | prev_focus                  |
    | Alt + Ctrl + J     | focus to a next window.        | next_focus                  |
    | Alt + Ctrl + Space | change layout method.          | next_layout                 |
    | Alt + Ctrl + Enter | run `powershell.exe`.          | run_shell                   |
    | Alt + Ctrl + 1     | go to workspace 1.             | workspace_1                 |
    | Alt + Ctrl + 2     | go to workspace 2.             | workspace_2                 |
    +--------------------+--------------------------------+-----------------------------+


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

