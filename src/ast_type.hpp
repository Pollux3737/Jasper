#pragma once

inline const char* ast_type_string[] = {
	"NumberLiteral",
	"StringLiteral",
	"ObjectLiteral",
	"DictionaryLiteral",
	"FunctionLiteral",

	"DeclarationList",
	"Declaration",
	"Identifier",
	"BinaryExpression",
	"CallExpression",
	"ArgumentList",
	"Block",
	"ReturnStatement",
};

enum class ast_type {
	NumberLiteral,
	StringLiteral,
	ObjectLiteral,
	DictionaryLiteral,
	FunctionLiteral,

	DeclarationList,
	Declaration,
	Identifier,
	BinaryExpression,
	CallExpression,
	ArgumentList,
	Block,
	ReturnStatement,
};
