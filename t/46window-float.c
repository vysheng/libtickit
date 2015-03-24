#include "tickit.h"
#include "tickit-window.h"
#include "taplib.h"
#include "taplib-mockterm.h"

int on_expose_fillchr(TickitWindow *win, TickitEventType ev, TickitEventInfo *args, void *data)
{
  for(int line = args->rect.top; line < args->rect.top + args->rect.lines; line++) {
    char buffer[80];
    for(int i = 0; i < args->rect.cols; i++)
      buffer[i] = *(char *)data;
    buffer[args->rect.cols] = 0;
    tickit_renderbuffer_text_at(args->rb, line, args->rect.left, buffer, NULL);
  }

  return 1;
}

int on_expose_textat(TickitWindow *win, TickitEventType ev, TickitEventInfo *args, void *data)
{
  tickit_renderbuffer_text_at(args->rb, 0, 0, data, NULL);
  return 1;
}

int on_input_capture(TickitWindow *win, TickitEventType ev, TickitEventInfo *args, void *data)
{
  *((TickitEventInfo *)data) = *args;
  return 1;
}

int main(int argc, char *argv[])
{
  TickitTerm *tt = make_term(25, 80);
  TickitWindow *root = tickit_window_new_root(tt);

  TickitWindow *rootfloat = tickit_window_new_float(root, 10, 10, 5, 30);
  tickit_window_tick(root);

  // Basics
  {
    int bind_id = tickit_window_bind_event(root, TICKIT_EV_EXPOSE, &on_expose_fillchr, "X");

    tickit_window_expose(root, &(TickitRect){ .top = 10, .lines = 1, .left = 0, .cols = 80 });
    tickit_window_tick(root);

    is_termlog("Termlog for print under floating window",
        GOTO(10,0), SETPEN(), PRINT("XXXXXXXXXX"),
        GOTO(10,40), SETPEN(), PRINT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"),
        NULL);

    TickitWindow *win = tickit_window_new_subwindow(root, 10, 20, 1, 50);

    tickit_window_bind_event(win, TICKIT_EV_EXPOSE, &on_expose_fillchr, "Y");

    tickit_window_expose(win, NULL);
    tickit_window_tick(root);

    is_termlog("Termlog for print sibling under floating window",
        GOTO(10,40), SETPEN(), PRINT("YYYYYYYYYYYYYYYYYYYYYYYYYYYYYY"),
        NULL);

    TickitWindow *popupwin = tickit_window_new_popup(win, 2, 2, 10, 10);
    tickit_window_tick(root);

    is_int(tickit_window_abs_top(popupwin),  12, "popupwin abs_top");
    is_int(tickit_window_abs_left(popupwin), 22, "popupwin abs_left");

    TickitEventInfo keyinfo;
    tickit_window_bind_event(popupwin, TICKIT_EV_KEY, &on_input_capture, &keyinfo);

    press_key(TICKIT_KEYEV_TEXT, "G", 0);

    is_int(keyinfo.type, TICKIT_KEYEV_TEXT, "key type after press_key on popupwin");

    // TODO: mouse events on popupwin

    tickit_window_destroy(popupwin);
    tickit_window_destroy(win);

    tickit_window_unbind_event_id(root, bind_id);
  }

  // Drawing on floats
  {
    int bind_id = tickit_window_bind_event(rootfloat, TICKIT_EV_EXPOSE, &on_expose_textat, "|-- Yipee --|");
    tickit_window_expose(rootfloat, NULL);
    tickit_window_tick(root);

    is_termlog("Termlog for print to floating window",
        GOTO(10,10), SETPEN(), PRINT("|-- Yipee --|"),
        NULL);

    TickitWindow *subwin = tickit_window_new_subwindow(rootfloat, 0, 4, 1, 6);

    tickit_window_bind_event(subwin, TICKIT_EV_EXPOSE, &on_expose_textat, "Byenow");
    tickit_window_expose(subwin, NULL);
    tickit_window_tick(root);

    is_termlog("Termlog for print to child of floating window",
        GOTO(10,14), SETPEN(), PRINT("Byenow"),
        NULL);

    tickit_window_destroy(subwin);
    tickit_window_unbind_event_id(rootfloat, bind_id);
  }

  // TODO: scrolling

  return exit_status();
}