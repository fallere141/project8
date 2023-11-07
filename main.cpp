#include <iostream>
#include <string>
#include "basic_instruction.h"
#include <fstream>
#include "project0.h"
#include <filesystem>
#include <regex>


//in struction function <function_nam> <int>
// get a para
std::string function() {
    std::string buffer_out;
    buffer_out.append("D=0\n"
                      "@SP\n"
                      "A=M\n"
                      "M=D\n"
                      "@SP\n"
                      "M=M+1\n");
    return buffer_out;

}

//in struction call <function_name>
// push back_address,arg,this,that,lcl
std::string push(std::string pointer, bool is_function_name) {
    std::string buffer_out;
    buffer_out.append("@" + pointer + "\n");
    if (is_function_name)
        buffer_out.append("D=A\n");
    else
        buffer_out.append("D=M\n");

    buffer_out.append("@SP\n");
    buffer_out.append("AM=M+1\n");
    buffer_out.append("A=A-1\n");
    buffer_out.append("M=D\n");
    return buffer_out;
}

//in call <function_name>
// arg=sp-n-5 and lcl=sp
std::string call_function_arg_and_goto(int arg) {
    std::string buffer_out;
    buffer_out.append("@SP\n");
    buffer_out.append("D=M\n");
    buffer_out.append("@LCL\n");
    buffer_out.append("M=D\n");
    buffer_out.append("@" + std::to_string(5 + arg) + "\n");
    buffer_out.append("D=D-A\n");
    buffer_out.append("@ARG\n");
    buffer_out.append("M=D\n");
    return buffer_out;

}

//translate call function instruction
std::string call_function(std::string label_front, std::string function_name, int arg, int &call_num) {
    std::string buffer_out;
    std::vector<std::string> push_iterator = {"LCL", "ARG", "THIS", "THAT"};
    //push return address
    buffer_out = push(function_name + "RET" + std::to_string(call_num), true);
    //push LCL,ARG,THIS,THAT
    for (const auto &i: push_iterator) {
        buffer_out.append(push(i, false));
    }
    buffer_out.append(call_function_arg_and_goto(arg));
    buffer_out.append("@" + function_name + "\n");
    buffer_out.append("0;JMP\n");
    //use ret+call_num to avoid jump confusion
    buffer_out.append("(" + function_name + "RET" + std::to_string(call_num) + ")\n");
    call_num++;
    return buffer_out;
}

// translate instructions
void translater(std::string filename, std::ofstream &outfile, int &id, int &call_num) {


    filename = delete_intend(filename);
    std::ifstream infile(filename);
    std::string buffer_in, buffer_out;
    std::string filename_out(filename);
    while (filename_out.back() != '.') {
        filename_out.pop_back();
    }
    std::string label_front = filename.substr(filename.find_last_of('/') + 1);
    label_front.erase(label_front.find_last_of('.'));
    label_front.append("$");
//    int call_num = 0;

// these are instructions translated by project7 program
    std::vector<std::string> project7_instruction = {"pop", "push", "add", "sub", "neg", "eq", "gt", "lt", "and",
                                                     "or", "not"};

    //get a line from a file and tranlate it
    while (std::getline(infile, buffer_in)) {
        Trimmed(buffer_in);
        // split the instruction
        std::vector<std::string> list_str = split(buffer_in);
        buffer_out = "// " + buffer_in + "\n";

        // translate project 7's instructions
        if (std::find(project7_instruction.begin(), project7_instruction.end(), list_str[0]) !=
            project7_instruction.end()) {
            buffer_out = WriteBasicInstruction(buffer_in, id, label_front);
        } else {
            // label instruction
            if (list_str[0] == "label") {
                std::string label = label_front + list_str[1];
                buffer_out.append("(" + label + ")\n");
                //if goto instruction
            } else if (list_str[0] == "if-goto") {
                std::string label = label_front + list_str[1];
                buffer_out.append("@SP\n");
                buffer_out.append("M=M-1\n");
                buffer_out.append("A=M\n");
                buffer_out.append("D=M\n");
                buffer_out.append("@" + label + "\n");
                buffer_out.append("D;JNE\n");
                // goto instruction
            } else if (list_str[0] == "goto") {
                std::string label = label_front + list_str[1];
                buffer_out.append("@" + label + "\n");
                buffer_out.append("0;JMP\n");

                // function start define instruction
            } else if (list_str[0] == "function") {

                buffer_out.append("(" + list_str[1] + ")\n");

                //repeat get arg for arg numbers
                int i = stoi(list_str[2]);
                for (int j = 0; j < i; ++j)buffer_out.append(function());

                //call function instruction
            } else if (list_str[0] == "call") {
                buffer_out.append(call_function(label_front, list_str[1], stoi(list_str[2]), call_num));
                //return instruction (I'm sorry, but it's to long and same...)
            } else if (list_str[0] == "return") {
                buffer_out.append(
                        "@LCL\nD=M\n@R13\nM=D\n@R13\nD=M\n@5\nD=D-A\nA=D\nD=M\n@R14\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n@ARG\nD=M\n@SP\nM=D+1\n@R13\nD=M\n@1\nD=D-A\nA=D\nD=M\n@THAT\nM=D\n@R13\nD=M\n@2\nD=D-A\nA=D\nD=M\n@THIS\nM=D\n@R13\nD=M\n@3\nD=D-A\nA=D\nD=M\n@ARG\nM=D\n@R13\nD=M\n@4\nD=D-A\nA=D\nD=M\n@LCL\nM=D\n@R14\nA=M\n0;JMP\n");
            }
        }

        //write into buffer
        outfile << buffer_out;
        buffer_out.clear();
    }
}


int main(int argc, char **argv) {

    //id for compare jump like lt,gt...
    int id = 0;

    std::string strPath(argv[1]);
    std::string relative_path(argv[0]);

    //find vm file to translate
    std::regex fileSuffix("(.*)(.vm)");

    //output file name
    std::string asm_name = strPath + "/" + strPath.substr(strPath.find_last_of('/') + 1) + ".asm";
    std::ofstream outfile;
    outfile.open(asm_name, std::ios::out);
    std::cout << asm_name << std::endl;

    //function ret num
    int call = 0;
    std::string buffer_out;

    //start set
    buffer_out.append("@256\nD=A\n@SP\nM=D\n");

    //call sys.init
    buffer_out.append(call_function("Sys.init", "Sys.init", 0, call));
    outfile << buffer_out;
    buffer_out.clear();

    //iter for the dictionary
    for (auto &DirectoryIter: std::__fs::filesystem::directory_iterator(strPath)) {
        auto filepath = DirectoryIter.path();
        auto filename = filepath.filename();

        //check if vm file
        if (std::regex_match(filename.string(), fileSuffix)) {

            // give comments in asm
            outfile << "//////" << std::endl;
            outfile << "// " << filename.replace_extension().string() << std::endl;
            //translate
            translater(filepath.string(), outfile, id, call);
            std::cout << filepath << std::endl;
        }
    }
    return 0;
}
