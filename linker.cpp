#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <map>
#include <iomanip>
#include <cctype>

using namespace std;


ifstream input;
int line_number = 0;
int lineoffset = 0;
char* next_token = NULL; // Pointer to the next token
vector<char> line_cstr; // Buffer for the current line
string deflist;
map<string, int> symbol_table;
map<string, int> module_table;
int module = 0;
int instr_no = 0;
bool fileNotEnd;
int def_count;
vector<int> base_addr;
vector<int> memoryMap;



void __parseerror(int errcode) {
    static string errstr[] = {
        "NUM_EXPECTED", // Number expected
        "SYM_EXPECTED", // Symbol expected
        "ADDR_EXPECTED", // Addressing expected which is A/E/I/R
        "SYM_TOO_LONG", // Symbol Name is too long
        "TOO_MANY_DEF_IN_MODULE", // > 16
        "TOO_MANY_USE_IN_MODULE", // > 16
        "TOO_MANY_INSTR" // total num_instr exceeds memory size (512)
    };
    cout << "Parse Error line " << line_number << " offset " << lineoffset << ": " << errstr[errcode] << endl;
    exit(0);
};

bool loadNextLine() {
    string line;
    //cout<<"Entered loadline : data check "<<endl; 
    if (!getline(input, line)) {
        //cout<<"Entered getfile false :"<<endl; 

        fileNotEnd = false;
        return false; // No more lines to read
    }

    line_number++;
    lineoffset = 1; // Reset line offset for the new line
    line_cstr.clear();
    line_cstr.assign(line.begin(), line.end());
    line_cstr.push_back('\0'); // Ensure null-termination for strtok

    // Prepare the first token of the new line
    next_token = strtok(line_cstr.data(), " \t\n\t");
    if(next_token != NULL){
        lineoffset = lineoffset + strlen(next_token);
        return true; //
    }
    
    return true;
}
char* getToken() {
    //cout<<"Entered Token Trial 13: "<<endl; 
    // cout<<"Token value : " << next_token <<endl; 
    //cout<<"Token value2 : "<<endl; 

    if (next_token == NULL) {
        //cout<<"Entered next token null : "<<endl; 

        if(loadNextLine()){
            return next_token;
        }else{
            //cout<<"No more token no more line "<<endl;
            return "-2"; // No more tokens and no more lines to read
        }
    }

    // Retrieve the current token and prepare for the next
    // char* current_token = strtok(nullptr, " \t\n"); // Get the next token in the current line

    else {
        // char * temp = next_token;
        //cout<<"Entered next token not null : "<<endl;
        next_token = strtok(NULL, " \t\n\t");
        if(next_token == NULL){
            return getToken();
        }
        lineoffset = static_cast<int>(next_token - line_cstr.data());
        
    }
    return next_token;   
};

// pass 1 functions
int readInt() {
    char* token = getToken();
    //cout << "readInt token: " << (token ? token : "null") << endl; // Debug print

    if (token == NULL) {
        //cout << "Entererd yuhuuuu "<< endl; // Debug print

        //fileNotEnd = false;
        return -1; // error
    }
    else if (isalpha(token[0])) {
        __parseerror(0); // NUM_EXPECTED
        return -1; // error
    }
    // else if (isalnum(token[0])) {
    //     return -1; // error
    // }
    // else if (!isdigit(token[0])) {
        
    //     return -1; // error
    // }
    
    return stoi(token);    
}
char* readSymbol(){
    char* token = getToken();
    if (token == NULL) {
        //fileNotEnd = false;
        return NULL; // error
    }
    if (isdigit(token[0])) {
        __parseerror(1); // SYM_EXPECTED
        return NULL; // error
    }
    for (int i = 0; i < strlen(token); i++){
        if (!isalnum(token[i])){
            return NULL; // error
        }
    };
    return token;
    
}
char* readMARIE(){
    //cout<<"Enetered readMARIE"<<endl;
    //cout<<"Enetered readMARIE2"<<endl;
    char* token = getToken();
    //cout<<"Token : "<< token <<endl;
    if (token == NULL) {
        return NULL; // error
    }
    if (isdigit(token[0])){
        //cout<<"enetred isdigit"<<endl;
        __parseerror(2);
        //return NULL;  // error
    }
    //if (token[0] != 'A' && token[0] != 'E' && token[0] != 'I' && token[0] != 'R'){
    //    __parseerror(2);
    //    return NULL;  // error
    //}
    if (strlen(token) > 1){
        __parseerror(2);
        return NULL;  // error
    }
    //cout<<"exit readMARIE"<<endl;
    return token;
}
void createSymbol(char * symbol, int def_val, int module){
    string s = symbol;
    symbol_table[s] = def_val;
    module_table[s] = module;
};
int readInst(){
    char* token = getToken();
    if (token == NULL) {
        return -1; // error
    }
    if (isalpha(token[0])){
        __parseerror(0); // NUM_EXPECTED
        return -1;  // error
    }
    return stoi(token);
}

