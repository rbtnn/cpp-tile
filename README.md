
# tile

This is Tiling Window Manager for MS Windows.

![](https://raw.github.com/rbtnn/tile/master/image.png)


## BUILD

> mingw32-make.exe -f makefile

* should be defined `BOOST_ROOT`.


## USAGE

> tile.exe


## LAYOUT METHODS

### arrange

    +-------------------------+--------+
    |                         |   1    |
    |                         +--------+
    |                         |   2    |
    |      MASTER PANE        +--------+
    |                         |   3    |
    |           0             +--------+
    |                         |        |
    |                         |...     |
    +-------------------------+--------+


### arrange\_twin

    +----------------+-----------------+
    |                |                 |
    |                |                 |
    |  MASTER PANE   |                 |
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
    |          MASTER PANE             |
    |                                  |
    |              0                   |
    |                                  |
    |                                  |
    +----------------------------------+


### arrange\_cross

    +-------+------------------+-------+
    |       |        1,5,...   |       |
    +-------+------------------+-------+
    |       |                  |       |
    |       |   MASTER PANE    |       |
    |4,8,...|        0         |2,6,...|
    |       |                  |       |
    +-------+------------------+-------+
    |       |        3,7,...   |       |
    +-------+------------------+-------+


## CONFIGURATION

Default key mappings of `tile.exe` is defined in `tile.json`.
You can edit it and customize key mappings.


### settings
* IGNORE\_CLASSNAMES
* NOT\_APPLY\_STYLE\_TO\_CLASSNAMES
* RUN\_PROCESS\_PATH
* LAYOUT\_METHOD\_NAMES


### keys
* focus\_window\_to\_master
Focus an active window to master pane. (KeyStroke: `Alt + Ctrl + m`)

* next\_layout
Change layout method.  (KeyStroke: `Alt + Ctrl + Space`)

* next\_focus
Focus to a next window.  (KeyStroke: `Alt + Ctrl + j`)

* previous\_focus
Focus to a previous window.  (KeyStroke: `Alt + Ctrl + k`)

* exit\_tile
Exit tile.exe.  (KeyStroke: `Alt + Ctrl + q`)

* kill\_client
Kill an active window.  (KeyStroke: `Alt + Ctrl + c`)

* run\_process
Run a process.  (KeyStroke: `Alt + Ctrl + Enter`)

* rescan
Rescan processes.  (KeyStroke: `Alt + Ctrl + r`)

* toggle\_transparency\_window
Toggle transparency to an active window.  (KeyStroke: `Alt + Ctrl + t`)

* swap\_next
Swap an active window and a next window.  (KeyStroke: `Alt + Ctrl + J`)

* swap\_previous
Swap an active window and a previous window.  (KeyStroke: `Alt + Ctrl + K`)

* workspace\_1
Go to workspace 1.  (KeyStroke: `Alt + Ctrl + 1`)

* workspace\_2
Go to workspace 2.  (KeyStroke: `Alt + Ctrl + 2`)

* workspace\_3
Go to workspace 3.  (KeyStroke: `Alt + Ctrl + 3`)

* workspace\_4
Go to workspace 4.  (KeyStroke: `Alt + Ctrl + 4`)

* workspace\_5
Go to workspace 5.  (KeyStroke: `Alt + Ctrl + 5`)

* workspace\_6
Go to workspace 6.  (KeyStroke: `Alt + Ctrl + 6`)

* workspace\_7
Go to workspace 7.  (KeyStroke: `Alt + Ctrl + 7`)

* workspace\_8
Go to workspace 8.  (KeyStroke: `Alt + Ctrl + 8`)

* workspace\_9
Go to workspace 9.  (KeyStroke: `Alt + Ctrl + 9`)

* move\_to\_workspace\_1
Move an active window to workspace 1.  (KeyStroke: `Alt + Ctrl + !`)

* move\_to\_workspace\_2
Move an active window to workspace 2.  (KeyStroke: `Alt + Ctrl + @`)

* move\_to\_workspace\_3
Move an active window to workspace 3.  (KeyStroke: `Alt + Ctrl + #`)

* move\_to\_workspace\_4
Move an active window to workspace 4.  (KeyStroke: `Alt + Ctrl + $`)

* move\_to\_workspace\_5
Move an active window to workspace 5.  (KeyStroke: `Alt + Ctrl + %`)

* move\_to\_workspace\_6
Move an active window to workspace 6.  (KeyStroke: `Alt + Ctrl + ^`)

* move\_to\_workspace\_7
Move an active window to workspace 7.  (KeyStroke: `Alt + Ctrl + &`)

* move\_to\_workspace\_8
Move an active window to workspace 8.  (KeyStroke: `Alt + Ctrl + *`)

* move\_to\_workspace\_9
Move an active window to workspace 9.  (KeyStroke: `Alt + Ctrl + (`)


## LICENSE

Distributed under MIT License. See LICENSE.txt


