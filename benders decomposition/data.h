#pragma once
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<algorithm>
const static double FUZZ = 1.0e-7;
class my_data {
public:
	int source_size;
	int demand_size;
	std::vector<double>supply;
	std::vector<double>demand;
	std::vector<std::vector<double>>c;
	std::vector<std::vector<double>>fixed_c;
	std::vector<std::vector<double>>M;
	my_data() {
		//read_data();
	
	}
	void read_data() {
		std::ifstream f("test1.txt");
		std::string one_line;
		std::getline(f, one_line);
		std::stringstream ss(one_line);
		ss >> source_size >> demand_size;
		ss.clear();
		c.resize(source_size);
		for(int i=0;i<source_size;i++)
			c[i].resize(demand_size);
		fixed_c.resize(source_size);
		for (int i = 0; i<source_size; i++)
			fixed_c[i].resize(demand_size);
		M.resize(source_size);
		for (int i = 0; i<source_size; i++)
			M[i].resize(demand_size);
		std::getline(f, one_line);
		ss= std::stringstream(one_line);
		for (int i = 0; i < source_size; i++) {
			ss >> one_line;
			double one_number = std::stof(one_line);
			supply.push_back(one_number);
		}
		ss.clear();
		std::getline(f, one_line);
		ss = std::stringstream(one_line);
		for (int i = 0; i < demand_size; i++) {
			ss >> one_line;
			double one_number = std::stof(one_line);
			demand.push_back(one_number);
		}
		for (int i = 0; i < source_size; i++) {
			std::getline(f, one_line);
			ss.clear();
			ss = std::stringstream(one_line);
			for (int j = 0; j < demand_size; j++) {
				ss >> one_line;
				double one_number = std::stof(one_line);
				c[i][j] = one_number;
			}
		}
		for (int i = 0; i < source_size; i++) {
			std::getline(f, one_line);
			ss.clear();
			ss = std::stringstream(one_line);
			for (int j = 0; j < demand_size; j++) {
				ss >> one_line;
				double one_number = std::stof(one_line);
				fixed_c[i][j] = one_number;
			}
		}
		for (int i = 0; i < source_size; i++) {
			for (int j = 0; j < demand_size; j++) {
				M[i][j] = std::min(supply[i], demand[j]);
			}
		}
	
	
	}
};
