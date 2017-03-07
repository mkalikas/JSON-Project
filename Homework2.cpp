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
				char tmpp = input.peek();

				if (tmpp == '}')
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
				// string
				std::string tmp;

				getline(input, value, '"');
				input.get(next);


				while (next != ',')
				{
					getline(input, tmp, '"');
					value = value + next;
					value = value + tmp;
					input.get(next);
				}

				String str_obj;
				str_obj.val = value;
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << str_obj << "\"";


			}
			if (next == '{')
			{
				// object
				weight++;
				char tmpp = input.peek();
				input.putback(tmpp);
				if (tmpp == '}')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": {}";
					input.get(next);
					input.get(next);
					goto endofile;


				}
				else
					std::cout << indent << "\"" << name << "\"" << ": {" << "\n";

				indent = indent + "\t";
				count++;
				input.get(next);
				parse(input);
			endofile:;

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
				// array
				weight++;
				//Array arr_obj;

				char tmppp = input.peek();
				input.putback(tmppp);
				if (tmppp == ']')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": []" << "\n";
					input.get(next);
					input.get(next);
					goto endofile2;
				}
				else
					std::cout << indent << "\"" << name << "\": [" << "\n";

				//count++;
				//indent = indent + "\t";
				input.get(next);
				//parse(input, count, indent);
			endofile2:;
			}
			if (next == 't' || next == 'f')
			{
				// true and false
				bool bool_obj;
				weight++;
				if (next == 't')
					bool_obj = true;
				else
					bool_obj = false;

				getline(input, nothing, 'e');

				std::cout << std::boolalpha << indent << "\"" << name << "\"" << ": " << "\"" << bool_obj << "\"";

				input.get(next);
			}
			if (next == 'n')
			{
				// null
				weight++;
				Null null_obj;
				//null_obj = NULL;
				getline(input, nothing, ',');
				//std::cout << indent << "\"" << name << "\"" << ": " << "\"" << null_obj << "\",\n";
				input.get(next);
			}
			if (isdigit(next))		// is it next = 5 or next = "5" ???
			{
				// number
				weight++;
				//Number num_obj;
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

				//num_obj = stod(number);

				//std::cout << indent << "\"" << name << "\"" << ": " << "\"" << num_obj<< "\",\n";

				input.get(next);		// need because I inserted the comma into the cout statement that was in next
			}

			input.get(comma);		// the comma should be skipped
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
				// string
				weight++;
				std::string tmp;

				getline(input, value, '"');
				input.get(next);


				while (next != ',')
				{
					getline(input, tmp, '"');
					value = value + next;
					value = value + tmp;
					input.get(next);
				}

				String str_obj;
				str_obj.val = value;
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << str_obj.val << "\"";

			}
			if (next == '{')
			{
				// object
				weight++;
				char tmpp = input.peek();
				input.putback(tmpp);
				if (tmpp == '}')
				{
					input.get(next);
					std::cout << indent << "\"" << name << "\": {" << "\n\n" << indent << "}"; // << endl;
					input.get(next);
					input.get(next);
					goto endofile3;
				}
				else
					std::cout << indent << "\"" << name << "\"" << ": {" << "\n";

				indent = indent + "\t";
				count++;
				input.get(next);
				parse(input);
				input.get(next);
			endofile3:;

			}
			if (next == '}')	// note sure if I need this, try to put it in while loop at top
			{
				std::cout << indent << "}";
				input.get(next);
				int size2 = indent.size();
				indent = indent.substr(0, size2 - 1);
			}
			if (next == '[')
			{
				// array
				weight++;
				Array arr_obj;

				char tmppp = input.peek();
				//input.putback(tmppp);
				if (tmppp == ']')
				{
					//input.get(next);
					std::cout << indent << "\"" << name << "\": [" << "\n\n" << indent << "]";
					input.get(next);
					input.get(next);
					goto endofif4;
				}
				else
					std::cout << indent << "\"" << name << "\": [" << "\n";

				//count++;
				//indent = indent + "\t";
				input.get(next);
				//parse(input, count, indent);

			endofile4:;
			}
			if (next == 't' || next == 'f')
			{
				// true and false
				weight++;
				Bool bool_obj;

				if (next == 't')
					bool_obj = true;
				else
					bool_obj = false;

				getline(input, nothing, 'e');

				std::cout << boolalpha << indent << "\"" << name << "\"" << ": " << "\"" << bool_obj << "\"";

				input.get(next);
			}
			if (next == 'n')
			{
				// null
				weight++;
				Null null_obj;
				null_obj = NULL;
				//getline(in, nothing, ',');
				input.get(next);	// get the 'u'
				input.get(next);	// get the 'l'
				input.get(next);	// get the 'l'
				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << null_obj << "\"";
				input.get(next);
			}
			if (isdigit(next))
			{
				// number
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

				std::cout << indent << "\"" << name << "\"" << ": " << "\"" << num_obj << "\"";
			}
		}
		else if (next == '}')
		{
			char temp = input.peek();
			input.putback(temp);

			std::cout << endl << indent << "}";
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

			//input.get(next);
			return;
		}
		else if (next == ']')
		{
			std::cout << endl << indent << "]";
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

	//string indent = "";
	parse(input);

	std::cout << "\n\n";

	std::cout << "Weight: " << weight << "\n\n";



	getline(input, nothing, '{');
	input.get(quotation);
	cout << quotation << endl;

	getline(input, name, '"');
	cout << name << endl;

	getline(input, nothing, '"');
	getline(input, value, '"');
	cout << value << endl;

	vector<Value *> a;
	// if you see '\', skip the next character

	input.close();
	out.close();
	return 0;
}
