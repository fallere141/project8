//
// Created by Fallere141 on 11/6/23.
//

#include <iostream>
#include "project7.h"
#include <string>
#include <fstream>

//split string to substring, delimiter is space
std::vector<std::string> split(std::string str) {
    std::string remain(str);
    int i = remain.find(" ");
    std::vector<std::string> list_str;
    while (i != std::string::npos) {
        list_str.push_back(remain.substr(0, i));
        int l = remain.length();
        remain = remain.substr(i + 1, l);
        i = remain.find(" ");
    }
    list_str.push_back(remain);
    return list_str;
}

void Trimmed(std::string &str){
    str.erase(0,str.find_first_not_of(" \r\n\t\v\f"));
    str.erase(str.find_last_not_of(" \r\n\t\v\f")+1);
}

void WriteBasicInstruction(std::string buffer_in,std::ofstream outfile,int &id){



//        if (buffer_in[buffer_in.length() - 1] == '\r') {
//            buffer_in.pop_back();
//        }
        //remove space from head and tail
        Trimmed(buffer_in);
        // split the instruction
        std::vector<std::string> list_str = split(buffer_in);

        //put original instruction as comment
        outfile<<"//"<<buffer_in<<std::endl;

        // if it is a push instruction
        if (list_str[0] == "push") {
            std::string pointer = list_str[1];

            //if the segment is constant
            if (pointer == "constant") {
                pointer = "SP";
                outfile << "@" << list_str[2] << std::endl;
                outfile << "D=A" << std::endl;
                outfile << "@" << pointer << std::endl;
                outfile << "AM=M+1" << std::endl;
                outfile << "A=A-1" << std::endl;
//            outfile << "@" <<pointer<< std::endl;
                outfile << "M=D" << std::endl;

            }
                // other segment except pointer and static
            else if (pointer == "local" or pointer == "this" or pointer == "that" or pointer == "temp" or
                     pointer == "argument") {
                if (pointer == "local")pointer = "LCL";
                else if (pointer == "this")pointer = "THIS";
                else if (pointer == "that")pointer = "THAT";
                else if (pointer == "temp")pointer = "5";
                else if (pointer == "argument")pointer = "ARG";
                outfile << "@" << pointer << std::endl;
                if(pointer=="5"){outfile << "D=A" << std::endl;}else outfile << "D=M" << std::endl;
                outfile << "@" << list_str[2] << std::endl;
                outfile << "A=A+D" << std::endl;
                outfile << "D=M" << std::endl;
                outfile << "@SP" << std::endl;
                outfile << "AM=M+1" << std::endl;
                outfile << "A=A-1" << std::endl;
                outfile << "M=D" << std::endl;
                //segment pointer
            } else if (pointer=="pointer"){
                if (list_str[2]=="0"){
                    outfile << "@R3" << std::endl;
                } else  outfile << "@R4" << std::endl;
                outfile << "D=M" << std::endl;
                outfile << "@SP" << std::endl;
                outfile << "AM=M+1" << std::endl;
                outfile << "A=A-1" << std::endl;
                outfile << "M=D" << std::endl;
            }
                //segemtn static
            else if (pointer=="static"){

                int num= 16+ stoi(list_str[2]);
                outfile<<"@"<<num<<std::endl;
                outfile << "D=M" << std::endl;
                outfile << "@SP" << std::endl;
                outfile << "AM=M+1" << std::endl;
                outfile << "A=A-1" << std::endl;
                outfile << "M=D" << std::endl;
            }

            // pop instruction
        } else if (list_str[0] == "pop") {
            std::string pointer = list_str[1];
            //decided segment
            if (pointer == "local")pointer = "LCL";
            else if (pointer == "this")pointer = "THIS";
            else if (pointer == "that")pointer = "THAT";
            else if (pointer == "temp")pointer = "5";
            else if (pointer == "argument")pointer = "ARG";
            else if (pointer=="static")pointer="16";

            //temp and static segment
            if (pointer=="5" or pointer=="16"){
                int num= stoi(pointer)+ stoi(list_str[2]);
                outfile << "@" << num << std::endl;
                outfile << "D=A" << std::endl;
                // pointer segment
                //temp and static segment
//            if (pointer=="5"){
//                int num= stoi(pointer)+ stoi(list_str[2]);
//                outfile << "@R" << num << std::endl;
//                outfile << "D=A" << std::endl;
                // pointer segment
            }else if(pointer=="pointer"){
                if (list_str[2]=="0")pointer="R3";
                else pointer="R4";
                outfile << "@"<<pointer<< std::endl;
                outfile << "D=A" << std::endl;
            }
            else {
                //other segment
                outfile << "@" << pointer << std::endl;
                outfile << "D=M" << std::endl;
                outfile << "@" << list_str[2] << std::endl;
                outfile << "D=D+A" << std::endl;
            }

            outfile << "@R13" << std::endl;
            outfile << "M=D" << std::endl;
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "@R13" << std::endl;
            outfile << "A=M" << std::endl;
            outfile << "M=D" << std::endl;

            //add
        } else if (list_str[0] == "add") {
            outfile << "@SP" << std::endl;
//            outfile << "A=M" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "M=D+M" << std::endl;
            //sub
        } else if (list_str[0] == "sub") {
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "M=M-D" << std::endl;
            //neg
        } else if (list_str[0] == "neg") {
            outfile << "@SP" << std::endl;

            outfile << "A=M-1" << std::endl;
            outfile << "M=-M" << std::endl;
            //gt
            // for those using jump instruction, I use an id to identify the difference of loop label
            // each these instruction will update id to id++.
        } else if (list_str[0] == "gt") {
            ++id;// update id in case the loop label are confused
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "D=M-D" << std::endl;
            outfile << "M=-1" << std::endl;
            outfile << "@TRUE" << id << std::endl;
            outfile << "D;JGT" << std::endl;
            outfile << "@SP" << std::endl;
            outfile << "A=M-1" << std::endl;
            outfile << "M=0" << std::endl;
            outfile << "(TRUE" << id << ")" << std::endl;
            //eq
        } else if (list_str[0] == "eq") {
            ++id;// update id in case the loop label are confused
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "D=M-D" << std::endl;
            outfile << "M=-1" << std::endl;
            outfile << "@TRUE" << id << std::endl;
            outfile << "D;JEQ" << std::endl;
            outfile << "@SP" << std::endl;
            outfile << "A=M-1" << std::endl;
            outfile << "M=0" << std::endl;
            outfile << "(TRUE" << id << ")" << std::endl;
            //lt
        } else if (list_str[0] == "lt") {
            ++id;// update id in case the loop label are confused
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "D=M-D" << std::endl;
            outfile << "M=-1" << std::endl;
            outfile << "@TRUE" << id << std::endl;
            outfile << "D;JLT" << std::endl;
            outfile << "@SP" << std::endl;
            outfile << "A=M-1" << std::endl;
            outfile << "M=0" << std::endl;
            outfile << "(TRUE" << id << ")" << std::endl;

            //and
        } else if (list_str[0] == "and") {
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "M=M&D" << std::endl;
            // or
        } else if (list_str[0] == "or") {
            outfile << "@SP" << std::endl;
            outfile << "AM=M-1" << std::endl;
            outfile << "D=M" << std::endl;
            outfile << "A=A-1" << std::endl;
            outfile << "M=M|D" << std::endl;
            //not
        } else if (list_str[0] == "not") {
            outfile << "@SP" << std::endl;
            outfile << "A=M-1" << std::endl;
            outfile << "M=!M" << std::endl;
        }

        //clean the buffer
        buffer_in.clear();








}