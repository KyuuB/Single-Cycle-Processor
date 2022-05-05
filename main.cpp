#include <iostream>
#include <fstream>
#include <string>
#include <vector> 
#include <bitset>
#include <sstream>
#include <iomanip>

using namespace std;

//Global Variables
int pc = 0;
int total_clock_cycles = 0;
int jump_target = 0;
int branch_target = 0;
int alu_zero = 0;

// Global control signals
int regWrite = 0;
int regDst = 0;
int branch = 0;
int aluSrc = 0;
int memWrite = 0;
int memToReg = 0;
int memRead = 0;
int instType = 0;
int jump = 0;

//registerfile with initialized hex values
static int registerfile [31] = { 0x0, 0x0, 0x0, 0, 0, 0, 0, 0, 0, 0x20, 5, 0, 0, 0, 0, 0, 0x70};

//Last step in the cycle, sw will update memory, while lw will update register, increment clock cycles
void Writeback(int* dataMemory, int result) {

    //Get registerID and the results from Execute()
    // update registerfile at the address and return the result from Memory.
    registerfile[*dataMemory] = result;

    //Increment clock cycles
    total_clock_cycles = total_clock_cycles + 1;

}

//Memory with addresses, 32 slots incremented by 4.
void Mem(int* address, int result, string op) {
    /*
    32 entries initialized to zero
    Used with store word sw and load word lw
    sw store value into memory
    get value int* x, store into 
    lw load memory into register value
    */
    //from execute, get memory address
    /*
    Mem() function should receive memory address for both LW and SW instructions and data to 
    write to the memory for SW. For the LW, the value stored in the indicated d-mem array entry 
    should be retrieved and sent to the Writeback() function that is explained below.   
    */

    // 0 4 8 c 10 14 18 1c 20 24 28 2c 30 34 38 3c 40 44 48 4c 50 54 58 5c 60 64 68 6c 70 74 78 7c 80
    // 0 1 2 3 4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
    static int d_mem[31] = {0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5, 0x10 };

    //Need address which will be the 0x00 ... 0x80
    //index = some hex number
    int index = 0;
    //shift left logial by two to get hex to index number
    index = *address << 2;
    
    int x, y;

    stringstream stream;

    x = index;
    cin >> x;
    stream << x;
    stream >> hex >> y;
    cout << y;

    int* dataMemory = &d_mem[index];
    
    //If lw then memory at address should be return to writeback to be entered int registerfile
    if (op == "lw") {
        Writeback(dataMemory, result);
    }

    

}

//Do most of the arithmetic, and offset additions.
int Execute(int* rs, int* rt, int* rd, int offset, int* pc, string alu_op) {
    /*
    ALU OP received 4 bit alu_op for add, <, and operations
    Zero output: 1 bit intitialized to zero, will be used with next_pc
    Branch target address updated and used by fetch() next_pc + 4
    */
    alu_zero = 0;
    
    int result = 0;
    //turning aluop from string to int
    int aluOp = stoi(alu_op);
    cout << "aluOp = " << aluOp << endl;
    cout << "offset = " << offset << endl;
    //Switch case for alu op
    switch(aluOp) {
        //AND statement
        case 0:
            if ((*rs & *rt) == 1) result = 1;
            else rd = 0;
            break;

        //OR
        case 1:
            if ((*rs || *rt) == 1) result = 1;
            else rd = 0;
            break;

        //ADD
        case 10:
            result = *rs + *rt;
            break;


        //SUB
        case 110:
            result = *rs - *rt;
            break;

        //SLT
        case 111:
            if (*rs < *rt) result = 1;
            else rd = 0;
            break;

        //NOR
        case 1100:
            if ((*rs || *rt) != 1) result = 1;
            break;
    }

    //MUX For if branch is enabled
    if (branch == 1) {
        branch_target = (offset * 4) + *pc;
        *pc = branch_target;
    }
    //If jump control signal is enabled, the pc = offset/immediate shift left by 2
    if (jump == 1) {
        jump_target = (offset * 4 );
        *pc = jump_target;
    }
    // cout << "rs = " << rs << endl;
    // cout << "rt = " << rt << endl;
    // cout << "rd = " << rd << endl;
    // cout << "offset = " << offset << endl;
    // cout << "alu_op = " << alu_op << endl;

    return result;

}

