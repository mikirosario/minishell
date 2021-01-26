# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/26 15:42:39 by mrosario          #+#    #+#              #
#    Updated: 2021/01/26 15:48:00 by mrosario         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC = main.c

OBJ = $(SRC:.c=.o)

CFLAG = -Wall -Werror -Wextra

$(NAME): #$(LIBFT)
	gcc $(CFLAG) $(SRC) -o $(NAME)

#$(LIBFT):
#	make -C ./libft
#	make clean -C ./libft

all: $(NAME)

debug: #$(LIBFT)
	gcc $(CFLAG) $(LIBS) $(FRMWRK) $(SRC) -g -o $(NAME)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)
	#make fclean -C ./libft

re: fclean all

.PHONY: all debug clean fclean re%