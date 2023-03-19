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

By default, hwm treats all windows as part of a tiling stack. The top of this stack is given half of the screen real estate, and each successive window is given half of the remaining space, until only one window is left, which is given the remaining empty screen space. Newly opened windows are automatically pushed to the top of this stack.

`Alt+Return`: moves a window to the top of the tiling stack, making it the primary window and pushing all other tiled windows down. If the focused window is in stacking mode, this is the way to return it to tiling mode.

`Alt+LeftClick`: dragging a window with this switches that window's mode to stacking, and moves it around the screen.

`Alt+RightClick`: dragging a stacking window with this resizes that window. Currently, this does nothing in tiling mode.
