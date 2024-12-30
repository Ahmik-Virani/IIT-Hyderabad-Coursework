#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <math.h>
#include <sstream>
#include <stack>

//The following ensure that 64 bits can be handled by our code
#define stoi stoll
#define int long long

using namespace std;

// Declare the memory
int memory[0x50001];

// Initialize the value of PC
int PC;

//Declare the various maps which will store the necessary information
unordered_map<string, string> registers;
unordered_map<string, string> funct3;
unordered_map<string, string> funct7;
unordered_map<string, char> format;
unordered_set<string> load_operands;
unordered_map<string, int> labels;
unordered_map<string, int64_t> register_values;
unordered_map<string, string> registers_alis;

vector<string> all_instructions;
unordered_map<int, string> offset_to_label;
unordered_map<string, int> last_exected_line;
stack<pair<string, int>> stack_of_functions;
unordered_set<int> break_lines;
int count_how_many_to_skip;

// Code to convert binary to hexadecimal
string binary_to_hex(string binary, bool trim=false, int length=8)
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

    for (int i = 0; i < binary.size(); i += 4)
    {
        string b = binary.substr(i, 4);
        hex.push_back(bin_to_hex[b]);
    }

    if(length * 4 > binary.size())
    {
        string zeros = "", fs = "";
        for(int i = length * 4 ; i < binary.size() ; i++)
        {
            zeros.push_back('0');
            fs.push_back('f');
        }

        if(hex[0] < '8' && hex[0] >= '0')
        {
            hex = zeros + hex;
        }
        else
        {
            hex = fs + hex;
        }
    }

    if(length * 4 < binary.size())
    {
        hex = hex.substr(binary.size() - length * 4);
    }

    if(trim)
    {
        string trimmed_hex = "";

        for(int i = 0 ; i < hex.size() ; i++)
        {
            if(hex[i] == '0')
            {
                continue;
            }
            else
            {
                trimmed_hex = hex.substr(i);
                return trimmed_hex;
            }
        }
        return "0";
    }

    return hex;
}

// Code to convert hexadecimal to binary
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

    for (int i = 0; i < hex.size(); i++)
    {
        bin += hex_to_bin[hex[i]];
    }

    return bin;
}

// Code to convert decimal to binary
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

// Function to convert binary string to decimal
int binary_to_decimal(const string &bin)
{
    int decimal = 0;
    int length = bin.size();

    // Iterate over the binary string
    for (int i = 0; i < length; i++)
    {
        // If the current character is '1', add the corresponding power of 2
        if (bin[i] == '1')
        {
            decimal += pow(2, length - 1 - i); // length - 1 - i gives the position from the right
        }
    }

    return decimal;
}

// Defining all registers
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

// Defining all funct3 values
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

// Defining all funct7 values
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

// Defining the format of various instructions
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

    // B format
    format["beq"] = 'B';
    format["bge"] = 'B';
    format["blt"] = 'B';
    format["bne"] = 'B';
    format["bltu"] = 'B';
    format["bgeu"] = 'B';

    // J format
    format["jal"] = 'J';

    // U format
    format["lui"] = 'U';
}

// Keeping track of all laod operands
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

//Whenever a new file is loaded, this will handle initializing any used variables that the previous file might have used
void initialize()
{
    for (int i = 0; i < 32; i++)
    {
        string reg = "x" + to_string(i);
        register_values[reg] = 0;
    }

    int PC = 0x00000;

    for (int i = 0x0; i < 0x50001; i++)
    {
        memory[i] = 0x00;
    }

    labels.clear();
    register_values.clear();
    all_instructions.clear();
    offset_to_label.clear();
    while (!stack_of_functions.empty())
    {
        stack_of_functions.pop();
    }
    stack_of_functions.push({"main", 0});
    last_exected_line.clear();
    break_lines.clear();

    count_how_many_to_skip = 0;
}

string aliasRemover(string r)
{
    if (r == "zero" || r == "x0")
    {
        r = "x0";
    }
    else if (r == "ra" || r == "x1")
    {
        r = "x1";
    }
    else if (r == "sp" || r == "x2")
    {
        r = "x2";
    }
    else if (r == "gp" || r == "x3")
    {
        r = "x3";
    }
    else if (r == "tp" || r == "x4")
    {
        r = "x4";
    }
    else if (r == "t0" || r == "x5")
    {
        r = "x5";
    }
    else if (r == "t1" || r == "x6")
    {
        r = "x6";
    }
    else if (r == "t2" || r == "x7")
    {
        r = "x7";
    }
    else if (r == "s0" || r == "fp" || r == "x8")
    {
        r = "x8";
    }
    else if (r == "s1" || r == "x9")
    {
        r = "x9";
    }
    else if (r == "a0" || r == "x10")
    {
        r = "x10";
    }
    else if (r == "a1" || r == "x11")
    {
        r = "x11";
    }
    else if (r == "a2" || r == "x12")
    {
        r = "x12";
    }
    else if (r == "a3" || r == "x13")
    {
        r = "x13";
    }
    else if (r == "a4" || r == "x14")
    {
        r = "x14";
    }
    else if (r == "a5" || r == "x15")
    {
        r = "x15";
    }
    else if (r == "a6" || r == "x16")
    {
        r = "x16";
    }
    else if (r == "a7" || r == "x17")
    {
        r = "x17";
    }
    else if (r == "s2" || r == "x18")
    {
        r = "x18";
    }
    else if (r == "s3" || r == "x19")
    {
        r = "x19";
    }
    else if (r == "s4" || r == "x20")
    {
        r = "x20";
    }
    else if (r == "s5" || r == "x21")
    {
        r = "x21";
    }
    else if (r == "s6" || r == "x22")
    {
        r = "x22";
    }
    else if (r == "s7" || r == "x23")
    {
        r = "x23";
    }
    else if (r == "s8" || r == "x24")
    {
        r = "x24";
    }
    else if (r == "s9" || r == "x25")
    {
        r = "x25";
    }
    else if (r == "s10" || r == "x26")
    {
        r = "x26";
    }
    else if (r == "s11" || r == "x27")
    {
        r = "x27";
    }
    else if (r == "t3" || r == "x28")
    {
        r = "x28";
    }
    else if (r == "t4" || r == "x29")
    {
        r = "x29";
    }
    else if (r == "t5" || r == "x30")
    {
        r = "x30";
    }
    else if (r == "t6" || r == "x31")
    {
        r = "x31";
    }

    return r;
}

