#include <iostream>
#include <sstream>
#include <fstream>

#include "rose.h"
#include "SgNodeHelper.h"
#include "State.h"
#ifndef _VARIABLE_H_
#define _VARIABLE_H_
#include "Variable.h"
#endif

//./hello /opt/analysis/code.cpp
using namespace std;

bool is_define_func(string path, string func) {
    ifstream in;
    string line;
    string s = func + "(";
	in.open(path);
	
	while(getline(in,line)) {
        if(line.find(s) != line.npos) {
            return true;
        }
    }
	
	in.close();
    
    return false;
}

vector<string> split_vector(string s, char split) {
	vector<string> v;
    istringstream iss(s);
	string token;
    while(getline(iss, token, split)) {
        v.push_back(token);
    }
	return v;
}

string split(string s, char split) {
	vector<string> v = split_vector(s, split);
	return v.back();
}

string get_output_name(string file_name, int end_pos) {
	ifstream in;
	in.open(file_name);
	
	string line_info;
	int line = 0;
	while (in.eof() == false && line < end_pos) { 
        getline(in, line_info);
		line++;
    }
	in.close();

	string return_name = split(line_info, ' ');
	return_name = return_name.substr(0, return_name.length() - 1);
	return return_name;
}

string get_type_name(string s) {
	if("f" == s) {
		return "float";
	}else if("d" == s) {
		return "double";
	}else if("i" == s) {
		return "int";
	}else if("c" == s) {
		return "char";
	}
	return s;
}

bool is_node_in_src(SgLocatedNode *node) {
    if (node == nullptr) {
	return false;
    }
    return true;
}

int get_start(string s) {
	int split_pos = (int)s.find_first_of("-");
	string start_str = s.substr(0, split_pos);
	return atoi(start_str.c_str());;
}

string renew_condition(string condition, vector<Variable> param_list) {
    for(Variable v : param_list) {
        if(v.var_statement.empty()) {
            continue;
        }
        
        string v_name = v.var_name;
        string v_statement = "(" + v.var_statement + ")";
        //cout << "renew:  condition  " << v_name << "  " << v_statement << endl;
        
        string::size_type pos = 0;
        string::size_type pre_pos = -1;
        bool is_first_time = true;
        while((pos = condition.find(v_name)) != string::npos) {
                //cout << "find " << v_name << " pos: " << pos << endl;
                if(v.var_name != v.var_statement) {
                    condition.replace(pos, v_name.length(), v_statement);
                }
                //cout << var_statement << endl;
                //cout << "pos: " << pos << "    pre_pos: " << pre_pos << endl;
                if(pos < pre_pos+v_statement.length() && !is_first_time) {
                    break;
                }else {
                    pre_pos = pos;
                    is_first_time = false;
                }
            }
    }
    return condition;
}

State copy_state(State s) {
    State state(s.file_path, s.start_pos, s.end_pos, s.param_list, s.output);
	        
    state.file_path = s.file_path;
    state.start_pos = s.start_pos;
    state.end_pos = s.end_pos;
    state.param_list = s.param_list;
    state.output = s.output;
    state.statement_list = s.statement_list;
    state.condition = s.condition;
    state.line_ptr = s.line_ptr;
    state.next_line_ptr = s.next_line_ptr;
	state.line_infos = s.line_infos;
    state.statement_ptr = s.statement_ptr;
    state.next_statement_ptr = s.next_statement_ptr;
    state.is_end = s.is_end;
    state.in_if = s.in_if;
    state.end_if_line = s.end_if_line;
    state.exit_if_line = s.exit_if_line;
    state.careless_lines = s.careless_lines;
            
    return state;
}

vector<SgStatement*> insert_statements(vector<SgStatement*> statement_list, vector<SgNode*> nodes, int statement_ptr) {
    vector<SgStatement*> statements;
    
    for(int i = 0 ; i < statement_list.size() ; i++) {
        statements.push_back(statement_list[i]);
        if(i == statement_ptr) {
            for(SgNode* n : nodes) {
                SgStatement* statement = dynamic_cast<SgStatement*>(n);
                statements.push_back(statement);
            }
        }
    }
                
    return statements;
}

string get_careless_info(vector<int> careless_lines) {
    stringstream info;
    
    int start = -1;
    int end = -1;
    int pre = -1;
    
    for(int i = 0 ; i < careless_lines.size() ; i++) {
        /*cout << endl;
        cout << "i: " << i << endl;
        cout << "careless_lines[i]: " << careless_lines[i] << endl;
        cout << "start: " << start << endl;
        cout << "end: " << end << endl;
        cout << "pre: " << pre << endl;*/
        
        if(start < 0) {
            start = careless_lines[i];
        }else {
            if(careless_lines[i] == pre+1) {
                
            }else {
                end = pre;
                if(start == end) {
                    //cout << "start: " << start << endl;
                    info << start << ";";
                }else {
                    //cout << "start: " << start << "  end: " << endl << endl;
                    info << start << "-" << end << ";";
                }
                start = careless_lines[i];
            }
        }
        pre = careless_lines[i];
    }
    
    if(start == careless_lines.back()) {
        info << start;
    }else {
        info << start << "-" << careless_lines.back();
    }
    
    return info.str();
}

