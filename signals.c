/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 18:30:45 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/10/27 19:19:56 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ignore_signals(int signum, siginfo_t *info, void *context)
{
	(void)signum;
	(void)info;
	(void)context;
}

void	handle_sigint(int signum, siginfo_t *info, void *context)
{
	(void)signum;
	(void)info;
	(void)context;
	printf("\n");
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

int	setup_signal_handler(int signal, void (*handler)(int, siginfo_t *, void *))
{
	struct sigaction	action;

	sigemptyset(&action.sa_mask);
	action.sa_sigaction = handler;
	action.sa_flags = SA_SIGINFO | SA_RESTART;
	if (sigaction(signal, &action, NULL) == -1)
	{
		perror("sigaction error");
		return (-1);
	}
	return (0);
}

int	initialize_shell(void)
{
	struct termios	term;

	if (setup_signal_handler(SIGINT, handle_sigint) == -1)
		return (-1);
	if (setup_signal_handler(SIGQUIT, ignore_signals) == -1)
		return (-1);
	if (tcgetattr(STDIN_FILENO, &term) == -1)
		return (-1);
	term.c_lflag &= ~ECHOCTL;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
		return (-1);
	return (0);
}
