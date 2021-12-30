NAME = tictactoe
SRC = main.cpp Game.cpp
OBJ = $(patsubst %.cpp, %.o, $(SRC))
FLAG = -std=c++14

all: $(NAME)

$(NAME): $(OBJ)
		c++ $(FLAG) -o $(NAME) $(OBJ)

%.o : %.cpp
		c++ $(FLAG) -c $< -o $@

clean:
		rm -rf $(OBJ)

fclean: clean
		rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