//Code for the R format step instructions
void step_R(vector<string> instruction)
{
    //Extract rd, rs1, rs2
    string inst = instruction[0];
    string rd = aliasRemover(instruction[1]);
    string rs1 = aliasRemover(instruction[2]);
    string rs2 = aliasRemover(instruction[3]);

    //Do the arithmetic according to the instructions
    if (inst == "add")
    {
        register_values[rd] = register_values[rs1] + register_values[rs2];
    }
    else if (inst == "sub")
    {
        register_values[rd] = register_values[rs1] - register_values[rs2];
    }
    else if (inst == "and")
    {
        register_values[rd] = register_values[rs1] & register_values[rs2];
    }
    else if (inst == "or")
    {
        register_values[rd] = register_values[rs1] | register_values[rs2];
    }
    else if (inst == "xor")
    {
        register_values[rd] = register_values[rs1] ^ register_values[rs2];
    }
    else if (inst == "sll")
    {
        register_values[rd] = register_values[rs1] << register_values[rs2];
    }
    else if (inst == "srl")
    {
        uint64_t shifting_amount = register_values[rs2];
        register_values[rd] = register_values[rs1] >> register_values[rs2];

        // Our mask is shifting_amount 0's followed by 64-shifting_amount 1's
        uint64_t mask = (1U << (64 - shifting_amount)) - 1;

            if(shifting_amount == 0)
            {
                for(int i = 0 ; i <= 63 ; i++)
                {
                    mask += pow(2, i);
                }
            }

        register_values[rd] = register_values[rd] & mask;
    }

    else if (inst == "sra")
    {
        register_values[rd] = register_values[rs1] >> register_values[rs2];
    }

    //Update the stack of functions and their last executed line accordingly
    string top_of_stack = stack_of_functions.top().first;
    int top_line = stack_of_functions.top().second;
    top_line = PC / 4 + 1;
    stack_of_functions.pop();
    stack_of_functions.push({top_of_stack, top_line});

    string executed_instruction = all_instructions[PC / 4];
    int pos = executed_instruction.find(' ');
    string first_word = executed_instruction.substr(0, pos);
    if (first_word.back() == ':')
    {
        executed_instruction = executed_instruction.substr(pos + 1);
    }
    string bin_PC = decimal_to_binary(PC, 32, 0);
    string hex_PC = binary_to_hex(bin_PC);
    hex_PC = "0x" + hex_PC;

    cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

    //Update the PC value and ensure that x0 will always remain at value of 0
    PC += 4;
    register_values["x0"] = 0;
}

