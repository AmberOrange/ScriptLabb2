#pragma once
#include "Regex.hpp"
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
VALUE: HEX | NUM | WORD | TABLE | STR
ASSIGN: ASSIGNKEY "=" VALUE
ASSIGNKEY: VAR | "[" KEY "]"
KEY: NUM | STR
*/
namespace rule {
	// NUM : "[0-9][0-9]*"
	int NUM(char const *text)
	{
		static CharClass num("0123456789");
		static Star numStar(&num);
		const static Seq seq({ &num, &numStar });
		return seq.match(text);
	}
	// HEX : "0x[0-9a-fA-F][0-9a-fA-F]*"
	int HEX(char const *text)
	{
		static CharClass num("0123456789abcdefABCDEF");
		static Star numStar(&num);
		static CharClass zero("0");
		static CharClass x("x");
		const static Seq seq({ &zero, &x, &num, &numStar });
		return seq.match(text);
	}
	// VAR : "[a-zA-Z][a-zA-Z]*"
	int VAR(char const *text)
	{
		static CharClass cha("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
		static Star chaStar(&cha);
		const static Seq seq({ &cha, &chaStar });
		return seq.match(text);
	}
	// STR : "\"[a-zA-Z]*\""
	int STR(char const *text)
	{
		static CharClass cha("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
		static Star chaStar(&cha);
		static CharClass cite("\"");
		const static Seq seq({ &cite, &chaStar, &cite });
		return seq.match(text);
	}
	// DELIM : "[,;]"
	int DELIM(char const *text)
	{
		static CharClass delim(",;");
		return delim.match(text);
	}
	// TABLE : "{" FIELDRULE "}"
	bool TABLE(char const *text)
	{

	}
}