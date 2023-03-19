#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>

typedef struct {
	Window win;
	int is_tiled;
	int orig_width;
	int orig_height;
} Tile;

int win_gap;
Display *dpy;

int tile_count;
Tile **tile;

Tile *add_tile(Window win)
{
	XWindowAttributes attr;
	Tile **resized;

	if (resized = realloc(tile, (tile_count + 1) * sizeof(Tile *))) {
		resized[tile_count] = calloc(1, sizeof(Tile));
	}

	if (!resized || !resized[tile_count]) {
		fprintf(stderr, "Error allocating memory.\n");
		return NULL;
	}

	tile = resized;

	XGetWindowAttributes(dpy, win, &attr);
	tile[tile_count]->win = win;
	tile[tile_count]->is_tiled = 1;
	tile[tile_count]->orig_width = attr.width;
	tile[tile_count]->orig_height = attr.height;

	tile_count++;

	return tile[tile_count - 1];
}

Tile **get_tile(Window win)
{
	for (int i = 0; i < tile_count; i++) {
		if (tile[i]->win == win) {
			return tile + i;
		}
	}

	return NULL;
}

int remove_tile(Window win)
{
	Tile **target = get_tile(win);
	if (!target) return 0;

	for (int i = target - tile + 1; i < tile_count; i++) {
		tile[i - 1] = tile[i];
	}

	tile_count--;

	return 1;
}

void raise_tile(Window win)
{
	Tile **src_p = get_tile(win);
	Tile *src;
	int src_i;

	if (!src_p) return;
	src = *src_p;
	src_i = src_p - tile;

	for (int i = src_i - 1; i >= 0; i--) {
		tile[i + 1] = tile[i];
	}

	tile[0] = src;
}

int quit(void)
{
	if (dpy) {
		XCloseDisplay(dpy);
		dpy = NULL;
	}

	if (tile) {
		for (unsigned i = 0; i < tile_count; i++) {
			if (tile[i]) free(tile[i]);
		}
		free(tile);
		tile = NULL;
	}
	tile_count = 0;

	return 0;
}

int init(void)
{
	Window root;
	Window parent;
	Window *child;
	unsigned child_count;
	XWindowAttributes attr;
	XSetWindowAttributes set_attr;

	if (!(dpy = XOpenDisplay(NULL)))
		return quit();

	// Register all visible windows
	if (!XQueryTree(dpy,
	                XDefaultRootWindow(dpy),
			&root,
			&parent,
			&child,
			&child_count)) {
		return quit();
	}

	for (unsigned i = 0; i < child_count; i++) {
		XGetWindowAttributes(dpy, child[i], &attr);
		if (attr.map_state == IsViewable &&
		    !attr.override_redirect) {
			add_tile(child[i]);
		}
	}

	XFree(child);

	// Add window creation/destruction event notifications
	set_attr.event_mask = SubstructureNotifyMask;
	XChangeWindowAttributes(dpy,
	                        XDefaultRootWindow(dpy),
				CWEventMask,
				&set_attr);

	XGrabKey(dpy,
	         XKeysymToKeycode(dpy, XStringToKeysym("Return")),
		 Mod1Mask,
		 XDefaultRootWindow(dpy),
		 True,
		 GrabModeAsync,
		 GrabModeAsync);
	XGrabKey(dpy,
	         XKeysymToKeycode(dpy, XStringToKeysym("Prior")),
		 Mod1Mask,
		 XDefaultRootWindow(dpy),
		 True,
		 GrabModeAsync,
		 GrabModeAsync);
	XGrabKey(dpy,
	         XKeysymToKeycode(dpy, XStringToKeysym("Next")),
		 Mod1Mask,
		 XDefaultRootWindow(dpy),
		 True,
		 GrabModeAsync,
		 GrabModeAsync);
	XGrabButton(dpy,
	            1,
		    Mod1Mask,
		    XDefaultRootWindow(dpy),
		    True,
		    ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
		    GrabModeAsync,
		    GrabModeAsync,
		    None,
		    None);
	XGrabButton(dpy,
	            3,
		    Mod1Mask,
		    XDefaultRootWindow(dpy),
		    True,
		    ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
		    GrabModeAsync,
		    GrabModeAsync,
		    None,
		    None);


	return 1;
}

