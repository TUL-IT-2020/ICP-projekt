# Soubor s programem:
FILE_NAME = ICP
PROJECT_FILES = callbacks.cpp app.cpp
# Imgui soubory:
IMGUI_FILES = ./imgui-master/imgui.cpp ./imgui-master/imgui_draw.cpp ./imgui-master/imgui_widgets.cpp ./imgui-master/imgui_tables.cpp ./imgui-master/backends/imgui_impl_opengl3.cpp ./imgui-master/backends/imgui_impl_glfw.cpp

# Nastaveni pro kompilator
CC = g++
CFLAGS = -pthread -std=c++17 -g -O2
INCLUDES = -I/usr/include -I/usr/include/opencv4 -I./imgui-master 
OPENCV_LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_videoio -lopencv_objdetect -lopencv_imgcodecs
GLFW_LIBS = -lglfw -lGL -lXrandr -lXi -lXinerama -lX11 -lrt -ldl
GLEW_LIBS = -lGLEW

# Set the LIBS variable to the sum of the OPENCV_LIBS and GLFW_LIBS variables
LIBS = $(OPENCV_LIBS) $(GLFW_LIBS) $(GLEW_LIBS)
TARGET = $(FILE_NAME).out
SRC = $(FILE_NAME).cpp $(IMGUI_FILES) $(PROJECT_FILES)

# Výchozí cíl
all: clean $(TARGET)

# Pravidlo pro sestavení programu
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(SRC) $(LIBS)

# Pravidlo pro vyčištění sestavení
clean:
	rm -f $(TARGET) *.o

# Pravidlo pro spuštění programu
run: $(TARGET)
	./$(TARGET)

phony: clean all run