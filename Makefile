# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/26 15:42:39 by mrosario          #+#    #+#              #
<<<<<<< HEAD
#    Updated: 2021/01/28 19:16:41 by mrosario         ###   ########.fr        #
=======
#    Updated: 2021/02/05 18:24:48 by mrosario         ###   ########.fr        #
>>>>>>> 532689b893087cc7491dabb447c07a1614052ec2
#                                                                              #
# **************************************************************************** #

NAME = minishell

SRC = main.c exit_handling.c memory_reservation.c memory_free.c tokenize.c \
flag_toggles.c cmd_execution.c

LIBFT = ./libft/libft.a

LIBFT = ./libft/libft.a

OBJ = $(SRC:.c=.o)

CFLAG = -Wall -Werror -Wextra

LIBS = -I ./libft/ -L ./libft -lft

$(NAME): $(LIBFT)
<<<<<<< HEAD
	gcc $(CFLAG) $(LIBS) $(SRC) -o $(NAME)
=======
	gcc $(CFLAG) $(SRC) -o $(NAME) $(LIBS) 
>>>>>>> 532689b893087cc7491dabb447c07a1614052ec2

$(LIBFT):
	make -C ./libft
	make clean -C ./libft

all: $(NAME)

debug: $(LIBFT)
<<<<<<< HEAD
	gcc $(CFLAG) $(LIBS) $(SRC) -g -o $(NAME)
=======
	gcc $(CFLAG) $(SRC) -g -o $(NAME) $(LIBS)
>>>>>>> 532689b893087cc7491dabb447c07a1614052ec2

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)
	make fclean -C ./libft

re: fclean all

.PHONY: all debug clean fclean re%