string symbolic_execution(string file_path, int start_pos, int end_pos, vector<Variable> param_list, Variable output, SgBasicBlock *func_body) {
    State state(file_path, start_pos, end_pos, param_list, output, func_body->get_statements());
    //cout << state.to_string();
    
    vector<State> state_list;
    state_list.push_back(state);
    
    vector<string> condition_list;
    vector<string> statement_list;
    vector<int> careless_lines;
    int i = 0;
    while(i < state_list.size()) {
        State s = state_list[i];
        while(!s.is_end) {
            //cout << "state " << i << endl;
            string r = s.next();
            if("if" == r) {
                //cout << "-----" << endl << "if statement" << endl;
                //s = solve_if(s);
                
                // get condition
                string if_line = s.line_infos[s.line_ptr];
                //cout << "if line:  " << if_line << endl;
                string condition = if_line.substr(2, if_line.length()-1);
                string::size_type pos = 0;
                if((pos = condition.find('{')) != string::npos) {
                    condition = condition.erase(pos, 1);
                }
                condition = renew_condition(condition, s.param_list);
                //cout << "condition:  " << condition << endl;
                
                State s2 = copy_state(s);
                
                
                // s:true
                s.add_condition(condition);
                // add statement
                SgStatement* statement = s.statement_list[s.statement_ptr];
                SgIfStmt* if_statement = dynamic_cast<SgIfStmt*>(statement);
                SgStatement* conditional_statement = if_statement->get_conditional();
                SgScopeStatement* scope = conditional_statement->get_scope();
                vector<SgNode*> node_list = scope->get_traversalSuccessorContainer();
                bool has_false = true;
                if(!node_list[2]) {
                    has_false = false;
                }
                //cout << "has false path: " << has_false << endl;
                
                SgNode* true_path = node_list[1];
                vector<SgNode*> nodes = true_path->get_traversalSuccessorContainer();
                s.statement_list = insert_statements(s.statement_list, nodes, s.statement_ptr);
                // set ptr
                s.in_if = true;
                s.next_line_ptr++;
                s.next_statement_ptr++;
                s.set_end_if_line(has_false);
                
                // s2:false
                s2.add_not_condition(condition);
                // add statement
                if(has_false) {
                    SgNode* false_path = node_list[2];
                    vector<SgNode*> false_nodes = false_path->get_traversalSuccessorContainer();
                    s2.statement_list = insert_statements(s2.statement_list, false_nodes, s2.statement_ptr);
                }
                // todo set ptr
                if(!has_false) {
                    s2.next_line_ptr = s.exit_if_line.back();
                    s2.next_statement_ptr++;
                }else {
                    s2.in_if = true;
                    s2.exit_if_line.push_back(s.exit_if_line.back());
                    s2.end_if_line.push_back(s.exit_if_line.back()-1);
                    s2.false_path_set_ptr();
                    s2.next_statement_ptr++;
                }
                
                state_list.push_back(s2);
            }
        }
        
        condition_list.push_back(s.condition);
        statement_list.push_back(s.output.var_statement);
        
        for(int careless : s.careless_lines) {
            if(!count(careless_lines.begin(), careless_lines.end(), careless)) {
                //cout << "not find careless:" << careless << endl;
                careless_lines.push_back(careless);
                sort(careless_lines.begin(), careless_lines.end());
            }
        }
        
        //cout << s.to_string() << endl;
        
        i++;
    }
    
    
    //cout << "final result: " << endl;
    stringstream final_result;
    for(int i = 0 ; i < statement_list.size() ; i++) {
        string condition = condition_list[i];
        if(condition.empty()) {
            condition = "true";
        }
        string statement = statement_list[i];
        final_result << "if(" << condition << ")" << statement << ";";
    }
    
    /*cout << "careless:" << endl;
    for(int c : careless_lines) {
        cout << c << "  ";
    }
    cout << endl;*/
    
    final_result << endl << "careless:" << endl << get_careless_info(careless_lines) << endl;
	
	return final_result.str();
}

int get_end_pos(string file_path, int start_pos) {
    ifstream in;
	in.open(file_path);
	
	int i = 0;
	string line;
	while(i < start_pos-1) {
	    getline(in, line);
	    i++;
	}
	
	while(in.eof() == false) {
	    getline(in, line);
	    i++;
	    if(line.find("return ")  != string::npos) {
	        //cout << "line: " << i << "    " << line;
	        break;
	    }
	}
	
	in.close();
    return i;
}