//Code for the I format step instructions
void step_I(vector<string> instruction)
{
    string rd, rs1, inst;
    inst = instruction[0];
    int imm;

    //Perform load as per the instruction
    if (inst == "lb" || inst == "lh" || inst == "lw" || inst == "ld" || inst == "lbu" || inst == "lhu" || inst == "lwu")
    {
        //Extract rd, rs1, and immediate
        rd = aliasRemover(instruction[1]);
        rs1 = aliasRemover(instruction[2]);
        imm = stoi(instruction[3]);
        // We assume that rs1 always stores addresses in form of integer
        int start_location = imm + register_values[rs1];
        if (inst == "lb")
        {
            int8_t byte = memory[start_location];
            int8_t res = byte;
            register_values[rd] = res;
            rd = res;
        }

        else if (inst == "lh")
        {
            int16_t byte0 = memory[start_location];
            int16_t byte1 = memory[start_location + 1];
            int16_t res = byte1 << 8 | byte0;
            register_values[rd] = res;
        }

        else if (inst == "lw")
        {
            vector<int32_t> byte(4);
            for (int i = 0; i < 4; i++)
            {
                byte[i] = memory[start_location + i];
            }
            int32_t res = (byte[3] << 24) | (byte[2] << 16) | (byte[1] << 8) | (byte[0]);
            register_values[rd] = res;
        }

        else if (inst == "ld")
        {
            vector<int64_t> byte(8);
            for (int i = 0; i < 8; i++)
            {
                byte[i] = memory[start_location + i];
            }
            int64_t res = (byte[7] << 56) | (byte[6] << 48) | (byte[5] << 40) | (byte[4] << 32) | (byte[3] << 24) | (byte[2] << 16) | (byte[1] << 8) | byte[0];
            register_values[rd] = res;
        }
        else if (inst == "lbu")
        {
            uint8_t byte = memory[start_location];
            uint8_t res = byte;
            register_values[rd] = res;
        }
        else if (inst == "lhu")
        {
            uint16_t byte0 = memory[start_location];
            uint16_t byte1 = memory[start_location + 1];
            uint16_t res = byte1 << 8 | byte0;
            register_values[rd] = res;
        }
        else if (inst == "lwu")
        {
            vector<uint32_t> byte(4);
            for (int i = 0; i < 4; i++)
            {
                byte[i] = memory[start_location + i];
            }
            uint32_t res = byte[3] << 24 | byte[2] << 16 | byte[1] << 8 | byte[0];
            register_values[rd] = res;
        }

        //Update the stack of functions and their last executed line accordingly
        string top_of_stack = stack_of_functions.top().first;
        int top_line = stack_of_functions.top().second;
        top_line = PC / 4 + 1;
        stack_of_functions.pop();
        stack_of_functions.push({top_of_stack, top_line});

        string executed_instruction = all_instructions[PC / 4];
        int pos = executed_instruction.find(' ');
        string first_word = executed_instruction.substr(0, pos);
        if (first_word.back() == ':')
        {
            executed_instruction = executed_instruction.substr(pos + 1);
        }
        string bin_PC = decimal_to_binary(PC, 32, 0);
        string hex_PC = binary_to_hex(bin_PC);
        hex_PC = "0x" + hex_PC;

        cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

        //Update the PC accordingly
        PC += 4;
    }
    else if (inst == "addi" || inst == "xori" || inst == "ori" || inst == "andi" || inst == "srai" || inst == "slli" || inst == "srli")
    {
        //Extract the rd, rs1, imm values
        rd = aliasRemover(instruction[1]);
        rs1 = aliasRemover(instruction[2]);
        imm = stoi(instruction[3]);

        //Perform the arithmetic as per the instruction
        if (inst == "addi")
        {
            register_values[rd] = register_values[rs1] + imm;
        }
        else if (inst == "xori")
        {
            register_values[rd] = register_values[rs1] ^ imm;
        }
        else if (inst == "ori")
        {
            register_values[rd] = register_values[rs1] | imm;
        }
        else if (inst == "andi")
        {
            register_values[rd] = register_values[rs1] & imm;
        }
        else if (inst == "slli")
        {
            register_values[rd] = register_values[rs1] << imm;
        }
        else if (inst == "srli")
        {
            uint32_t shifting_amount = imm;
            register_values[rd] = register_values[rs1] >> imm;

            // Our mask is shifting_amount 0's followed by 64-shifting_amount 1's
            uint64_t mask = (1U << (64 - shifting_amount)) - 1;

            if(shifting_amount == 0)
            {
                for(int i = 0 ; i <= 63 ; i++)
                {
                    mask += pow(2, i);
                }
            }

            register_values[rd] = register_values[rd] & mask;
            register_values[rd] &= mask;
        }
        else if (inst == "srai")
        {
            register_values[rd] = register_values[rs1] >> imm;
        }

        //Update the stack of functions and their last executed line accordingly
        string top_of_stack = stack_of_functions.top().first;
        int top_line = stack_of_functions.top().second;
        top_line = PC / 4 + 1;
        stack_of_functions.pop();
        stack_of_functions.push({top_of_stack, top_line});

        string executed_instruction = all_instructions[PC / 4];
        int pos = executed_instruction.find(' ');
        string first_word = executed_instruction.substr(0, pos);
        if (first_word.back() == ':')
        {
            executed_instruction = executed_instruction.substr(pos + 1);
        }
        string bin_PC = decimal_to_binary(PC, 32, 0);
        string hex_PC = binary_to_hex(bin_PC);
        hex_PC = "0x" + hex_PC;

        cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

        //Update PC value
        PC += 4;
    }
    else if (inst == "jalr")
    {
        //Extract the rd, rs1 and immediate value
        rd = aliasRemover(instruction[1]);
        rs1 = aliasRemover(instruction[2]);
        imm = stoi(instruction[3]);

        //Update the stack of functions and their last executed line accordingly
        string top_of_stack = stack_of_functions.top().first;
        int top_line = stack_of_functions.top().second;
        top_line = PC / 4 + 1;
        stack_of_functions.pop();
        stack_of_functions.push({top_of_stack, top_line});

        string executed_instruction = all_instructions[PC / 4];
        int pos = executed_instruction.find(' ');
        string first_word = executed_instruction.substr(0, pos);
        if (first_word.back() == ':')
        {
            executed_instruction = executed_instruction.substr(pos + 1);
        }
        string bin_PC = decimal_to_binary(PC, 32, 0);
        string hex_PC = binary_to_hex(bin_PC);
        hex_PC = "0x" + hex_PC;

        cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

        //Ensure to remove the called function from the stack of functions
        stack_of_functions.pop();

        //Update the destination address and value of PC accordingly
        register_values[rd] = PC + 4;
        PC = imm + register_values[rs1];
    }

    //Ensure that x0 values remains at 0
    register_values["x0"] = 0;
}

//Code for the S format step instructions
void step_S(vector<string> instruction)
{
    //Extract the rs1, rs2 and immediate values
    string rs1, rs2, inst;
    int imm;
    inst = instruction[0];
    rs2 = aliasRemover(instruction[1]);
    imm = stoi(instruction[3]);

    rs1 = aliasRemover(instruction[2]);
    int mem_loc = imm + register_values[rs1];
    uint64_t store_value = register_values[rs2];

    //Perform store as per the instruction
    if (inst == "sb")
    {
        uint8_t byte = store_value;
        memory[mem_loc] = byte;
    }
    if (inst == "sh")
    {
        vector<uint8_t> byte(2);
        byte[0] = store_value;
        byte[1] = store_value >> 8;

        memory[mem_loc] = byte[0];
        memory[mem_loc + 1] = byte[1];
    }
    if (inst == "sw")
    {
        vector<uint8_t> byte(4);
        for (int i = 0; i < 4; i++)
        {
            byte[3 - i] = store_value >> (3 - i) * 8;
        }
        for (int i = 0; i < 4; i++)
        {
            memory[mem_loc + i] = byte[i];
        }
    }
    if (inst == "sd")
    {
        vector<uint8_t> byte(8);
        for (int i = 0; i < 8; i++)
        {
            byte[7 - i] = store_value >> (7 - i) * 8;
        }
        for (int i = 0; i < 8; i++)
        {
            memory[mem_loc + i] = byte[i];
        }
    }


    //Update the stack of functions and their last executed line accordingly
    string top_of_stack = stack_of_functions.top().first;
    int top_line = stack_of_functions.top().second;
    top_line = PC / 4 + 1;
    stack_of_functions.pop();
    stack_of_functions.push({top_of_stack, top_line});

    string executed_instruction = all_instructions[PC / 4];
    int pos = executed_instruction.find(' ');
    string first_word = executed_instruction.substr(0, pos);
    if (first_word.back() == ':')
    {
        executed_instruction = executed_instruction.substr(pos + 1);
    }
    string bin_PC = decimal_to_binary(PC, 32, 0);
    string hex_PC = binary_to_hex(bin_PC);
    hex_PC = "0x" + hex_PC;
    cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';
    
    //Update the PC and ensure that x0 remains at value 0
    PC = PC + 4;
    register_values["x0"] = 0;
}

