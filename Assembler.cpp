#include <iostream>
#include <fstream>
#include <bitset>
#include <locale>
#include <ctype.h>
#include <map>

using namespace std;

// Coder receive string dest, comp jump from Parser 
// and returns string of int (ex. "010") according to the input
// called only when the command is C_COMMAND
struct Coder {
  string dest;
  string comp;
  string jump;
  
  //determines the type of dest
  string whichDest(string dest) {
    string dest_bin; // dest(binary)
    if (dest == "M") {dest_bin = "001";}
    else if (dest == "D") {dest_bin = "010";}
    else if (dest == "MD") {dest_bin = "011";}
    else if (dest == "A") {dest_bin = "100";}
    else if (dest == "AM") {dest_bin = "101";}
    else if (dest == "AD") {dest_bin = "110";}
    else if (dest == "AMD") {dest_bin = "111";}
    else {dest_bin = "000";}
    return dest_bin;
  }

  //determines the type of comp
  string whichComp(string comp) {
    string comp_bin;
    if (comp == "0") {comp_bin = "0101010";}
    else if (comp == "1") {comp_bin = "0111111";}
    else if (comp == "-1") {comp_bin = "0111010";}
    else if (comp == "D") {comp_bin = "0001100";}
    else if (comp == "A") {comp_bin = "0110000";}
    else if (comp == "M") {comp_bin = "1110000";}
    else if (comp == "!D") {comp_bin = "0001101";}
    else if (comp == "!A") {comp_bin = "0110001";}
    else if (comp == "!M") {comp_bin = "1110001";}
    else if (comp == "-D") {comp_bin = "0001111";}
    else if (comp == "-A") {comp_bin = "0110011";}
    else if (comp == "-M") {comp_bin = "1110011";}
    else if (comp == "D+1") {comp_bin = "0011111";}
    else if (comp == "A+1") {comp_bin = "0110111";}
    else if (comp == "M+1") {comp_bin = "1110111";}
    else if (comp == "D-1") {comp_bin = "0001110";}
    else if (comp == "A-1") {comp_bin = "0110010";}
    else if (comp == "M-1") {comp_bin = "1110010";}
    else if (comp == "D+A") {comp_bin = "0000010";}
    else if (comp == "D+M") {comp_bin = "1000010";}
    else if (comp == "D-A") {comp_bin = "0010011";}
    else if (comp == "D-M") {comp_bin = "1010011";}
    else if (comp == "A-D") {comp_bin = "0000111";}
    else if (comp == "M-D") {comp_bin = "1000111";}
    else if (comp == "D&A") {comp_bin = "0000000";}
    else if (comp == "D&M") {comp_bin = "1000000";}
    else if (comp == "D|A") {comp_bin = "0010101";}
    else if (comp == "D|M") {comp_bin = "1010101";}
    else {comp_bin = "0101010";}
    return comp_bin;
  }

  //determines the type of comp
  string whichJump(string jump) {
    string jump_bin;
    if (jump == "JGT") {jump_bin = "001";}
    else if (jump == "JEQ") {jump_bin = "010";}
    else if (jump == "JGE") {jump_bin = "011";}
    else if (jump == "JLT") {jump_bin = "100";}
    else if (jump == "JNE") {jump_bin = "101";}
    else if (jump == "JLE") {jump_bin = "110";}
    else if (jump == "JMP") {jump_bin = "111";}
    else {jump_bin = "000";}
    return jump_bin;
  }

  //stores dest, comp, jump value to structure member variables
  void code(string deststr, string compstr, string jumpstr) {
    dest = whichDest(deststr);
    comp = whichComp(compstr);
    jump = whichJump(jumpstr);
  }
};

// Parser receives a line of command to:
// 1) determine the commandtype (A or C_COMMAND)
// 2) if A_COMMAND, obtains the string of int for the address in 15 bits binary
// 3) if C_COMMAND, obtains each part in string (deststr, compstr, jumpstr)
struct Parser {
  string command; //stores the value of assemblie command (string)
  //string binarycommand; //stores the value of coded command (in binary)
  string deststr;
  string compstr;
  string jumpstr;
  string address;
  string commandtype;

