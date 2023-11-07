#include <iostream>
#include <string>
#include "basic_instruction.h"
#include <fstream>
#include "project0.h"
#include <filesystem>
#include <regex>

//void translater(std::string filename,std::ofstream outfile ,int &id) ;
std::string function(){
    std::string buffer_out;
    buffer_out.append("D=0\n"
                      "@SP\n"
                      "A=M\n"
                      "M=D\n"
                      "@SP\n"
                      "M=M+1\n");
    return buffer_out;

}

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

std::string call_function_arg_and_goto(int arg){
    std::string buffer_out;
    buffer_out.append("@SP\n");
    buffer_out.append("D=M\n");
    buffer_out.append("@LCL\n");
    buffer_out.append("M=D\n");
    buffer_out.append("@"+std::to_string(5+arg)+"\n");
    buffer_out.append("D=D-A\n");
    buffer_out.append("@ARG\n");
    buffer_out.append("M=D\n");
    return buffer_out;

}

std::string call_function(std::string label_front, std::string function_name, int arg,int &call_num) {
    std::string buffer_out;
    std::vector<std::string> push_iterator = {"LCL", "ARG", "THIS", "THAT"};
    //push return address
    buffer_out = push(  function_name + "RET" + std::to_string(call_num), true);
    //push LCL,ARG,THIS,THAT
    for (const auto& i: push_iterator) {
        buffer_out.append(push(i, false));
    }
    buffer_out.append(call_function_arg_and_goto(arg));
    buffer_out.append("@" +   function_name + "\n");
    buffer_out.append("0;JMP\n");
    buffer_out.append("("+function_name + "RET" + std::to_string(call_num)+")\n");
    call_num++;
    return buffer_out;
}


void translater(std::string filename, std::ofstream &outfile, int &id,int &call_num) {

//    std::string filename;
    filename = delete_intend(filename);
    std::ifstream infile(filename);
    std::string buffer_in, buffer_out;
    std::string filename_out(filename);
    while (filename_out.back() != '.') {
        filename_out.pop_back();
    }
//    filename_out.append("asm");
//    std::ofstream outfile;
//    outfile.open(asm_name, std::ios::app);
    std::string label_front = filename.substr(filename.find_last_of('/') + 1);
    label_front.erase(label_front.find_last_of('.'));
    label_front.append("$");
//    int call_num = 0;

    std::vector<std::string> project7_instruction = {"pop", "push", "add", "sub", "neg", "eq", "gt", "lt", "and",
                                                     "or", "not"};
    while (std::getline(infile, buffer_in)) {
        Trimmed(buffer_in);
        // split the instruction
        std::vector<std::string> list_str = split(buffer_in);
        buffer_out = "// " + buffer_in + "\n";

        if (std::find(project7_instruction.begin(), project7_instruction.end(), list_str[0]) !=
            project7_instruction.end()) {
            buffer_out = WriteBasicInstruction(buffer_in, id,label_front);
        } else {
            if (list_str[0] == "label") {
                std::string label = label_front + list_str[1];
//                buffer_out.append("(" + list_str[1] + ")\n");
                buffer_out.append("(" + label + ")\n");
            } else if (list_str[0] == "if-goto") {
                std::string label = label_front + list_str[1];
                buffer_out.append("@SP\n");
                buffer_out.append("M=M-1\n");
                buffer_out.append("A=M\n");
                buffer_out.append("D=M\n");
//                buffer_out.append("@" + list_str[1] + "\n");
                buffer_out.append("@" + label + "\n");
                buffer_out.append("D;JNE\n");
            } else if (list_str[0] == "goto") {
                std::string label = label_front + list_str[1];
                buffer_out.append("@" + label + "\n");
                buffer_out.append("0;JMP\n");
            } else if (list_str[0] == "function") {

                buffer_out.append("(" + list_str[1] + ")\n");
                int i=stoi(list_str[2]);
                for(int j=0;j<i;++j)buffer_out.append(function());

            }else if(list_str[0]== "call"){
                buffer_out.append(call_function(label_front,list_str[1], stoi(list_str[2]),call_num));
            }else if(list_str[0]=="return"){
                buffer_out.append("@LCL\nD=M\n@R13\nM=D\n@R13\nD=M\n@5\nD=D-A\nA=D\nD=M\n@R14\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n@ARG\nD=M\n@SP\nM=D+1\n@R13\nD=M\n@1\nD=D-A\nA=D\nD=M\n@THAT\nM=D\n@R13\nD=M\n@2\nD=D-A\nA=D\nD=M\n@THIS\nM=D\n@R13\nD=M\n@3\nD=D-A\nA=D\nD=M\n@ARG\nM=D\n@R13\nD=M\n@4\nD=D-A\nA=D\nD=M\n@LCL\nM=D\n@R14\nA=M\n0;JMP\n");
            }


        }


        outfile << buffer_out;
        buffer_out.clear();
    }
}


int main(int argc, char **argv) {


    int id = 0;

    std::string strPath(argv[1]);
    std::string relative_path(argv[0]);

    std::regex fileSuffix("(.*)(.vm)");

    std::string asm_name = strPath + "/" + strPath.substr(strPath.find_last_of('/') + 1) + ".asm";


    std::ofstream outfile;
    outfile.open(asm_name, std::ios::out);
    std::cout << asm_name << std::endl;
    int call=0;
    std::string buffer_out;
    buffer_out.append("@256\nD=A\n@SP\nM=D\n");
    buffer_out.append(call_function("Sys.init","Sys.init",0,call));
    outfile<<buffer_out;
    buffer_out.clear();

    for (auto &DirectoryIter: std::__fs::filesystem::directory_iterator(strPath)) {
        auto filepath = DirectoryIter.path();
        auto filename = filepath.filename();

        if (std::regex_match(filename.string(), fileSuffix)) {
//            std::string vm_path=relative_path+"/"+filepath.string();
            outfile << "//////" << std::endl;
            outfile << "// " << filename.replace_extension().string() << std::endl;

            translater(filepath.string(), outfile, id,call);
            std::cout << filepath << std::endl;
//            std::cout << vm_path << std::endl;
        }
    }
    return 0;


    return 0;
}
