/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pwojnaro <pwojnaro@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 18:30:45 by pwojnaro          #+#    #+#             */
/*   Updated: 2024/11/01 18:17:13 by pwojnaro         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// The handle_sigint function is used to handle the SIGINT signal. It is called
// when the user presses Ctrl+C. The function prints a newline character and
// clears the current line using the readline library functions.
// The toggle_sigquit function is used to enable or disable the default behavior
// of the SIGQUIT signal. If enable_default is true, the default behavior is
// enabled. If enable_default is false, the default behavior is ignored.
// The initialize_shell function is used to set up the shell environment. It
// initializes the SIGINT signal handler and disables the default behavior of
// the SIGQUIT signal. It also disables echoing of control characters in the
// terminal.

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
