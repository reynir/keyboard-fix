#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/timeb.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define MAGIC_TIME_S 0
#define MAGIC_TIME_MS 500

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
  struct timeb tp;

  Window window;
  XEvent event;
  KeySym prev_char;
  struct timeb prev_tp;

  Display *display = XOpenDisplay(NULL);
  Window root = XDefaultRootWindow(display); 

  XGetInputFocus(display, &window, &revert);
  XSelectInput(display, window, KeyPressMask | FocusChangeMask);

  while (true)  {
    XNextEvent(display, &event);
    /* Read the time and do stuff */
    ftime(&tp);

    if (event.type == KeyPress) {
      keycode = XLookupKeysym(&event.xkey, 0);

      if (prev_tp == NULL)
        continue; 
      /* check to see if the two key strokes happened withing time limit */
      if (tp.time - prev_tp.time > MAGIC_TIME_S ||
          (tp.time - prev_tp.time = MAGIC_TIME_S &&
           tp.millitm - prev_tp.millitm > MAGIC_TIME_MS)) {
        prev_tp = tp;
        prev_char = keycode;
        continue;
      }        

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
    prev_tp = tp;
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