  map<string, int> symboltable;
  int nextSymbolNum;

  void reset() {
    command = ""; 
    deststr = "";
    compstr = "";
    jumpstr = "";
    address = "";
    commandtype = "";
  }

  void makeSymboltable() {
    nextSymbolNum = 16;
    symboltable["SP"] = 0x0000;
    symboltable["LCL"] = 0x0001;
    symboltable["ARG"] = 0x0002;
    symboltable["THIS"] = 0x0003;
    symboltable["THAT"] = 0x0004;
    symboltable["R0"] = 0x0000;
    symboltable["R1"] = 0x0001;
    symboltable["R2"] = 0x0002;
    symboltable["R3"] = 0x0003;
    symboltable["R4"] = 0x0004;
    symboltable["R5"] = 0x0005;
    symboltable["R6"] = 0x0006;
    symboltable["R7"] = 0x0007;
    symboltable["R8"] = 0x0008;
    symboltable["R9"] = 0x0009;
    symboltable["R10"] = 0x000a;
    symboltable["R11"] = 0x000b;
    symboltable["R12"] = 0x000c;
    symboltable["R13"] = 0x000d;
    symboltable["R14"] = 0x000e;
    symboltable["R15"] = 0x000f;
    symboltable["SCREEN"] = 0x4000;
    symboltable["KBD"] = 0x6000;
  }

  //determines the commandtype
  string commandType() {
    string commandtype;
    if ((command.substr(0, 2) == "//") || (command.size() == 0)) {
      commandtype = "SKIP";
    } else if (command[0] == '(') {
      commandtype = "SYMBOL_DEF";
    } else if (command[0] == '@') {
      commandtype = "A_COMMAND";
    }
    else {
      commandtype = "C_COMMAND";
    }
    return commandtype;
  }


/*
//called only when the command includes '='
  string dest() {
    string deststr;
    for (int i = 0; i < command.size(); i++) {
      if (command[i] != '=') {
        deststr.push_back(command[i]);
      } else { break; }
    }
    return deststr;
  }

//called any time
  string comp() {
    string compstr;
    
    //setting up start and end variables
    bool start = true;
    if (command.find('=')) {
      start = false;
    }
    bool end = false;

    for (int i = 0; i < command.size(); i++) {
      if (command[i] == ';') {
        end = true;
      }
      if (end) break;
      if (start && !end) {
        compstr.push_back(command[i]);
      }
      if (command[i] == '=') {
        start = true;
      }
    }

    return compstr;
  }

//called only when the command includes ';'
  string jump() {
    string jumpstr;
    bool start = false;
    for (int i = 0; i < command.size(); i++) {
      if (start) {
        jumpstr.push_back(command[i]);
      }
      if (command[i] == ';') {
        start = true;
      }
    }
  }
*/

  //called only when the command includes '@' (a_command)
  string symbol() {
    string symbol = command.substr(command.find('@')+1);
    return symbol;
  }

  //adds a new symbol and an associated decimal number to the symbol table
  void addSymbol(int line_num) {
    string symbol = command.substr(command.find('(')+1, command.find(')')-1);
    int symbolInNum = line_num + 1;
    symboltable[symbol] = line_num;
  }

  void addVariable(string variable) {
    symboltable[variable] = nextSymbolNum;
    nextSymbolNum++;
  }
  
  //if A_COMMAND, assign values to deststr, compstr, and jumpstr according to the given command
  //if C_COMMAND, assign a value to address, which is a string of int in binary after '@'
  void parse() {
    commandtype = commandType(); //A, C, or L_COMMAND

//A_COMMAND
    if (commandtype == "A_COMMAND") {
      address = command.substr(command.find('@')+1);
    }
//C_COMMAND
    else if (commandtype == "C_COMMAND") {
      //extracting the dest, comp, jump (string)

      string command_comp_jump = command;
      if (command.find('=')) {
        deststr = command.substr(0, command.find('='));
        command_comp_jump = command.substr(command.find('=')+1);
      }
      if (command.find(';')) {
        compstr = command_comp_jump.substr(0, command.find(';'));
        jumpstr = command_comp_jump.substr(command.find(';')+1);
      } else {
        compstr = command_comp_jump;
      }
    }
  }
};

