#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <math.h>

#define stoi stoll

using namespace std;

unordered_map<string, string> registers;
unordered_map<string, string> funct3;
unordered_map<string, string> funct7;
unordered_map<string, char> format;
unordered_set<string> load_operands;
unordered_map<string, int> labels;

//Code to convert binary to hexadecimal
string binary_to_hex(string binary)
{
    unordered_map<string, char> bin_to_hex;
    bin_to_hex["0000"] = '0';
    bin_to_hex["0001"] = '1';
    bin_to_hex["0010"] = '2';
    bin_to_hex["0011"] = '3';
    bin_to_hex["0100"] = '4';
    bin_to_hex["0101"] = '5';
    bin_to_hex["0110"] = '6';
    bin_to_hex["0111"] = '7';
    bin_to_hex["1000"] = '8';
    bin_to_hex["1001"] = '9';
    bin_to_hex["1010"] = 'a';
    bin_to_hex["1011"] = 'b';
    bin_to_hex["1100"] = 'c';
    bin_to_hex["1101"] = 'd';
    bin_to_hex["1110"] = 'e';
    bin_to_hex["1111"] = 'f';

    string hex = "";

    // We know string length is 32
    for (int i = 0; i < 32; i += 4)
    {
        string b = binary.substr(i, 4);
        hex.push_back(bin_to_hex[b]);
    }

    return hex;
}

//Code to convert hexadecimal to binary
string hex_to_binary(string hex)
{
    unordered_map<char, string> hex_to_bin;
    hex_to_bin['0'] = "0000";
    hex_to_bin['1'] = "0001";
    hex_to_bin['2'] = "0010";
    hex_to_bin['3'] = "0011";
    hex_to_bin['4'] = "0100";
    hex_to_bin['5'] = "0101";
    hex_to_bin['6'] = "0110";
    hex_to_bin['7'] = "0111";
    hex_to_bin['8'] = "1000";
    hex_to_bin['9'] = "1001";
    hex_to_bin['a'] = "1010";
    hex_to_bin['b'] = "1011";
    hex_to_bin['c'] = "1100";
    hex_to_bin['d'] = "1101";
    hex_to_bin['e'] = "1110";
    hex_to_bin['f'] = "1111";

    string bin = "";

    for (int i = 0; i < hex.size(); i ++)
    {
        bin += hex_to_bin[hex[i]];
    }

    return bin;
}

//Code to convert decimal to binary
string decimal_to_binary(int decimal, int no_of_bits, bool isSigned)
{
    bool isNegative = false;
    if (decimal < 0)
    {
        isNegative = true;
    }

    decimal = abs(decimal);

    string bin = "";
    while (decimal != 0)
    {
        bin.push_back(decimal % 2 + '0');
        decimal /= 2;
    }

    while (bin.size() < no_of_bits)
    {
        bin.push_back('0');
    }

    reverse(bin.begin(), bin.end());
    if (!isSigned || !isNegative)
    {
        return bin;
    }

    // Take 1's complement
    for (int i = 0; i < bin.size(); i++)
    {
        if (bin[i] == '0')
        {
            bin[i] = '1';
        }
        else
        {
            bin[i] = '0';
        }
    }

    // Add 1 to find 2's complement
    for (int i = bin.size() - 1; i >= 0; i--)
    {
        if (bin[i] == '0')
        {
            bin[i] = '1';
            break;
        }
        bin[i] = '0';
    }
    return bin;
}

//Defining all registers
void initialize_registers()
{
    for (int i = 0; i <= 31; i++)
    {
        string register_no = "x";
        register_no += to_string(i);
        string this_register = decimal_to_binary(i, 5, false);
        registers[register_no] = this_register;

        if (i == 0)
        {
            registers["zero"] = this_register;
        }
        else if (i == 1)
        {
            registers["ra"] = this_register;
        }
        else if (i == 2)
        {
            registers["sp"] = this_register;
        }
        else if (i == 3)
        {
            registers["gp"] = this_register;
        }
        else if (i == 4)
        {
            registers["tp"] = this_register;
        }
        else if (i >= 5 && i <= 7)
        {
            string temporary_register_no = "t";
            temporary_register_no += to_string(i - 5);
            registers[temporary_register_no] = this_register;
        }
        else if (i == 8)
        {
            registers["s0"] = this_register;
            registers["fp"] = this_register;
        }
        else if (i == 9)
        {
            registers["s1"] = this_register;
        }
        else if (i >= 10 && i <= 17)
        {
            string argument_register_no = "a";
            argument_register_no += to_string(i - 10);
            registers[argument_register_no] = this_register;
        }
        else if (i >= 18 && i <= 27)
        {
            string saved_register_no = "s";
            saved_register_no += to_string(i - 18 + 2);
            registers[saved_register_no] = this_register;
        }
        else if (i >= 28 && i <= 31)
        {
            string temporary_register_no = "t";
            temporary_register_no += to_string(i - 28 + 3);
            registers[temporary_register_no] = this_register;
        }
    }
}

