#ifndef MAZE_GEN_CPP
#define MAZE_GEN_CPP

#include "App.hpp" // Fixed include directive
#include <random>
#include <opencv2/opencv.hpp>

// maze map
// characters in maze: 
// '#' wall
// '.' empty
// 'e' end position (target, goal, gateway, etc.)
// 'X' start position
//


/* Class Map
* print map
* get map
* generate labyrinth
*/
class Map {
public:
    cv::Mat map = cv::Mat(10, 25, CV_8U); // unsigned char

    // default constructor
    Map() {
        // default constructor
    }
    // constructor with map size
    Map(int rows, int cols) {
        map = cv::Mat(rows, cols, CV_8U);
    }
    
    // print map
    void printMap();
    // get map
    uchar fetchMapValue(int x, int y);
    // generate labyrinth
    void genLabyrinth();
};

// Print map
void Map::printMap() {
    for (int j = 0; j < map.rows; j++) {
		for (int i = 0; i < map.cols; i++) {
			if ((i == start_position.x) && (j == start_position.y))
				std::cout << 'X';
			else
				std::cout << fetchMapValue(map, i, j);
		}
		std::cout << std::endl;
	}
}

// Secure access to map
uchar Map::fetchMapValue(int x, int y)
{
	x = std::clamp(x, 0, map.cols);
	y = std::clamp(y, 0, map.rows);

	//at(row,col)!!!
	return map.at<uchar>(y, x);
}

// Random map gen
void App::genLabyrinth() {
	cv::Point2i start_position, end_position;

	// C++ random numbers
	std::random_device r; // Seed with a real random value, if available
	std::default_random_engine e1(r());
	std::uniform_int_distribution<int> uniform_height(1, map.rows - 2); // uniform distribution between int..int
	std::uniform_int_distribution<int> uniform_width(1, map.cols - 2);
	std::uniform_int_distribution<int> uniform_block(0, 15); // how often are walls generated: 0=wall, anything else=empty

	//inner maze 
	for (int j = 0; j < map.rows; j++) {
		for (int i = 0; i < map.cols; i++) {
			switch (uniform_block(e1))
			{
			case 0:
				map.at<uchar>(cv::Point(i, j)) = '#';
				break;
			default:
				map.at<uchar>(cv::Point(i, j)) = '.';
				break;
			}
		}
	}

	//walls
	for (int i = 0; i < map.cols; i++) {
		map.at<uchar>(cv::Point(i, 0)) = '#';
		map.at<uchar>(cv::Point(i, map.rows - 1)) = '#';
	}
	for (int j = 0; j < map.rows; j++) {
		map.at<uchar>(cv::Point(0, j)) = '#';
		map.at<uchar>(cv::Point(map.cols - 1, j)) = '#';
	}

	//gen start_position inside maze (excluding walls)
	do {
		start_position.x = uniform_width(e1);
		start_position.y = uniform_height(e1);
	} while (fetchMapValue(map, start_position.x, start_position.y) == '#'); //check wall

	//gen end different from start, inside maze (excluding outer walls) 
	do {
		end_position.x = uniform_width(e1);
		end_position.y = uniform_height(e1);
	} while (start_position == end_position); //check overlap
	map.at<uchar>(cv::Point(end_position.x, end_position.y)) = 'e';

	std::cout << "Start: " << start_position << std::endl;
	std::cout << "End: " << end_position << std::endl;

	//print map
	

	//set player position in 3D space (transform X-Y in map to XYZ in GL)
	camera.Position.x = (start_position.x) + 1.0 / 2.0f;
	camera.Position.z = (start_position.y) + 1.0 / 2.0f;
	camera.Position.y = camera.camera_height;
}

#endif // MAZE_GEN_CPP