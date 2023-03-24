
CC = c++
CFLAGS = -Wall -Wextra -Werror -O2 -MD -g -std=c++98

NAME = webserv
SRC_DIR = src
SRC = main.cpp Webserv.cpp Server.cpp XML/XMLDocument.cpp XML/XMLElement.cpp UUID/UUID.cpp Logger.cpp Request.cpp Response.cpp util.cpp MimeTypes.cpp Route.cpp
BUILD_DIR = build
INCLUDE_DIR = includes
INCLUDES = -I./$(INCLUDE_DIR)
LIB_DIR = lib
LIBS =


# Do not change these
SRC_FILES = $(addprefix $(SRC_DIR)/, $(SRC))
OBJ_DIR = $(BUILD_DIR)/obj
OBJ = $(SRC:.cpp=.o)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(OBJ))

-include $(OBJ_FILES:.o=.d)

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "\033[0;32mCompiling $<\033[0m"
	@mkdir -p $(basename $@)
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

testSocketClient:
	@echo "\033[0;32mCompiling $(SRC_DIR)/testSocketClient.cpp\033[0m"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(SRC_DIR)/testSocketClient.cpp -o $(OBJ_DIR)/testSocketClient.o
	@$(CC) $(CFLAGS) $(OBJ_DIR)/testSocketClient.o $(LIBS) $(INCLUDES) -o $(BUILD_DIR)/testSocketClient
	@echo "\033[0;33mFinished building testSocketClient\033[0m"

$(NAME): $(OBJ_FILES)
	@echo "\033[0;34mLinking $@\033[0m"
	@$(CC) $(CFLAGS) $(OBJ_FILES) $(LIBS) $(INCLUDES) -o $(BUILD_DIR)/$(NAME)
	@echo "\033[0;33mFinished building $@\033[0m"

clean:
	@echo "\033[0;31mDeleting $(OBJ_DIR)\033[0m"
	@rm -rf $(OBJ_DIR)

fclean: clean
	@echo "\033[0;31mDeleting $(BUILD_DIR)\033[0m"
	@rm -rf $(BUILD_DIR)

re: fclean all

.PHONY: all clean fclean re