//Code for the U format step instructions
void step_U(vector<string> instruction)
{
    //Extract the rd and immediate values
    string inst, rd;
    inst = instruction[0];
    rd = aliasRemover(instruction[1]);
    int imm;

    //We accept both decimal and hexadecimal values
    if(instruction[2][1] == 'x')
    {
        string bin_instr = hex_to_binary(instruction[2].substr(2));
        int decimal = binary_to_decimal(bin_instr);
        imm = decimal;
    }
    else
    {
        imm = stoi(instruction[2]);
    }

    imm = imm << 12;
    register_values[rd] = imm;

    //Update the stack of functions and their last executed line accordingly
    string top_of_stack = stack_of_functions.top().first;
    int top_line = stack_of_functions.top().second;
    top_line = PC / 4 + 1;
    stack_of_functions.pop();
    stack_of_functions.push({top_of_stack, top_line});

    string executed_instruction = all_instructions[PC / 4];
    int pos = executed_instruction.find(' ');
    string first_word = executed_instruction.substr(0, pos);
    if (first_word.back() == ':')
    {
        executed_instruction = executed_instruction.substr(pos + 1);
    }
    string bin_PC = decimal_to_binary(PC, 32, 0);
    string hex_PC = binary_to_hex(bin_PC);
    hex_PC = "0x" + hex_PC;

    cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

    //Update the PC and keep x0 value at 0
    PC = PC + 4;
    register_values["x0"] = 0;
}

//Code for the B format step instructions
void step_B(vector<string> instruction)
{
    //Keep track of the new pc value and extract the rs1 and rs2 values
    int new_pc = PC + stoi(instruction[3]);

    string rs1 = aliasRemover(instruction[1]);
    string rs2 = aliasRemover(instruction[2]);

    string inst = instruction[0];

    //Keep track of old PC if branch not taken
    int old_PC = PC;

    //Execute the branch if condition satisfied as per the given instruction
    if (inst == "beq")
    {
        if (register_values[rs1] == register_values[rs2])
        {
            PC = new_pc;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == "bne")
    {
        if (register_values[rs1] != register_values[rs2])
        {
            PC = new_pc;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == "blt")
    {
        if (register_values[rs1] < register_values[rs2])
        {
            PC = new_pc;
        }
        else
        {
            PC += 4;
        }
    }
    else if (inst == "bge")
    {
        if (register_values[rs1] >= register_values[rs2])
        {
            PC = new_pc;
        }
        else
        {
            PC += 4;
        }
    }
    else if(inst == "bgeu")
    {
        uint64_t val_rs1 = register_values[rs1], val_rs2 = register_values[rs2];
        if(val_rs1 >= val_rs2)
        {
            PC = new_pc;
        }
        else
        {
            PC += 4;
        }
    }
    else if(inst == "bltu")
    {
        uint64_t val_rs1 = register_values[rs1], val_rs2 = register_values[rs2];
        if(val_rs1 < val_rs2)
        {
            PC = new_pc;
        }
        else
        {
            PC += 4;
        }
    }

    //Update the stack of functions and their last executed line accordingly
    string top_of_stack = stack_of_functions.top().first;
    int top_line = stack_of_functions.top().second;
    top_line = old_PC / 4 + 1;
    stack_of_functions.pop();
    stack_of_functions.push({top_of_stack, top_line});

    string executed_instruction = all_instructions[old_PC / 4];
    int pos = executed_instruction.find(' ');
    string first_word = executed_instruction.substr(0, pos);
    if (first_word.back() == ':')
    {
        executed_instruction = executed_instruction.substr(pos + 1);
    }
    string bin_PC = decimal_to_binary(old_PC, 32, 0);
    string hex_PC = binary_to_hex(bin_PC);
    hex_PC = "0x" + hex_PC;

    cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

    //Ensure that the x0 value remains at 0
    register_values["x0"] = 0;
}

//Code for the J format step instructions
void step_J(vector<string> instruction)
{
    //Store the new pc value to jump to
    int new_pc = PC + stoi(instruction[2]);

    //Extract the rd value
    string rd = aliasRemover(instruction[1]);

    int old_PC = PC;

    string inst = instruction[0];
    if (inst == "jal")
    {
        register_values[rd] = PC + 4;
        PC = new_pc;
    }

    //Update the stack of functions and their last executed line accordingly
    string top_of_stack = stack_of_functions.top().first;
    int top_line = stack_of_functions.top().second;
    top_line = old_PC / 4 + 1;
    stack_of_functions.pop();
    stack_of_functions.push({top_of_stack, top_line});

    stack_of_functions.push({offset_to_label[PC], -1});

    string executed_instruction = all_instructions[old_PC / 4];
    int pos = executed_instruction.find(' ');
    string first_word = executed_instruction.substr(0, pos);
    if (first_word.back() == ':')
    {
        executed_instruction = executed_instruction.substr(pos + 1);
    }
    string bin_PC = decimal_to_binary(old_PC, 32, 0);
    string hex_PC = binary_to_hex(bin_PC);
    hex_PC = "0x" + hex_PC;

    cout << "Executed: " << executed_instruction << "; PC=" << hex_PC << '\n';

    //Ensure that the value of x0 is 0
    register_values["x0"] = 0;
}

// Code for R format instructions
void execute_R(vector<vector<string>> &assembly_code, int instruction_no)
{
    // Handling errors
    if (assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Wrong syntax, for R type format, you must enter 4 operands" << endl;
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rd";
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs1";
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][3]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs2";
        exit(0);
    }

    // Noting the vaiours parts of the instrution
    string opcode = "0110011";
    string f3 = funct3[assembly_code[instruction_no][0]];
    string f7 = funct7[assembly_code[instruction_no][0]];
    string rd = registers[assembly_code[instruction_no][1]];
    string rs1 = registers[assembly_code[instruction_no][2]];
    string rs2 = registers[assembly_code[instruction_no][3]];

    // Concatinating the instruction
    string instr = f7 + rs2 + rs1 + f3 + rd + opcode;

    // Now let us write into the text segment of the memory:
    // We have the instruction number in the variable instruction_no

    string hex_instruction = binary_to_hex(instr);

    vector<string> binary_bytes(4);

    for (int i = 0; i < 4; i++)
    {
        binary_bytes[i] = hex_to_binary(hex_instruction.substr(6 - 2 * i, 2));
    }

    uint8_t byte[4];
    for (int i = 0; i < 4; i++)
    {
        byte[i] = binary_to_decimal(binary_bytes[i]); // characters 67 of hex_instruction
    }

    for (int i = 0; i < 4; i++)
    {
        memory[instruction_no * 4 + i] = byte[i];
    }
}

// Code for I format instructions
void execute_I(vector<vector<string>> &assembly_code, int instruction_no)
{
    // Handling error
    if (assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Wrong syntax, for I type format, you must enter 4 operands" << endl;
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rd";
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs1";
        exit(0);
    }

    string opcode;

    // Giving the correct opcode to the corresponding instruction
    if (load_operands.find(assembly_code[instruction_no][0]) != load_operands.end())
    {
        opcode = "0000011";
    }
    else if (assembly_code[instruction_no][0] == "jalr")
    {
        opcode = "1100111";
    }
    else
    {
        opcode = "0010011";
    }

    // Noting the vaiours parts of the instrution
    string f3 = funct3[assembly_code[instruction_no][0]];
    string rd = registers[assembly_code[instruction_no][1]];
    string rs1 = registers[assembly_code[instruction_no][2]];
    string imm;

    // Assigning the immediate along with checking if it is within range
    if (assembly_code[instruction_no][0] == "srli" || assembly_code[instruction_no][0] == "slli")
    {
        if (stoi(assembly_code[instruction_no][3]) < 0 || stoi(assembly_code[instruction_no][3]) >= pow(2, 6))
        {
            cout << "Error in line: " << instruction_no + 1 << endl
                 << "Value of immediate is out of range";
            exit(0);
        }

        imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 6, true);
        imm = "000000" + imm;
    }
    else if (assembly_code[instruction_no][0] == "srai")
    {
        if (stoi(assembly_code[instruction_no][3]) < 0 || stoi(assembly_code[instruction_no][3]) >= pow(2, 6))
        {
            cout << "Error in line: " << instruction_no + 1 << endl
                 << "Value of immediate is out of range";
            exit(0);
        }

        imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 6, true);
        imm = "010000" + imm;
    }
    else
    {
        if (stoi(assembly_code[instruction_no][3]) < -pow(2, 11) || stoi(assembly_code[instruction_no][3]) >= pow(2, 11))
        {
            cout << "Error in line: " << instruction_no + 1 << endl
                 << "Value of immediate is out of range";
            exit(0);
        }

        imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 12, true);
    }

    // Concatinating the values
    string instr = imm + rs1 + f3 + rd + opcode;

    string hex_instruction = binary_to_hex(instr);

    vector<string> binary_bytes(4);

    for (int i = 0; i < 4; i++)
    {
        binary_bytes[i] = hex_to_binary(hex_instruction.substr(6 - 2 * i, 2));
    }

    uint8_t byte[4];
    for (int i = 0; i < 4; i++)
    {
        byte[i] = binary_to_decimal(binary_bytes[i]); // characters 67 of hex_instruction
    }

    for (int i = 0; i < 4; i++)
    {
        memory[instruction_no * 4 + i] = byte[i];
    }
}