//Defining all funct3 values
void initialize_funct3()
{
    funct3["add"] = "000";
    funct3["sub"] = "000";
    funct3["and"] = "111";
    funct3["or"] = "110";
    funct3["xor"] = "100";
    funct3["sll"] = "001";
    funct3["srl"] = "101";
    funct3["sra"] = "101";

    funct3["addi"] = "000";
    funct3["xori"] = "100";
    funct3["ori"] = "110";
    funct3["andi"] = "111";
    funct3["slli"] = "001";
    funct3["srli"] = "101";
    funct3["srai"] = "101";

    funct3["lb"] = "000";
    funct3["lh"] = "001";
    funct3["lw"] = "010";
    funct3["ld"] = "011";
    funct3["lbu"] = "100";
    funct3["lhu"] = "101";
    funct3["lwu"] = "110";

    funct3["jalr"] = "000";

    funct3["sb"] = "000";
    funct3["sh"] = "001";
    funct3["sw"] = "010";
    funct3["sd"] = "011";

    funct3["beq"] = "000";
    funct3["bne"] = "001";
    funct3["blt"] = "100";
    funct3["bge"] = "101";
    funct3["bltu"] = "110";
    funct3["bgeu"] = "111";
}

//Defining all funct7 values
void initialize_funct7()
{
    funct7["add"] = "0000000";
    funct7["sub"] = "0100000";
    funct7["and"] = "0000000";
    funct7["or"] = "0000000";
    funct7["xor"] = "0000000";
    funct7["sll"] = "0000000";
    funct7["srl"] = "0000000";
    funct7["sra"] = "0100000";
}

//Defining the format of various instructions
void initialize_format()
{
    // R format
    format["add"] = 'R';
    format["sub"] = 'R';
    format["xor"] = 'R';
    format["or"] = 'R';
    format["and"] = 'R';
    format["sll"] = 'R';
    format["srl"] = 'R';
    format["sra"] = 'R';

    // I format
    format["addi"] = 'I';
    format["xori"] = 'I';
    format["ori"] = 'I';
    format["andi"] = 'I';
    format["slli"] = 'I';
    format["srli"] = 'I';
    format["srai"] = 'I';

    format["ld"] = 'I';
    format["lw"] = 'I';
    format["lh"] = 'I';
    format["lb"] = 'I';
    format["lbu"] = 'I';
    format["lhu"] = 'I';
    format["lwu"] = 'I';
    format["jalr"] = 'I';

    // S format
    format["sb"] = 'S';
    format["sh"] = 'S';
    format["sw"] = 'S';
    format["sd"] = 'S';

    //B format
    format["beq"] = 'B';
    format["bge"] = 'B';
    format["blt"] = 'B';
    format["bne"] = 'B';
    format["bltu"] = 'B';
    format["bgeu"] = 'B';

    //J format
    format["jal"] = 'J';

    //U format
    format["lui"] = 'U';
}

//Keeping track of all laod operands 
void initialize_load_operands()
{
    load_operands.insert("lb");
    load_operands.insert("lh");
    load_operands.insert("lw");
    load_operands.insert("ld");
    load_operands.insert("lbu");
    load_operands.insert("lhu");
    load_operands.insert("lwu");
}

//Code for R format instructions
string execute_R(vector<vector<string>> &assembly_code, int instruction_no)
{
    //Handling errors
    if(assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Wrong syntax, for R type format, you must enter 4 operands" << endl;
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rd";
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs1";
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][3]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs2";
        exit(0);
    }

    //Noting the vaiours parts of the instrution
    string opcode = "0110011";
    string f3 = funct3[assembly_code[instruction_no][0]];
    string f7 = funct7[assembly_code[instruction_no][0]];
    string rd = registers[assembly_code[instruction_no][1]];
    string rs1 = registers[assembly_code[instruction_no][2]];
    string rs2 = registers[assembly_code[instruction_no][3]];

    //Concatinating the instruction
    string instr = f7 + rs2 + rs1 + f3 + rd + opcode;

    return binary_to_hex(instr);
}

