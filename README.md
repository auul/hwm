# hwm - the humble window manager
A minimalistic tiling/stacking hybrid window manager for X11.

## Pronunciation
Hum? Hume? Whom.

## Building and Installing
To build hwm, simply navigate to the directory where you have cloned the repository, and enter the following commands:

```
make && sudo make install
```

This will install hwm by default into /usr/bin/, where it can be accessed under your system's PATH.

To start hwm as your default X window manager, add the following to the end of your ~/.xinitrc file:

```
hwm &
xterm
```

Currently hwm does not contain any facility for launching programs; they must all be launched manually via the terminal, or through an external hotkey utility.

## Using

By default, hwm displays all windows in tiling mode. Each window, in order of creation, is allocated half of the remaining screen real estate, and the final tile occupies the rest of the available space. The aspect ratio of each tile is determined based on the ratio of the available space; that is, the larger dimension (width or height) is split in half, and the window occupies the full remaining extent of the other dimension.

To switch a window from tiling mode to stacking mode, simply `Alt+Left Click` and drag it out of it's tiled position. The dimensions of the stacking window will revert to the last dimensions reported before the window was tiled. This also allows you to move stacking windows around the screen.

To return a window to tiling mode from stacking mode, focus upon it and press `Alt+Return`.

To cycle a window between tile positions, focus upon it and press `Alt+Page Up` or `Alt+Page Down` to cycle back and forth, respectively. Focus will follow the tiled window until the mouse is used to choose a new focus window.

Currently there is no way to resize a window, tiling or stacking. This is a major oversight which should be corrected in future versions.