// Code for S format instructions
void execute_S(vector<vector<string>> &assembly_code, int instruction_no)
{
    // Handling error
    if (assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Wrong syntax, for S type format, you must enter 4 operands" << endl;
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs2";
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs1";
        exit(0);
    }
    else if (stoi(assembly_code[instruction_no][3]) < -pow(2, 11) || stoi(assembly_code[instruction_no][3]) >= pow(2, 11))
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Value of immediate is out of range";
        exit(0);
    }

    // Noting the various parts of the instruction
    string opcode = "0100011";
    string f3 = funct3[assembly_code[instruction_no][0]];
    string rs1 = registers[assembly_code[instruction_no][2]];
    string rs2 = registers[assembly_code[instruction_no][1]];
    string imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 12, true);

    // Concatinating the various parts
    string instr = imm.substr(0, 7) + rs2 + rs1 + f3 + imm.substr(7) + opcode;

    string hex_instruction = binary_to_hex(instr);

    vector<string> binary_bytes(4);

    for (int i = 0; i < 4; i++)
    {
        binary_bytes[i] = hex_to_binary(hex_instruction.substr(6 - 2 * i, 2));
    }

    uint8_t byte[4];
    for (int i = 0; i < 4; i++)
    {
        byte[i] = binary_to_decimal(binary_bytes[i]); // characters 67 of hex_instruction
    }

    for (int i = 0; i < 4; i++)
    {
        memory[instruction_no * 4 + i] = byte[i];
    }
}

// Code for B format instructions
void execute_B(vector<vector<string>> &assembly_code, int instruction_no)
{
    // Handling error
    if (assembly_code[instruction_no].size() != 4)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Wrong syntax, for B type format, you must enter 4 operands" << endl;
        exit(0);
    }
    if (labels.find(assembly_code[instruction_no][3]) == labels.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Label not defined";
        exit(0);
    }
    else
    {
        assembly_code[instruction_no][3] = to_string(labels[assembly_code[instruction_no][3]] - instruction_no * 4);
    }
    if (registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs1";
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][2]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rs2";
        exit(0);
    }
    else if (stoi(assembly_code[instruction_no][3]) < -pow(2, 12) || stoi(assembly_code[instruction_no][3]) >= pow(2, 12) - 1)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Value of immediate is out of range";
        exit(0);
    }

    // Noting the various parts of the instruction
    string opcode = "1100011";
    string f3 = funct3[assembly_code[instruction_no][0]];
    string rs1 = registers[assembly_code[instruction_no][1]];
    string rs2 = registers[assembly_code[instruction_no][2]];
    string imm = decimal_to_binary(stoi(assembly_code[instruction_no][3]), 13, true);

    // Concatinating the parts to form the instruction
    string instr = imm.substr(0, 1) + imm.substr(2, 6) + rs2 + rs1 + f3 + imm.substr(8, 4) + imm.substr(1, 1) + opcode;

    string hex_instruction = binary_to_hex(instr);

    vector<string> binary_bytes(4);

    for (int i = 0; i < 4; i++)
    {
        binary_bytes[i] = hex_to_binary(hex_instruction.substr(6 - 2 * i, 2));
    }

    uint8_t byte[4];
    for (int i = 0; i < 4; i++)
    {
        byte[i] = binary_to_decimal(binary_bytes[i]); // characters 67 of hex_instruction
    }

    for (int i = 0; i < 4; i++)
    {
        memory[instruction_no * 4 + i] = byte[i];
    }
}

