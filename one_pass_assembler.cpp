#include<iostream>
#include<vector>
#include<string>
#include<map>
#include<stack>
#include<queue>
#include<fstream>
#include<algorithm>
#include<sstream>
#include<math.h>

using namespace std;

class Assembler
{
	int locctr, starting_address, obj_rec_no, obj_address;
	map<string, vector<int> > symtab;
	map<int, vector<string> > obj_rec;
	map<string, string> optab;
	string header_rec, src_file_name, symtab_file_name, end_rec, program_name, optab_file_name, object_file_name;

public:
	Assembler(string sc, string symtab, string op_tab, string obj);
	void generate_object_code();
	string convert_to_hexa(int a);
	int convert_to_decimal(string a);
	vector<string> get_vector_from_line(string line);
	void generate_optab();
	string add_chars(string line, char a, int l, int g);
	void add_entry(string a, int k);
	int return_int(char a);
	int get_integer(string a);
	void display_object_records();
	void display_object_code();
	void display_source_code();
	void display_symtab();
	void correct_object_code(vector<int> locs);
	void display_optab();
	void write_object_code();
	void write_symtab();
};

void Assembler :: add_entry(string a, int k)
{
	int z = 0;
	if(obj_rec[obj_rec_no].size() >= 11)
	{
		z = 1;
		obj_rec_no++;
	}
	if(obj_rec[obj_rec_no].size() == 0)
		obj_rec[obj_rec_no].push_back(convert_to_hexa(locctr));
	obj_rec[obj_rec_no].push_back(a);
	if(z == 0 && k == 1)
		obj_rec_no++;
	obj_address += 3;
	return;
}

void Assembler :: correct_object_code(vector<int> locs)
{
	int i = 0, j = 0, k = 1;
	while(i <= obj_rec_no && k < locs.size())
	{
		int t = 1;
		if(obj_rec[i].size() == 0)
			break;
		while(t < obj_rec[i].size() && k < locs.size())
		{
			if(j == locs.at(k))
			{
				if((obj_rec[i].at(t)).length() == 3)
				{
					string temp = obj_rec[obj_rec_no].at(t);
					(obj_rec[i])[t] = temp.substr(0, temp.length() - 1) + add_chars(convert_to_hexa((int)pow(2, 15) + locctr), '0', 4, 0);					
				}
				else
				{
					(obj_rec[i])[t] = (obj_rec[i]).at(t) + add_chars(convert_to_hexa(locctr), '0', 4, 0);
				}
				k++;
			}
			t++;
			j += 3;
		}
		i++;
	}
	return;
}
void Assembler :: generate_object_code()
{
	ifstream sc(src_file_name.c_str());
	int k = 0;
	for(string line; getline(sc, line);)
	{
		vector<string> instr = get_vector_from_line(line);
		if(k == 0)
		{
			k = 1;
			if(instr.size() == 3)
			{
				if(instr[1].compare(string("START")) == 0)
				{
					locctr = get_integer(instr[2]);
					starting_address = locctr;
					program_name = instr[0];
					continue;
				}
				else
				{
					locctr = 0;
					starting_address = 0;
				}
			}
			else
			{
				locctr = 0;
				starting_address = 0;
			}
		}
		if(instr[0].compare(string(".")) == 0)
			continue;
		if(instr.size() == 1)
		{
			if(instr[0].compare(string("END")) == 0)
				break;
			if(optab.find(instr[0]) == optab.end())
			{
				cout<<"Opcode not found in the optab"<<endl;
				exit(0);
			}
			else
			{
				add_entry(optab[instr[0]] + string("0000"), 1);
				obj_rec_no++;
				locctr += 3;
			}
		}
		else if(instr.size() < 1 || instr.size() > 3)
		{
			cout<<"Incorrect Instruction Format"<<endl;
			exit(0);
		}
		else
		{
			string label, opcode, operand, temp;
			temp = string("");
//			cout<<"The size of instr is "<<instr.size()<<endl;
			if(instr.size() == 2)
			{
				opcode = instr[0];
				operand = instr[1];
			}
			else if (instr.size() == 3)
			{
				label = instr[0];
				opcode = instr[1];
				operand = instr[2];
			}
			if(instr.size() == 3)
			{
				if(symtab.find(label) == symtab.end())
				{
					symtab[label].push_back(locctr);
				}
				else if (symtab[label].size() > 1)
				{
					correct_object_code(symtab[label]);
					symtab[label].clear();
					symtab[label].push_back(locctr);
				}
			}
			if (optab.find(opcode) != optab.end())
			{
				int m = 0;
				temp = optab[opcode];
				if(operand.find(',') != string::npos)
				{
					m = 1;
					operand = operand.substr(0, operand.find(','));
				}
				if(symtab.find(operand) == symtab.end())
				{
					symtab[operand].push_back(-1);
					symtab[operand].push_back(obj_address);
					if (m == 1)
						add_entry(temp + string(1, '1'), 0);
					else
						add_entry(temp, 0);						
				}
				else if (symtab[operand].size() == 1)
				{
					if(m == 0)
						temp += add_chars(convert_to_hexa(symtab[operand][0]), '0', 4, 0);
					else
						temp += add_chars(convert_to_hexa(symtab[operand][0] + (int)pow(2, 15)), '0', 4, 0);						
					add_entry(temp, 0);
				}
				else if(symtab[operand].size() > 1)
				{
					symtab[operand].push_back(obj_address);
					if (m == 0)
						add_entry(temp, 0);
					else
						add_entry(temp + string(1, '1'), 0);						
				}
				locctr += 3;
			}
			else
			{
				if(opcode.compare(string("WORD")) == 0)
				{
					locctr += 3;
				}
				else if (opcode.compare(string("RESW")) == 0)
				{
					locctr += 3*(get_integer(operand));
				}
				else if (opcode.compare(string("BYTE")) == 0)
				{
					locctr += operand.length() - 3;
				}
				else if (opcode.compare(string("RESB")) == 0)
				{
					locctr += get_integer(operand);
				}
				else
				{
					cout<<"Incorrect Opcode"<<endl;
					exit(0);
				}
			}
		}
//		display_symtab();
	}
//	display_object_code();
	write_symtab();
	write_object_code();
}

