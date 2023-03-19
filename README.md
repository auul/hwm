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

`Alt+LeftClick`: Dragging while pressing this combo will switch a tiled window to stacking mode, or move a window already in stacking mode.

`Alt+Return`: Returns the focused window to tiling mode and retiles the screen.

`Alt+PageUp`: Cycles a tiled window backwards through tile positions.

`Alt+PageDown`: Cycles a tiled window forwards through tile positions.
