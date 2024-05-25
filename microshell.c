#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

void	put_two_strings(char *str, char *str2, int fd)
{
	while (*str)
		write(fd, str++, 1);
	if (str2)
	{
		while (*str2)
				write(fd, str2++, 1);
	}
	write(fd, "\n", 1);
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

void	child_process(char **argv, int io[2], int size)
{
	char	**cmd_table = (char**)malloc(sizeof(char *) * size);

	for (int i = 0; i < size; i++)
			cmd_table[i] = argv[i];
	dup2(io[1], 1);
	execve(argv[0], cmd_table, NULL);
}

void	shell(char **argv, int cmd_count)
{
	int			i;
	int			temp;
	int			pipefd[2];
	pid_t		pid;

	i = -1;
	if (cmd_count == 1)
	{
		pid = fork();
		if (pid == 0)
				execve(argv[0], argv, NULL);
		return ;
	}
	while (argv[++i])
	{
		if (!strcmp(argv[i], "|"))
		{
			argv[i][0] = '\0';
			temp = i + 1;
			break;
		}
	}
	if (pipe(pipefd) == -1)
			return ;

	pid = fork();
	if (pid == 0)
			child_process(argv, pipefd, i);
	argv += temp;
}

int	main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)envp;
	int		cmd_count = cmd_counter(argv);

	argv++;
	for (int i = 0; i < cmd_count; i++)
			shell(argv, cmd_count);
}
