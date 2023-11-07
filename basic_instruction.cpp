//
// Created by Fallere141 on 11/6/23.
//

#include <iostream>
#include "project7.h"
#include <string>
#include <fstream>
#include "basic_instruction.h"

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

void Trimmed(std::string &str) {
    str.erase(0, str.find_first_not_of(" \r\n\t\v\f"));
    str.erase(str.find_last_not_of(" \r\n\t\v\f") + 1);
}

std::string WriteBasicInstruction(std::string buffer_in, int &id, std::string filename) {


    std::string outfile;
    //remove space from head and tail
    Trimmed(buffer_in);
    // split the instruction
    std::vector<std::string> list_str = split(buffer_in);

    //put original instruction as comment
//        outfile"//"+buffer_in+"\n");
    outfile.append("// " + buffer_in + "\n");

    // if it is a push instruction
    if (list_str[0] == "push") {
        std::string pointer = list_str[1];

        //if the segment is constant
        if (pointer == "constant") {
            pointer = "SP";
//                outfile.append( "@" + list_str[2] + "\n");
            outfile.append("@" + list_str[2] + "\n");
//                outfile.append( "D=A\n");
            outfile.append("D=A\n");
//                outfile.append( "@" + pointer + "\n");
            outfile.append("@" + pointer + "\n");
//                outfile.append( "AM=M+1\n");
            outfile.append("AM=M+1\n");
//                outfile.append( "A=A-1\n");
            outfile.append("A=A-1\n");
//                outfile.append( "M=D\n");
            outfile.append("M=D\n");
        }
            // other segment except pointer and static
        else if (pointer == "local" or pointer == "this" or pointer == "that" or pointer == "temp" or
                 pointer == "argument") {
            if (pointer == "local")pointer = "LCL";
            else if (pointer == "this")pointer = "THIS";
            else if (pointer == "that")pointer = "THAT";
            else if (pointer == "temp")pointer = "5";
            else if (pointer == "argument")pointer = "ARG";
            outfile.append("@" + pointer + "\n");
            if (pointer == "5") {

                outfile.append("D=A\n");
            } else outfile.append("D=M\n");
            outfile.append("@" + list_str[2] + "\n");
            outfile.append("A=A+D\n");
            outfile.append("D=M\n");
            outfile.append("@SP\n");
            outfile.append("AM=M+1\n");
            outfile.append("A=A-1\n");
            outfile.append("M=D\n");
            //segment pointer
        } else if (pointer == "pointer") {
            if (list_str[2] == "0") {
                outfile.append("@R3\n");
            } else outfile.append("@R4\n");
            outfile.append("D=M\n");
            outfile.append("@SP\n");
            outfile.append("AM=M+1\n");
            outfile.append("A=A-1\n");
            outfile.append("M=D\n");
        }
            //segemtn static
        else if (pointer == "static") {

//            int num = 16 + stoi(list_str[2]);
//            outfile.append("@" + std::to_string(num) + "\n");
            filename.pop_back();
            outfile.append("@" + filename + "." + list_str[2] + "\n");
            outfile.append("D=M\n");
            outfile.append("@SP\n");
            outfile.append("AM=M+1\n");
            outfile.append("A=A-1\n");
            outfile.append("M=D\n");
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
        else if (pointer == "static")pointer = "16";

        //temp and static segment
        if (pointer == "5" or pointer == "16") {
            if (pointer == "5") {
                int num = stoi(pointer) + stoi(list_str[2]);
                outfile.append("@" + std::to_string(num) + "\n");
                outfile.append("D=A\n");
            } else{
                filename.pop_back();
                outfile.append("@" + filename+"."+list_str[2] + "\n");
                outfile.append("D=A\n");


            }
            // pointer segment
            //temp and static segment
            // pointer segment
        } else if (pointer == "pointer") {
            if (list_str[2] == "0")pointer = "R3";
            else pointer = "R4";
            outfile.append("@" + pointer + "\n");
            outfile.append("D=A\n");
        } else {
            //other segment
            outfile.append("@" + pointer + "\n");
            outfile.append("D=M\n");
            outfile.append("@" + list_str[2] + "\n");
            outfile.append("D=D+A\n");
        }

        outfile.append("@R13\n");
        outfile.append("M=D\n");
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("@R13\n");
        outfile.append("A=M\n");
        outfile.append("M=D\n");

        //add
    } else if (list_str[0] == "add") {
        outfile.append("@SP\n");
//            outfile.append( "A=M\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("M=D+M\n");
        //sub
    } else if (list_str[0] == "sub") {
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("M=M-D\n");
        //neg
    } else if (list_str[0] == "neg") {
        outfile.append("@SP\n");

        outfile.append("A=M-1\n");
        outfile.append("M=-M\n");
        //gt
        // for those using jump instruction, I use an id to identify the difference of loop label
        // each these instruction will update id to id++.
    } else if (list_str[0] == "gt") {
        ++id;// update id in case the loop label are confused
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("D=M-D\n");
        outfile.append("M=-1\n");
        outfile.append("@TRUE" + std::to_string(id) + "\n");
        outfile.append("D;JGT\n");
        outfile.append("@SP\n");
        outfile.append("A=M-1\n");
        outfile.append("M=0\n");
        outfile.append("(TRUE" + std::to_string(id) + ")\n");
        //eq
    } else if (list_str[0] == "eq") {
        ++id;// update id in case the loop label are confused
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("D=M-D\n");
        outfile.append("M=-1\n");
        outfile.append("@TRUE" + std::to_string(id) + "\n");
        outfile.append("D;JEQ\n");
        outfile.append("@SP\n");
        outfile.append("A=M-1\n");
        outfile.append("M=0\n");
        outfile.append("(TRUE" + std::to_string(id) + ")\n");
        //lt
    } else if (list_str[0] == "lt") {
        ++id;// update id in case the loop label are confused
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("D=M-D\n");
        outfile.append("M=-1\n");
        outfile.append("@TRUE" + std::to_string(id) + "\n");
        outfile.append("D;JLT\n");
        outfile.append("@SP\n");
        outfile.append("A=M-1\n");
        outfile.append("M=0\n");
        outfile.append("(TRUE" + std::to_string(id) + ")\n");

        //and
    } else if (list_str[0] == "and") {
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("M=M&D\n");
        // or
    } else if (list_str[0] == "or") {
        outfile.append("@SP\n");
        outfile.append("AM=M-1\n");
        outfile.append("D=M\n");
        outfile.append("A=A-1\n");
        outfile.append("M=M|D\n");
        //not
    } else if (list_str[0] == "not") {
        outfile.append("@SP\n");
        outfile.append("A=M-1\n");
        outfile.append("M=!M\n");
    }

    //clean the buffer
    return outfile;
}