//Creates signals that will let execute know several logic paths to take
string ControlUnit(string opcode, string funct) {
    /*
    From opcode generate 9 control signals, global variables for each control signal
    Execute, mem, and writeback all use control signals
    */
   
   string alu_op = "1111";

   //Initialize global control signals to reset for each cycle
    regWrite = 0;
    regDst = 0;
    branch = 0;
    aluSrc = 0;
    memWrite = 0;
    memToReg = 0;
    memRead = 0;
    instType = 0;
    jump = 0;

    //R-type
    if (opcode == "000000") {
        //Global variables set for each opcode
        regDst = 1; regWrite = 1; instType = 10;
        //Find 4-bit ALU Op values using funct for R-type instructions
        // ADD
        if (funct == "100000" ) {
            alu_op = "0010";
        }
        // SUB
        else if (funct == "100010") {
            alu_op = "0110";
        }
        // AND
        else if (funct == "100100") {
            alu_op = "0000";
        }
        // OR
        else if (funct == "100101") {
            alu_op = "0001";
        }
        // SLT
        else if (funct == "101010"){
            alu_op = "0111";
        }
        // NOR
        else if (funct == "100111"){
            alu_op = "1100";
        }
        else {
            cout << "ERROR! FUNCT NOT FOUND!" << endl;
        }

        cout << "alu-op  =  " << alu_op << endl;
    }
    // lw 
    else if (opcode == "100011") {
        aluSrc = 1; memToReg = 1; regWrite = 1; memRead = 1;
    }
    // sw
    else if (opcode == "101011") {
        aluSrc = 1; memWrite = 1;
    }
    // beq
    else if (opcode == "000100" ) {
        branch = 1; instType = 1;
    }
    // j
    else if (opcode == "000010") {
        jump = 1; 
    }
    // No matching opcode
    else {
        cout << "ERROR! OPCODE NOT FOUND!" << endl;
    }

    return alu_op;

}

