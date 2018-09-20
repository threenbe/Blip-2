#include <iostream>
#include "Parse.h"
#include "String.h"
#include "BST.h"
#include <stack>
#include <vector>

static int32_t exponent(int32_t base, uint32_t n) {
    if (n == 0) { return 1; }
    else { return (int32_t) (base * exponent(base, n-1)); }
}

static int32_t s2i(String& number) {
    int32_t num = 0;
    bool is_neg = false;
    uint32_t i = 0;
    uint32_t sub = 1;
    if (number[0] == '-') {
        is_neg = true;
        i = 1;
        sub = 2;
    }

    for (; i < number.size(); i++) {
        int32_t temp_num = (int32_t) (number[i] - '0');
        int32_t factor = exponent(10, number.size() - sub);
        num += (temp_num*factor);
        sub++;
    }

    if (is_neg) { num *= -1; }
    return num;
}

static int32_t operate(String& op, int32_t arg1, int32_t arg2) {
	String plus = String("+");
	String minus = String("-");
	String times = String("*");
	String div = String("/");
	String mod = String("%");
	String and_ = String ("&&");
	String or_ = String("||");
	String less = String("<");
	String more = String(">");
	String eq = String("==");
	String uneq = String("!=");
	String less_eq = String("<=");
	String more_eq = String(">=");

	if (op == plus) 
		return arg1 + arg2;
	if (op == minus)
		return arg1 - arg2;
	if (op == times)
		return arg1 * arg2;
	if (op == div)
		return arg1 / arg2;
	if (op == mod)
		return arg1 % arg2;
	if (op == and_)
		return arg1 && arg2;
	if (op == or_)
		return arg1 || arg2;
	if (op == less)
		return arg1 < arg2;
	if (op == more)
		return arg1 > arg2;
	if (op == eq)
		return arg1 == arg2;
	if (op == uneq)
		return arg1 != arg2;
	if (op == less_eq)
		return arg1 <= arg2;
	if (op == more_eq)
		return arg1 >= arg2;
}

/***************************************execute***************************************
This function actually solves an expression. It is only called to solve a single expression
at a time. The result is pushed onto the stack.
INPUTS: references to value and operator stacks
OUTPUTS: none
*/
static void execute(std::stack<int32_t>& value_stack, std::stack<String>& operator_stack) {
	String not_ = String("!");
	String neg = String("~");
	int32_t pop1;
	int32_t pop2;
	String op;

	op = operator_stack.top();
	operator_stack.pop();
	pop2 = value_stack.top();
	value_stack.pop();

	if (op == not_) {
		pop2 = !(pop2);
		value_stack.push(pop2);
	}
	else if (op == neg) {
		pop2 *= -1;
		value_stack.push(pop2);
	}
	else {
		pop1 = value_stack.top();
		value_stack.pop();
		int32_t result = operate(op, pop1, pop2);
		value_stack.push(result);		
	}
}

/***************************************evaluate***************************************
This function evaluates part of an expression. It parses an expression until it either finds
two operands (constant number or variable) after a binary operator or one operand after a unary
operator and then solves the expression by calling the execute function. If there are symbols in place of where
operators are supposed to be, then the expressions with those symbols are evaluated first, and the preceding operators
are evaluated recursively. The final result of an expression should be the sole value left on the value_stack.
INPUTS: references to value_stack, operator_stack, symbol_table
OUTPUTS: none
*/
static void evaluate(std::stack<int32_t>& value_stack, std::stack<String>& operator_stack, BST<String>& symbol_table) {
	String neg = String("~");
	String not_ = String("!");

	read_next_token();
	String tok = String(next_token());
	if (next_token_type == NUMBER) {
		value_stack.push(token_number_value);
	}
	else if (next_token_type == NAME) {
		value_stack.push(symbol_table.getVar(tok));
	}

	else if (next_token_type == SYMBOL) {

		operator_stack.push(tok);

		if (tok == neg || tok == not_) {
			String peek = String(peek_next_token());

			if (next_token_type == NUMBER) {
				read_next_token();
				value_stack.push(token_number_value);
			} else if (next_token_type == NAME) {
				read_next_token();
				tok = String(next_token());
				value_stack.push(symbol_table.getVar(tok));
			} else if (next_token_type == SYMBOL) {
				evaluate(value_stack, operator_stack, symbol_table);
			}
			execute(value_stack, operator_stack);
		}

		else {
			String peek = String(peek_next_token());

			if (next_token_type == NUMBER) {
				read_next_token();
				value_stack.push(token_number_value);
			} else if (next_token_type == NAME) {
				read_next_token();
				tok = String(next_token());
				value_stack.push(symbol_table.getVar(tok));
			} else if (next_token_type == SYMBOL) {
				evaluate(value_stack, operator_stack, symbol_table);
			}

			peek = String(peek_next_token());

			if (next_token_type == NUMBER) {
				read_next_token();
				value_stack.push(token_number_value);
			} else if (next_token_type == NAME) {
				read_next_token();
				tok = String(next_token());
				value_stack.push(symbol_table.getVar(tok));
			} else if (next_token_type == SYMBOL) {
				evaluate(value_stack, operator_stack, symbol_table);
			}
			execute(value_stack, operator_stack);
		}
	}
}

