# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miki <miki@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/26 15:42:39 by mrosario          #+#    #+#              #
#    Updated: 2021/01/27 19:46:58 by miki             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC = main.c

OBJ = $(SRC:.c=.o)

CFLAG = -Wall -Werror -Wextra

#$(LIBFT):
#	make -C ./libft
#	make clean -C ./libft

$(NAME): #$(LIBFT)
	gcc $(CFLAG) $(SRC) -o $(NAME)

all: $(NAME)

debug: #$(LIBFT)
	gcc $(CFLAG) $(SRC) -g -o $(NAME)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)
	#make fclean -C ./libft

re: fclean all

.PHONY: all debug clean fclean re%