// Code for J format instructions
void execute_J(vector<vector<string>> &assembly_code, int instruction_no)
{
    // Handling error
    if (assembly_code[instruction_no].size() != 3)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Wrong syntax, for J type format, you must enter 3 operands" << endl;
        exit(0);
    }
    if (labels.find(assembly_code[instruction_no][2]) == labels.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Label not defined";
        exit(0);
    }
    else
    {
        assembly_code[instruction_no][2] = to_string(labels[assembly_code[instruction_no][2]] - instruction_no * 4);
    }

    if (registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rd";
        exit(0);
    }
    else if (stoi(assembly_code[instruction_no][2]) < -1 * pow(2, 20) || stoi(assembly_code[instruction_no][2]) >= pow(2, 20) - 1)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Value of immediate is out of range";
        exit(0);
    }

    // Noting the various parts of the instrcution
    string opcode = "1101111";
    string rd = registers[assembly_code[instruction_no][1]];
    string imm = decimal_to_binary(stoi(assembly_code[instruction_no][2]), 21, true);

    // Concatinating the instruction
    string instr = imm.substr(0, 1) + imm.substr(10, 10) + imm.substr(9, 1) + imm.substr(1, 8) + rd + opcode;

    string hex_instruction = binary_to_hex(instr);

    vector<string> binary_bytes(4);

    for (int i = 0; i < 4; i++)
    {
        binary_bytes[i] = hex_to_binary(hex_instruction.substr(6 - 2 * i, 2));
    }

    uint8_t byte[4];
    for (int i = 0; i < 4; i++)
    {
        byte[i] = binary_to_decimal(binary_bytes[i]); // characters 67 of hex_instruction
    }

    for (int i = 0; i < 4; i++)
    {
        memory[instruction_no * 4 + i] = byte[i];
    }
}

// Code for U format instructions
void execute_U(vector<vector<string>> &assembly_code, int instruction_no)
{
    // Handling error
    if (assembly_code[instruction_no].size() != 3)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Wrong syntax, for U type format, you must enter 3 operands" << endl;
        exit(0);
    }
    else if (registers.find(assembly_code[instruction_no][1]) == registers.end())
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Incorrect value of rd";
        exit(0);
    }

    int dec_imm;
    //We accept both decimal and hexadecimal values
    if(assembly_code[instruction_no][2][1] == 'x')
    {
        string bin_instr = hex_to_binary(assembly_code[instruction_no][2].substr(2));
        int decimal = binary_to_decimal(bin_instr);
        dec_imm = decimal;
    }
    else
    {
        dec_imm = stoi(assembly_code[instruction_no][2]);
    }
    

    if (dec_imm < 0 || dec_imm > 4294963200)
    {
        cout << "Error in line: " << instruction_no + 1 << endl
             << "Value of immediate is out of range";
        exit(0);
    }

    // Noting the vaious parts of the instrction
    string opcode = "0110111";
    string rd = registers[assembly_code[instruction_no][1]];
    string imm = decimal_to_binary(dec_imm, 20, true);

    // Concatinating the instruction
    string instr = imm + rd + opcode;

    string hex_instruction = binary_to_hex(instr);

    vector<string> binary_bytes(4);

    for (int i = 0; i < 4; i++)
    {
        binary_bytes[i] = hex_to_binary(hex_instruction.substr(6 - 2 * i, 2));
    }

    uint8_t byte[4];
    for (int i = 0; i < 4; i++)
    {
        byte[i] = binary_to_decimal(binary_bytes[i]); // characters 67 of hex_instruction
    }

    for (int i = 0; i < 4; i++)
    {
        memory[instruction_no * 4 + i] = byte[i];
    }
}

//Function to store dword
int store_dword(vector<string> instruction, int mem_loc)
{
    //Go through all the values that the user has specified and store them starting for location specified by the mem_loc
    for(int i = 1 ; i < instruction.size() ; i++)
    {
        uint64_t store_value = stoi(instruction[i]);
        vector<uint8_t> byte(8);
        for (int i = 0; i < 8; i++)
        {
            byte[7 - i] = store_value >> (7 - i) * 8;
        }
        for (int i = 0; i < 8; i++)
        {
            memory[mem_loc + i] = byte[i];
        }
        mem_loc += 8;
    }

    //Return the new mem_loc where you can write to next
    return mem_loc;
}

//Function to store word
int store_word(vector<string> instruction, int mem_loc)
{
    //Go through all the values that the user has specified and store them starting for location specified by the mem_loc
    for(int i = 1 ; i < instruction.size() ; i++)
    {
        uint64_t store_value = stoi(instruction[i]);
        vector<uint8_t> byte(4);
        for (int i = 0; i < 4; i++)
        {
            byte[3 - i] = store_value >> (3 - i) * 8;
        }
        for (int i = 0; i < 4; i++)
        {
            memory[mem_loc + i] = byte[i];
        }
        mem_loc += 4;
    }

    //Return the new mem_loc where you can write to next
    return mem_loc;
}

//Function to store half word
int store_half(vector<string> instruction, int mem_loc)
{
    //Go through all the values that the user has specified and store them starting for location specified by the mem_loc
    for(int i = 1 ; i < instruction.size() ; i++)
    {
        uint64_t store_value = stoi(instruction[i]);
        vector<uint8_t> byte(2);
        byte[0] = store_value;
        byte[1] = store_value >> 8;

        memory[mem_loc] = byte[0];
        memory[mem_loc + 1] = byte[1];
        mem_loc += 2;
    }

    //Return the new mem_loc where you can write to next
    return mem_loc;
}

//Function to store bytw
int store_byte(vector<string> instruction, int mem_loc)
{
    //Go through all the values that the user has specified and store them starting for location specified by the mem_loc
    for(int i = 1 ; i < instruction.size() ; i++)
    {
        uint64_t store_value = stoi(instruction[i]);
        uint8_t byte = store_value;
        memory[mem_loc] = byte;
        mem_loc++;
    }

    //Return the new mem_loc where you can write to next
    return mem_loc;
}

