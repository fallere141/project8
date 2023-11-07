#include <iostream>
#include <fstream>
#include <string>
#include <utility>

std::string delete_intend(std::string filename) {



    std::ifstream infile(filename);
//    std::string filename(std::move(filename2));

    while(filename.back()!='.'){
        filename.pop_back();
    }
    filename.append("out");



    std::ofstream outfile;
    outfile.open(filename,std::ios::out);
    std::string buffer_in;
    std::string buffer_out;
    std::string space_buffer;
    int flag_comments=0;
    int flag_multi_comments=0;
    int space_flag=0;
    int release=0;
    int flag_first_line=1;


    while (std::getline(infile, buffer_in)) {
        // read a line from file


        //check evey word from buffer
        for (auto i: buffer_in) {

            //if in multi line comments, check whether to
            if (flag_multi_comments == 1) {
                if (i == '*') {
                    release = 1;
                    continue;
                }
                if (release==1){
                    if (i=='/'){
                        flag_multi_comments=0;
                    }
                    release=0;
                }
                continue;
            }

            //ignore space front the line and if in multi line comments, ignore it.
            if (space_flag == 0 and i != ' ' or space_flag==1 ) {
                if (flag_comments==1 and i=='*'){
                    flag_comments=0;
                    flag_multi_comments=1;
                    continue;
                }

                //check if i is a start of comments
                if (i=='/'){
                    if(flag_comments==0){
                        flag_comments=1;
                    } else {

                        //check if line comments and to be ignored
                        flag_comments = 0;
                        break;
                    }
                } else {

                    //if not comments, we should put '/' back
                    if (flag_comments==1) {
                    buffer_out+='/';
                    flag_comments=0;
                    }


                    buffer_out += i;
                    space_flag = 1;
                }

            }

        }

        // if '/' is the last word in a line, we should not put it into next line.
        if(flag_comments==1){
            buffer_out+='/';
            flag_comments=0;
        }

        if (!buffer_out.empty()&&buffer_out!="\r") {

            if (flag_first_line==1){
                outfile << buffer_out;
                flag_first_line=0;
            } else
            outfile <<std::endl<< buffer_out;
        }
        buffer_out.clear();
        space_flag=0;
    }
    return filename;
}
