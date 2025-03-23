#ifndef MAZE_GEN_CPP
#define MAZE_GEN_CPP

#include "App.hpp" // Fixed include directive
#include <random>
#include <opencv2/opencv.hpp>

/* maze map
characters in maze: 
' ' = empty
. = empty

// walls:
S = stone
W = wood
B = blue
X = end lever

// enemies:
p = player start
e = normal enemy
k = killed enemy
B = boss enemy

// items:
o = object/obsctacle
g = gold
h = health
m = munition

d = door
*/

/* Class Map
* print map
* get map
* generate labyrinth
*/
class Map {
private:
	cv::Mat map = cv::Mat(10, 25, CV_8U); // unsigned char
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
};



Map::Map(const std::string& file_name) {
	std::ifstream file(file_name);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open file: " + file_name);
	}

	// Read first line to get map size: rows, cols
	std::string first_line;
	std::getline(file, first_line);
	std::istringstream iss(first_line);
	int rows, cols;	
	iss >> rows >> cols;
	map = cv::Mat(rows, cols, CV_8U);

	std::cout << "Map size: " << rows << "x" << cols << std::endl;
	
	std::string line;
	int row = 0;
	while (std::getline(file, line)) {
		// trim line
		line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
		line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
		// skip empty lines
		if (line.empty()) {
			continue;
		}
		// check map size
		if (row >= map.rows) {
			throw std::runtime_error("Too many rows in file: " + file_name);
		}
		if (line.size() != map.cols) {
			throw std::runtime_error("Row " + std::to_string(row) + " has wrong number of columns in file: " + file_name);
		}
		// fill map
		for (int col = 0; col < map.cols; col++) {
			if (line[col] == 'p') {
				start_position = cv::Point(col, row);
				line[col] = '.';
			}
			else if (line[col] == 'X') {
				end_position = cv::Point(col, row);
			}
			// S, W, B, d - pridej do mapy
			if (line[col] == 'S' || line[col] == 'W' || line[col] == 'B' || line[col] == 'd') {
				
			} else {
				line[col] = '.';
			}
			map.at<uchar>(cv::Point(col, row)) = line[col];
		}
		row++;
	}
	if (row != map.rows) {
		throw std::runtime_error("Too few rows in file: " + file_name);
	}
}

// Print map
void Map::printMap() {
    for (int j = 0; j < map.rows; j++) {
		for (int i = 0; i < map.cols; i++) {
			if ((i == start_position.x) && (j == start_position.y)) {
				std::cout << 'X';
			}
			else {
				std::cout << fetchMapValue(i, j);
			}
		}
		std::cout << std::endl;
	}
	
	std::cout << "Start: " << start_position << std::endl;
	std::cout << "End: " << end_position << std::endl;
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
void Map::genenerateLabyrinth(int rows, int cols) {
	map = cv::Mat(rows, cols, CV_8U);

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
	} while (fetchMapValue(start_position.x, start_position.y) == '#'); //check wall

	//gen end different from start, inside maze (excluding outer walls) 
	do {
		end_position.x = uniform_width(e1);
		end_position.y = uniform_height(e1);
	} while (start_position == end_position); //check overlap
	map.at<uchar>(cv::Point(end_position.x, end_position.y)) = 'e';
}

#endif // MAZE_GEN_CPP