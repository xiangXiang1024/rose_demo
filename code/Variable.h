#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Variable {
    public:
	    string var_name;
        string var_type;
        string var_statement;

        Variable(const string _var_name, const string _var_type);
        
        Variable(const string _var_name, const string _var_type, string statement);

        string to_string();

        void set_statement(string s) {
            var_statement = s;
        }

        void renew_statement(Variable v) {
            //cout << "renew " << var_name << "    " << v.var_name << endl;
            
            string v_name = v.var_name;
            string v_statement = "(" + v.var_statement + ")";
            
            string::size_type pos = 0;
            string::size_type pre_pos = -1;
            bool is_first_time = true;
            while((pos = var_statement.find(v_name)) != string::npos) {
                
                //cout << "find " << v_name << " pos: " << pos << endl;
                if(v.var_name != v.var_statement) {
                    var_statement.replace(pos, v_name.length(), v_statement);
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

        void renew_statement(vector<Variable> v_list) {
            for(Variable v : v_list) {
                if(v.var_statement.empty()) {
                    continue;
                }
                //if(v.var_name == var_name) {
                    //var_statement = v.var_statement;
                //    continue;
                //}
                renew_statement(v);
            }
        }

        string output_info() {
            return var_name+"("+var_type+"):"+var_statement;
        }
};
