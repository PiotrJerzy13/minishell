/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 18:30:45 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/12/03 18:12:35 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_sigint(int signum)
{
	(void)signum;
	printf("\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

void	toggle_sigquit(int enable_default)
{
	struct sigaction	sa_quit;

	sigemptyset(&sa_quit.sa_mask);
	if (enable_default)
	{
		sa_quit.sa_handler = SIG_DFL;
	}
	else
	{
		sa_quit.sa_handler = SIG_IGN;
	}
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

int	initialize_shell(void)
{
	struct sigaction	sa_int;
	struct termios		term;

	signal(SIGQUIT, SIG_IGN);
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_handler = handle_sigint;
	sa_int.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &sa_int, NULL) == -1)
	{
		perror("Failed to set up SIGINT handler");
		return (-1);
	}
	toggle_sigquit(0);
	if (tcgetattr(0, &term) == -1)
		return (-1);
	term.c_lflag &= ~ECHOCTL;
	if (tcsetattr(0, TCSANOW, &term) == -1)
		return (-1);
	return (0);
}