int do_something(int argc, char *argv[]) {
	stringstream ir_output;
	// Generate the ROSE AST.
	SgProject* project = frontend(argc,argv);
	// get functions
	std::vector<SgNode*> declare_functions = NodeQuery::querySubTree(project,V_SgFunctionDeclaration);
	std::vector<SgNode*> functions = NodeQuery::querySubTree(project,V_SgFunctionDefinition);
	//cout << "get functions" << endl;
	
	vector<SgNode *> files = project->get_fileList_ptr()->get_traversalSuccessorContainer();
	string file_name;
	for (auto f : files) { 
		auto file = dynamic_cast<SgSourceFile *>(f);
		ir_output << file->getFileName() << endl;
		file_name = file->getFileName();
	}
	
	//cout << "declare func num: " << declare_functions.size() << endl;
	/*for(SgNode* f : declare_functions) {
	    auto* func_declare = dynamic_cast<SgFunctionDeclaration *>(f);
	    string func_declare_name = func_declare->get_name();
	    cout << "declare function: " << func_declare_name << endl;
	}*/
	
	//cout << "define func num: " << functions.size() << endl;

	for (int a = 0 ; a < functions.size() ; a++) {
	    SgNode* f = functions[a];
		//ir_output << endl;
		//cout << endl;
		//auto* func_declare = dynamic_cast<SgFunctionDeclaration *>(f);
		
		auto* func = dynamic_cast<SgFunctionDefinition *>(f);
		string func_name = SgNodeHelper::getFunctionName(func);
	    //cout << "function: " << func_name << endl;
		
		// todo judge is a function defined in cpp file
		/*if(func_name != "func1") {
		    continue;
		}else {
		    ir_output << endl;
		    cout << "a: " << a << endl;
		    cout << "function: " << func_name << endl;
		}*/
		
		if(is_define_func(file_name, func_name)) {
		    ir_output << endl;
		    //cout << "a: " << a << endl;
		    //cout << "function: " << func_name << endl;
		}else {
		    continue;
		}
		
		//cout << "line: " << SgNodeHelper::sourceLineColumnToString(func, "-") << endl;
		string line_str = SgNodeHelper::sourceLineColumnToString(func, "-");
		int start_pos = get_start(line_str)+1;
		
		SgBasicBlock *func_body = func->get_body();
		//cout << "get func body" << endl;

		const SgStatementPtrList& statement_list = func_body->get_statements();

		int end_pos = get_end_pos(file_name, start_pos);
		ir_output << "line:" << endl << start_pos << "-" << end_pos << endl;
		
		vector<SgInitializedName*> vectors = SgNodeHelper::getFunctionDefinitionFormalParameterList(func);
		//cout << "getFunctionDefinitionFormalParameterList" << endl;
		//cout << "parameters:" << endl;
		ir_output << "inputs:" << endl;
		vector<Variable> input_list;
		for(SgInitializedName* s : vectors) {
			//cout << s->get_name().getString() << "(";
			SgType *type = s->get_type();
			//cout << get_type_name(type->get_mangled().getString()) << ")	";
			
			Variable input(s->get_name().getString(), get_type_name(type->get_mangled().getString()));
			ir_output << input.to_string() << ";";
			input_list.push_back(input);
		}
		//cout << endl;
		ir_output << endl;
		
		string output_name = get_output_name(file_name, end_pos);
		SgType* result = SgNodeHelper::getFunctionReturnType(func);
		ir_output << "outputs:" << endl;
		Variable output(output_name, get_type_name(result->get_mangled().getString()));
		string output_statement = symbolic_execution(file_name, start_pos, end_pos, input_list, output, func_body);// 符号执行
		//cout << "output_statement: " << output_statement << endl;
		ir_output << output.to_string() << ":" << output_statement << endl;
		if(SgNodeHelper::isFloatingPointType(result)) {
			//cout << "return:  " << get_type_name(result->get_mangled().getString()) << endl;
		}else {
			//cout << "don't care" << endl;
		}		
	}

	string source_name = split(file_name, '/');
	string ir_name;
	ir_name.assign(source_name.begin(), source_name.end()-3);
	string ir_path = "../result/" + ir_name + "ir";
	//cout << "ir path: " << ir_path << endl;
	ofstream ir_stream;
	ir_stream.open(ir_path,ios::trunc);
	ir_stream << ir_output.str();
	ir_stream.close();

     return 0;
}

int main(int argc, char *argv[]) {
    //cout << "argc: " << argc << endl;
    //cout << "argv: " << endl;
    for(int i = 0 ; i < argc ; i++) {
        //cout << argv[i] << endl;
    }
    
	return do_something(argc, argv);
}