// pass 2 functions
void printSymbolTable() {
    cout << "Symbol Table" << endl;
          
    for (const auto& symbol : symbol_table) {
        //cout << symbol.first << "=" << symbol.second << endl;
        //cout << "module_table: " << module_table[symbol.first] << endl;
        if(module_table[symbol.first]==0){
            cout << symbol.first << "=" << symbol.second << endl;
        }
        else{
            cout << symbol.first << "=" << symbol.second+base_addr[module_table[symbol.first]-1] << endl;
        }

    }

}



bool pass1(){
    // deflist
    fileNotEnd = true;
    module =0;
    while(fileNotEnd!=false){
        
        int def_count = readInt();
        if (def_count == -1){
            __parseerror(0);
        }
        if (def_count > 16){
            __parseerror(4);
        }
        if (def_count > 0){
            
            //cout << "def_count: " << def_count << endl;
            for (int i = 0; i < def_count; i++){
                char* symbol = readSymbol();
                if (symbol == NULL){
                    __parseerror(1);
                }  
                //cout<<"Symbol : "<<symbol<<endl; 
                //cout << "Next token after symbol '" << symbol << "': " << (getToken() ? getToken() : "null") << endl; // Debug only
                int def_val = readInt();
                if (def_val == -1){
                    __parseerror(0);
                }
                // if (symbol && def_val){
                //     cout << "iteration no.:" << i << "symbol: " << symbol << "token:" << def_val << endl;
                // }
                // else{
                //     cout <<" wrong symbol and int value "<< endl;
                // };
                createSymbol(symbol, def_val, module);
                //cout << "iteration no.:" << i << " symbol: <"<< symbol << "> def_value: " << def_val << endl;

            };
        };


        // uselist
        //cout<<"uselist starts here"<<endl;
        int use_count = readInt();
        //cout<<"use count :<< "<<use_count<<endl;
        if (use_count == -1){
            __parseerror(0);
        }
        if (use_count > 16){
            __parseerror(5);
        }
        if (use_count > 0){
            for (int i = 0; i < int(use_count); i++){
                //cout<<"reading uselist symbols"<<endl;
                char* symbol = readSymbol();
                // cout<<"uselist Symbol : "<<symbol <<endl;
                if (symbol == NULL){
                    __parseerror(1);
                }
            };
        };
        
        // instructions
        //cout<<"instructions starts here"<<endl; 
        int inst_count = readInt();
        if (inst_count == -1){
            __parseerror(0);
        }
        if (inst_count > 512){
            __parseerror(6);
        }
        if (inst_count > 0){
            if (inst_count > 512){
                __parseerror(6);
            }
            else{
                if (base_addr.empty()){
                    base_addr.push_back(inst_count);
                }
                else{
                    int z = base_addr.back();
                    if (z + inst_count > 512)
                    {

                        __parseerror(6);
                        return false;
                    }
                    base_addr.push_back(z + inst_count);
                }
            }
            //cout<<"inst_count debugged : "<<inst_count<<endl;
            for (int i = 0; i < inst_count; i++){
                //cout<<"instr sfter  itersyio : "<<i<<endl;
                char* addressmode = readMARIE();
                //cout<<"addressmode debugged is this : "<<addressmode<<endl;
                // if (char x == NULL && char x == 
                if (addressmode = NULL){
                    __parseerror(2);
                }

                else{
                    int instr = readInst();
                    // cout<<"instr deubgged: "<<instr<<endl;
                    if (instr == -1){
                        __parseerror(2);    
                    }
                }
                //cout<<"addressmode : "<<addressmode<<endl;
                
                //int operand = readInst();
                //cout<<"operand debugged: "<<operand<<endl;
                
            }
        }    
        module++;
        // cout<<"module : "<<module<<endl;
        
    }
    return true;
};

