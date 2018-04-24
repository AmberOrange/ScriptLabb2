#include "Regex.hpp"
#include "Rules.hpp"
#include <iostream>
#include <string>

int main() {
	//const char *test = "0541543x4323";
	std::string testStrings[] =
	{
		"0x1",
		"0xfed",
		"0xCBA9",
		"0x00000000",
		"12ef",
		"0y7",
		"0xG900"
	};

	CharClass hex("0123456789abcdefABCDEF");
	Star hexNum(&hex);
	Seq custom({ new CharClass("0"), new CharClass("x"), &hexNum });

	for (std::string s : testStrings)
	{
		std::cout << s << " " << (Compare(&custom, s) ? "True" : "False") << std::endl;
	}

	//std::cout << *(test+1);
	std::cin.ignore();
	return 0;
}


// TABLE : "{" TABLE2 "}"
// TABLE2 : "gs" TABLE2 | empty
/*
{gsgsgsgsgsgsgswqe}

TABLE: {gsgsgsgsgsgsgswqe}
	TERM: {
	TABLE2: gsgsgsgsgsgsgswqe}
		TERM: gs
		TABLE2: gsgsgsgsgsgswqe}
			TABLE2: gsgsgsgsgs
				...
					TABLE2: wqe}
	TERM: }





{}
TABLE : {}
	TERM: {
	TABLE2: }
		Empty
	TERM: }

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

{1,2;3}
TABLE : {1,2;3}
	TERM: {
	TABLE2 : 1,2;3}
		NUMRULE : 1,
			NUM : 1
			DELIM : ,
		EMPTY
		NUMRULE : 2;
			NUM : 2
			DELIM : ;
		EMPTY


*/