/***************************************begin_eval***************************************
This helper function begins the evaluation of an expression. It keeps calling the evaluate
function until the expression has been solved in its entirety.
INPUTS: references to the value stack, operator stack, and symbol table
OUTPUTS: none
*/
static void begin_eval(std::stack<int32_t>& value_stack, std::stack<String>& operator_stack, BST<String>& symbol_table) {
	String comment = String("//");
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");
	String if_ = String("if");
	String else_ = String("else");
	String fi = String("fi");
	String do_ = String("do");
	String od = String("od");

	evaluate(value_stack, operator_stack, symbol_table);
	String peek = String(peek_next_token());

	while (peek != var && peek != set && peek != output && peek != text && next_token_type != END 
		&& peek != comment && peek != if_ && peek != else_ && peek != fi && peek != do_ && peek != od) {

		evaluate(value_stack, operator_stack, symbol_table);
		peek = String(peek_next_token());
	}
}

static int32_t exp_vec_eval(BST<String>& symbol_table, std::vector<String>& exp_vec, uint32_t& iter);

static void execute_do(BST<String>& symbol_table, std::vector<String>& do_vec);

static void parse_do(std::vector<String>& input_vec, uint32_t& iter, BST<String>& symbol_table) {
	std::vector<String> exp_vec;
	std::vector<String> do_vec;

	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");
	String if_ = String("if");
	String else_ = String("else");
	String do_ = String("do");
	String od = String("od");
	String name_ = String("NAME");
	String number_ = String("NUMBER");
	String symbol_ = String("SYMBOL");
	bool empty = false;

	String tok_type = input_vec[iter];
	String tok = input_vec[iter+1];
	iter += 2;

	while (tok != text && tok != output && tok != var && tok != set && tok != if_ && tok != else_ && tok != do_ && tok != od) {
		if (tok_type == name_) {
			exp_vec.push_back(name_);
		} 
		else if (tok_type == number_) {
			exp_vec.push_back(number_);
		}
		else if (tok_type == symbol_) {
			exp_vec.push_back(symbol_);
		}

		exp_vec.push_back(tok);

		tok_type = input_vec[iter];
		tok = input_vec[iter+1];
		iter += 2;
	}

	if (tok == od) { empty = true; }

	uint32_t nest_flag = 0;
	while (tok != od) {
		if (tok_type == name_) {
			do_vec.push_back(name_);
		}
		else if (tok_type == number_) {
			do_vec.push_back(number_);
		}
		else if (tok_type == symbol_) {
			do_vec.push_back(symbol_);
		}

		do_vec.push_back(tok);
		if (tok == do_) { nest_flag += 1; }

		tok_type = input_vec[iter];
		tok = input_vec[iter+1];
		iter += 2;

		if (tok == od && nest_flag > 0) {
			nest_flag -= 1;
			do_vec.push_back(name_);
			do_vec.push_back(tok);
			tok_type = input_vec[iter];
			tok = input_vec[iter+1];
			iter += 2;
		}
	}

	while(1) {
		uint32_t iter = 0;
		int32_t result = exp_vec_eval(symbol_table, exp_vec, iter);

		if (result) {
			if (empty == false) { execute_do(symbol_table, do_vec); }
		}
		else {
			break;
		}
	}
}