void tile_all(void)
{
	int remain_w = XDisplayWidth(dpy, 0);
	int remain_h = XDisplayHeight(dpy, 0);
	int this_x, this_y, this_w, this_h;
	int count = 0;

	for (int i = 0; i < tile_count; i++) {
		if (tile[i]->is_tiled) {
			count++;
		}
	}

	for (int i = 0; i < tile_count; i++) {
		if (tile[i]->is_tiled) {
			count--;

			if (count) {
				// Each tile fits half of the width or height,
				// depending on which is larger, and the full
				// extent of the other remaining dimension.
				if (remain_w > remain_h) {
					this_w = (remain_w / 2) - win_gap;
					this_h = remain_h - win_gap;
					this_x = (remain_w / 2) + (win_gap / 2);
					this_y = (win_gap / 2);
					remain_w = remain_w / 2;
				} else {
					this_w = remain_w - win_gap;
					this_h = (remain_h / 2) - win_gap;
					this_x = (win_gap / 2);
					this_y = (remain_h / 2) + (win_gap / 2);
					remain_h = remain_h / 2;
				}
			} else {
				// Last tile fits the remaining space
				this_x = (win_gap / 2);
				this_y = (win_gap / 2);
				this_w = remain_w - win_gap;
				this_h = remain_h - win_gap;
			}

			XMoveResizeWindow(dpy,
			                  tile[i]->win,
					  this_x,
					  this_y,
					  this_w,
					  this_h);
		}
	}
}

int main(int argc, char **args)
{
	int retile = 1;
	XEvent evt;
	XButtonEvent drag_start;
	XWindowAttributes drag_attr;
	XWindowAttributes attr;
	Tile *drag_tile;
	Tile **tile_p;

	if (argc > 1) {
		win_gap = atoi(args[1]);
		if (win_gap < 0) {
			win_gap = 0;
		}
	}

	if (!init()) {
		return 1;
	}
	tile_all();

	drag_start.subwindow = None;

	for(int run = 1; run;) {
		XNextEvent(dpy, &evt);

		switch (evt.type) {
		case KeyPress:
			if (evt.xkey.subwindow != None) {
				if (evt.xkey.keycode == XKeysymToKeycode(
					dpy, XStringToKeysym("Return"))
				) {
					drag_tile = *(get_tile(evt.xkey.subwindow));
					drag_tile->is_tiled = 1;
					raise_tile(evt.xkey.subwindow);
					tile_all();
				}
			}
			break;
		case ButtonPress:
			if (evt.xbutton.subwindow != None) {
				drag_start = evt.xbutton;
				drag_tile = *(get_tile(drag_start.subwindow));
				XGetWindowAttributes(dpy,
				                     drag_start.subwindow,
						     &drag_attr);
			}
			break;
		case MotionNotify:
			if (drag_start.subwindow != None &&
			    drag_start.button == 1) {
				int xdiff = evt.xbutton.x_root
				          - drag_start.x_root;
				int ydiff = evt.xbutton.y_root
				          - drag_start.y_root;

				if (retile) {
					retile = 0;
					drag_tile->is_tiled = 0;
					tile_all();
				}

				XMoveResizeWindow(dpy,
				                  drag_start.subwindow,
					          drag_attr.x + xdiff,
					          drag_attr.y + ydiff,
						  drag_tile->orig_width,
						  drag_tile->orig_height);
			} else if (drag_start.subwindow != None &&
			           drag_start.button == 3 &&
				   !drag_tile->is_tiled) {
				int xdiff = evt.xbutton.x_root
				          - drag_start.x_root;
				int ydiff = evt.xbutton.y_root
				          - drag_start.y_root;

				drag_tile->orig_width = drag_attr.width + xdiff;
				drag_tile->orig_height = drag_attr.height + ydiff;
				XMoveResizeWindow(dpy,
				                  drag_start.subwindow,
						  drag_attr.x,
						  drag_attr.y,
						  drag_tile->orig_width,
						  drag_tile->orig_height);
			}
			break;
		case ButtonRelease:
			retile = 1;
			drag_start.subwindow = None;
			break;
		case MapNotify:
			XGetWindowAttributes(dpy, evt.xmap.window, &attr);
			if (!attr.override_redirect) {
				add_tile(evt.xmap.window);
				tile_all();
			}
			break;
		case UnmapNotify:
			if (remove_tile(evt.xmap.window)) {
				tile_all();
			}
			break;
		}
	}

	return quit();
}
