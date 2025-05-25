#include "Map.hpp"

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
			throw std::runtime_error("Too many rows in file: " + file_name + ", row number: " + std::to_string(row));
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
			map.at<uchar>(cv::Point(col, row)) = line[col];
		}
		row++;
	}
	if (row != map.rows) {
		throw std::runtime_error("Too few rows in file: " + file_name + ", row number: " + std::to_string(row));
	}
}

bool Map::outOfBounds(int x, int y) {
	if (x < 0 || x >= map.cols || y < 0 || y >= map.rows) {
		return true;
	}
	return false;
}

// Print map
void Map::printMap() {
    for (int j = 0; j < map.rows; j++) {
		for (int i = 0; i < map.cols; i++) {
			if ((i == start_position.x) && (j == start_position.y)) {
				std::cout << 'p';
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
	char wall = 'S';
	char empty = '.';
	for (int j = 0; j < map.rows; j++) {
		for (int i = 0; i < map.cols; i++) {
			switch (uniform_block(e1))
			{
			case 0:
				map.at<uchar>(cv::Point(i, j)) = wall;
				break;
			default:
				map.at<uchar>(cv::Point(i, j)) = empty;
				break;
			}
		}
	}

	//walls
	for (int i = 0; i < map.cols; i++) {
		map.at<uchar>(cv::Point(i, 0)) = wall;
		map.at<uchar>(cv::Point(i, map.rows - 1)) = wall;
	}
	for (int j = 0; j < map.rows; j++) {
		map.at<uchar>(cv::Point(0, j)) = wall;
		map.at<uchar>(cv::Point(map.cols - 1, j)) = wall;
	}

	//gen start_position inside maze (excluding walls)
	do {
		start_position.x = uniform_width(e1);
		start_position.y = uniform_height(e1);
	} while (fetchMapValue(start_position.x, start_position.y) == wall); //check wall

	//gen end different from start, inside maze (excluding outer walls) 
	do {
		end_position.x = uniform_width(e1);
		end_position.y = uniform_height(e1);
	} while (start_position == end_position); //check overlap
	map.at<uchar>(cv::Point(end_position.x, end_position.y)) = 'X';
}
