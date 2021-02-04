# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/26 15:42:39 by mrosario          #+#    #+#              #
#    Updated: 2021/02/04 20:54:19 by mrosario         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC = main.c exit_handling.c memory_handling.c tokenize.c flag_toggles.c \
cmd_execution.c

LIBFT = ./libft/libft.a

OBJ = $(SRC:.c=.o)

CFLAG = -Wall -Werror -Wextra

LIBS = -I ./libft/ -L ./libft -lft

$(NAME): $(LIBFT)
	gcc $(CFLAG) $(SRC) -o $(NAME) $(LIBS) 

$(LIBFT):
	make -C ./libft
	make clean -C ./libft

all: $(NAME)

debug: $(LIBFT)
	gcc $(CFLAG) $(SRC) -g -o $(NAME) $(LIBS)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)
	make fclean -C ./libft

re: fclean all

.PHONY: all debug clean fclean re%