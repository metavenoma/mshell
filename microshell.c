#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

void	print_error(char *error, char *arg)
{
	while (*error)
		write(2, error++, 1);
	if (arg)
	{
		while(*arg)
			write(2, arg++, 1);
	}
	write(2, "\n", 1);
}

void	exec(char **argv, char **envp, int fd_in, int i, int temp)
{
	argv[i] = NULL;
	if (fd_in != 0)
	{
		dup2(fd_in, STDIN_FILENO);
		close(fd_in);
	}
	execve(argv[temp], &argv[temp], envp);
	print_error("error: cannot execute ", argv[temp]);
}

void	shell(char **argv, char **envp, int argc)
{
	int	i = 0;
	int	fd_in = 0;
	int	pipefd[2];
	pid_t	pid;

	while (argv[i])
	{
		int temp = i;		
		while (argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
			i++;
		if (!strcmp(argv[temp], "cd"))
		{
			if (!argv[temp + 1] || (argv[temp + 1] && argv[temp + 2] != NULL && strcmp(argv[temp + 2], ";")))
				print_error("error: cd: bad arguments", NULL);
			if (chdir(argv[temp + 1]) == -1)
				print_error("error: cd: cannot change directory to ", argv[temp + 1]);
		}
		else if (i != 0 && (argv[i] == NULL || !strcmp(argv[i], ";")))
		{
			pid = fork();
			if (pid == 0)
			{
				exec(argv, envp, fd_in, i, temp);
			}
			else
			{
				close(fd_in);
				while (waitpid(-1, NULL, WUNTRACED) != -1)
					;
				fd_in = 0;
			}
		}
		else if (i != 0 && !strcmp(argv[i], "|"))
		{
			if (pipe(pipefd) == -1)
				return ;
			pid = fork();
			if (pid == 0)
			{
				dup2(pipefd[1], STDOUT_FILENO);
				close(pipefd[1]);
				close(pipefd[0]);
				exec(argv, envp, fd_in, i, temp);
			}
			else
			{
				if (fd_in != 0)
					close(fd_in);
				close(pipefd[1]);
				fd_in = pipefd[0];
			}
		}
		if (i < argc - 1)
			i++;
	}
}

int	main(int argc, char **argv, char **envp)
{
	if (argc < 2)
	{
		print_error("error: wrong arguments", NULL);
		return (0);
	}
	argv++;
	shell(argv, envp, argc);
}
