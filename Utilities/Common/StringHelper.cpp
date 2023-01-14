#include "stdafx.h"
#include "StringHelper.h"

std::vector<std::string> StringHelper::Split(std::string input, const std::string& token)
{
	std::vector<std::string> output;

	size_t pos;
	while ((pos = input.find(token)) != -1)
	{
		output.push_back(input.substr(0, pos));
		input.erase(0, pos + token.length());
	}

	output.push_back(input);

	return output;
}