//Code for I format instructions
string execute_I(vector<vector<string>> &assembly_code, int instruction_no)
{
    //Handling error
    if(assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Wrong syntax, for I type format, you must enter 4 operands" << endl;
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rd";
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs1";
        exit(0);
    }
    
    string opcode;

    //Giving the correct opcode to the corresponding instruction
    if (load_operands.find(assembly_code[instruction_no][0]) != load_operands.end())
    {
        opcode = "0000011";
    }
    else if(assembly_code[instruction_no][0] == "jalr")
    {
        opcode = "1100111";
    }
    else
    {
        opcode = "0010011";
    }

    //Noting the vaiours parts of the instrution
    string f3 = funct3[assembly_code[instruction_no][0]];
    string rd = registers[assembly_code[instruction_no][1]];
    string rs1 = registers[assembly_code[instruction_no][2]];
    string imm;

    //Assigning the immediate along with checking if it is within range
    if (assembly_code[instruction_no][0] == "srli" || assembly_code[instruction_no][0] == "slli")
    {
        if(stoi(assembly_code[instruction_no][3]) < 0 || stoi(assembly_code[instruction_no][3]) >= pow(2, 6))
        {
            cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
            exit(0);
        }

        imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 6, true);
        imm = "000000" + imm;
    }
    else if (assembly_code[instruction_no][0] == "srai")
    {
        if(stoi(assembly_code[instruction_no][3]) < 0 || stoi(assembly_code[instruction_no][3]) >= pow(2, 6))
        {
            cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
            exit(0);
        }

        imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 6, true);
        imm = "010000" + imm;
    }
    else
    {
        if(stoi(assembly_code[instruction_no][3]) < -pow(2, 11) || stoi(assembly_code[instruction_no][3]) >= pow(2, 11))
        {
            cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
            exit(0);
        }

        imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 12, true);
    }

    //Concatinating the values
    string instr = imm + rs1 + f3 + rd + opcode;

    return binary_to_hex(instr);
}

//Code for S format instructions
string execute_S(vector<vector<string>> &assembly_code, int instruction_no)
{
    //Handling error
    if(assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Wrong syntax, for S type format, you must enter 4 operands" << endl;
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs2";
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs1";
        exit(0);
    }
    else if(stoi(assembly_code[instruction_no][3]) < -pow(2, 11) || stoi(assembly_code[instruction_no][3]) >= pow(2, 11))
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
        exit(0);
    }

    //Noting the various parts of the instruction
    string opcode = "0100011";
    string f3 = funct3[assembly_code[instruction_no][0]];
    string rs1 = registers[assembly_code[instruction_no][2]];
    string rs2 = registers[assembly_code[instruction_no][1]];
    string imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 12, true);

    //Concatinating the various parts
    string instr = imm.substr(0, 7) + rs2 + rs1 + f3 + imm.substr(7) + opcode;

    return binary_to_hex(instr);
}

//Code for B format instructions
string execute_B(vector<vector<string>> &assembly_code, int instruction_no)
{
    //Handling error
    if(assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Wrong syntax, for B type format, you must enter 4 operands" << endl;
        exit(0);
    }
    if(labels.find(assembly_code[instruction_no][3]) == labels.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Label not defined";
        exit(0);
    }
    else
    {
        assembly_code[instruction_no][3] = to_string(labels[assembly_code[instruction_no][3]] - instruction_no * 4);
    }
    if(registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs1";
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rs2";
        exit(0);
    }
    else if(stoi(assembly_code[instruction_no][3]) < -pow(2, 12) || stoi(assembly_code[instruction_no][3]) >= pow(2, 12) - 1)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
        exit(0);
    }

    //Noting the various parts of the instruction
    string opcode = "1100011";
    string f3 = funct3[assembly_code[instruction_no][0]];
    string rs1 = registers[assembly_code[instruction_no][1]];
    string rs2 = registers[assembly_code[instruction_no][2]];
    string imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 13, true);

    //Concatinating the parts to form the instruction 
    string instr = imm.substr(0, 1) + imm.substr(2, 6) + rs2 + rs1 + f3 + imm.substr(8, 4) + imm.substr(1, 1) + opcode;

    return binary_to_hex(instr);
}

//Code for J format instructions
string execute_J(vector<vector<string>> &assembly_code, int instruction_no)
{
    //Handling error
    if(assembly_code[instruction_no].size() != 3)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Wrong syntax, for J type format, you must enter 3 operands" << endl;
        exit(0);
    }
    if(labels.find(assembly_code[instruction_no][2]) == labels.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Label not defined";
        exit(0);
    }
    else
    {
        assembly_code[instruction_no][2] = to_string(labels[assembly_code[instruction_no][2]] - instruction_no * 4);
    }

    if(registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rd";
        exit(0);
    }
    else if(stoi(assembly_code[instruction_no][2]) < -1 * pow(2, 20) || stoi(assembly_code[instruction_no][2]) >= pow(2, 20) - 1)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
        exit(0);
    }

    //Noting the various parts of the instrcution
    string opcode = "1101111";
    string rd = registers[assembly_code[instruction_no][1]];
    string imm = decimal_to_binary(stoi(assembly_code[instruction_no][2]), 21, true);

    //Concatinating the instruction
    string instr = imm.substr(0, 1) + imm.substr(10, 10) + imm.substr(9, 1) + imm.substr(1, 8) + rd + opcode;

    return binary_to_hex(instr);
}

