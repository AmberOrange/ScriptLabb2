#include "Regex.hpp"
#include "Rules.hpp"
#include <iostream>
#include <string>



void ValidateExpression(const char* text)
{
	const char* backup = text;
	Tree *child;
	bool result = rule::TABLE(&text, &child);
	std::cout << backup << " : "
		<< (result ? "True" : "False") << std::endl;

	if (result)
	{
		child->dump();
		delete child;
	}
	std::cout << "\n\n\n";
}

int main() {
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

	std::cout << std::endl << std::endl;

	ValidateExpression("{}");
	ValidateExpression("{1,2;3}");
	ValidateExpression("{1,2;3,}");
	ValidateExpression("{easyas=\"abc\";2;2,[\"hello\"]=\"world\",[3]=4}");
	ValidateExpression("{{1,2,3},data={0x77}}");
	ValidateExpression("{{}");
	ValidateExpression("{;}");
	ValidateExpression("{1,,}");
	ValidateExpression("{34=7}");
	ValidateExpression("{alpha=beta=gamma}");


	std::cin.ignore();
	return 0;
}