static void execute_do(BST<String>& symbol_table, std::vector<String>& do_vec) {

	String comment = String("//");
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");
	String if_ = String("if");
	String else_ = String("else");
	String fi = String("fi");
	String do_ = String("do");
	String name_ = String("NAME");

	uint32_t iter = 0;
	String type = do_vec[iter];
	String tok = do_vec[iter+1];
	iter += 2;

	while (iter < do_vec.size()) {

		if (tok == text) {
			tok = do_vec[iter+1];
			iter += 2;
			std::cout << tok.c_str();
		}

		else if (tok == output) {
			std::cout << exp_vec_eval(symbol_table, do_vec, iter);
		}

		else if (tok == var) {
			tok = do_vec[iter+1];
			iter += 2;

			if (symbol_table.varExists(tok)) {

				std::cout << "variable " << tok.c_str() << " incorrectly re-initialized\n";

				symbol_table.updateVar(tok, exp_vec_eval(symbol_table, do_vec, iter));
			} else {

				symbol_table.insert(tok, exp_vec_eval(symbol_table, do_vec, iter));
			}
		}

		else if (tok == set) {
			tok = do_vec[iter+1];
			iter += 2;

			if (symbol_table.varExists(tok)) {

				symbol_table.updateVar(tok, exp_vec_eval(symbol_table, do_vec, iter));
			} else {

				std::cout << "variable " << tok.c_str() << " not declared\n";

				symbol_table.insert(tok, exp_vec_eval(symbol_table, do_vec, iter));
			}
		}

		else if (tok == if_) {
			if (!(exp_vec_eval(symbol_table, do_vec, iter))) {
				uint32_t if_flag = 0;
				while ((tok != else_ && tok != fi) || if_flag) {
					tok = do_vec[iter+1];
					iter += 2;
					if (tok == if_) { if_flag += 1; }
					if (tok == fi && if_flag > 0) {
						if_flag -= 1;
						if (do_vec[iter+1] == else_ && if_flag == 0) {
							tok = do_vec[iter+1];
							iter += 2;
						}
					} 
				}
			}
		}

		else if (tok == else_) {
			tok = do_vec[iter+1];
			iter += 2;
			uint32_t nest_flag = 0;
			while (tok != fi) {
				tok = do_vec[iter+1];
				iter += 2;
				if (tok == if_) { nest_flag += 1; }
				else if (tok == fi && nest_flag > 0) {
					nest_flag -= 1;
					tok = do_vec[iter+1];
					iter += 2;
				}
			}
		}

		else if (tok == do_) {
			parse_do(do_vec, iter, symbol_table);
		}

		if (iter >= do_vec.size()) { break; }
		type = do_vec[iter];
		tok = do_vec[iter+1];
		iter += 2;
	}
}

static int32_t exp_vec_eval(BST<String>& symbol_table, std::vector<String>& exp_vec, uint32_t& iter) {
	String neg = String("~");
	String not_ = String("!");
	String name_ = String("NAME");
	String number_ = String("NUMBER");


	String first_tok_type = exp_vec[iter];
	String first_tok = exp_vec[iter+1];
	iter += 2;

	if (first_tok_type == number_) {
		return s2i(first_tok);
	} else if (first_tok_type == name_) {
		return symbol_table.getVar(first_tok);
	}

	int32_t arg1 = exp_vec_eval(symbol_table, exp_vec, iter);
	if (first_tok == neg) { return (-1 * arg1); }
	else if (first_tok == not_) { return !(arg1); }
	int32_t arg2 = exp_vec_eval(symbol_table, exp_vec, iter);
	return operate(first_tok, arg1, arg2);
}