void Assembler :: write_object_code()
{
	ofstream fout(object_file_name.c_str());
	fout<<"H^"<<add_chars(program_name, ' ', 6, 1)<<"^"<<add_chars(convert_to_hexa(starting_address), '0', 6, 0)<<"^"<<add_chars(convert_to_hexa(obj_address/3 - 1), '0', 6, 0)<<endl;
	for(int i = 0; i <= obj_rec_no; i++)
	{
		if(obj_rec[i].size() == 0)
			break;
		vector<string> temp = obj_rec[i];
		fout<<"T^"<<add_chars(obj_rec[i][0], '0', 6, 0)<<"^"<<add_chars(convert_to_hexa((obj_rec[i].size() - 1)*3), '0', 2, 0)<<"^";
		for(int j = 1; j < obj_rec[i].size(); j++)
		{
			if(j != obj_rec[i].size() - 1)
				fout<<obj_rec[i][j]<<"^";
			else
				fout<<obj_rec[i][j]<<endl;
		}
	}
	fout<<"E^"<<add_chars(convert_to_hexa(starting_address), '0', 6, 0)<<endl;
	fout.close();
	return;
}

void Assembler :: write_symtab()
{
	ofstream fout(symtab_file_name.c_str());
	map<string, vector<int> >::iterator it;
	for(it = symtab.begin(); it != symtab.end(); ++it)
	{
		fout<<it->first<<" : "<<(it->second)[0]<<endl;
	}
	fout.close();
	return;
}

void Assembler :: display_object_records()
{
	cout<<"Displaying the object code which has been generated"<<endl;
	for(int i = 0; i <= obj_rec_no; i++)
	{
		if(obj_rec[i].size() == 0)
			break;
		vector<string> object_codes = obj_rec[i];
		cout<<"Starting address for the record is "<<object_codes[0]<<endl;
		for(int j = 1; j< object_codes.size(); j++)
		{
			cout<<object_codes[j]<<" ";
		}
		cout<<endl;
	}
	return;
}

void Assembler :: display_symtab()
{
	map<string, vector<int> >::iterator it;
	for(it = symtab.begin(); it!=symtab.end(); ++it)
	{
		vector<int> temp= it->second;
		if(temp.size() == 1)
		{
			cout<<it->first<<" : "<<temp[0]<<endl;
		}
		else
		{
			cout<<"The records for "<<it->first<<" to be changed at "<<endl;
			for(int j = 1; j <temp.size(); j++)
			{
				cout<<temp[j]<<" ";
			}
			cout<<endl;
		}
	}
	return;
}

void Assembler :: display_optab()
{
	map<string, string> :: iterator it;
	for(it = optab.begin(); it!=optab.end(); ++it)
	{
		cout<<it->first<<" : "<<it->second<<endl;
	}
	return;
}

int Assembler :: return_int(char a)
{
	if (a == '0')
		return 0;
	else if (a == '1')
		return 1;
	else if (a == '2')
		return 2;
	else if (a == '3')
		return 3;
	else if (a == '4')
		return 4;
	else if (a == '5')
		return 5;
	else if (a == '6')
		return 6;
	else if (a == '7')
		return 7;
	else if (a == '8')
		return 8;
	else 
		return 9;
}

