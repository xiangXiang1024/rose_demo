#include "Variable.h"

using namespace std;

Variable::Variable(const string _var_name, const string _var_type):
    var_name(_var_name), var_type(_var_type) {
    ////cout << to_string();
    var_statement = _var_name;
}

Variable::Variable(const string _var_name, const string _var_type, string statement):
    var_name(_var_name), var_type(_var_type), var_statement(statement) {
    ////cout << to_string();
    //var_statement = _var_name;
}

string Variable::to_string() {
    return var_name+"("+var_type+")";
}

