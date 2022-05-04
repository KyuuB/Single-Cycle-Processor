#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <bitset>
#include <sstream>
#include <iomanip>

using namespace std;

int pc = 0;
int total_clock_cycles = 0;
int jump_target = 0;
// control signals

// Fetch machine code one at a time, cycle through using pc
string Fetch(int *pc) {

    //Initialized values for file, line, machine code string, totalLines, and linenumber
    fstream myfile;
    string line;
    string machineCode;
    int totalLines;
    int lineNumber = 0;

    //ADD INPUT FROM USER FOR WHICH FILE WE WOULD LIKE
    // ! ! ! ! ! ! ! ! ! ! ! ! ! ! 
    myfile.open("sample_part1.txt");
    if (myfile.is_open()) {
        while(getline(myfile, line)) {
            if (lineNumber == *pc){
                machineCode = line;
            }
            lineNumber = lineNumber + 4;
        }
        cout << "machineCode = " << machineCode << endl;
    }

    int next_pc = *pc + 4;

    //Add logic that will copy one of the three possible pc values
    // (next_pc, branch_target, and jump_target),
    // This will be used in the next fetch instruction.
    *pc = next_pc;

    return machineCode;
}

// HW3 extended
void Decode(string machineCode) { 
    /*
    Array register values read from resiter file 32 values "registerfile"
    Sign extension (page lecture slide 15 processor 1)
    Jump target address shift left 2, use next_pc to calculate first 4 bits
    */
    
    //Gets rid of new line that comes in text file
    machineCode.pop_back();
    //Machine code turns to instruction
    string inst = machineCode;
    //Get opcode from binary input using bitset of 6 for all opcodes 
    bitset<6> set0(inst); //Gets Opcode here.
    
    //Based on opcode we determine between I, J, and R type instructions

    //If opcode is Zero, then it is an R type instruction.
    if (set0.to_ulong() == 0) { 
        cout << "Instruction Type: R" << endl;

        //Need function to figure out the operation
        string funct = inst.substr(26, 6);
        bitset<6> set6(funct);
    
        //If else for all operations of R type.
        //Go through all instructions until we have a matching function code
        if (set6.to_ulong() == 32) {
            cout << "Operation: add" << endl;
        }
        else if (set6.to_ulong() == 33) 
        {
            cout << "Operation: addu" << endl;
        }
        else if (set6.to_ulong() == 36) 
        {
            cout << "Operation: and" << endl;
        }
        else if (set6.to_ulong() == 8) 
        {
            cout << "Operation: jr" << endl;
        }
        else if (set6.to_ulong() == 39) 
        {
            cout << "Operation: nor" << endl;
        }
        else if (set6.to_ulong() == 37) 
        {
            cout << "Operation: or" << endl;
        }
        else if (set6.to_ulong() == 42) 
        {
            cout << "Operation: slt" << endl;
        }
        else if (set6.to_ulong() == 43) 
        {
            cout << "Operation: sltu" << endl;
        }
        else if (set6.to_ulong() == 0) 
        {
            cout << "Operation: sll" << endl;
        }
        else if (set6.to_ulong() == 2) 
        {
            cout << "Operation: srl" << endl;
        }
        else if (set6.to_ulong() == 34) 
        {
            cout << "Operation: sub" << endl;
        }
        else if (set6.to_ulong() == 35) 
        {
            cout << "Operation: subu" << endl;
        }
        //No matching then this should be a problem.
        else {
            cout << "ERROR!" << endl;
            cout << set6.to_ulong() << endl;
        }

        //New strings for rs, rt, rd, and shamt, with a bitset of 5 for each.
        //Convert all bitsets into decimal.
        string rs = inst.substr(6, 5);
        bitset<5> set2(rs);
        cout << "Rs: $" << dec << set2.to_ulong() << endl;

        string rt = inst.substr(11, 5);
        bitset<5> set3(rt);
        cout << "Rt: $" << dec << set3.to_ulong() << endl;

        string rd = inst.substr(16, 5);
        bitset<5> set4(rd);
        cout << "Rd: $" << dec << set4.to_ulong() << endl;

        string shamt = inst.substr(21, 5);
        bitset<5> set5(shamt);
        cout << "Shamt: " << dec << set5.to_ulong() << endl;

        //Funct was calculted earlier to figure out the operation.
        //Display both decimal and hex.
        cout << "Funct: " << dec << set6.to_ulong() << " (or 0x" << hex << set6.to_ulong() << ")" << endl;
    }
    //If opcode is not zero, then it is either j or i type instruction
    else {
        //If opcode is 2, then j instruction
        if (set0.to_ulong() == 2){
            cout << "Instruction Type: J" << endl;
            cout << "Operation: j" << endl;

            //Substring of the instruction with a bitset of 26 for the address/immediate
            string address = inst.substr(6, 26);
            bitset<26> set7(address);
            //Convert bitset to decimal and hex
            cout << "Immediate: " << dec << set7.to_ulong() << " (or 0x" << hex << set7.to_ulong() << ")" << endl;
        }
        //If opcode is 3, then jal 
        else if (set0.to_ulong() == 3)
        {
            cout << "Instruction Type: J" << endl;
            cout << "Operation: jal" << endl;
            //Bit set 
            string address = inst.substr(6, 26);
            bitset<26> set8(address);
            cout << "Immediate: " << dec << set8.to_ulong() << " (or 0x" << hex << set8.to_ulong() << ")" << endl;
        }
        else {
            cout << "Instruction Type: I" << endl;

            //If else for all operations of I type.

            if (set0.to_ulong() == 8) {
                cout << "Operation: addi" << endl;
            }
            else if (set0.to_ulong() == 9)
            {
                cout << "Operation: addiu" << endl;
            }
            else if (set0.to_ulong() == 12)
            {
                cout << "Operation: andi" << endl;
            }
            else if (set0.to_ulong() == 4)
            {
                cout << "Operation: beq" << endl;
            }
            else if (set0.to_ulong() == 5)
            {
                cout << "Operation: bne" << endl;
            }
            else if (set0.to_ulong() == 36)
            {
                cout << "Operation: lbu" << endl;
            }
            else if (set0.to_ulong() == 37)
            {
                cout << "Operation: lhu" << endl;
            }
            else if (set0.to_ulong() == 48)
            {
                cout << "Operation: ll" << endl;
            }
            else if (set0.to_ulong() == 15)
            {
                cout << "Operation: lui" << endl;
            }
            else if (set0.to_ulong() == 35)
            {
                cout << "Operation: lw" << endl;
            }
            else if (set0.to_ulong() == 13)
            {
                cout << "Operation: ori" << endl;
            }
            else if (set0.to_ulong() == 10)
            {
                cout << "Operation: slti" << endl;
            }
            else if (set0.to_ulong() == 11)
            {
                cout << "Operation: sltiu" << endl;
            }
            else if (set0.to_ulong() == 40)
            {
                cout << "Operation: sb" << endl;
            }
            else if (set0.to_ulong() == 56)
            {
                cout << "Operation: sc" << endl;
            }
            else if (set0.to_ulong() == 41)
            {
                cout << "Operation: sh" << endl;
            }
            else if (set0.to_ulong() == 43)
            {
                cout << "Operation: sw" << endl;
            }
            else {
                cout << "ERROR!" << endl;
            }
            //Show rs and rt using a substring and bitset of 5 and converting to decimal
            string rs = inst.substr(6, 5);
            bitset<5> set9(rs);
            cout << "Rs: $" << dec << set9.to_ulong() << endl;
            
            string rt = inst.substr(11, 5);
            bitset<5> set10(rt);
            cout << "Rt: $" << dec << set10.to_ulong() << endl;

            //Bitset of 26 and converting to decimal and hex 
            string immediate = inst.substr(16, 16);
            bitset<26> set11(immediate);
            cout << "Immediate: " << dec << set11.to_ulong() << " (or 0x" << hex << set11.to_ulong() << ")" << endl;
        }
    }
    
    cout << endl;

}

