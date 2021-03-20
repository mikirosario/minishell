# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mvillaes <mvillaes@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/01/26 15:42:39 by mrosario          #+#    #+#              #
#    Updated: 2021/03/20 21:04:11 by mvillaes         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minishell

BUILTINS := builtins
LIB := clean_lib

SRC = main.c exit_handling.c memory_reservation.c memory_free.c flag_toggles.c \
cmd_execution.c env_var_handling.c iamerror.c signals.c pipe_handling.c \
process_raw_line.c process_cmdline.c process_token.c process_char.c  envp_dup.c \
char_check.c char_operations.c redirection.c child_process.c find_cmd_path.c \
$(BUILTINS)/export.c \
$(BUILTINS)/builtins_helper.c \
$(BUILTINS)/cd.c \
$(BUILTINS)/echo.c \
$(BUILTINS)/env.c \
$(BUILTINS)/unset.c \
$(BUILTINS)/pwd.c \
$(LIB)/ft_bzero.c \
$(LIB)/ft_calloc.c \
$(LIB)/ft_del.c \
$(LIB)/ft_free_split.c \
$(LIB)/ft_isalpha.c \
$(LIB)/ft_isdigit.c \
$(LIB)/ft_isspace.c \
$(LIB)/ft_itoa.c \
$(LIB)/ft_lstadd_back_bonus.c \
$(LIB)/ft_lstnew_bonus.c \
$(LIB)/ft_memchr.c \
$(LIB)/ft_memcmp.c \
$(LIB)/ft_memcpy.c \
$(LIB)/ft_putchar.c \
$(LIB)/ft_skipspaces.c \
$(LIB)/ft_split.c \
$(LIB)/ft_strchr.c \
$(LIB)/ft_strcmp.c \
$(LIB)/ft_strdup.c \
$(LIB)/ft_strjoin.c \
$(LIB)/ft_strlcat.c \
$(LIB)/ft_strlen.c \
$(LIB)/ft_strncmp.c \
$(LIB)/ft_strncpy.c \
$(LIB)/ft_isbitset.c \
$(LIB)/ft_lstclear_bonus.c \
$(LIB)/ft_lstlast_bonus.c \
$(LIB)/ft_strtrim.c \
$(LIB)/ft_strlcpy.c \
$(LIB)/ft_lstdelone_bonus.c

# LIBFT = ./libft/libft.a

OBJ = $(SRC:.c=.o)

CFLAG = -Wall -Werror -Wextra

# LIBS = -I ./libft/ -L ./libft -lft

$(NAME): $(LIBFT)
	gcc $(CFLAG) $(SRC) -o $(NAME) $(LIBS) 

# $(LIBFT):
# 	make -C ./libft
# 	make clean -C ./libft

all: $(NAME)

debug: $(LIBFT)
	gcc $(CFLAG) $(SRC) -g -o $(NAME) $(LIBS)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)
	# make fclean -C ./libft

re: fclean all

rebug: fclean debug

.PHONY: all debug clean fclean re rebug