#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "environment.hpp"
#include "eval.hpp"
#include "execute.hpp"
#include "parse.hpp"
#include "token_array.hpp"
#include "value.hpp"
#include "typed_ast.hpp"
#include "exit_status_type.hpp"

int main() {

	std::ifstream in_fs("source.jp");

	std::stringstream file_content;
	std::string line;

	while(std::getline(in_fs, line)){
		file_content << line << '\n';
	}

	std::string source = file_content.str();

	exit_status_type exit_code = execute(source, false, +[](Type::Environment& env) -> exit_status_type {

		// NOTE: We currently implement funcion evaluation in eval(ASTCallExpression)
		// this means we need to create a call expression node to run the program.
		// TODO: We need to clean this up

		{
			TokenArray ta;
			auto top_level_call_ast = parse_expression("__invoke()", ta);
			auto top_level_call = TypedAST::convertAST(top_level_call_ast.m_result.get());

			auto* result = eval(top_level_call, env);

			if (result)
				Type::print(result);
			else
				std::cout << "(nullptr)\n";

			delete top_level_call;
		}

		return exit_status_type::Ok;
	});

	return static_cast<int>(exit_code);
}