int Assembler :: get_integer(string a)
{
	int i;
	int t=0;
	char p;
	for(i=0; i<a.length(); i++)
	{
		t = (t*10) + return_int(a[i]);
	}
	return t;
}

string Assembler :: add_chars(string line, char a, int l, int g)
{
	int p = line.length();
	if(g == 0)
		for(int i = 0; i < l - p; i++)
			line = string(1, a) + line;
	else
		for(int i = 0; i < l - p; i++)
			line += string(1, a);
	return line;
}

vector<string> Assembler :: get_vector_from_line(string line)
{
	string element;
	vector<string> sc_line;
	for(int i = 0; i < line.length(); i++)
	{
		if(line[i] == ' ' && element.length() != 0)
		{
			sc_line.push_back(element);
			element.clear();
		}
		else if (line[i] != ' ')
		{
			element += string(1, line[i]);
		}
	}
	if(element.length() != 0)
	{
		sc_line.push_back(element);
	}
	return sc_line;
}

void Assembler :: generate_optab()
{
	ifstream sc(optab_file_name.c_str());

	for(string line; getline(sc, line);)
	{
		vector<string> instr = get_vector_from_line(line);
		optab[instr[0]] += instr[1];
	}
	sc.close();
	return;
}

void Assembler :: display_source_code()
{
	ifstream ifs(src_file_name.c_str());
	for(string line; getline(ifs, line);)
		cout<<line<<endl;
	return;
}

void Assembler :: display_object_code()
{
	ifstream ifs(object_file_name.c_str());
	for(string line; getline(ifs, line);)
		cout<<line<<endl;
	return;
}

Assembler::Assembler(string sc, string symtab, string op_tab, string obj)
{
	obj_address = 0;
	locctr = -1;
	src_file_name = sc;
	symtab_file_name = symtab;
	starting_address = -1;
	obj_rec_no = 0;
	optab_file_name = op_tab;
	object_file_name = obj;
	program_name = string("DEFAULT");
	generate_optab();
}

string Assembler :: convert_to_hexa(int a)
{
	stringstream ss;
	ss<<hex<<a;
	string hex_value;
	hex_value = ss.str();
	transform(hex_value.begin(), hex_value.end(), hex_value.begin(), ::toupper);
	return hex_value;
}

int Assembler :: convert_to_decimal(string a)
{
	stringstream ss;
	ss<<a;
	int decimal_value;
	ss>>hex>>decimal_value;
	return decimal_value;
}

int main()
{
	string optab_fname, sc_fname, symtab_fname, int_fname, object_fname;
	int i, j;
	while (1)
	{
		cout<<"1 TO INPUT NEW PROGRAM\n";
		cout<<"2 TO EXIT\n";
		cin>>i;
		cin.ignore();
		switch(i)
		{
			case 1:
			{
				cout<<"OPTAB FILE NAME?\n";
				getline(cin, optab_fname);
				cout<<"SOURCE CODE FILE NAME\n";
				getline(cin, sc_fname);
				cout<<"FILENAME WHERE SYMTAB IS TO BE STORED\n";
				getline(cin, symtab_fname);
				cout<<"OBJECT CODE FILE NAME\n";
				getline(cin, object_fname);
				Assembler prog(sc_fname, symtab_fname, optab_fname, object_fname);
				cout<<"1 TO DISPLAY OPTAB ELSE ENTER 0\n";
				cin>>j;
				cin.ignore();
				cout<<"\n";
				if(j == 1)
				{
					prog.display_optab();
					cout<<endl;
				}
				cout<<"1 TO DISPLAY SOURCE CODE ELSE ENTER 0\n";
				cin>>j;
				cin.ignore();
				cout<<"\n";
				if(j == 1)
				{
					prog.display_source_code();
					cout<<endl;
				}
				prog.generate_object_code();
				cout<<"1 TO DISPLAY SYMTAB ELSE ENTER 0\n";
				cin>>j;
				cin.ignore();
				cout<<"\n";
				if(j == 1)
				{
					prog.display_symtab();
					cout<<endl;
				}
				cout<<"1 TO DISPLAY OBJECT CODE\n";
				cin>>j;
				cin.ignore();
				cout<<"\n";
				if(j == 1)
				{
					prog.display_object_code();
					cout<<endl;
				}
				cout<<"ALL DATA STRUTCTURES AND CORRESPONDING FILES TO THE PROGRAM STORED UNDER CORRESPONDING FILE NAMES RECIEVED AS INPUT\n\n";
				break;
			}
			case 2:
			{
				return 0;
			}
			default:
				cout<<"Enter a valid choice\n";
		}
	}
	return 0;
}