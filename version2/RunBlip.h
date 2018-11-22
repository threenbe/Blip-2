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

static void execute_vec(std::vector<String> run_vec, BST<String> symbol_table) {
	String text = String("text");
	String output = String("output");
	String var = String("var");
	String set = String("set");
	String if_ = String("if");
	String else_ = String("else");
	String fi = String("fi");
	String do_ = String("do");
	String od = String("od");

	uint32_t iter = 0;
	std::vector<uint32_t> nested_do_iters;
	uint32_t do_num = 0;
	String tok = run_vec[iter+1];
	iter += 2;

	while (1) {

		if (tok == text) {
			tok = run_vec[iter+1];
			iter += 2;
			std::cout << tok.c_str();
		}

		else if (tok == output) {
			std::cout << exp_vec_eval(symbol_table, run_vec, iter);
		}

		else if (tok == var) {
			tok = run_vec[iter+1];
			iter += 2;

			if (symbol_table.varExists(tok)) {

				std::cout << "variable " << tok.c_str() << " incorrectly re-initialized\n";

				symbol_table.updateVar(tok, exp_vec_eval(symbol_table, run_vec, iter));
			} else {

				symbol_table.insert(tok, exp_vec_eval(symbol_table, run_vec, iter));
			}
		}

		else if (tok == set) {
			tok = run_vec[iter+1];
			iter += 2;

			if (symbol_table.varExists(tok)) {

				symbol_table.updateVar(tok, exp_vec_eval(symbol_table, run_vec, iter));
			} else {

				std::cout << "variable " << tok.c_str() << " not declared\n";

				symbol_table.insert(tok, exp_vec_eval(symbol_table, run_vec, iter));
			}
		}

		else if (tok == if_) {
			if (!(exp_vec_eval(symbol_table, run_vec, iter))) {
				uint32_t if_count = 0;

				while (tok != else_ && tok != fi) {
					tok = run_vec[iter+1];
					iter += 2;
					if (tok == if_) { if_count += 1; }
					while (tok == else_ && if_count > 0) {
						tok = run_vec[iter+1];
						iter += 2;
					}
					while (tok == fi && if_count > 0) { 
						if_count -= 1;
						tok = run_vec[iter+1];
						iter += 2;
					}
				}
			}
		}

		else if (tok == else_) {
			tok = run_vec[iter+1];
			iter += 2;
			uint32_t if_count = 0;

			while (tok != fi) {
				tok = run_vec[iter+1];
				iter += 2;
				if (tok == if_) { if_count += 1; }
				while (tok == fi && if_count > 0) {
					if_count -= 1;
					tok = run_vec[iter+1];
					iter += 2;
				}
			}
		}

		else if (tok == do_) {
			do_num += 1;
			nested_do_iters.push_back(iter);

			if (!(exp_vec_eval(symbol_table, run_vec, iter))) {
				do_num -= 1;
				nested_do_iters.pop_back();
				uint32_t do_count = 0;
				while (tok != od) {
					tok = run_vec[iter+1];
					iter += 2;
					if (tok == do_) { do_count += 1; }
					while (tok == od && do_count > 0) {
						do_count -= 1;
						tok = run_vec[iter+1];
						iter += 2;
					}
				}
			}
		}

		else if (tok == od) {
			iter = nested_do_iters[do_num-1] - 2;
			nested_do_iters.pop_back();
			do_num -= 1;
		}

		if (iter >= run_vec.size()) { break; }
		tok = run_vec[iter+1];
		iter += 2;
	}
}

/***************************************run***************************************
This function is to be called by a main function wants to read and execute a blip file.
Parses the file and carries out instructions accordingly.
INPUTS: none
OUTPUTS: none
*/
void run() {
	std::vector<String> run_vec;
	BST<String> symbol_table;

	String comment = String("//");

	read_next_token();
	String tok = String(next_token());
		
	while (next_token_type != END) {

		if (tok == comment) {
			skip_line();
			read_next_token();
			tok = String(next_token());
			continue;
		}

		if (next_token_type == NAME) {
			run_vec.push_back(String("NAME"));
		} else if (next_token_type == NUMBER) {
			run_vec.push_back(String("NUMBER"));
		} else if (next_token_type == SYMBOL) {
			run_vec.push_back(String("SYMBOL"));
		}

		run_vec.push_back(tok);

		read_next_token();
		tok = String(next_token());
	}

	execute_vec(run_vec, symbol_table);
}
