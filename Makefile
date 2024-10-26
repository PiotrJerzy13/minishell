# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror
OBJDIR = ./bin
SRCS = main.c enviroment.c memories.c commands.c token.c token_helper.c enviroment2.c copy_enviroment.c builtin.c
OBJS = $(SRCS:%.c=$(OBJDIR)/%.o)

# Name of the executable
TARGET = Minishell

# Default rule to compile the program
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

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
