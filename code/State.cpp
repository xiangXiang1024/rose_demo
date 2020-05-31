#include "State.h"

using namespace std;

string simplify(string s) {
    while(s.find(' ') == 0 || s.find('\t') == 0) {
        s = s.erase(0, 1);
    }
    string::size_type pos = 0;
    while((pos = s.find(';')) != string::npos) {
        s = s.erase(pos, 1);
    }
    return s;
}

State::State(const string path, int start, int end, vector<Variable> params, Variable result, SgStatementPtrList& statement_ptrs):
    file_path(path), start_pos(start), end_pos(end), param_list(params), output(result) {
    condition = "";
    line_ptr = -1;
    next_line_ptr = 0;
    statement_ptr = -1;
    next_statement_ptr = 0;
    is_end = false;
    
    in_if = false;
    
    // statement_list
    vector<SgStatement*> statements;
    for(auto s : statement_ptrs) {
        SgStatement* statement = dynamic_cast<SgStatement*>(s);
        statements.push_back(statement);
    }
    statement_list = statements;
    
    // line_infos
    ifstream in;
	in.open(file_path);
	
	int i = 0;
	string line;
	while(i < start-1) {
	    getline(in, line);
	    i++;
	}
	
	vector<string> lines;
	while(i < end) {
	    getline(in, line);
	    lines.push_back(simplify(line));
	    i++;
	}
	
	in.close();
	line_infos = lines;
}

State::State(const string path, int start, int end, vector<Variable> params, Variable result):
    file_path(path), start_pos(start), end_pos(end), param_list(params), output(result) {
    condition = "";
    line_ptr = -1;
    next_line_ptr = 0;
    statement_ptr = -1;
    next_statement_ptr = 0;
    is_end = false;
}

/*
State::State(const State& s) {
    file_path = s.file_path;
    start_pos = s.start_pos;
    end_pos = s.end_pos;
    param_list = s.param_list;
    output = s.output;
    statement_list = s.statement_list;
    condition = s.condition;
    line_ptr = s.line_ptr;
	line_infos = s.line_infos;
    statement_ptr = s.statement_ptr;
}*/

void State::get_next_ptr(string statement_type) {
    if("SgReturnStmt" == statement_type) {
        is_end = true;
        return;
    }
    
    // todo if
    if(in_if) {
        next_statement_ptr++;
        // todo next_line_ptr
        if(next_line_ptr == end_if_line.back()) {
            next_line_ptr = exit_if_line.back();
            end_if_line.pop_back();
            exit_if_line.pop_back();
            if(end_if_line.size() == 0 && exit_if_line.size() == 0) {
                in_if = false;
            }
        }else {
            next_line_ptr++;
        }
        
        cout << to_string();
        
        return;
    }
    
    next_line_ptr++;
    next_statement_ptr++;
}

string State::next() {
    cout << "state next" << endl;
    
    line_ptr = next_line_ptr;
    statement_ptr = next_statement_ptr;
    
    // do something
    string line = line_infos[line_ptr];
    SgStatement* statement = statement_list[statement_ptr];
    string statement_type = statement -> class_name();
    if("SgReturnStmt" == statement_type) {// return
        solve_return();
    }else if("SgVariableDeclaration" == statement_type) {// declare
        solve_declaration(line);
    }else if("SgExprStatement" == statement_type) {// expr
        solve_expr(line);
    }else if("SgIfStmt" == statement_type) {// if
        //solve_if(line, statement);
        return "if";
    }
    
    get_next_ptr(statement_type);
    
    cout << "has get next ptr" << endl;
    
    //cout << to_string();
    return "";
}

vector<string> get_split_vector(string s, char split) {
	vector<string> v;
    istringstream iss(s);
	string token;
    while(getline(iss, token, split)) {
        v.push_back(token);
    }
	return v;
}

string remove_blank(string s) {
    string::size_type pos = 0;
    while((pos = s.find(';')) != string::npos) {
        s = s.erase(pos, 1);
    }
    while((pos = s.find(' ')) != string::npos) {
        s = s.erase(pos, 1);
    }
    while((pos = s.find('\t')) != string::npos) {
        s = s.erase(pos, 1);
    }
    return s;
}

Variable parse_declaration(string declaration) {
    int equal_pos = declaration.find("=");
    string declare = declaration.substr(0, equal_pos);
    //cout << "declare: " << declare << endl;

    vector<string> declare_vector = get_split_vector(declare, ' ');
    string type = declare_vector.front();
    string name = declare_vector.back();
    Variable v(name, type);
    string statement = declaration.substr(equal_pos+1);
    v.set_statement(remove_blank(statement));
    //cout << "parse_declaration: " << v.output_info() << endl;
    return v;
}

