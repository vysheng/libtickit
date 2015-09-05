#include <tickit.h>

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static void (*debug_func)(const char *str, void *data);
static void *debug_func_data;

static FILE *debug_fh;

static bool flag_enabled(const char *flag)
{
  return true; // todo
}

void tickit_debug_set_func(void (*func)(const char *str, void *data), void *data)
{
  debug_func      = func;
  debug_func_data = data;

  if(debug_fh)
    fclose(debug_fh);
}

void tickit_debug_set_fh(FILE *fh)
{
  if(debug_fh)
    fclose(debug_fh);

  debug_fh = fh;

  if(debug_func)
    debug_func = NULL;
}

bool tickit_debug_open(const char *path)
{
  FILE *fh = fopen(path, "a");
  if(!fh)
    return false;

  tickit_debug_set_fh(fh);
  return true;
}

void tickit_debug_logf(const char *flag, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  tickit_debug_vlogf(flag, fmt, args);

  va_end(args);
}

void tickit_debug_vlogf(const char *flag, const char *fmt, va_list args)
{
  if(!debug_fh && !debug_func)
    return;

  if(!flag_enabled(flag))
    return;

  struct timeval now;
  gettimeofday(&now, NULL);

  char timestamp[9];
  strftime(timestamp, sizeof timestamp, "%H:%M:%S", localtime(&now.tv_sec));

#define LINE_PREFIX "%s.%03d [%-3s]: "

  if(debug_func) {
    size_t len;
    va_list args_copy;
    va_copy(args_copy, args);
    len = snprintf(NULL, 0, LINE_PREFIX, timestamp, 0, flag) +
      vsnprintf(NULL, 0, fmt, args_copy) +
      1;
    va_end(args_copy);

    char *buf = malloc(len + 1);
    {
      char *s = buf;

      s += sprintf(s, LINE_PREFIX, timestamp, (int)(now.tv_usec / 1000), flag);
      s += vsprintf(s, fmt, args);
      s += sprintf(s, "\n");
    }

    (*debug_func)(buf, debug_func_data);

    free(buf);
  }
  else if(debug_fh) {
    fprintf(debug_fh, LINE_PREFIX, timestamp, (int)(now.tv_usec / 1000), flag);
    vfprintf(debug_fh, fmt, args);
    fprintf(debug_fh, "\n");
  }

#undef LINE_PREFIX
}
