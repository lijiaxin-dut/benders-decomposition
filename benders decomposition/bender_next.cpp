#include "gurobi_c++.h"
#include<iostream>
#include<vector>
#include"data.h"
using namespace std;

class benders_next {
public:
	my_data datas;
	GRBEnv env = GRBEnv();//enviroiment
	GRBModel master = GRBModel(env);
	GRBModel sub_problem = GRBModel(env);
	GRBLinExpr subobj=0;//子问题目标函数
	//对偶变量
	GRBVar *u;//资源约束对偶变量
	GRBVar *v;//需求约束对偶变量
	GRBVar **w;//x y 约束对偶变量
	vector<double>u_source; //子问题目标函数里对偶变量u对应系数
	vector<double>v_demand;//子问题目标函数里对偶变量v对应系数
	vector<vector<double>>w_M;//子问题中目标函数里对偶变量w对应系数
	GRBConstr **sub_con;//子问题的约束
	vector<vector<double>>x_num;//原问题的x值
	GRBVar **y;//主问题的变量
	GRBVar sub_cost;//子问题的目标函数,对应主问题的q
	vector<vector<int>>y_1;//子问题初始y的值
	double UB;
	double LB;
	benders_next(my_data datas) {
		this->datas = datas;
	}
	/*~benders_next() {
		delete u;
		delete v;
		for (int i = 0; i < datas.source_size; i++)
		{
			delete[] w[i];
			delete[] y[i];
		}
		delete[]w;
		delete[]y;
	}*/
	void create_model() {
		try {
			y_1 = vector<vector<int>>(datas.source_size, vector<int>(datas.demand_size, 1));
			u = new GRBVar[datas.source_size];
			v = new GRBVar[datas.demand_size];
			w = new GRBVar*[datas.source_size];
			for (int i = 0; i < datas.source_size; i++) {
				w[i] = new GRBVar[datas.demand_size];
			}
			u_source.resize(datas.source_size);
			v_demand.resize(datas.demand_size);
			w_M = vector<vector<double>>(datas.source_size, vector<double>(datas.demand_size,0));
			y = new GRBVar*[datas.source_size];
			for (int i = 0; i < datas.source_size; i++) {
				y[i] = new GRBVar[datas.demand_size];
			}
			sub_con = new GRBConstr*[datas.source_size];
			for (int i = 0; i < datas.source_size; i++) {
				sub_con[i] = new GRBConstr[datas.demand_size];
			}
			x_num= vector<vector<double>>(datas.source_size, vector<double>(datas.demand_size, 0));
			//添加参数
			sub_cost = master.addVar(0.0, INT_MAX, 0, GRB_CONTINUOUS, "sub cost");
			for (int i = 0; i < datas.source_size; i++) {
				u[i] = sub_problem.addVar(0, INFINITY,0, GRB_CONTINUOUS, "u_" + to_string(i));
			}
			for (int i = 0; i < datas.demand_size; i++) {
				v[i] = sub_problem.addVar(0, INFINITY, 0, GRB_CONTINUOUS, "v_" + to_string(i));
			}
			for (int i = 0; i < datas.source_size; i++) {
				for (int j = 0; j < datas.demand_size; j++) {
					y[i][j] = master.addVar(0, 1, 0, GRB_BINARY, "y_" + to_string(i) + to_string('_') + to_string(j));
					w[i][j] = sub_problem.addVar(0, INFINITY, 0, GRB_CONTINUOUS, "w_" + to_string(i) + to_string('_') + to_string(j));
				}
			}
			//主问题
			GRBLinExpr master_obj = 0;
			for (int i=0; i < datas.source_size; i++) {
				for (int j = 0; j < datas.demand_size; j++) {
					master_obj += datas.fixed_c[i][j] * y[i][j];
				}
			}
			master.setObjective((master_obj + sub_cost),GRB_MINIMIZE);
			//子问题
			//子问题目标函数
			GRBLinExpr sub_object = 0;
			for (int i = 0; i < datas.source_size; i++) {
				u_source[i] = -datas.supply[i];
				sub_object += u[i] * u_source[i];
				//subobj += u[i] * u_source[i];
			}
			for (int i = 0; i < datas.demand_size; i++) {
				v_demand[i] = datas.demand[i];
				sub_object += v[i] * v_demand[i];
				//subobj += v[i] * v_demand[i];
			}
			for (int i = 0; i < datas.source_size; i++) {
				for (int j = 0; j < datas.demand_size; j++) {
					w_M[i][j] = -datas.M[i][j];
					sub_object+= w_M[i][j] * y_1[i][j] * w[i][j];
					//subobj += w_M[i][j] * y_1[i][j] * w[i][j];
				}
			}
			sub_problem.setObjective(sub_object, GRB_MAXIMIZE);
			//子问题约束
			for (int i = 0; i < datas.source_size; i++) {
				for (int j = 0; j < datas.demand_size; j++) {
					GRBLinExpr e = 0;
					e = -u[i] + v[j] - w[i][j];
					sub_con[i][j]= sub_problem.addConstr(e <= datas.c[i][j], "C_" + to_string(i) + "_" + to_string(j));
				}
			}
			sub_problem.set(GRB_IntParam_Presolve, GRB_PRESOLVE_OFF);
			sub_problem.getEnv().set(GRB_IntParam_OutputFlag, 0);
			master.getEnv().set(GRB_IntParam_OutputFlag, 0);

		}
		catch (GRBException e) {
			cout << e.getMessage() << endl;
		}
		catch (...) {
			cout << "wrong" << endl;
		}
	
	}
	void benders_solve() {
		UB = INT_MAX;
		LB = INT_MIN;
		try {
			while (UB>LB+FUZZ) {
				//根据松弛的主问题中的 变量y值重置子问题目标函数
				GRBLinExpr subobj = 0;
				for (int i = 0; i < datas.source_size; i++) {
					subobj += u_source[i] * u[i];
				}
				for (int i = 0; i < datas.demand_size; i++) {
					subobj += v_demand[i] * v[i];
				}
				for (int i = 0; i < datas.source_size; i++) {
					for (int j = 0; j < datas.demand_size; j++) {
						subobj += w_M[i][j] * w[i][j] * y_1[i][j];
					}
				}
				sub_problem.setObjective(subobj, GRB_MAXIMIZE);
				//sub_problem.set(GRB_IntParam_InfUnbdInfo, 1);
				sub_problem.optimize();
				for (int i = 0; i < datas.source_size; i++) {
					for (int j = 0; j < datas.demand_size; j++) {
						x_num[i][j] = sub_con[i][j].get(GRB_DoubleAttr_Pi);//得到对偶变量pi
					}
				}
				auto status = sub_problem.get(GRB_IntAttr_Status);//得到子问题的求解状态
				
				//添加极射线的约束
				if (status == GRB_UNBOUNDED) {

					GRBLinExpr e = 0;
					for (int i = 0; i < datas.source_size; i++) {
						e += u[i].get(GRB_DoubleAttr_UnbdRay)*u_source[i];
					}
					for (int i = 0; i < datas.demand_size; i++) {
						e += v[i].get(GRB_DoubleAttr_UnbdRay)*v_demand[i];
					}
					for (int i = 0; i < datas.source_size; i++) {
						for (int j = 0; j < datas.demand_size; j++) {
							e += w[i][j].get(GRB_DoubleAttr_UnbdRay)*w_M[i][j] * y[i][j];
						}
					}
					master.addConstr(e <= 0);
				}
				//添加极点的约束
				else if (status == GRB_OPTIMAL) {
					GRBLinExpr e = 0;
					for (int i = 0; i < datas.source_size; i++) {
						e += u[i].get(GRB_DoubleAttr_X)*u_source[i];
					}
					for (int i = 0; i < datas.demand_size; i++) {
						e += v[i].get(GRB_DoubleAttr_X)*v_demand[i];
					}

					for (int i = 0; i < datas.source_size; i++) {
						for (int j = 0; j < datas.demand_size; j++) {
							e += y[i][j] * w_M[i][j] * w[i][j].get(GRB_DoubleAttr_X);
						}
					}
					master.addConstr(e <= sub_cost);
					double sum_cij_fij = 0;
					for (int i = 0; i < datas.source_size; i++) {
						for (int j = 0; j < datas.demand_size; j++) {
							sum_cij_fij += datas.fixed_c[i][j] * y_1[i][j];
						}
					}
					cout << "UB " << UB << endl;
					UB = min(UB, (sum_cij_fij + sub_problem.get(GRB_DoubleAttr_ObjVal)));
				}
				else
				{
					//error
				}
				//求解主问题
				master.optimize();
				LB = master.get(GRB_DoubleAttr_ObjVal);
				cout << "LB:" << LB << endl;
				for (int i = 0; i < datas.source_size; i++) {
					for (int j = 0; j < datas.demand_size; j++) {
						double aa = y[i][j].get(GRB_DoubleAttr_X);
						if (aa > 0.5) {
							y_1[i][j] = 1;
						}
						else {
							y_1[i][j] = 0;
						}
					}
				}

			}
		}
		
		catch (GRBException E) {
			cout << E.getMessage() << endl;
		}
		catch (...) {
			cout << "error" << endl;
		}
		cout << UB << endl;
		cout << LB << endl;
	}
};

int main() {

	my_data datas;
	datas.read_data();
	benders_next bn(datas);
	bn.create_model();
	bn.benders_solve();
}

