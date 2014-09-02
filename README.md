
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
    | Alt + Ctrl + 3     | go to workspace 3.             | workspace_3                 |
    | Alt + Ctrl + 4     | go to workspace 4.             | workspace_4                 |
    | Alt + Ctrl + 5     | go to workspace 5.             | workspace_5                 |
    | Alt + Ctrl + 6     | go to workspace 6.             | workspace_6                 |
    | Alt + Ctrl + 7     | go to workspace 7.             | workspace_7                 |
    | Alt + Ctrl + 8     | go to workspace 8.             | workspace_8                 |
    | Alt + Ctrl + 9     | go to workspace 9.             | workspace_9                 |
    +--------------------+--------------------------------+-----------------------------+


## LAYOUTS

### arrange

    +-------------------------+--------+
    |                         |   1    |
    |                         +--------+
    |                         |   2    |
    |      MASTER EREA        +--------+
    |                         |   3    |
    |           0             +--------+
    |                         |        |
    |                         |...     |
    +-------------------------+--------+


### arrange\_twin

    +----------------+-----------------+
    |                |                 |
    |                |                 |
    |  MASTER EREA   |                 |
    |                |                 |
    |       0        |        1        |
    |                |                 |
    +-----------+----------+-----------+
    |     2     |    3     |    ...    |
    +-----------+----------+-----------+


### arrange\_maximal

    +----------------------------------+
    |                                  |
    |                                  |
    |                                  |
    |          MASTER EREA             |
    |                                  |
    |              0                   |
    |                                  |
    |                                  |
    +----------------------------------+


## LICENSE

Distributed under MIT License. See LICENSE.txt

