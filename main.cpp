#include <iostream>
#include <fstream>
#include <string>
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

//jal and jr Global control signals
int jalRegDst = 0;
int jalMemToReg = 0;
int jrJump = 0;

//2 register files for both sample parts. Top is for part 1, bottom is part 2.    
static int registerfile [64] = { 0x0, 0x0, 0, 0, 0, 0, 0, 0, 0, 0x20, 5, 0, 0, 0, 0, 0, 0x70};
//static int registerfile [64] = { 0x0, 0x0, 0, 0, 0x5, 0x2, 0, 0xa, 0, 0, 0, 0, 0, 0, 0, 0, 0x20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

//This will help when finding the index of the registers, by comparing it to the first register ($zero)
int* zero = registerfile;

//Last step in the cycle, we use the results and write them back to registers. Print modified content.
void Writeback(int* dataMemory, int result) {
    
    //Increment clock cycles
    total_clock_cycles = total_clock_cycles + 1;

    //Print out modified registers and the current clock cycle
    cout << "total_clock_cycles " << total_clock_cycles << " :" << endl;

    //R-type instruction changes, excluding jr.
    if (regWrite == 1 && jrJump == 0) {
        //Find index with dataMemory address - $zero
        int index = dataMemory - zero;
        //Change registerfile[index] to the result from function call
        registerfile[index] = result;
        
        //Turn result into hex string to print in hex
        stringstream ss;
        ss << hex << result;
        string res ( ss.str() );

        //Prints out modified contents
        cout << "$";
        //Switch case using register index to register names. ($3 = $v0, $9 = $t0)    
        switch (index)
        {
        case 0:
            cout << "Oi, what are you doing?! This shouldn't be touched." << endl;
            break;
        case 1:
            cout << "at is modified to 0x" << res;
            break;
        case 2:
            cout << "v0 is modified to 0x" << res;
            break;
        case 3:
            cout << "v1 is modified to 0x" << res;
            break;
        
        case 4:
            cout << "a0 is modified to 0x" << res;
            break;
        case 5:
            cout << "a1 is modified to 0x" << res;
            break;
        case 6:
            cout << "a2 is modified to 0x" << res;
            break;
        case 7:
            cout << "a3 is modified to 0x" << res;
            break;
        case 8:
            cout << "t0 is modified to 0x" << res;
            break;
        case 9:
            cout << "t1 is modified to 0x" << res;
            break;
        case 10:
            cout << "t2 is modified to 0x" << res;
            break;
        case 11:
            cout << "t3 is modified to 0x" << res;
            break;
        case 12:
            cout << "t4 is modified to 0x" << res;
            break;
        case 13:
            cout << "t5 is modified to 0x" << res;
            break;
        case 14:
            cout << "t6 is modified to 0x" << res;
            break;
        case 15:
            cout << "t7 is modified to 0x" << res;
            break;
        case 16:
            cout << "s0 is modified to 0x" << res;
            break;
        case 17:
            cout << "s1 is modified to 0x" << res;
            break;
        case 18:
            cout << "s2 is modified to 0x" << res;
            break;
        case 19:
            cout << "s3 is modified to 0x" << res;
            break;
        case 20:
            cout << "s4 is modified to 0x" << res;
            break;
        case 21:
            cout << "s5 is modified to 0x" << res;
            break;
        case 22:
            cout << "s6 is modified to 0x" << res;
            break;
        case 23:
            cout << "s7 is modified to 0x" << res;
            break;
        case 24:
            cout << "t8 is modified to 0x" << res;
            break;
        case 25:
            cout << "t9 is modified to 0x" << res;
            break;
        case 26:
            cout << "k0 is modified to 0x" << res;
            break;
        case 27:
            cout << "k1 is modified to 0x" << res;
            break;
        case 28:
            cout << "gp is modified to 0x" << res;
            break;
        case 29:
            cout << "sp is modified to 0x" << res;
            break;
        case 30:
            cout << "fp is modified to 0x" << res;
            break;
        case 31:
            cout << "ra is modified to 0x" << res;
            break;

        default:
            break;
        }
        cout << endl;
    }


    //Memwrite used by sw, will store the value of a register into memory
    if (memWrite == 1) {
        //Changing decimal to hex for printout
        stringstream s1;
        s1 << hex << *dataMemory;
        string value ( s1.str() );
        
        stringstream s2;
        s2 << hex << result;
        string memory ( s2.str() );

        //Prints memory address and the value that was stored in memory address
        cout << "memory 0x" << memory << " is modified to 0x" << value << endl;
    }

    //This is the write back for the jal instruction. From memory to register
    if (jalMemToReg == 1) {
        //Use result which is address from memory, sets return address for jal
        // result here is the address field from jump instructions
        registerfile[*dataMemory] = result / 4;

        //Change decimal to hex for printout
        stringstream s3;
        s3 << hex << registerfile[31];
        string raValue (s3.str());

        //Print out changes to ra using value of registerfile[*dataMemory]
        cout << "$ra is modified to 0x" << raValue << endl;
    }
}

