# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lreadline  # Link with the readline library
OBJDIR = ./bin
SRCS = main.c enviroment.c memories.c handle_commands.c expansions.c token_quotes.c commands.c token.c \
token_helper.c copy_enviroment.c builtin.c signals.c command_node.c redirector.c heredoc.c shell_initializer.c \
input_parser.c builtins_helper.c execute_command.c process_token.c execute_pipes.c handle_special_token.c \
handle_special_char.c handle_builtins.c handle_redirections.c double_quotes.c execution.c execute_command2.c replacing.c \
helper_functions.c replacing2.c


OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

# Name of the executable
TARGET = minishell

# Default rule to compile the program
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile .c files into .o object files in the OBJDIR
$(OBJDIR)/%.o: %.c
	@mkdir -p $(OBJDIR)  # Create bin directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and the executable
clean:
	rm -f $(OBJS)
	@if [ -d $(OBJDIR) ]; then find $(OBJDIR) -type d -empty -exec rmdir {} +; fi

# Full clean including object files and the executable
fclean: clean
	rm -f $(TARGET)
	rm -rf $(OBJDIR)

re: fclean all

.PHONY: all clean fclean re