// Takes machine code from fetch, int pointer pc, and int pointer jump target
void Decode(string machineCode, int* pc, int* jump_target) { 
    /*
    Array register values read from resiter file 32 values "registerfile"
    Sign extension (page lecture slide 15 processor 1)
    Jump target address shift left 2, use next_pc to calculate first 4 bits
    */

    //Initialized values

    //changed to pointers to get values and register id for execute
    int* Rs = 0;
    int* Rt = 0;
    int* Rd = 0;
    int index = 0;
    int offset = 0;
    int* address = 0;
    int result = 0;
    string opcode = "";
    string funct = "";
    string alu_op = "1111";

    //Gets rid of new line that comes in text file
    machineCode.pop_back();

    //set instruction as machineCode
    string inst = machineCode;

    //Get opcode from binary input using bitset of 6 for all opcodes 
    opcode = machineCode.substr(0, 6);
    bitset<6> set0(inst); //Gets Opcode here.
    //Based on opcode we determine between I, J, and R type instructions

    //If opcode is Zero, then it is an R type instruction.
    if (set0.to_ulong() == 0) { 
        //R Instruction Type
        //cout << Instruction Type: R << endl;
        //Need function to figure out the operation
        funct = inst.substr(26, 6);
        cout << "funct = " << funct << endl;
        bitset<6> set6(funct);
        //If else for all operations of R type.
        //Go through all instructions until we have a matching function code
        if (set6.to_ulong() == 32) {
            cout << "Operation: add" << endl;
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
        else if (set6.to_ulong() == 34) 
        {
            cout << "Operation: sub" << endl;
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
        //Turn machine code into register file id
        index = stoi(rs, nullptr, 2);
        //File ID -> value 
        Rs = &(registerfile[index]);
        cout << "*Rs = " << *Rs << endl;

        string rt = inst.substr(11, 5);
        bitset<5> set3(rt);
        index = stoi(rt, nullptr, 2);
        Rt = &(registerfile[index]);
        cout << "*Rt = " << *Rt << endl;

        string rd = inst.substr(16, 5);
        bitset<5> set4(rd);
        cout << "Rd: $" << dec << set4.to_ulong() << endl;
        //Turn machine code into register file id
        index = stoi(rd, nullptr, 2);
        Rd = &(registerfile[index]);

        string shamt = inst.substr(21, 5);
        bitset<5> set5(shamt);
        //cout << "Shamt: " << dec << set5.to_ulong() << endl;
        

        //Funct was calculated earlier to figure out the operation.
        //Display both decimal and hex.
        //cout << "Funct: " << dec << set6.to_ulong() << " (or 0x" << hex << set6.to_ulong() << ")" << endl;
    }
    //If opcode is not zero, then it is either j or i type instruction
    else {
        //If opcode is 2, then j instruction
        if (set0.to_ulong() == 2){
            //cout << "Instruction Type: J" << endl;
            cout << "Operation: j" << endl;

            //Substring of the instruction with a bitset of 26 for the address/immediate
            string address = inst.substr(6, 26);
            bitset<26> set7(address);
            //Convert bitset to decimal and hex
            //cout << "Immediate: " << dec << set7.to_ulong() << " (or 0x" << hex << set7.to_ulong() << ")" << endl;
            int jt = stoi(address, nullptr, 2);
            *jump_target = jt * 4;
            
        }
        //If opcode is 3, then jal 
        else if (set0.to_ulong() == 3)
        {
            //cout << "Instruction Type: J" << endl;
            cout << "Operation: jal" << endl;
            //Bit set 
            string address = inst.substr(6, 26);
            bitset<26> set8(address);
            //cout << "Immediate: " << dec << set8.to_ulong() << " (or 0x" << hex << set8.to_ulong() << ")" << endl;
            int jt = stoi(address, nullptr, 2);
            *jump_target = jt * 4;
        }
        else {
            //cout << "Instruction Type: I" << endl;

            //If else for all operations of I type.

            if (set0.to_ulong() == 4)
            {
                cout << "Operation: beq" << endl;
            }
            else if (set0.to_ulong() == 35)
            {
                cout << "Operation: lw" << endl;
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
            index = stoi(rs, nullptr, 2);
            Rs = &(registerfile[index]);

            string rt = inst.substr(11, 5);
            bitset<5> set10(rt);
            cout << "Rt: $" << dec << set10.to_ulong() << endl;
            index = stoi(rt, nullptr, 2);
            Rt = &(registerfile[index]);
            
            //Set a default for rd
            Rd = &(registerfile[0]);

            //Bitset of 26 and converting to decimal and hex 
            string immediate = inst.substr(16, 16);
            bitset<26> set11(immediate);
            //cout << "Immediate: " << dec << set11.to_ulong() << " (or 0x" << hex << set11.to_ulong() << ")" << endl;
            //May need to calculate offset 
            offset = stoi(immediate, nullptr, 2);
        }
    }

    //Use Control unit to set control signals and return alu_op for Execute;
    alu_op = ControlUnit(opcode, funct);
    result = Execute(Rs, Rt, Rd, offset, pc, alu_op);
    //PROBABLY CHANGE, NEED DATA MEMORY
    Writeback(&result, result);
}

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
    myfile.open("sample_part1.txt", ios::in);
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

// lw, sw, add, sub, and, or, slt, nor, beq, j
int main() {

    //Initialized values for the text file, lines in the file, and number of lines.
    ifstream myfile;
    string line;
    int numLines = 0;

    //USED TO GET INPUT FROM USER
    // string file;
    // cout << "Enter the program file name to run: " << endl << endl;
    // cin >> file;
    // myfile.open(file, ios::in);
    // if (myfile.is_open()) {
    //     while(getline(myfile, line)) {
    //         numLines = numLines + 1;
    //     }
    // }


    // //Find number of lines in the text file to get a limit for the while loop.
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
        //Get machine code from fetch using global variable pc
        string fetchedCode = "";
        fetchedCode = Fetch(&pc);
        
        //Decode the machine code from fetched.
        Decode(fetchedCode, &pc, &jump_target);

        //MIGHT CHANGE FOR DECODE TO BE INSED Fetch()

        //ControlUnit() inside of decode where control signals and alu-code will be generated

        //Execute() is inside of Decode function handles alu computations will return computation

        //Mem() accessed with address from execute, 

        //Print out modified registers and the current clock cycle
        cout << "total_clock_cycles " << total_clock_cycles << " :" << endl;
        cout << "register is modified to " << endl;
        
        //pc in decimal, changed to hex
        stringstream ss;
        ss << std::hex << pc;
        string hexpc(ss.str());
        cout << "pc is modified to 0x" << hexpc << endl << endl;
    }
    
    //Print terminated program and total clock cycles
    cout << "program terminated: " << endl;
    cout << "total execution time is " << total_clock_cycles << " cycles" << endl;
    
    return 0;
}