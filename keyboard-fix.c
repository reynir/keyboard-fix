#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#define MAGIC_TIME_S 0
#define MAGIC_TIME_MICROS (500*1000)

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
  struct timeval tv;

  Window window;
  XEvent event;
  KeySym prev_char;
  struct timeval prev_tv;

  Display *display = XOpenDisplay(NULL);
  Window root = XDefaultRootWindow(display); 

  XGetInputFocus(display, &window, &revert);
  XSelectInput(display, window, KeyPressMask | FocusChangeMask);

  /* This is just to ensure prev_tv has some reasonable value. It's
   * maybe not needed at all. */
  gettimeofday(&prev_tv, NULL);

  while (true)  {
    XNextEvent(display, &event);
    /* Read the time and do stuff */
    gettimeofday(&tv, NULL);

    if (event.type == KeyPress) {
      keycode = XLookupKeysym(&event.xkey, 0);

      /* check to see if the two key strokes happened within time limit */
      if (tv.tv_sec - prev_tv.tv_sec > MAGIC_TIME_S ||
          (tv.tv_sec - prev_tv.tv_sec == MAGIC_TIME_S &&
           tv.tv_usec - prev_tv.tv_usec > MAGIC_TIME_MICROS)) {
        prev_tv = tv;
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

      prev_tv = tv;
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
