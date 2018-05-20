#pragma once
#include<vector>
#include<fstream>
#include<string>
#include<sstream>
#include<algorithm>
#include<random>
#include<chrono>


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
	void random_data(int source_size,int demand_size) {
		std::default_random_engine e(std::chrono::system_clock::now().time_since_epoch().count());//当前时间做种子
		std::uniform_int_distribution<>d(1, 20);
		this->source_size = source_size;
		this->demand_size = demand_size;
		c.resize(source_size);
		for (int i = 0; i<source_size; i++)
			c[i].resize(demand_size);
		fixed_c.resize(source_size);
		for (int i = 0; i<source_size; i++)
			fixed_c[i].resize(demand_size);
		M.resize(source_size);
		for (int i = 0; i<source_size; i++)
			M[i].resize(demand_size);

		for (int i = 0; i < source_size; i++) {
			supply.push_back(d(e));
		}

		for (int i = 0; i < demand_size; i++) {
			demand.push_back(d(e));
		}
		for (int i = 0; i < source_size; i++) {
			for (int j = 0; j < demand_size; j++) {
				c[i][j] = d(e);
			}
		}
		for (int i = 0; i < source_size; i++) {
			for (int j = 0; j < demand_size; j++) {
				fixed_c[i][j] = d(e);
			}
		}
		for (int i = 0; i < source_size; i++) {
			for (int j = 0; j < demand_size; j++) {
				M[i][j] = std::min(supply[i], demand[j]);
			}
		}
		
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