void State::solve_declaration(string line) {
    cout << "solve_declaration" << endl;
    
    Variable v = parse_declaration(line);
    //cout << "get variable: " << v.output_info() << endl;
    v.renew_statement(param_list);
    //cout << "after renew: " << v.output_info() << endl;
    param_list.push_back(v);
}

void State::parse_expr(string expr) {
    cout << "parse_expr" << endl;
    expr = remove_blank(expr);
    //cout << "line: " << expr << endl;
    
    int equal_pos = expr.find("=");
    string var_name = expr.substr(0, equal_pos);
    //cout << "var: " << var_name << endl;
    string statement = expr.substr(equal_pos+1);
    //cout << "statement: " << statement << endl;
    
    int i = 0;
    for(i = 0 ; i < param_list.size() ; i++) {
        if(param_list[i].var_name == var_name) {
           // cout << "find " << var_name << "   i = " << i << endl;
            break;
        }
    }
    if(i >= param_list.size()) {
        return;
    }
    
    string var_type = param_list[i].var_type;
    Variable v(var_name, var_type, statement);
    //cout << "new: " << var_name << "    " << v.output_info() << endl;
    
    v.renew_statement(param_list);
    v.var_statement = remove_blank(v.var_statement);
    
    //cout << "after renew: " << var_name << "    " << v.output_info() << endl;
    
    param_list[i] = v;
}

void State::solve_expr(string line) {
    cout << "solve_expr" << endl;
    
    parse_expr(line);
    
    //cout << "after expr:" << endl;
    //cout << to_string();
}

void State::solve_return() {
    cout << "solve_return" << endl;
    
    for(Variable v : param_list) {
        if(v.var_name == output.var_name) {
            output = v;
            //cout << "return: " << output.output_info() << endl;
            break;
        }
    }
}

void State::solve_if(string line, SgStatement* statement) {
    cout << "solve_if" << endl;
    SgIfStmt* if_statement = dynamic_cast<SgIfStmt*>(statement);
    SgStatement* conditional_statement = if_statement->get_conditional();
    SgScopeStatement* scope = conditional_statement->get_scope();
    vector<SgNode*> node_list = scope->get_traversalSuccessorContainer();
    cout << "sgnodes: " << endl;
    for(SgNode* n : node_list) {
        string class_name = n -> class_name();
        cout << class_name << endl;
        if("SgExprStatement" == class_name) {
            
        }else if("SgBasicBlock" == class_name) {
            vector<SgNode*> nodes = n->get_traversalSuccessorContainer();
            for(SgNode* n2 : nodes) {
                cout << "    " << n2->class_name() << endl;
            }
        }
    }
}

int count(string s, char ch) {
    int count = 0;
    
    for(int i = 0 ; i < s.length() ; i++) {
        if(ch == s[i]) {
            count++;
        }
    }
    
    return count;
}

void State::set_end_if_line(bool has_false) {
    cout << "set_end_if_line" << endl;
    
    int else_line = line_ptr;
    if(has_false) {
        for( ; else_line < line_infos.size() ; else_line++) {
            if(line_infos[else_line].find("else") != string::npos) {
                if(line_infos[else_line].find("}") != string::npos) {
                    end_if_line.push_back(else_line);
                }else {
                    end_if_line.push_back(else_line-1);
                }
                break;
            }
        }
    }
    
    //cout << "else line: " << else_line << endl;
    
    int left_count = 0, right_count = 0;
    if(line_infos[else_line].find("}") != string::npos) {
        right_count = -1;
    }
    for(int i = else_line ; i < line_infos.size() ; i++) {
        left_count += count(line_infos[i], '{');
        right_count += count(line_infos[i], '}');
        if(left_count == right_count && left_count > 0) {
            exit_if_line.push_back(i+1);
            if(!has_false) {
                end_if_line.push_back(i);
            }
            //cout << "end_if_line: " << end_if_line << endl;
            break;
        }
    }
}

void State::false_path_set_ptr() {
    cout << "false_path_set_ptr" << endl;
    
    int else_line = line_ptr;
    for( ; else_line < line_infos.size() ; else_line++) {
        if(line_infos[else_line].find("else") != string::npos) {
            if(line_infos[else_line].find("{") != string::npos) {
                next_line_ptr = else_line+1;
            }else {
                next_line_ptr = else_line+2;
                break;
            }
        }
    }
    
    cout << "next_line_ptr: " << next_line_ptr << endl;
    
    /*int left_count = 0, right_count = 0;
    if(line_infos[else_line].find("}") != string::npos) {
        right_count = -1;
    }
    for(int i = else_line ; i < line_infos.size() ; i++) {
        left_count += count(line_infos[i], '{');
        right_count += count(line_infos[i], '}');
        if(left_count == right_count && left_count > 0) {
            end_if_line.push_back(i+1);
            cout << "end_if_line: " << (i+1) << endl;
            break;
        }
    }*/
}