static void parse_do(BST<String>& symbol_table) {
	std::vector<String> exp_vec;
	std::vector<String> do_vec;

	String comment = String("//");
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");
	String if_ = String("if");
	String else_ = String("else");
	String do_ = String("do");
	String od = String("od");
	bool empty = false;

	read_next_token();
	String tok = String(next_token());

	while (tok != text && tok != output && tok != var && tok != set && tok != if_ && tok != else_ && tok != do_ && tok != od) {
		if (next_token_type == NAME) {
			exp_vec.push_back(String("NAME"));
		}
		else if (next_token_type == NUMBER) {
			exp_vec.push_back(String("NUMBER"));
		}
		else if (next_token_type == SYMBOL) {
			if (tok != comment) { exp_vec.push_back(String("SYMBOL")); }
			else { skip_line(); }
		}

		if (tok != comment) { exp_vec.push_back(tok); }

		read_next_token();
		tok = String(next_token());
	}

	if (tok == od) { empty = true; }

	uint32_t nest_flag = 0;
	while (tok != od) {
		if (next_token_type == NAME) {
			do_vec.push_back(String("NAME"));
		}
		else if (next_token_type == NUMBER) {
			do_vec.push_back(String("NUMBER"));
		}
		else if (next_token_type == SYMBOL) {
			if (tok != comment) { do_vec.push_back(String("SYMBOL")); }
			else { skip_line(); }
		}

		if (tok != comment) { do_vec.push_back(tok); }
		if (tok == do_) { nest_flag += 1; }

		read_next_token();
		tok = String(next_token());

		if (tok == od && nest_flag > 0) {
			nest_flag -= 1;
			do_vec.push_back(String("NAME"));
			do_vec.push_back(tok);
			read_next_token();
			tok = String(next_token());
		}		
	}

	while(1) {
		uint32_t iter = 0;
		int32_t result = exp_vec_eval(symbol_table, exp_vec, iter);

		if (result) {
			if (empty == false) { execute_do(symbol_table, do_vec); }
		} else {
			break;
		}
	}
}


/***************************************run***************************************
This function is to be called by a main function wants to read and execute a blip file.
Parses the file and carries out instructions accordingly.
INPUTS: none
OUTPUTS: none
*/
void run() {
	std::stack<int32_t> value_stack;
	std::stack<String> operator_stack;
	BST<String> symbol_table;

	String comment = String("//");
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");
	String if_ = String("if");
	String else_ = String("else");
	String fi = String("fi");
	String do_ = String("do");

	read_next_token();
	String tok;
		
	while (next_token_type != END) {

		if (next_token_type == SYMBOL) {
			tok = String(next_token());

			if (tok == comment) {
				skip_line();
			} 
		}

		else if (next_token_type == NAME) {
			tok = String(next_token());

			if (tok == text) {
				read_next_token();
				String tok_text = String(next_token());
				std::cout << tok_text.c_str();
			}

			else if (tok == output) {
				begin_eval(value_stack, operator_stack, symbol_table);
				std::cout << value_stack.top();
				value_stack.pop();
			}

			else if (tok == var) {
				read_next_token();
				String var_tok = String(next_token());

				if (symbol_table.varExists(var_tok)) {

					std::cout << "variable " << var_tok.c_str() << " incorrectly re-initialized\n";

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.updateVar(var_tok, value_stack.top());
					value_stack.pop();
				} else {

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.insert(var_tok, value_stack.top());
					value_stack.pop();
				}
			}

			else if (tok == set) {
				read_next_token();
				String var_tok = String(next_token());

				if (symbol_table.varExists(var_tok)) {

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.updateVar(var_tok, value_stack.top());
					value_stack.pop();
				} else {

					std::cout << "variable " << var_tok.c_str() << " not declared\n";

					begin_eval(value_stack, operator_stack, symbol_table);

					symbol_table.insert(var_tok, value_stack.top());
					value_stack.pop();
				}
			}

			else if (tok == if_) {
				begin_eval(value_stack, operator_stack, symbol_table);
				if (value_stack.top()) {
					value_stack.pop();
				}
				else {
					uint32_t if_flag = 0;
					while ((tok != else_ && tok != fi) || if_flag) {
						read_next_token();
						tok = String(next_token());
						if (tok == if_) { if_flag += 1; }
						if (tok == fi && if_flag > 0) { 
							if_flag -= 1;
							String peek = String(peek_next_token());
							if (peek == else_ && if_flag == 0) { 
								read_next_token(); 
								tok = String(next_token()); 
							}
						}
					}
				}
			}

			else if ((tok == else_)) {
				read_next_token();
				tok = String(next_token());
				uint32_t nest_flag = 0;
				while (tok != fi) {
					read_next_token();
					tok = String(next_token());
					if (tok == if_) { nest_flag += 1; }
					else if (tok == fi && nest_flag > 0) { 
						nest_flag -= 1;
						read_next_token();
						tok = String(next_token());
					}
				}
			}

			else if (tok == do_) {
				parse_do(symbol_table);
			}
		}

		read_next_token();
	}
}
