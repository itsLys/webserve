NAME		= webserve
CPPC		= c++
CFLAGS		= -ggdb -Wall -Wextra -Werror --std=c++98 -pedantic
LFLAGS		= -lstdc++
SRC         = Client.cpp \
			  ClientTable.cpp \
			  TcpServer.cpp \
			  Socket.cpp \
			  EventLoop.cpp \
			  helper.cpp \
			  main.cpp \

OBJ         = $(SRC:.cpp=.o)

all: $(NAME)
	@echo "$(NAME) done!"

push: fclean
	read -p "commit message: " msg; \
		git add .; \
		git commit -am "$$msg"; \
		git push

run: re
	make clean
	./webserve

runv: re
	valgrind ./webserve

$(NAME): $(OBJ)
	$(CPPC) $(OBJ) $(LFLAGS) -o $(NAME)

%.o: %.cpp
	$(CPPC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: clean
