#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

typedef struct s_cmd
{
	char	**cmd;
	int		fd[2];
}	t_cmd;

void	putstr_error(char *str, char *str2)
{
	while (*str)
		write(2, str++, 1);
	if (str2)
	{
		while (*str2)
				write(2, str2++, 1);
	}
	write(2, "\n", 1);
}

int		cmd_counter(char **args)
{
	int	i = 0;
	int	res = 1;

	while (args[i])
	{
		if (strcmp(args[i], "|") == 0 || strcmp(args[i], ";") == 0)
			res++;
		i++;
	}
	return (res);
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)envp;
	int	cmd_count = cmd_counter(argv);
	
	printf("%i\n", cmd_count);
	putstr_error("error: cd: cannot change directory to ", NULL);
}
