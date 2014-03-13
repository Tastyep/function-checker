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
      printf("%d | Breaking %s: %c\n", *count, func, *c);
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

int	pipe(int pipefd[2])
{
  char	state = 'y';
  int	repeat = 0;
  int	(*o_pipe)(int pipefd[2]);

  o_pipe = dlsym(RTLD_NEXT, "pipe");
  return ((break_func("pipe", &repeat, &state)) ?
	  -1 : (*o_pipe)(pipefd));
}

int	open(const char *pathname, int flags)
{
  char	state = 'y';
  int	repeat = 0;
  int	(*o_open)(const char *pathname, int flags);

  o_open = dlsym(RTLD_NEXT, "open");
  printf("on file %s\n", pathname);
  return ((break_func("open", &repeat, &state)) ?
	  -1 : (*o_open)(pathname, flags));
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
      printf("read/%d: ", fd);
      fflush(stdout);
      (*o_read)(fd, buf, count);
      return (val);
    }
  printf("read/%d: ",fd);
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
