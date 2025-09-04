#include "../../minishell.h"

static int	is_valid_number(char *str)
{
	int	i;

	if (!str || !str[0])
		return (0);
	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	if (!str[i])
		return (0);
	while (str[i])
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (1);
}

static void	cleanup_and_exit(t_list **env, int exit_code)
{
	ft_lstclear(env, &env_free);
	rl_clear_history();
	exit(exit_code);
}

int	builtin_exit(int argc, char **argv, t_list **env)
{
	int		exit_code;
	long	temp;

	ft_printf("exit\n");
	
	if (argc == 1)
	{
		cleanup_and_exit(env, 0);
		return (0); // Never reached
	}
	
	if (argc > 2)
	{
		ft_printf("minishell: exit: too many arguments\n");
		return (1);
	}
	
	if (!is_valid_number(argv[1]))
	{
		ft_printf("minishell: exit: %s: numeric argument required\n", argv[1]);
		cleanup_and_exit(env, 2);
		return (2); // Never reached
	}
	
	temp = ft_atoi(argv[1]);
	exit_code = (int)(temp % 256);
	if (exit_code < 0)
		exit_code = 256 + exit_code;
		
	cleanup_and_exit(env, exit_code);
	return (exit_code); // Never reached
}
