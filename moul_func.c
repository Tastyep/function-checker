#define _GNU_SOURCE

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int		break_func(const char *func, int *count, char *c)
{
  char		tmp[10] = {0};
  int		i = 0;

  if (*count > 0)
    {
      --(*count);
      printf("%d | Breaking malloc: %c\n", *count, *c);
      return (*c == 'y');
    }
  printf("Last: %c | Break %s y/n ?\n", *c, func);
  while (i == 0 || (i < 10 && tmp[i - 1] != '\n'))
    tmp[i++] = getchar();
  tmp[i - 1] = '\0';
  if (!tmp[0])
    return (*c == 'y');
  else if (tmp[0] == 'e')
    exit(0);
  if (tmp[1])
    *count = atoi(&tmp[1]);
  *c = tmp[0];
  return (*c == 'y' || *c == '\n');
}

int	get_num()
{
  char	tab[10] = {0};
  int	i = 0;

  while (i == 0 || (i < 10 && tab[i - 1] != '\n'))
    tab[i++] = getchar();
  tab[i - 1] = '\0';
  return (atoi(tab));
}

int	open(const char *pathname, int flags)
{
  int	(*o_open)(const char *pathname, int flags);

  o_open = dlsym(RTLD_NEXT, "open");
  return ((*o_open)(pathname, flags));
}

ssize_t	read(int fd, void *buf, size_t count)
{
  static int	repeat = 0;
  static char	state = 'y';
  static int	val = 0;
  int		old = repeat;
  ssize_t	(*o_read)(int fd, void *buf, size_t count);

  o_read = dlsym(RTLD_NEXT, "read");
  printf("\n");
  if (break_func("read", &repeat, &state))
    {
      if (!old || !repeat)
	{
	  printf("Value: ");
	  fflush(stdout);
	  val = get_num();
	}
      printf("stdin: ");
      fflush(stdout);
      (*o_read)(fd, buf, count);
      return (val);
    }
  printf("stdin: ");
  fflush(stdout);
  return ((*o_read)(fd, buf, count));
}

void	*malloc(size_t size)
{
  static int	repeat = 0;
  static char	state = 'y';
  void	*(*o_malloc)(size_t size);

  o_malloc = dlsym(RTLD_NEXT, "malloc");
  return (break_func("malloc", &repeat, &state) ? NULL : (*o_malloc)(size));
}
