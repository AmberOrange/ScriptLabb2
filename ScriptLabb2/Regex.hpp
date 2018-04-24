#pragma once
#include <string>
#include <vector>

class Regex
{
public:
	virtual int match(char const *) const = 0;
};

class CharClass : public Regex
{
private:
	std::string contents;
public:
	CharClass(std::string c) : contents(c) {}
	int match(char const *text) const
	{
		return contents.find(*text) == std::string::npos ? -1 : 1;
	}
};

class Star : public Regex
{
private:
	const Regex *mOperand;
public:
	Star(const Regex *operand) : mOperand(operand) {};
	int match(char const *text) const
	{
		int n, consumed = 0;
		while ((n = mOperand->match(text)) > 0)
		{
			consumed += n;
			text += n;
		}
		return consumed;
	}
};

class Seq : public Regex
{
private:
	std::vector<Regex*> mCells;
public:
	Seq(std::vector<Regex*> cells) : mCells(cells) {};
	int match(char const *text) const
	{
		int chars, consumed = 0;
		for (auto c : mCells)
		{
			if ((chars = c->match(text)) < 0) return -1;
			consumed += chars;
			text += chars;
		}
		return consumed;
	}
};

bool Compare(Regex *method, std::string &text)
{
	//int num = method->match(text.c_str());
	//int actNum = text.length();
	return method->match(text.c_str()) == text.length();
}