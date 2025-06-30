CXX = c++
CXXFLAGS = -Wall -Wextra -Werror $(INCLUDES) -std=c++98 -fsanitize=address -g3
INCLUDES = -Iinclude

# COLORS
GREEN = \033[0;32m
RED = \033[0;31m
RESET = \033[0m
ORANGE = \033[0;38;5;166m

NAME = webserv

# Source Files
SRCDIR = src/
SRCS_FIL = \
		main.cpp \
		sockets-polling/poll.cpp \
		sockets-polling/handle-connections.cpp \
		sockets-polling/utils.cpp \
		sockets-polling/setup-listening-socket.cpp \
		sockets-polling/read-request-header.cpp \
		sockets-polling/read-request-body.cpp \
		sockets-polling/Connection.cpp \
		sockets-polling/timeout.cpp \
		sockets-polling/chunked-encoding.cpp \
		\
		config-parser/Configuration.cpp \
		http-request/http-request.cpp \
		http-request/utils.cpp \
		sockets-polling/utils2.cpp \
		http-response/http-response.cpp \
		http-response/utils.cpp \
		http-response/get-handler.cpp \
		http-response/post-handler.cpp \
		cgi-handler/cgi-handle.cpp \
		cgi-handler/cgi-response.cpp \
		utils/utils.cpp

SRCS = $(addprefix $(SRCDIR), $(SRCS_FIL))

# Object files
OBJDIR = objs/
OBJS = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRCS))

# Build targets
all: $(OBJDIR) $(NAME)

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(addprefix $(OBJDIR), $(dir $(SRCDIR)))

$(NAME): $(OBJS)
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) && echo "$(GREEN)$(NAME) was created$(RESET)"


$(OBJDIR)%.o: $(SRCDIR)%.c
	@$(CXX) $(CXXFLAGS) -c $< -o $@ && echo "$(GREEN)object files were created$(RESET)"

# Clean Up
RM = rm -rf

clean:
	@$(RM) $(OBJDIR) && echo "$(ORANGE)object files were deleted$(RESET)"

fclean: clean
	@$(RM) $(NAME) && echo "$(ORANGE)$(NAME) was deleted$(RESET)"

re: fclean all

.PHONY: all clean fclean re bonus
