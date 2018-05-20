#include "gurobi_c++.h"
#include<iostream>
#include<vector>
#include<list>
#include<iterator>
#include"data.h"
using namespace std;

void build_single_model() {
	my_data data;
	data.read_data();

	try {
		GRBEnv env = GRBEnv();
		GRBModel model = GRBModel(env);
		GRBVar **x = new GRBVar*[data.source_size];
		GRBVar **y = new GRBVar*[data.source_size];
		for (int i = 0; i < data.source_size; i++) {
			x[i] = new GRBVar[data.demand_size];
			y[i] = new GRBVar[data.demand_size];
		}

		for (int i = 0; i < data.source_size; i++) {
			for (int j = 0; j < data.demand_size; j++) {
				x[i][j] = model.addVar(0, INFINITY, 0, GRB_CONTINUOUS, "x_" + to_string(i) + to_string('_') + to_string(j));
				y[i][j] = model.addVar(0, 1, 0, GRB_BINARY, "y_" + to_string(i) + to_string('_') + to_string(j));
			}
		}

		GRBLinExpr e = 0;
		for (int i = 0; i < data.source_size; i++) {
			for (int j = 0; j < data.demand_size; j++) {
				e += data.fixed_c[i][j] * y[i][j] + data.c[i][j] * x[i][j];

			}
		}
		model.setObjective(e, GRB_MINIMIZE);
		//Ìí¼ÓÔ¼Êø
		//supply
		for (int i = 0; i < data.source_size; i++) {
			GRBLinExpr e = 0;
			for (int j = 0; j < data.demand_size; j++) {
				e += x[i][j];
			}
			model.addConstr(e <= data.supply[i], "supply_" + to_string(i));
		}
		//demand

		for (int i = 0; i < data.demand_size; i++) {
			GRBLinExpr e = 0;
			for (int j = 0; j < data.source_size; j++) {
				e += x[j][i];
			}
			model.addConstr(e >= data.demand[i], "demand_" + to_string(i));
		}
		for (int i = 0; i < data.source_size; i++) {
			for (int j = 0; j < data.demand_size; j++) {
				model.addConstr(x[i][j] <= y[i][j] * data.M[i][j]);
			}
		}
		model.optimize();
		cout << model.get(GRB_DoubleAttr_ObjVal) << endl;
	}
	catch (GRBException e) {
		cout << e.getMessage() << endl;
	}


}

//
//int main() {
//
//	build_single_model();
//
//
//}