//removes whitespace and clean the input string
void removeSpaces(string &str) {
  int count = 0;
  for (int i = 0; i < str.size(); i++) {
    if ((str[i] != ' ') && (str[i] != '\n') && (str[i] != '\t') && (str[i] != '\v')) {
      str[count] = str[i];
      count++;
    }
  }
  str.erase(str.begin()+count, str.end());
}

//convert the string form of decimal number to the string form of binary number
string getBinaryAddressInString(string address) {
  int address_int = stoi(address);
  string binary_address = bitset<15>(address_int).to_string();
  return binary_address;
}

bool isNumeric(string s) {
  for (int i = 0; i < s.size(); i++) {
    if (!isdigit(s[i])) {
      return false;
    }
  }
  return true;
}

// main function
// input: the name of the asm file you want to convert to Hack
//        (example) Add.asm or Add
int main() {
  //.asm file
  string filename;
  cin >> filename;

  //construct .hack file name from the input .asm file name
  string ofilename;
  if (filename.find('.')) {
    ofilename = filename.substr(0, filename.find('.')) + ".hack";
  } else { ofilename = filename + ".hack"; }
  
  filename = "Examples (in assembly)/" + filename;
  ofilename = "Outputs (in Hack)/" + ofilename;
  
  //.asm file to read and .hack file to write on
  ifstream assemblie(filename);
  ofstream ofile(ofilename);

  //stores which line we are looking at in the .asm file
  int line_count = 0;

  //placeholder for each line of commands in the getline function
  string line;

  //create instances for Parser and Coder
  Parser parser;
  Coder coder;

  parser.makeSymboltable();

  bool incrementLine = false;

  // FIRST ITERATION
  while(getline(assemblie, line)) {
    //erace the '\n' and '\r' in the end of each line
    if ((line[line.size() - 1] == '\n') || (line[line.size() - 1] == '\r')) {
      line.erase(line.size() - 1);
    }

    //clean the command
    removeSpaces(line);

    incrementLine = false;

    parser.command = line;
    parser.parse();

    if (parser.commandtype == "SYMBOL_DEF") {
      parser.addSymbol(line_count);
    } else if (parser.commandtype == "A_COMMAND" || parser.commandtype == "C_COMMAND") {
      incrementLine = true;
    }

    if (incrementLine) { line_count++; }
  }

  //reset the line to the beginning for the getline function
  assemblie.clear();
  assemblie.seekg(0);
  line_count = 0;

  //SECOND ITERATION
  while(getline(assemblie, line)) {
    //erace the '\n' and '\r' in the end of each line
    if ((line[line.size() - 1] == '\n') || (line[line.size() - 1] == '\r')) {
      line.erase(line.size() - 1);
    }

    //placeholder for the return value of binarycommand
    string binarycommand;

    //clean the command
    removeSpaces(line);

    parser.reset();

    //assign the current line to parser's command and parse it
    parser.command = line;
    parser.parse();

    //assign a value to binarycommand according to the commandtype
    if (parser.commandtype == "SKIP" || parser.commandtype == "SYMBOL_DEF") {
      continue;
    } else if (parser.commandtype == "A_COMMAND") {
      string address = parser.symbol();
      string address_bin;
      if (isNumeric(address)) {
        address_bin = getBinaryAddressInString(parser.address);
      } else {
        if ( !parser.symboltable.count(address) ) {
          parser.addVariable(address);
        }
        int address_dec = parser.symboltable.at(address);
        address_bin = bitset<15>(address_dec).to_string();
      }
      binarycommand = "0" + address_bin;
    } else if (parser.commandtype == "C_COMMAND") {
      coder.code(parser.deststr, parser.compstr, parser.jumpstr);
      binarycommand = "111" + coder.comp + coder.dest + coder.jump;
    }

    //adds a new line except for before the first line, and then binarycommand
    if (line_count != 0) {
      ofile << endl;
    }
    ofile << binarycommand;

    //increments line_count
    line_count++;
  }

  //close the files
  assemblie.close();
  ofile.close();
}