signed main()
{
    //Initialize all the required fields
    initialize_registers();
    initialize_funct3();
    initialize_funct7();
    initialize_format();
    initialize_load_operands();
    initialize();

    ifstream instruction_file;
    vector<vector<string>> assembly_code;

    // Infinite loop that reads user inputs
    while (true)
    {
        // Now save the next file
        string command;
        getline(cin, command);

        if (!command.find("load"))
        {
            // First reset all the registers/memory/PC before loading the next file
            initialize();
            assembly_code.clear();

            //Check if the user is specifying the .data or .text field
            bool data = false;
            string data_type;
            int data_base_address = 0x10000;

            // Load the next file into a queue of strings, such that after execution of each run, we can dequeue the top
            string instruction;
            instruction_file.open(command.substr(5));
            while (getline(instruction_file, instruction))
            {
                //If the instruction is data, then ensure that until we see .text, we keep storing data
                if (instruction == ".data")
                {
                    data = true;
                    count_how_many_to_skip++;
                    continue;
                }

                if (data || (instruction == ".text"))
                {
                    //Keep track of how many lines were used for data segment and the first line of text (.text)
                    count_how_many_to_skip++;

                    if (instruction == ".text")
                    {
                        data = false;
                    }
                    else
                    {
                        //Store the required dword/word/byte/half word
                        vector<string> to_store;
                        string cur = "";
                        for (char c : instruction)
                        {
                            if (c == ' ' || c == ',')
                            {
                                if (cur != "")
                                {
                                    to_store.push_back(cur);
                                }
                                cur = "";
                            }
                            else
                            {
                                cur.push_back(c);
                            }
                        }
                        if (cur != "")
                        {
                            to_store.push_back(cur);
                        }

                        if (to_store[0] == ".dword")
                        {
                            data_base_address = store_dword(to_store, data_base_address);
                        }
                        else if (to_store[0] == ".half")
                        {
                            data_base_address = store_half(to_store, data_base_address);
                        }
                        else if (to_store[0] == ".word")
                        {
                            data_base_address = store_word(to_store, data_base_address);
                        }
                        else if (to_store[0] == ".byte")
                        {
                            data_base_address = store_byte(to_store, data_base_address);
                        }
                    }
                    continue;
                }

                //remove unnecessary space from the beginning of any line
                bool trim = true;
                string trimmed_instr = "";
                for(int i = 0 ; i < instruction.size() ; i++)
                {
                    if(trim && instruction[i] == ' ')
                    {
                        continue;
                    }
                    else
                    {
                        trim = false;
                        trimmed_instr.push_back(instruction[i]);
                    }
                }
                instruction = trimmed_instr;

                //Go through the current instruction and store it in a 2d matrix of strings
                vector<string> cur_instruction;
                all_instructions.push_back(instruction);
                string cur = "";
                for (int i = 0; i < instruction.size(); i++)
                {
                    if (instruction[i] != ' ' && instruction[i] != '(')
                    {
                        cur.push_back(instruction[i]);
                    }
                    else
                    {
                        if (cur[cur.size() - 1] == ':')
                        {
                            cur.pop_back();
                            labels[cur] = PC;
                            offset_to_label[PC] = cur;
                            cur = "";
                            continue;
                        }

                        if (cur[cur.size() - 1] == ',' || cur[cur.size() - 1] == ')')
                        {
                            cur.pop_back();
                        }

                        cur_instruction.push_back(cur);
                        cur = "";
                    }
                }

                if (cur != "")
                {
                    if (cur[cur.size() - 1] == ',' || cur[cur.size() - 1] == ')')
                    {
                        cur.pop_back();
                    }
                    cur_instruction.push_back(cur);
                }

                // If in wrong position, bring the operands back to the correct position in the matrix
                if (load_operands.find(cur_instruction[0]) != load_operands.end() || format[cur_instruction[0]] == 'S' || cur_instruction[0] == "jalr")
                {
                    swap(cur_instruction[2], cur_instruction[3]);
                }

                assembly_code.push_back(cur_instruction);

                PC += 4;
            }
            instruction_file.close();
            //Ensure that the PC value is 0
            PC = 0;

            //Convert the assembly code to machine code hexadecimal and store in text memory
            for (int instruction_no = 0; instruction_no < assembly_code.size(); instruction_no++)
            {
                if (format[assembly_code[instruction_no][0]] == 'R')
                {
                    execute_R(assembly_code, instruction_no);
                }
                else if (format[assembly_code[instruction_no][0]] == 'I')
                {
                    execute_I(assembly_code, instruction_no);
                }
                else if (format[assembly_code[instruction_no][0]] == 'S')
                {
                    execute_S(assembly_code, instruction_no);
                }
                else if (format[assembly_code[instruction_no][0]] == 'B')
                {
                    execute_B(assembly_code, instruction_no);
                }
                else if (format[assembly_code[instruction_no][0]] == 'J')
                {
                    execute_J(assembly_code, instruction_no);
                }
                else if (format[assembly_code[instruction_no][0]] == 'U')
                {
                    execute_U(assembly_code, instruction_no);
                }
                else
                {
                    cout << "Error on line: " << instruction_no + 1 << endl
                         << "Incorrect syntax";
                    exit(0);
                }
            }
        }
        else if (command == "run")
        {
            //Run the entire code till the break point
            int no_of_instruction = assembly_code.size();

            bool to_break = false;
            int break_point_line_to_remove;

            if (PC / 4 == no_of_instruction)
            {
                cout << "Nothing to step\n";
            }
            else if(break_lines.find(PC / 4 + 1) != break_lines.end())
            {
                cout << "Execution stopped at breakpoint\n";
            }
            else
            {
                while (PC / 4 < no_of_instruction)
                {
                    if (break_lines.find(PC / 4 + 2) != break_lines.end())
                    {
                        to_break = true;
                    }

                    vector<string> cur_instr = assembly_code[PC / 4];

                    if (format[cur_instr[0]] == 'R')
                    {
                        step_R(cur_instr);
                    }
                    else if (format[cur_instr[0]] == 'I')
                    {
                        step_I(cur_instr);
                    }
                    else if (format[cur_instr[0]] == 'S')
                    {
                        step_S(cur_instr);
                    }
                    else if (format[cur_instr[0]] == 'B')
                    {
                        step_B(cur_instr);
                    }
                    else if (format[cur_instr[0]] == 'J')
                    {
                        step_J(cur_instr);
                    }
                    else if (format[cur_instr[0]] == 'U')
                    {
                        step_U(cur_instr);
                    }
                    else
                    {
                        exit(0);
                    }

                    if (to_break)
                    {

                        cout << "Execution stopped at breakpoint\n";
                        break;
                    }
                }

                //If last line executed, then ensure to pop main from the stack
                if (PC / 4 == no_of_instruction)
                {
                    stack_of_functions.pop();
                }
            }
        }
        else if (command == "step")
        {
            //Go step by step
            int no_of_instruction = assembly_code.size();

            if (PC / 4 == no_of_instruction)
            {
                cout << "Nothing to step\n";
            }

            else
            {
                vector<string> cur_instr = assembly_code[PC / 4];

                if (format[cur_instr[0]] == 'R')
                {
                    step_R(cur_instr);
                }
                else if (format[cur_instr[0]] == 'I')
                {
                    step_I(cur_instr);
                }
                else if (format[cur_instr[0]] == 'S')
                {
                    step_S(cur_instr);
                }
                else if (format[cur_instr[0]] == 'B')
                {
                    step_B(cur_instr);
                }
                else if (format[cur_instr[0]] == 'J')
                {
                    step_J(cur_instr);
                }
                else if (format[cur_instr[0]] == 'U')
                {
                    step_U(cur_instr);
                }
                else
                {
                    exit(0);
                }

                //If last line is executed, then ensure to pop main from the stack
                if (PC / 4 == no_of_instruction)
                {
                    stack_of_functions.pop();
                }
            }
        }
        else if (command == "regs")
        {
            //Print the register values which are stored in the map
            cout << "Registers:\n";
            for (int i = 0; i < 32; i++)
            {
                string this_register = "x";
                if (i < 10)
                {
                    char num = '0' + i;
                    this_register += num;
                }
                else
                {
                    int dummyi = i;
                    char num1 = dummyi % 10 + '0';
                    dummyi /= 10;
                    char num2 = dummyi % 10 + '0';
                    this_register += num2;
                    this_register += num1;
                }

                int value_of_register_int = register_values[this_register];
                string value_of_register_bin = decimal_to_binary(value_of_register_int, 64, 1);
                string value_of_register_hex = binary_to_hex(value_of_register_bin, true, 16);

                cout << this_register << " = 0x" << value_of_register_hex << '\n';
            }
        }
        else if (command == "exit")
        {
            //Exit the simulator
            cout << "Exited the simulator";
            break;
        }
        else if (!command.find("mem"))
        {//Here we show the count number of memory bytes starting from memory location specified by the user
            string s_addr = ""; //store the start address from which we have to display the memory
            string s_count = ""; //Count of the number of memory locations we have to display

            bool check_space = false;
            //We now extract the value of addr and count by parsing the line
            for (int i = 4; i < command.size(); i++)
            {
                if (!check_space)
                {
                    if (command[i] == ' ')
                    {
                        check_space = true;
                    }
                    else
                    {
                        s_addr.push_back(command[i]);
                    }
                }
                else
                {
                    s_count.push_back(command[i]);
                }
            }

            s_addr = s_addr.substr(2); //We remove the leading 0x in the string
            //We now want to convert it to decimal representtion, via binary representation
            string bin_s_addr = hex_to_binary(s_addr);
            int addr = binary_to_decimal(bin_s_addr);

            int count = stoi(s_count);

            //Printing the locations of the memory as specified using a looop
            for (int i = addr; i < addr + count; i++)
            {
                int mem_val = memory[i];
                string bin_mem_val = decimal_to_binary(mem_val, 8, 0);
                string hex_mem_val = "0x"+binary_to_hex(bin_mem_val, true, 2);
                int mem_loc = i;
                string mem_loc_bin = decimal_to_binary(i, 20, 0);
                string mem_loc_hex = "0x"+binary_to_hex(mem_loc_bin, false, 5);
                cout << "Memory[" << mem_loc_hex << "]" << " = " << hex_mem_val << '\n';
            }
        }
        else if (command == "show-stack")
        {   //Here we execute the show-stack command
            cout << "Call Stack:\n";

            //We use a dummy stack to pop the labels from the main stack and print the for the user
            vector<pair<string, int>> dummy_stack;
            stack<pair<string, int>> dummy_stack_function = stack_of_functions;
            while (!dummy_stack_function.empty())
            {
                dummy_stack.push_back(dummy_stack_function.top());
                dummy_stack_function.pop();
            }
            for (int i = dummy_stack.size() - 1; i >= 0; i--)
            {//Using a loop to print the labels in stack, bottom to top
                cout << dummy_stack[i].first << ":" << dummy_stack[i].second + count_how_many_to_skip << '\n';
            }

            if (stack_of_functions.empty())
            {
                cout << "Empty Call Stack: Execution complete\n";
            }
        }
        else if (!command.find("del break"))
        {//This block deletes a beakpoint in the program
            int break_point = stoi(command.substr(10));
            //We delete the breakpoint
            if (break_lines.find(break_point - count_how_many_to_skip) != break_lines.end())
            {
                break_lines.erase(break_point - count_how_many_to_skip);
                cout << "Deleted breakpoint at line : " << break_point << '\n';
            }
            else
            {
                cout << "No breakpoints present\n";
            }
        }
        else if (!command.find("break"))
        {
            //We parse the string to get the line number where we have to insert a breakpoint
            int break_point = stoi(command.substr(6));
            //We take into account the lines that are occupied by the data section, and subtract them
            break_lines.insert(break_point - count_how_many_to_skip);
            cout << "Inserted break point at line : " << break_point << '\n';
        }
        cout << '\n';
    }

    return 0;
}