//Memory accessed through memWrite and memRead, (lw, sw)
int Mem(int* address, int result) {
    
    //Alternate d_mem, depending on sample text used
    //sample text 1 d_mem
    static int d_mem[31] = {0x0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x5, 0x10 };
    
    //sample text 2 d_mem
    //static int d_mem[31] = { 0x0 };

    //If lw then memory at address should be return to writeback to be entered int registerfile
    if (memRead == 1) {
        //
        *address = d_mem[result];
        return *address;
    }
    //sw should save the value at address(register value) into memory (d_mem);
    else if (memWrite == 1) {
        d_mem[result] = *address;
        return d_mem[result];
    }
    
    else {
        return result;
    }
    
}

//Execute used decode information and alu-op for logic and calculations
void Execute(int* rs, int* rt, int* rd, int offset, int* pc, string alu_op) {
    
    alu_zero = 0;
    int result = 0;
    int index = 0;
    int* returnAddress;
    int* registerValue;
    //Set aluop from string to int for switch case
    int aluOp = stoi(alu_op);

    //Switch case for which arithmetic based on alu-op
    switch(aluOp) {
        //AND statement
        case 0:
            if ((*rs & *rt) == 1) result = 1;
            else result = 0;
            break;
        //OR
        case 1:
            if ((*rs | *rt) == 1) result = 1;
            else result = 0;
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
            else result = 0;
            break;

        //NOR
        
        case 1100:
            if ((*rs || *rt) != 1) result = 1;
            else result = 0;
            break;
        default:
            break;
    }
    //MUX for regDst
    //regDst 0 means rt is the write register, if regDst is 1, then rd is the write register
    //Sets return address used to write result into register
    if (regDst == 1) {
        returnAddress = rd;
    }
    //If a jal function is being used, we will use 31 as the return address for $ra
    else if (jalRegDst == 1){
        *returnAddress = 31; 
    }
    //Common return address set as rt
    else {
        returnAddress = rt;
    }

    //Several logics using control signals to set address or values to be used in Mem(), Writeback(), or both.
    if (jrJump == 1) {
        *pc = *rs;
    }
    if (jalMemToReg == 1) {
        //This result will be sent to register ra ie 31
        result = (*pc) * 4;
    }
    //Datapath For if branch is enabled, compare values for outcome (beq)
    if ((branch == 1) && (*rs == *rt)) {
        branch_target = (offset * 4) + *pc;
        *pc = branch_target;
    }
    //Datapath for jump
    //If jump control signal is enabled, the pc = offset/immediate shift left by 2
    if (jump == 1) {
        jump_target = (offset * 4 );
        *pc = jump_target;
    }
    //Datapath for lw 
    //lw 
    if (aluSrc == 1 && memToReg == 1 && regWrite == 1 && memRead == 1) {
        index = ((*rs) + (offset)) / 4;
        //lw uses Mem to grab value to be returned for regWrite
        result = Mem(registerValue, index);
    }
    //Datapath for sw
    //sw
    if (aluSrc == 1 && memWrite == 1) {
        *registerValue = ((*rs) + (offset));
        //sw uses Mem to use register value, and stores it into memory
        // returns value in memory
        result = Mem(registerValue, index);
    }

    //Using result and addresses calculated in Execute(), we show the modified content and 
    // change registerfile where needed
    Writeback(returnAddress, result);
}

