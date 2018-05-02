#pragma once
#include "Regex.hpp"
#include <list>
#include <iostream>
/*
NUM : "[0-9][0-9]*"
HEX : "0x[0-9a-fA-F][0-9a-fA-F]*"
VAR : "[a-zA-Z][a-zA-Z]*"
STR : "\"[a-zA-Z]*\""
DELIM : "[,;]"
TABLE : "{" FIELDRULE "}"
FIELDRULE: FIELD FIELDRULE2 | EMPTY
FIELDRULE2: DELIM FIELDRULE | EMPTY
FIELD: ASSIGN | VALUE
VALUE: HEX | NUM | VAR |  STR | TABLE
ASSIGN: ASSIGNKEY "=" VALUE
ASSIGNKEY: VAR | "[" KEY "]"
KEY: NUM | STR
*/

class Tree
{
public:
	std::string lexeme, tag;
	std::list<Tree*> children;
	Tree(std::string t, const char *l, int size)
		: tag(t), lexeme(l, size) {}
	~Tree()
	{
		for (Tree *n : this->children)
			if (n != nullptr) delete n;
	}
	void dump(int depth = 0)
	{
		for (int i = 0; i<depth; i++)
			std::cout << "  ";
		std::cout << this->tag << ": " << this->lexeme << std::endl;
		for (Tree *n : this->children)
			if(n != nullptr) n->dump(depth+1);
		// Recurse over the children
	}
};

namespace rule {
	bool NUM(const char **text, Tree **result);
	bool HEX(const char **text, Tree **result);
	bool VAR(const char **text, Tree **result);
	bool STR(const char **text, Tree **result);
	bool DELIM(const char **text, Tree **result);
	bool TABLE(const char **text, Tree **result);
	bool TERM(char term, const char **text, Tree **result);
	bool FIELDRULE(const char **text, Tree **result);
	bool FIELDRULE2(const char **text, Tree **result);
	bool FIELD(const char **text, Tree **result);
	bool VALUE(const char **text, Tree **result);
	bool ASSIGN(const char **text, Tree **result);
	bool ASSIGNKEY(const char **text, Tree **result);
	bool KEY(const char **text, Tree **result);

