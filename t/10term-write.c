#include "tickit.h"
#include "taplib.h"

#include <string.h>

int main(int argc, char *argv[])
{
  TickitTerm *tt;
  int    fd[2];
  char   buffer[1024];
  size_t len;

  /* We'll need a real filehandle we can write/read.
   * pipe() can make us one */
  pipe(fd);

  plan_tests(9);

  tt = tickit_term_new_for_termtype("xterm");

  ok(!!tt, "tickit_term_new_for_termtype");

  tickit_term_set_output_fd(tt, fd[1]);

  is_int(tickit_term_get_output_fd(tt), fd[1], "tickit_term_get_output_fd");

  /* Already it should have written its DECSLRM probe string */
  len = read(fd[0], buffer, sizeof buffer);
  buffer[len] = 0;

  is_str_escape(buffer, "\e[?69h\e[?69$p", "buffer after initialisation contains DECSLRM probe");

  tickit_term_print(tt, "Hello world!");

  len = read(fd[0], buffer, sizeof buffer);
  buffer[len] = 0;

  is_int(len, 12, "read() length after tickit_term_print");
  is_str_escape(buffer, "Hello world!", "buffer after tickit_term_print");

  tickit_term_goto(tt, 2, 5);

  len = read(fd[0], buffer, sizeof buffer);
  buffer[len] = 0;

  is_str_escape(buffer, "\e[3;6H", "buffer after tickit_term_goto line+col");

  tickit_term_goto(tt, 4, -1);

  len = read(fd[0], buffer, sizeof buffer);
  buffer[len] = 0;

  is_str_escape(buffer, "\e[5d", "buffer after tickit_term_goto line");

  tickit_term_goto(tt, -1, 10);

  len = read(fd[0], buffer, sizeof buffer);
  buffer[len] = 0;

  is_str_escape(buffer, "\e[11G", "buffer after tickit_term_goto col");

  tickit_term_destroy(tt);

  ok(1, "tickit_term_destroy");

  return exit_status();
}
