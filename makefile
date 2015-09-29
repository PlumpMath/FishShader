CXX = clang++
SDL = -framework SDL2
OPENGL = -framework OpenGL
GLEWPATH = /usr/local/Cellar/glew/1.11.0
GLEW = -L$(GLEWPATH)/lib -lGLEW
GLMPATH = /usr/local/Cellar/glm/0.9.5.4
INC = -I$(GLEWPATH)/include -I$(GLMPATH)/include

# If your compiler is a bit older you may need to change -std=c++11 to -std=c++0x
CXXFLAGS = $(INC) -Wall -c -std=c++11 -F/Library/Frameworks
LDFLAGS = $(INC) $(SDL) $(OPENGL) $(GLEW) -F/Library/Frameworks
EXE = Fish

all: $(EXE) run

$(EXE): main.o
	$(CXX) $(LDFLAGS) $< -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY : clean
clean:
	rm *.o && rm $(EXE)

.PHONY : run
run: $(EXE)
	./$(EXE)