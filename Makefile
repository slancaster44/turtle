 
CXX := clang++
CXXFLAGS :=

TARGET_NAME := turtle

SRC := src/lexer.cpp src/token.cpp src/ast.cpp src/parser.cpp src/codegen.cpp src/main.cpp
LLVM_FLAGS := `llvm-config --cxxflags --ldflags --system-libs --libs --libs core`

CLEAN_LIST := $(TARGET_NAME)

default:
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST_SRC) $(LLVM_FLAGS) -o $(TARGET_NAME)

clean:
	rm -f $(CLEAN_LIST)

run:
	make
	echo ""
	./$(TARGET_NAME)
	make -s clean