bool pass2(){
    int instrcutions;
    int instr_count = 0;
    int moduleSize;
    // deflist
    //cout << "pass 2 module check: " << endl;
    for(int j = 0; j < module; j++){
        // cout << "pass 2 j check: " << j << endl;
        
        int def_count = readInt();
        if (def_count > 0){
            
            for (int i = 0; i < def_count; i++){
                char* symbol = readSymbol();
                int def_val = readInt();
            };
        };
        
        // uselist
        int use_count = readInt();
        // cout << "pass2 use_count: " << use_count << endl;

        char* use_list2[use_count];
        std::vector<int> use_list_addr; 
        if (use_count > 0){
            //use_list2.resize(use_count);
            use_list_addr.resize(use_count, false);
            

            for (int i = 0; i < int(use_count); i++){
                char* symbol = readSymbol();
               
                //use_list2[i] = symbol;
                use_list2[i] = strdup(symbol);
                // cout<< "pass2 use_list[i]: "<< use_list2[i]<< endl;   
            };
            
        };
        
        // instructions
        int inst_count = readInt();
        // cout << "pass2 inst_count: " << inst_count << endl;

        if (inst_count > 0){
            for (int i = 0; i < inst_count; i++){
                char* addressmode = readMARIE();
                // cout << "pass2 addressmode: " << addressmode[0] << endl;
                //char* x = readMARIE();
                int instr = readInst();
                // cout << "pass2 instr: " << instr << endl;
                if (instr > 9999){
                    string error_ = "Error: Illegal opcode; treated as 9999";
                    instr =9999;
                    cout << setw(3) << setfill('0') << instr_count << ": "<< instr<< "" <<error_<<endl;
                    instr_count++;
                }
                int operand = instr % 1000;
                int opcode = instr / 1000;
                int outputOperand = operand; // Default to using the original operand
                switch(addressmode[0]){
                    case 'M':{
                        string error1;
                        if (operand >= module){
                            string error1 = "Error: Illegal module operand; treated as module=0";
                            outputOperand = 0;
                        }
                        else {
                            // No error: update the operand to the calculated module base.
                            outputOperand = base_addr.at(operand); 
                        }
                        // base module addr func
                        // cout << setw(3) << setfill('0') << instr_count << ": "<< opcode << setw(3) << setfill('0') << outputOperand << (error1.empty() ? "" : " " + error1) << endl;
                        break;
                    }
                    case 'A':{
                        string error2;
                        if (operand > 512){
                            string error2 = "Error: Absolute address exceeds machine size; zero used";
                            //cout << error2 << endl;
                            outputOperand = 0;
                        }
                        //setw setfill print
                        cout << setw(3) << setfill('0') << instr_count << ": " << opcode << setw(3) << setfill('0') << operand << " " << error2 << endl;
                        break;
                    }
                    case 'R':{
                        // cout << "pass2 inside R: " << endl;
                        // cout << "pass2 value of i: " << i << endl; // should be 2
                        // cout << "pass2 value of inst_count: " << inst_count << endl; 
                        // cout << "pass2 value of operand: " << operand << endl;
                        // cout << "pass2 value of module: " << j << endl;
                        // for (auto element : base_addr) {
                        //     cout << "pass2 value of base_addr element: " << element << endl;

                        // }
                        // cout << "pass2 value of base addr at 0: " << base_addr.at(0) << endl;
                        // // cout << "pass2 value of base addr at -1: " << base_addr.at(-1) << endl;

                        string error3;
                        if (operand > inst_count){
                            string error3 = "Error: Relative address exceeds module size; relative zero used";
                            if (j == 0){
                                operand = 0;
                            }
                            else{
                                operand = base_addr.at(j-1);
                            }
                        }
                        else{
                            // cout << "entering else of R pass2" << endl; 
                            if(j!=0){
                                outputOperand = operand + base_addr.at(j-1); // op = 1 +  5
                                // cout << "pass2 value of outeroperand: " << outputOperand << endl;
                            }
                            else{
                                operand = 0;

                            }
                            // operand = operand + base_addr.at(i-1);

                        }
                        //cout << "pass2 value of operand: " << outputOperand << endl;
                        //setw setfill print
                        cout << setw(3) << setfill('0') << instr_count << ": "<< opcode << setw(3) << outputOperand << " " << error3 << endl;
                        // cout << "pass2 exit R: " << endl;
                        break;
                    }
                    case 'I':{
                        string error4;
                        if (operand > 900){
                            operand = 999;
                            string error4 = "Error: Illegal immediate value; treated as 9999";
                            //cout << error3 << endl;
                        }
                        cout << setw(3) << setfill('0') << instr_count << ": "<< opcode << setw(3) << operand << " " << error4 << endl;
                        break;
                    }
                    case 'E':{
                        string error5;
                        // cout << "pass2 operand: " << operand << endl;
                        //  for (int i = 0; i < use_count; i++) {
                        //      cout << "module tbale stored is[" << i << "]: " << module_table['z'] << endl;
                        //  }

                        if (operand >= use_count){
                            
                            string error5 = "Error: External operand exceeds length of uselist; treated as relative=0";
                            if (i=0){
                                operand = 0;
                            }
                            else{
                                operand = base_addr.at(module-1);
                            }
                        }
                        else{
                            // cout<<"operand pass2 de: "<<operand<<endl;
                            string symbol_ = use_list2[operand];
                            
                            // cout<<"symbol_ pass2 debug : "<<symbol_<<endl;
                            //
                            // if (symbol_table.find(symbol_) != symbol_table.end()) {
                            //     operand = symbol_table[symbol_];
                            //     cout<<"operand pass2 : "<<operand<<endl;
                            // } else {
                                // cout<<"outputOperand pass2 symboltable is: "<<module_table["xy"]<<endl;
                                
                            outputOperand = base_addr[module_table[symbol_]-1] + symbol_table[symbol_];
                                // cout<<"outputOperand pass2 symboltable is: "<<outputOperand<<endl;
                            
                        }
                        //setw setfill print
                        cout << setw(3) << setfill('0') << instr_count << ": " << opcode << setw(3) << setfill('0') << outputOperand << " " << error5 << endl;
                        break;
                    }
                    default:{
                        break;
                    }
                    instr_count++;
                        
                }
            

            }
        
        }
        for (int i = 0; i < use_count; i++){
            if (use_list_addr[i] == false){
                cout << "Warning: Module " << j << ": " << use_list2[i] << " appeared in the uselist but was not actually used" << endl;
            }
        }   
        return true;
    }
};



int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "No input file specified." << endl;
        return 1; // Exit with an error code.
    }
    input.open(argv[1], ios::in);


    if (!input.is_open()) {
        cout << "Unable to open file" << endl;
        return -1;
    }

    if (!pass1()) {
        cout << "Processing failed during pass1." << endl;
    } 
    // else {
    //     cout << "Pass1 completed successfully." << endl;
    // }
    printSymbolTable();
    input.clear();
    input.close();

    input.open("D:/D_Drive/college_classes/os/lab1_assign_input_output/lab1_assign/input-1", ios::in);
    // ifstream input("D:/D_Drive/college_classes/os/lab1_assign_input_output/lab1_assign/input-1", ios::in);
    // cout << "check input file for pass2" << input << endl;

    // if (!input.is_open()) {
    //     cout << "Unable to open file" << endl;
    //     return -1;
    // }

    if (!input) {
        cout << "Failed to reopen the file." << endl;
    } else {
        cout << "Memory Table" << endl;
        // Proceed with reading the file or other operations.
    }

    if (!pass2()) {
        cout << "Processing failed during pass2." << endl;
    } 
    // else {
    //     cout << "Pass2 completed successfully." << endl;
    // }
  


    input.close();
    return 0;

}