//Creates signals that will let execute know several logic paths to take as well as
// calculate the opcode from ALU Control
string ControlUnit(string opcode, string funct) {
    
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
    jalMemToReg = 0;
    jalRegDst = 0;
    jrJump = 0;

    //ALU Control, determined by opcode and function
    if (opcode == "000000") {
        //Set global values for R-type instruction
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
        // JR
        else if (funct == "001000") {
            jrJump = 1;
        }
        else {
            std::cout << "ERROR! FUNCT NOT FOUND!" << endl;
        }
    }
    
    //Set control signals depending on the operation
    // lw,  alu-op = ADD for   offset  +  base register rs
    else if (opcode == "100011") {
        aluSrc = 1; memToReg = 1; regWrite = 1; memRead = 1; alu_op = "0010";
    }
    // sw, alu-op = ADD for     offset     + base register rs
    else if (opcode == "101011") {
        aluSrc = 1; memWrite = 1; alu_op = "0010";
    }
    // beq, subtract alu-op for comparing
    else if (opcode == "000100" ) {
        //branch true, instruction type branch, and alu-op is SUB for determining if same
        branch = 1; instType = 1; alu_op = "0110";
    }
    // j
    else if (opcode == "000010") {
        jump = 1; 
    }
    // jal
    else if (opcode == "000011") {
        jump = 1; jalMemToReg = 1; jalRegDst = 1;
    }
    // No matching opcode
    else {
        std::cout << "ERROR! OPCODE NOT FOUND!" << endl;
    }
    return alu_op;
}

// Takes machine code from fetch, int pointer pc, and int pointer jump target
void Decode(string machineCode, int* pc, int* jump_target) { 

    //Initialized values
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
        //Need function to figure out the operation
        funct = inst.substr(26, 6);
        bitset<6> set6(funct);
        //If else for all operations of R type.
        //Go through all instructions until we have a matching function code
        //Print out operations
        // if (set6.to_ulong() == 32) {
        //     cout << "Operation: add" << endl;
        // }
        // else if (set6.to_ulong() == 36) 
        // {
        //     cout << "Operation: and" << endl;
        // }
        // else if (set6.to_ulong() == 8) 
        // {
        //     cout << "Operation: jr" << endl;
        // }
        // else if (set6.to_ulong() == 39) 
        // {
        //     cout << "Operation: nor" << endl;
        // }
        // else if (set6.to_ulong() == 37) 
        // {
        //     cout << "Operation: or" << endl;
        // }
        // else if (set6.to_ulong() == 42) 
        // {
        //     cout << "Operation: slt" << endl;
        // }
        // else if (set6.to_ulong() == 34) 
        // {
        //     cout << "Operation: sub" << endl;
        // }
        // //No matching then this should be a problem.
        // else {
        //     cout << "ERROR!" << endl;
        //     cout << set6.to_ulong() << endl;
        // }

        //New strings for rs, rt, rd, and shamt, with a bitset of 5 for each.
        // and convert all bitsets into decimal.
        string rs = inst.substr(6, 5);
        bitset<5> set2(rs);
        //Turn machine code into register file id
        index = stoi(rs, nullptr, 2);
        //File ID -> value 
        Rs = &(registerfile[index]);

        string rt = inst.substr(11, 5);
        bitset<5> set3(rt);
        index = stoi(rt, nullptr, 2);
        Rt = &(registerfile[index]);
        
        string rd = inst.substr(16, 5);
        bitset<5> set4(rd);
        //Turn machine code into register file id
        index = stoi(rd, nullptr, 2);
        Rd = &(registerfile[index]);

        string shamt = inst.substr(21, 5);
        bitset<5> set5(shamt);
    
    }
    //If opcode is not zero, then it is either j or i type instruction
    else {
        //If opcode is 2, then j instruction
        if (set0.to_ulong() == 2){

            //Substring of the instruction with a bitset of 26 for the address/immediate
            string address = inst.substr(6, 26);
            bitset<26> set7(address);
            //Convert bitset to decimal and hex
            //Set jump target and the offset used in Execute()
            // offset = address << 2   shift left by two 
            int jt = stoi(address, nullptr, 2);
            offset = jt;
            *jump_target = offset;
            Rs = &(registerfile[0]);
            Rt = &(registerfile[0]);
            Rd = &(registerfile[0]);
            
        }
        //If opcode is 3, then jal 
        else if (set0.to_ulong() == 3)
        {
            //cout << "Instruction Type: J" << endl;\
            //Bit set 
            string address = inst.substr(6, 26);
            bitset<26> set8(address);
            // Using address bit set, shift left 2, and set offset
            offset = stoi(address, nullptr, 2);
            Rs = &(registerfile[0]);
            Rt = &(registerfile[0]);
            Rd = &(registerfile[0]);
        }
        //Finally, it must be an I - type instruction
        else {

            //If else for all operations of I type.

            // if (set0.to_ulong() == 4)
            // {
            //     cout << "Operation: beq" << endl;
            // }
            // else if (set0.to_ulong() == 35)
            // {
            //     cout << "Operation: lw" << endl;
            // }
            // else if (set0.to_ulong() == 43)
            // {
            //     cout << "Operation: sw" << endl;
            // }
            // else {
            //     cout << "ERROR!" << endl;
            // }

            // Find rs and rt from bit set
            string rs = inst.substr(6, 5);
            bitset<5> set9(rs);
            index = stoi(rs, nullptr, 2);
            Rs = &(registerfile[index]);

            string rt = inst.substr(11, 5);
            bitset<5> set10(rt);
            index = stoi(rt, nullptr, 2);
            Rt = &(registerfile[index]);

            //Set defaults for values that are not included in all instruction formats
            Rd = &(registerfile[0]);

            //Bitset of 26 and converting to decimal and hex 
            string immediate = inst.substr(16, 16);
            bitset<26> set11(immediate);
            //cout << "Immediate: " << dec << set11.to_ulong() << " (or 0x" << hex << set11.to_ulong() << ")" << endl;
            //Calculate offset as shift left 2 of immediate
            offset = stoi(immediate, nullptr, 2);
        }
    }

    //Use Control unit to set control signals and return alu_op for Execute;
    // opcode and funct were calculated in decode
    alu_op = ControlUnit(opcode, funct);

    //Using items from decode and Control Unit, we can execute.
    Execute(Rs, Rt, Rd, offset, pc, alu_op);
}

