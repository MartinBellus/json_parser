#program name
TARGET=parser
TEST_TARGET=parser_test

#build file
OBJDIR = build
TEST_DIR = test
SRC_DIR = src

#c++ compiler
CC = g++

CSOURCES := $(wildcard $(SRC_DIR)/*.cpp)

ifeq ($(MAKECMDGOALS),test)
CFLAGS = -std=c++23 -g3 -Wall -c -Wno-reorder
CSOURCES := $(filter-out $(SRC_DIR)/main.cpp, $(CSOURCES))
else
CFLAGS = -std=c++23 -O3 -Wall -c -Wno-reorder
endif

TEST_CSOURCES := $(wildcard $(TEST_DIR)/*.cpp)
TEST_COBJECTS := $(patsubst $(TEST_DIR)/%.cpp, $(OBJDIR)/%.o, $(TEST_CSOURCES))
COBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJDIR)/%.o, $(CSOURCES))

INCLUDE := $(addprefix -I, $(wildcard **/include))

#linker
LINKER = g++
LFLAGS =
INCLUDE = "-Iinclude"


# colors
Color_Off='\033[0m'
Black='\033[1;30m'
Red='\033[1;31m'
Green='\033[1;32m'
Yellow='\033[1;33m'
Blue='\033[1;34m'
Purple='\033[1;35m'
Cyan='\033[1;36m'
White='\033[1;37m'

.PHONY: all clean test

all: $(TARGET)
test: $(TEST_TARGET)

$(TARGET): $(COBJECTS)
	@$(LINKER) $(COBJECTS) -o $@ $(LFLAGS)
	@echo -e $(Yellow)"Linking complete!"$(Color_Off)

$(TEST_TARGET): $(COBJECTS) $(TEST_COBJECTS)
	@$(LINKER) $(COBJECTS) $(TEST_COBJECTS) -o $@ $(LFLAGS)
	@echo -e $(Yellow)"Linking complete!"$(Color_Off)

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJDIR)
	@echo -e $(Blue)"C++ compiling "$(Purple)$<$(Color_Off)
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	@echo -e $(Blue)"C++ compiled "$(Purple)$<$(Blue)" successfully!"$(Color_Off)

$(OBJDIR)/%.o: $(TEST_DIR)/%.cpp | $(OBJDIR)
	@echo -e $(Blue)"C++ compiling "$(Purple)$<$(Color_Off)
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@
	@echo -e $(Blue)"C++ compiled "$(Purple)$<$(Blue)" successfully!"$(Color_Off)

$(OBJDIR):
	@mkdir -p  $(OBJDIR)

clean:
	@rm -rf $(TARGET) $(TEST_TARGET) $(OBJDIR)
	@echo -e $(Cyan)"Cleaning Complete!"$(Color_Off)
