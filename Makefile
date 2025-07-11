CXX = c++
CXXFLAGS = -Wall -Wextra -Werror $(INCLUDES) -std=c++98 -MMD -MP #-fsanitize=address -g3
INCLUDES = -Iinclude

# COLORS
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m
ORANGE = \033[0;38;5;166m

NAME = webserv

# Source Files
SRCDIR = src/
SRCS := $(shell find $(SRCDIR) -name '*.cpp')

# Object files
OBJDIR = objs/
OBJS = $(patsubst $(SRCDIR)%.cpp, $(OBJDIR)%.o, $(SRCS))

# Dependency files (one per object file)
DEPS = $(OBJS:.o=.d)

# SUBDIRS = $(sort $(dir $(SRCS_FIL)))
SUBDIRS = $(sort $(dir $(patsubst $(SRCDIR)%,%,$(SRCS))))

# Build targets
all: $(OBJDIR) $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(addprefix $(OBJDIR), $(SUBDIRS))

$(NAME): $(OBJS) 
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) && echo "$(GREEN)$(NAME) was created$(RESET)"


$(OBJDIR)%.o: $(SRCDIR)%.cpp 
	@$(CXX) $(CXXFLAGS) -c $< -o $@ && echo "$(GREEN)object files were created$(RESET)"

# Clean Up
RM = rm -rf

clean:
	@$(RM) $(OBJDIR) && echo "$(ORANGE)object files were deleted$(RESET)"

fclean: clean
	@$(RM) $(NAME) && echo "$(ORANGE)$(NAME) was deleted$(RESET)"

re: fclean all

# Include dependency files if they exist
-include $(DEPS)

.PHONY: all clean fclean re bonus
