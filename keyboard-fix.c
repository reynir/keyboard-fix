#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

void ListenForKeyboardEvents();
void SendKeyPressEventFor(KeySym keysym, Display *display, Window *window);

int main(int argc, char **argv) {
  pid_t pid = 0;
	pid = fork();

  if (pid == 0) {
    ListenForKeyboardEvents();
  } else {
    exit(0);
  }
  
  return 0;
}

void ListenForKeyboardEvents() {
  unsigned int keycode;
  unsigned int revert;
  
  Window window;
  XEvent event;
  KeySym prev_char;

  Display *display = XOpenDisplay(NULL);
  Window root = XDefaultRootWindow(display); 

  XGetInputFocus(display, &window, &revert);
  XSelectInput(display, window, KeyPressMask | FocusChangeMask);

  while (true)  {
    XNextEvent(display, &event);
    if (event.type == KeyPress) {
      keycode = XLookupKeysym(&event.xkey, 0);

      if (prev_char == XK_a && keycode == XK_a) {
        SendKeyPressEventFor(XK_BackSpace, display, &window);
        SendKeyPressEventFor(XK_BackSpace, display, &window);
        SendKeyPressEventFor(XK_aring, display, &window);
      } else if (prev_char == XK_a && keycode == XK_e) {
        SendKeyPressEventFor(XK_BackSpace, display, &window);
        SendKeyPressEventFor(XK_BackSpace, display, &window);
        SendKeyPressEventFor(XK_ae, display, &window);
      } else if (prev_char == XK_o && keycode == XK_e) {
        SendKeyPressEventFor(XK_BackSpace, display, &window);
        SendKeyPressEventFor(XK_BackSpace, display, &window);
        SendKeyPressEventFor(XK_oslash, display, &window);
      }

      prev_char = keycode;
    } else {
      XGetInputFocus(display, &window, &revert);
      XSelectInput(display, window, KeyPressMask | FocusChangeMask);
    }
  }
}

void SendKeyPressEventFor(KeySym keysym, Display *display, Window *window) {
  XKeyEvent event;
  
  event.display = display;
  event.window = *window;
  event.type = KeyPress;
  event.keycode = XKeysymToKeycode(display, keysym);
  
  XSendEvent(display, *window, True, KeyPressMask, (XEvent *) &event);
  XFlush(display);
}