	// NUM : "[0-9][0-9]*"
	bool NUM(const char **text, Tree **result)
	{
		static CharClass num("0123456789");
		static Star numStar(&num);
		static Seq seq({ &num, &numStar });
		int length = seq.match(*text);
		if (length == -1) return false;
		*result = new Tree("NUM", *text, length);
		*text += length;
		return true;
	}
	// HEX : "0x[0-9a-fA-F][0-9a-fA-F]*"
	bool HEX(const char **text, Tree **result)
	{
		static CharClass num("0123456789abcdefABCDEF");
		static Star numStar(&num);
		static CharClass zero("0");
		static CharClass x("x");
		static Seq seq({ &zero, &x, &num, &numStar });
		int length = seq.match(*text);
		if (length == -1) return false;
		*result = new Tree("HEX", *text, length);
		*text += length;
		return true;
	}
	// VAR : "[a-zA-Z][a-zA-Z]*"
	bool VAR(const char **text, Tree **result)
	{
		static CharClass cha("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
		static Star chaStar(&cha);
		static Seq seq({ &cha, &chaStar });
		int length = seq.match(*text);
		if (length == -1) return false;
		*result = new Tree("VAR", *text, length);
		*text += length;
		return true;
	}
	// STR : "\"[a-zA-Z]*\""
	bool STR(const char **text, Tree **result)
	{
		static CharClass cha("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
		static Star chaStar(&cha);
		static CharClass cite("\"");
		static Seq seq({ &cite, &chaStar, &cite });
		int length = seq.match(*text);
		if (length == -1) return false;
		*result = new Tree("STR", *text, length);
		*text += length;
		return true;
	}
	// DELIM : "[,;]"
	bool DELIM(const char **text, Tree **result)
	{
		static CharClass delim(",;");
		int length = delim.match(*text);
		if (length == -1) return false;
		*result = new Tree("DELIM", *text, length);
		*text += length;
		return true;
	}

	bool TERM(char term, const char **text, Tree **result)
	{
		if (**text == term)
		{
			*result = new Tree("TERM", *text, 1);
			(*text)++;
			return true;
		}
		else return false;
	}

	// TABLE : "{" FIELDRULE "}"
	bool TABLE(const char **text, Tree **result)
	{
		Tree *child[3] = { nullptr };
		const char *backup = *text;
		if (!TERM('{', text, &child[0])) return false;
		if (!FIELDRULE(text, &child[1])) return false;
		if (!TERM('}', text, &child[2])) return false;
		*result = new Tree("TABLE", backup, (int)(*text - backup));
		(*result)->children.push_back(child[0]);
		(*result)->children.push_back(child[1]);
		(*result)->children.push_back(child[2]);
		return true;
	}
	//FIELDRULE: FIELD FIELDRULE2 | EMPTY
	bool FIELDRULE(const char **text, Tree **result)
	{
		Tree *child[2] = { nullptr };
		const char *backup = *text;
		if (FIELD(text, &child[0]) && FIELDRULE2(text, &child[1]))
		{
			*result = new Tree("FIELDRULE", backup, (int)(*text - backup));
			(*result)->children.push_back(child[0]);
			(*result)->children.push_back(child[1]);
		}
		else
			*text = backup;
		return true;
	}
	//FIELDRULE2 : DELIM FIELDRULE | EMPTY
	bool FIELDRULE2(const char **text, Tree **result)
	{
		Tree *child[2] = { nullptr };
		const char *backup = *text;
		if (DELIM(text, &child[0]) && FIELDRULE(text, &child[1]))
		{
			*result = new Tree("FIELDRULE2", backup, (int)(*text - backup));
			(*result)->children.push_back(child[0]);
			(*result)->children.push_back(child[1]);
		} else
			*text = backup;
		return true;
	}
	//FIELD : ASSIGN | VALUE
	bool FIELD(const char **text, Tree **result)
	{
		Tree *child = nullptr;
		const char *backup = *text;
		if (ASSIGN(text, &child) || VALUE(text, &child))
		{
			*result = new Tree("FIELD", backup, (int)(*text - backup));
			(*result)->children.push_back(child);
			return true;
		}
		return false;
	}
	//VALUE: HEX | NUM | VAR | STR | TABLE
	bool VALUE(const char **text, Tree **result)
	{
		Tree *child = nullptr;
		const char *backup = *text;
		if (HEX(text, &child)
			|| NUM(text, &child)
			|| VAR(text, &child)
			|| STR(text, &child)
			|| TABLE(text, &child))
		{
			*result = new Tree("VALUE", backup, (int)(*text - backup));
			(*result)->children.push_back(child);
			return true;
		}
		return false;
	}
	//ASSIGN : ASSIGNKEY "=" VALUE
	bool ASSIGN(const char **text, Tree **result)
	{
		Tree *child[3] = { nullptr };
		const char *backup = *text;
		if (!ASSIGNKEY(text, &child[0])) return false;
		if (!TERM('=', text, &child[1])) return false;
		if (!VALUE(text, &child[2])) return false;
		*result = new Tree("ASSIGN", backup, (int)(*text - backup));
		(*result)->children.push_back(child[0]);
		(*result)->children.push_back(child[1]);
		(*result)->children.push_back(child[2]);
		return true;
	}
	//ASSIGNKEY : VAR | "[" KEY "]"
	bool ASSIGNKEY(const char **text, Tree **result)
	{
		const char *backup = *text;
		Tree *child1 = nullptr;
		if (VAR(text, &child1))
		{
			*result = new Tree("ASSIGNKEY", backup, (int)(*text - backup));
			(*result)->children.push_back(child1);
			return true;
		}
		else {
			Tree *child[3] = { nullptr };
			if (!(TERM('[', text, &child[0]))) return false;
			if (!KEY(text, &child[1])) return false;
			if (!TERM(']', text, &child[2])) return false;
			*result = new Tree("ASSIGNKEY", backup, (int)(*text - backup));
			(*result)->children.push_back(child[0]);
			(*result)->children.push_back(child[1]);
			(*result)->children.push_back(child[2]);
			return true;
		}
	}
	//KEY : NUM | STR
	bool KEY(const char **text, Tree **result)
	{
		Tree *child = nullptr;
		const char *backup = *text;
		if (NUM(text, &child) || STR(text, &child))
		{
			*result = new Tree("KEY", backup, (int)(*text - backup));
			(*result)->children.push_back(child);
			return true;
		}
		return false;
	}
}