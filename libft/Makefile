# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mrosario <mrosario@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2019/11/09 00:04:22 by mrosario          #+#    #+#              #
#    Updated: 2021/01/28 19:41:24 by mrosario         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

SRC = ft_atoi.c ft_bzero.c ft_isalnum.c ft_isalpha.c ft_isascii.c \
	  ft_isdigit.c ft_isprint.c ft_memccpy.c ft_memchr.c ft_memcmp.c \
	  ft_memcpy.c ft_memmove.c ft_memset.c ft_strchr.c ft_strlcat.c \
	  ft_strlen.c ft_strncmp.c ft_strnstr.c ft_strrchr.c ft_tolower.c \
	  ft_toupper.c ft_calloc.c ft_strdup.c ft_strlcpy.c ft_substr.c \
	  ft_strjoin.c ft_strtrim.c ft_split.c ft_itoa.c ft_strmapi.c \
	  ft_putchar_fd.c ft_putstr_fd.c ft_putendl_fd.c ft_putnbr_fd.c \
	  ft_isspace.c ft_putchar.c ft_putstr.c ft_filler.c ft_llitoa.c \
	  ft_llitoa_base.c ft_strtolower.c ft_thousep.c ft_lluitoa.c \
	  ft_lluitoa_base.c ft_itoa_base.c ft_wputstr.c ft_wstrlen.c \
	  ft_chrcmp.c ft_get_next_line.c ft_skipspaces.c ft_skipdigits.c \
	  ft_setbit.c ft_isbitset.c ft_del.c ft_getnextnum.c ft_strcmp.c

BSRC = ft_lstnew_bonus.c ft_lstadd_front_bonus.c ft_lstsize_bonus.c \
	   ft_lstlast_bonus.c ft_lstadd_back_bonus.c ft_lstdelone_bonus.c \
	   ft_lstclear_bonus.c ft_lstiter_bonus.c ft_lstmap_bonus.c

PFSRC = ft_printf.c preppers.c parsers.c printers.c intprinters.c \
		hexprinters.c intformat.c uintformat.c charformat.c checkers.c \
		intprinteraux.c ft_printf_bonus.c preppers_bonus.c \
		parsers_bonus.c printers_bonus.c intprinters_bonus.c \
		hexprinters_bonus.c intformat_bonus.c uintformat_bonus.c \
		charformat_bonus.c checkers_bonus.c intprinteraux_bonus.c

OBJ = $(SRC:.c=.o)

BOBJ = $(BSRC:.c=.o)

PFOBJ = $(PFSRC:.c=.o)

CFLAG = -Wall -Werror -Wextra -I

all: $(NAME)

$(NAME): libft.h ./printf/libftprintf.h
	gcc $(CFLAG) ./ -c $(SRC) $(BSRC) $(addprefix ./printf/,$(PFSRC))
	ar -rc $(NAME) $(OBJ) $(BOBJ) $(PFOBJ)
	ranlib $(NAME)

clean:
	rm -f *.o

fclean: clean
	rm -f $(NAME)

re: fclean all

bonus: libft.h ./printf/libftprintf.h
	gcc $(CFLAG) ./ -c $(SRC) $(BSRC) $(addprefix ./printf/,$(PFSRC))
	ar -rc $(NAME) $(OBJ) $(BOBJ) $(PFOBJ)
	ranlib $(NAME)

.PHONY: all clean fclean re bonus