//Code for U format instructions
string execute_U(vector<vector<string>> &assembly_code, int instruction_no)
{
    //Handling error
    if(assembly_code[instruction_no].size() != 3)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Wrong syntax, for U type format, you must enter 3 operands" << endl;
        exit(0);
    }
    else if(registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Incorrect value of rd";
        exit(0);
    }
    else if(stoi(assembly_code[instruction_no][2]) < 0 || stoi(assembly_code[instruction_no][2]) > 4294963200)
    {
        cout << "Error in line: " << instruction_no + 1 << endl << "Value of immediate is out of range";
        exit(0);
    }

    //Noting the vaious parts of the instrction
    string opcode = "0110111";
    string rd = registers[assembly_code[instruction_no][1]];
    string bin = decimal_to_binary(stoi(assembly_code[instruction_no][2]), 20, true);
    string imm = bin;

    //Concatinating the instruction
    string instr = imm + rd + opcode;

    return binary_to_hex(instr);
}

int main()
{
    //Initialze all the necessary functions/registers/formats before starting
    initialize_registers();
    initialize_funct3();
    initialize_funct7();
    initialize_format();
    initialize_load_operands();

    //Open the file
    ifstream input;
    input.open("input.s");
    if (input.fail())
    {
        cout << "Error: File failed to open\n";
        return 1; // Indicate that an error occured
    }

    string instruction;                     //To keep track of line-by-line input
    vector<vector<string>> assembly_code;   //To store the input operands

    //iteration 1
    int offset = 0;
    int i = 0;
    while (getline(input, instruction))
    {
        //If the first is ;, just ignore this line
        if(instruction[0] == ';')
        {
            continue;
        }

        //Seperate the instruction into registers/immediate value/opcode and so on
        vector<string> cur_instruction;
        string cur = "";
        for (int i = 0; i < instruction.size(); i++)
        {
            if (instruction[i] != ' ' && instruction[i] != '(')
            {
                cur.push_back(instruction[i]);
            }
            else
            {
                if(cur[cur.size() - 1] == ':')
                {
                    cur.pop_back();
                    labels[cur] = offset;
                    cur = "";
                    continue;
                }
                
                if (cur[cur.size() - 1] == ',')
                {
                    cur.pop_back();
                }

                cur_instruction.push_back(cur);
                cur = "";
            }
        }
        offset += 4;            //After going through this instruction, ensure that you go to the next line
        
        if (cur != "")
        {
            if (cur[cur.size() - 1] == ',' || cur[cur.size() - 1] == ')')
            {
                cur.pop_back();
            }
            cur_instruction.push_back(cur);
        }

        //If in wrong position, bring the operands back to the correct position in the matrix
        if (load_operands.find(cur_instruction[0]) != load_operands.end() || format[cur_instruction[0]] == 'S' || cur_instruction[0] == "jalr")
        {
            swap(cur_instruction[2], cur_instruction[3]);
        }

        assembly_code.push_back(cur_instruction);
    }
    input.close();      //Close the input file

    // Iteration 2

    //Open the output file
    ofstream output;
    output.open("output.hex");

    //Iterate through the instructions and store them in the output file
    for (int instruction_no = 0; instruction_no < assembly_code.size(); instruction_no++)
    {
        if (format[assembly_code[instruction_no][0]] == 'R')
        {
            output << execute_R(assembly_code, instruction_no) << endl;
        }
        else if (format[assembly_code[instruction_no][0]] == 'I')
        {
            output << execute_I(assembly_code, instruction_no) << endl;
        }
        else if (format[assembly_code[instruction_no][0]] == 'S')
        {
            output << execute_S(assembly_code, instruction_no) << endl;
        }
        else if(format[assembly_code[instruction_no][0]] == 'B')
        {
            output << execute_B(assembly_code, instruction_no) << endl;
        }
        else if(format[assembly_code[instruction_no][0]] == 'J')
        {
            output << execute_J(assembly_code, instruction_no) << endl;
        }
        else if(format[assembly_code[instruction_no][0]] == 'U')
        {
            output << execute_U(assembly_code, instruction_no) << endl;
        }
        else
        {
            cout << "Error on line: " << instruction_no + 1 << endl << "Incorrect syntax";
            exit(0);
        }
    }
    output.close();         //Close the output file
    return 0;
}