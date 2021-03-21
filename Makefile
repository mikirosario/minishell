# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: miki <miki@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/26 15:42:39 by mrosario          #+#    #+#              #
#    Updated: 2021/03/21 10:02:12 by miki             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

BUILTINS := builtins

SRC = main.c exit_handling.c memory_reservation.c memory_free.c flag_toggles.c \
cmd_execution.c env_var_handling.c iamerror.c signals.c pipe_handling.c \
process_raw_line.c process_cmdline.c process_token.c process_char.c  envp_dup.c \
char_check.c char_operations.c redirection.c child_process.c find_cmd_path.c \
cmd_history_alloc.c terminal_settings.c termcaps.c \
$(BUILTINS)/export.c \
$(BUILTINS)/builtins_helper.c \
$(BUILTINS)/cd.c \
$(BUILTINS)/echo.c \
$(BUILTINS)/env.c \
$(BUILTINS)/unset.c \
$(BUILTINS)/pwd.c \

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

rebug: fclean debug

.PHONY: all debug clean fclean re rebug