void Execute() {
    /*
    ALU OP received 4 bit alu_op add, <, and operations
    Zero output: 1 bit intitialized to zero, will be used with next_pc
    Branch target address updated and used by fetch() next_pc + 4
    */
}

void Mem() {
    /*
    32 entries initialized to zero
    Used with store word sw and load word lw
    */
}

void Writeback() {
    /*
    Get results and update register file array
    Increment clock cycles
    */

   total_clock_cycles = total_clock_cycles + 1;

}

void ControlUnit() {
    /*
    From opcode generate 9 control signals, global variables for each control signal
    Execute, mem, and writeback all use control signals
    */

   
}

// lw, sw, add, sub, and, or, slt, nor, beq, j
int main() {

    //Initialized values for the text file, lines in the file, and number of lines.
    ifstream myfile;
    string line;
    int numLines = 0;

    // USED TO GET INPUT FROM USER
    //string file;
    //cout << "Enter the program file name to run: " << endl << endl;
    //cin >> file;
    // myfile.open(file, ios::in);
    // if (myfile.is_open()) {
    //     while(getline(myfile, line)) {
    //         numLines = numLines + 1;
    //     }
    // }


    //Find number of lines in the text file to get a limit for the while loop.
    myfile.open("sample_part1.txt", ios::in);
    if (myfile.is_open()) {
        while(getline(myfile, line)) {
            numLines = numLines + 1;
        }
    }

    //maxAddress will be the end of the text file 
    int maxAddress = numLines * 4;
    // Cycle Starts here
    while (pc < maxAddress)
    {
        //Get machine code form fetch using global variable pc
        string fetchedCode = "";
        fetchedCode = Fetch(&pc);
        
        //Decode the machine code from fetched.
        Decode(fetchedCode);

        Writeback();
        cout << "total_clock_cycles " << total_clock_cycles << ":" << endl;
        cout << "register is modified to " << endl;
        cout << "pc is modified to 0x" << pc << endl << endl;
    }
    
    cout << "program terminated: " << endl;
    cout << "total execution time is " << total_clock_cycles << " cycles" << endl;
    
    return 0;
}