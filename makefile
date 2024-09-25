SRCSPRINTF = malcolm.c
OBJSPRINTF = $(patsubst %.c, objects/%.o, $(SRCSPRINTF))

CFLAGS = -Wall -Werror -Wextra -g3 -Ilibft

NAME = ft_malcolm

$(NAME) : libft/libft.a $(OBJSPRINTF)
	gcc $(CFLAGS) -o $(NAME) $(OBJSPRINTF) ./libft/libft.a -lX11 -lXext -lm

all : $(NAME)

libft/libft.a: FORCE
	$(MAKE) all -C ./libft

objects/%.o : %.c malcolm.h libft/libft.h
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

clean : 
	$(MAKE) clean -C ./libft
	rm -rf objects

fclean : clean
	rm -f $(NAME)

re : fclean all

FORCE:

.PHONY:  all clean fclean re