#ifndef MAZE_GEN_CPP
#define MAZE_GEN_CPP

#include <random>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>

/* Class Map
* print map
* get map
* generate labyrinth
*/
class Map {
    private:
        cv::Mat map = cv::Mat(10, 25, CV_8U); // unsigned char
        // list of characters of walls:
        static constexpr char wall_chars[9] = {'B', 'D', 'E', 'S', 'T', 'U', 'W', 'X', 'Y',}; // walls
    public:
        cv::Point2i start_position, end_position;

        // default constructor
        Map() {}
        // constructor with map size
        Map(int rows, int cols) {
            genenerateLabyrinth(rows, cols);
        }
        // from text file
        Map(const std::string& file_name);
        
        // print map
        void printMap();
        // get map
        uchar fetchMapValue(int x, int y);
        // generate labyrinth
        void genenerateLabyrinth(int rows, int cols);
    
        int getCols() { return map.cols; }
        int getRows() { return map.rows; }

        bool outOfBounds(int x, int y);
        bool containsSolid(int x, int y);
        bool containsWall(int x, int y);
    };

#endif // MAZE_GEN_CPP