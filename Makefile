NAME = webserver
CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -fsanitize=address 
SRCS = main.cpp $(wildcard server/*.cpp) $(wildcard utils/*.cpp) $(wildcard parser/*.cpp) $(wildcard webserv/*.cpp) $(wildcard cgi/*.cpp) $(wildcard client/*.cpp)  $(wildcard Request/*.cpp) $(wildcard Response/*.cpp)   
OBJDIR = obj
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.cpp=.o))
INC = webserv/webserv.hpp server/server.hpp parser/parser.hpp utils/utils.hpp  cgi/cgi.hpp client/Client.hpp Request/Request.hpp Response/Response.hpp

all: $(NAME)

$(NAME): $(OBJS) 
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(MAGENTA)$(NAME): $(GREEN)created$(RESET)"
	@echo "$(WHITE)=> Usage: ./$(GREEN)$(NAME)$(WHITE) or ./$(GREEN)$(NAME)$(BLUE) [config_file]"

$(OBJDIR)/%.o: %.cpp $(INC)
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(MAGENTA)$(NAME): $(BLUE)$<$(GREEN) -> $(YELLOW)$@$(RESET)"

clean:
	@rm -rf $(OBJDIR)
	@echo "$(MAGENTA)$(NAME): $(YELLOW)$(OBJDIR) $(RED)deleted$(RESET)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(MAGENTA)$(NAME):$(RED) deleted$(RESET)"

re: fclean all

.PHONY: all clean fclean re

RED = \033[1;31m
GREEN = \033[1;32m
MAGENTA = \033[1;35m
BLUE = \033[1;34m
YELLOW = \033[1;33m
RESET = \033[0m
WHITE = \033[1;37m