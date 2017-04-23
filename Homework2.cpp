/*
Written by Maria Kalikas

*/

#include<iostream>
#include <windows.h>
//#include "stdafx.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdio.h>
//#include <urlmon.h>
//#include <array>
#pragma comment(lib, "urlmon.lib")

struct Value{
    virtual ~Value() = default;
    virtual int weight() const;
};

struct Null : Value {
    std::string nul;
};

struct Bool : Value {
    Bool(bool B) : val(B) {}
    bool val;
};

struct Number : Value {
    double val;
	Number(double num) : val(num) {};
};

struct String : Value, std::string {
    using std::string::string;
    std::string val;
};

struct Array : Value, std::vector<Value*> {
    using std::vector<Value *>::vector;
    int weight() const {
        int n = 0;

        for(Value *v : *this)
            n+=v->weight();

        return n;
    }
    Array() {
		vector<Value *> vec;
	}
    ~Array(){
        for(Value *v : *this)
            delete v;
    }
};


// JSON Parser - Identifies the input in a console application

std::string indent = "";
int weight = 0;

void parse(std::ifstream & input) 
{
	std::string name;
	std::string value;
	std::string nothing;
	char next;
	char quotation;
	char colon;
	char space;
	char comma;

	int count = 0;

	bool getName = false;

	input.get(next);

	while (!input.eof())
	{

		while (true)
		{

			if (next == '{')
			{
				char temp = input.peek();

				if (temp == '}')
				{
					input.get(next);
					input.get(next);
					std::cout << indent << "{}\n";
				}
				else
					std::cout << indent << "{\n";
			}

			if (next == '}')
			{
				int size0 = indent.size();
				indent = indent.substr(0, size0 - 1);



			if (next == ',')
				std::cout << ",\n";

			if (next == '}')
				std::cout << indent << "}\n";

			if (!input.get(next) && next != ',')
				break;

			input.get(next);
		}

		if (next == '{')
		{
			weight++;

			getName = true;

			if (getName == true)
			{
				input.get(quotation);	// Used to get the quotation mark
				getline(input, name, '"');
				input.get(colon);	// Used to get the colon after the name
				input.get(space);	// Used to get the space after the colon
				input.get(next);
			}


			if (next == '"')
			{
				weight++;
				// String
				std::string temp;

				getline(input, value, '"');
				input.get(next);


				while (next != ',')
				{
					getline(input, temp, '"');
					value = value + next;
					value = value + temp;
					input.get(next);
				}

				String str_obj;
				str_obj.val = value;
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << str_obj.val << "\"";


			}
			if (next == '{')
			{
				// Object
				weight++;
				char temp = input.peek();
				input.putback(temp);
				if (temp == '}')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": {}";
					input.get(next);
					input.get(next);
					goto eof1;


				}
				else
					std::cout << indent << "\"" << name << "\"" << ": {" << "\n";

				indent = indent + "\t";
				count++;
				input.get(next);
				parse(input);
			eof1:;

			}
			if (next == '}')
			{
				std::cout << indent << "}";
				input.get(next);
				int size1 = indent.size();
				indent = indent.substr(0, size1 - 1);
			}
			if (next == '[')
			{
				// Array
				weight++;
				Array arr_obj;

				char tmppp = input.peek();
				input.putback(temp);
				if (temp == ']')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": []" << "\n";
					input.get(next);
					input.get(next);
					goto eof2;
				}
				else
					std::cout << indent << "\"" << name << "\": [" << "\n";

				count++;
				indent = indent + "\t";
				input.get(next);
				parse(input, count, indent);
			eof2:;
			}
			if (next == 't' || next == 'f')
			{
				// Boolean values - true and false
				Bool bool_obj;
				weight++;
				if (next == 't')
					bool_obj.val = true;
				else
					bool_obj.val = false;

				getline(input, nothing, 'e');

				std::cout << std::boolalpha << indent << "\"" << name << "\"" << ": " << "\"" << bool_obj.val << "\"";

				input.get(next);
			}
			if (next == 'n')
			{
				// Null
				weight++;
				Null null_obj;
				null_obj.val = NULL;
				getline(input, nothing, ',');
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << null_obj.val << "\",\n";
				input.get(next);
			}
			if (isdigit(next))	
			{
				// Number
				weight++;
				Number num_obj;
				std::stringstream ss;
				std::string number;

				ss << next;
				ss >> number;

				input.get(next);
				while (isdigit(next) || next == '.')
				{
					number = number + next;
					input.get(next);
				}

				num_obj = stod(number);

				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << num_obj.val << "\",\n";

				input.get(next);		
			}

			input.get(comma);	
			input.get(space);
			input.get(next);
		}
		else if (next == '"')
		{
			getline(input, name, '"');
			input.get(colon); // Used to get the colon after the name
			input.get(space); // Used to get the space after the colon
			input.get(next); // Look at the next type of character punctuation

			if (next == '"')
			{
				// String
				weight++;
				std::string temp;

				getline(input, value, '"');
				input.get(next);


				while (next != ',')
				{
					getline(input, temp, '"');
					value = value + next;
					value = value + temp;
					input.get(next);
				}

				String str_obj;
				str_obj.val = value;
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << str_obj.val << "\"";

			}
			if (next == '{')
			{
				// Object
				weight++;
				char tmpp = input.peek();
				input.putback(tmpp);
				if (tmpp == '}')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": {" << "\n\n" << indent << "}"; 
					input.get(next);
					input.get(next);
					goto eof3;
				}
				else
					std::cout << indent << "\"" << name << "\"" << ": {" << "\n";

				indent = indent + "\t";
				count++;
				input.get(next);
				parse(input);
				input.get(next);
			eof3:;

			}
			if (next == '}')
			{
				std::cout << indent << "}";
				input.get(next);
				int size2 = indent.size();
				indent = indent.substr(0, size2 - 1);
			}
			if (next == '[')
			{
				// Array
				weight++;
				Array arr_obj;

				char tmppp = input.peek();
				input.putback(temp);
				if (tempp == ']')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": [" << "\n\n" << indent << "]";
					input.get(next);
					input.get(next);
					goto eof4;
				}
				else
					std::cout << indent << "\"" << name << "\": [" << "\n";

				count++;
				indent = indent + "\t";
				input.get(next);
				parse(input, count, indent);

			eof4:;
			}
			if (next == 't' || next == 'f')
			{
				// Bool values - true and false
				weight++;
				Bool bool_obj;

				if (next == 't')
					bool_obj.val = true;
				else
					bool_obj.val = false;

				getline(input, nothing, 'e');

				std::cout << boolalpha << indent << "\"" << name << "\"" << ": " << "\"" << bool_obj.val << "\"";

				input.get(next);
			}
			if (next == 'n')
			{
				// Null
				weight++;
				Null null_obj;
				null_obj.val = NULL;
				getline(in, nothing, ',');
				input.get(next);	// get the 'u'
				input.get(next);	// get the 'l'
				input.get(next);	// get the 'l'
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << null_obj.val << "\"";
				input.get(next);
			}
			if (isdigit(next))
			{
				// Number
				weight++;
				Number num_obj;
				stringstream ss;
				std::string number;

				ss << next;
				ss >> number;

				input.get(next);
				while (isdigit(next) || next == '.')
				{
					number = number + next;
					input.get(next);
				}

				num_obj = stod(number);

				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << num_obj.val << "\"";
			}
		}
		else if (next == '}')
		{
			char temp = input.peek();
			input.putback(temp);

			std::cout << "\n" << indent << "}";
			int size = indent.size();
			count--;
			indent = indent.substr(0, size - 1);

			if (temp == '}')
			{
				std::cout << "\n" << indent << temp;
				size = indent.size();
				count--;
				indent = indent.substr(0, size - 1);
				input.get(next);

			}

			input.get(next);
			return;
		}
		else if (next == ']')
		{
			std::cout << "\n" << indent << "]";
			input.get(next);
		}
		else
			return;
	}

}


int main()
{
	ifstream input;
	ofstream out;
	int count = 0;

	std::string name;
	std::string value;
	std::string nothing;
	char next;
	char quotation;
	char colon;
	char space;

	input.open("reddit.txt");
	out.open("results.txt");

	std::string indent = "";
	parse(input);

	std::cout << "\n\n";

	std::cout << "Weight: " << weight << "\n\n";



	getline(input, nothing, '{');
	input.get(quotation);
	std::cout << quotation << "\n";

	getline(input, name, '"');
	std::cout << name << "\n";

	getline(input, nothing, '"');
	getline(input, value, '"');
	cout << value << endl;

	vector<Value *> a;
	// if you see '\', skip the next character

	input.close();
	out.close();
	return 0;
}
