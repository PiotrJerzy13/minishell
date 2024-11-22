# Compiler and flags
CC = cc
CFLAGS = -Wall -Wextra -Werror
LDFLAGS = -lreadline  # Link with the readline library
OBJDIR = ./bin
SRCS = main.c enviroment.c memories.c commands.c token.c token_helper.c copy_enviroment.c builtin.c signals.c command_helper.c heredoc.c token_helper2.c token_helper3.c handle_buildins.c builtins_helper.c execute_command.c execute_commands2.c token_helper4.c token_helper5.c command_helper2.c command_helper3.c main_helper.c
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
