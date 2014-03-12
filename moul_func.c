#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int		break_func(const char *func)
{
  static char	c = 'y';
  static int	count = 0;
  char		tmp[10] = {0};
  int		i = 0;

  if (count > 0)
    {
      --count;
      printf("%d | Breaking malloc: %c\n", count, c);
      return (c == 'y');
    }
  printf("Last: %c | Break %s y/n ?\n", c, func);
  while (i == 0 || (i < 10 && tmp[i - 1] != '\n'))
    tmp[i++] = getchar();
  tmp[i - 1] = '\0';
  if (!tmp[0])
    return (c == 'y');
  else if (tmp[0] == 'e')
    exit(0);
  if (tmp[1])
    count = atoi(&tmp[1]);
  c = tmp[0];
  return (c == 'y' || c == '\n');
}

int	ret_val()
{
  printf("value: ");
  return ((int)getchar());
}

int	open(const char *pathname, int flags)
{
  int	(*o_open)(const char *pathname, int flags);

  o_open = dlsym(RTLD_NEXT, "open");
  return ((*o_open)(pathname, flags));
  return (break_func("open") ? ret_val() : (*o_open)(pathname, flags));
}

ssize_t	read(int fd, void *buf, size_t count)
{
  ssize_t (*o_read)(int fd, void *buf, size_t count);

  o_read = dlsym(RTLD_NEXT, "read");
  return ((*o_read)(fd, buf, count));
  return (break_func("read") ? 0 : (*o_read)(fd, buf, count));
}

void	*malloc(size_t size)
{
  void	*(*o_malloc)(size_t size);

  o_malloc = dlsym(RTLD_NEXT, "malloc");
  return ((break_func("malloc") == 1) ? NULL : (*o_malloc)(size));
}
