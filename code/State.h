#ifndef _VARIABLE_H_
#define _VARIABLE_H_
#include "Variable.h"
#endif

#include "rose.h"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

class State{
    public:
        string file_path;
        int start_pos;
        int end_pos;
        vector<Variable> param_list;
        Variable output;
        vector<SgStatement*> statement_list;
        
	    string condition;
	    int line_ptr;
	    int next_line_ptr;
	    vector<string> line_infos;
	    int statement_ptr;
	    int next_statement_ptr;
	    bool is_end;
	    
	    bool in_if;
	    vector<int> end_if_line;    // line of }
	    vector<int> exit_if_line;
        
        State(const string path, int start, int end, vector<Variable> params, Variable result, SgStatementPtrList& statement_ptrs);
        
        State(const string path, int start, int end, vector<Variable> params, Variable result);
        
        State();
        
        //State(const State& s);
        
        string to_string() {
            ostringstream ostr;
            ostr << "*****State*****" << endl;
            ostr << "file: " << file_path << endl;
            ostr << "start: " << start_pos << "    end: " << end_pos << endl;
            ostr << "params: " << endl;
            for(Variable v : param_list) {
                ostr << v.output_info() << endl;
            }
            ostr << "output: " << output.output_info() << endl;
            
            ostr << endl;
            ostr << "condition: " << condition << endl;
            ostr << "line ptr: " << line_ptr << "    next line ptr: " << next_line_ptr << endl;
            ostr << "statement ptr: " << statement_ptr << "    next statement ptr: " << next_statement_ptr << endl;
            if(in_if) {
                ostr << "end_if_line: ";
                for(int i : end_if_line) {
                    ostr << i << "    ";
                }
                ostr << endl;
                
                ostr << "exit_if_line: ";
                for(int i : exit_if_line) {
                    ostr << i << "    ";
                }
                ostr << endl;
            }
            ostr << "line_infos: "<< endl;
            for(int i = 0 ; i < line_infos.size() ; i++) {
                ostr << i << "  " << line_infos[i] << endl;
            }
            ostr << "statement: " << endl;
            for(int i = 0 ; i < statement_list.size() ; i++) {
                SgStatement* statement = statement_list[i];
		        ostr << i << "  " << statement->class_name() << endl;
            }
            
            ostr << "***************" << endl;
            
            return ostr.str();
        }
        
        void add_condition(string c) {
            if(condition.empty()) {
                condition = c;
            }else {
                condition = "(" + condition + ") && (" + c + ")";
            }
        }
        
        void add_not_condition(string c) {
            add_condition("!("+c+")");
        }
        
        string next();
        
        //bool is_end();
        
        void set_end_if_line(bool has_false);
        
        void false_path_set_ptr();
        
    
    private:
        void get_next_ptr(string statement_type);
        
        void solve_declaration(string line);
        
        void solve_expr(string line);
        
        void solve_return();
        
        void parse_expr(string expr);
        
        void solve_if(string line, SgStatement* statement);
};