// Fetch machine code one at a time using program counter, and filename
void Fetch(int *pc, string filename) {

    //Initialized values for file, line, machine code string, totalLines, and linenumber
    fstream myfile;
    string line;
    string machineCode;
    int totalLines;
    int lineNumber = 0;

    //Open file and and iterate through machine cod
    myfile.open(filename, ios::in);
    if (myfile.is_open()) {
        while(getline(myfile, line)) {
            if (lineNumber == *pc){
                machineCode = line;
            }
            lineNumber = lineNumber + 4;
        }
    }   

    // set next pc from pc + 4
    int next_pc = *pc + 4;
    *pc = next_pc;

    //Using machine code call fetch
    Decode(machineCode, pc, &jump_target);
}

//Gets file and starts with Fetch();
int main() {

    //Initialized values for the text file, lines in the file, and number of lines.
    ifstream myfile;
    string line;
    int numLines = 0;

    //Get file name from user input
    string file;
    cout << "Enter the program file name to run: " << endl << endl;
    cin >> file;
    cout << endl;

    //Find number of lines in the text file to get a limit for the while loop.
    myfile.open(file);
    
    if (myfile.is_open(), ios::in) {
        while(getline(myfile, line)) {
            numLines = numLines + 1;
        }
    }

    //maxAddress will be the end of the text file 
    int maxAddress = numLines * 4;

    // Cycle of fetching machine code from file
    while (pc < maxAddress)
    {
        //Fetch to start cycle and grab machine code using program counter
        Fetch(&pc,file);
        
        //Decode called after fetch, uses machine code grabbed from fetch

        //ControlUnit() inside of decode where control signals and alu-code will be generated

        //Execute() is inside of Decode function handles calculations using alu op and control signals

        //Mem() accessed with address from in Execute()

        //Writeback() accessed with modification and modified from execute()
        
        //pc in decimal, changed to hex
        stringstream ss;
        ss << hex << pc;
        string hexpc(ss.str());
        cout << "pc is modified to 0x" << hexpc << endl << endl;
    }
    
    //Print program termination and total clock cycles
    cout << "program terminated: " << endl;
    cout << "total execution time is " << total_clock_cycles << " cycles" << endl;

    myfile.close();

    return 0;
    
}