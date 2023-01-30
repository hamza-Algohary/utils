#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <filesystem>
#include <map>
#include <boost/process.hpp>
#include "Text.hpp"

#include "macros.hpp"

namespace My{
inline auto median(double x , double y){
    return (std::max(x,y)-std::min(x,y))/2.0;
}

template<typename T>
inline void limit_bounds(T &var , T min , T max){
    if(var<min) var = min;
    else if(var>max) var = max;
}

inline My::Text spawn(std::string command){
    using namespace boost::process;
    ipstream stream;
    child c(command , std_out > stream);
    return My::Text(stream);
} 

struct QuickStringStream : public std::string{
    //QuickStringStream(){}
    //QuickStringStream(std::string str){this->append(str);}
    inline QuickStringStream operator<<(auto x){
        std::stringstream ss;
        ss << x;

        QuickStringStream qss;
        qss.append((*this) + ss.str());

        return qss;
    }
}ss;
    inline std::string execute(std::string command , int max_line_length=100){
        char buffer[max_line_length];
        std::string result = "";

        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            return "popen failed!";
        }

        while (!feof(pipe)) {
            if (fgets(buffer, max_line_length, pipe) != NULL){
                result += buffer;
            }
        }

        pclose(pipe);
        return result;
    }
    inline void shell(std::string command){
        std::system(command.c_str());
    }
    inline bool string_to_bool(std::string str , std::pair<std::vector<std::string> , std::vector<std::string>> words = {{"true"},{"false"}}){
        if(std::find(words.first.begin() , words.first.end() , str)!=words.first.end()){
            return true;
        }else if(std::find(words.second.begin(),words.second.end(),str)!=words.second.end()){
            return false;
        }
        throw "Unconvertable to bool";
    }

    inline bool is_in(auto var , auto vars){
        return std::find(vars.begin() , vars.end() , var) != vars.end();
    }


inline std::string getenv(std::string var) noexcept {
    auto result = std::getenv(var.c_str()); //If the environment variable is not available it returns NULL.
    return result?result:"";
    /*if(result) return result;
    else return "";*/
}

inline std::string expand_env_vars(std::string str , char var_begin = '$' , std::vector<char> var_end = {' ','\\','/'}) noexcept{
    Text text = str;
    std::string output = "";
    while(text.str()!=""){
        output.append(text.get_until({var_begin}).str());
        output.append(My::getenv(text.get_until(var_end , false).str().c_str()));
    }
    return output;
}

class FileCheker{
    std::map<std::string , std::string> files;
    std::vector<std::string> folders;
public:
    inline void add_file(std::string name , std::string default_content=""){
        files[name] = default_content;
    }
    inline void add_folder(std::string name){
        folders.push_back(name);
    }
    inline void check(){
        using namespace std::filesystem;

        for(auto folder : folders){
            create_directories(folder);
        }
        for(auto file:files){
            create_directories(path(file.first).parent_path());
            if(!exists(file.first)){
                std::ofstream fout(file.first);
                if (!fout.is_open()){
                    std::cerr /*<< termcolor::red*/ << "Warning! " /*<< termcolor::reset*/ << "Unable to access " << file.first << " falling back to default configuration.";
                }else{
                    fout << file.second;
                }             
            }
        }
    }

};
};
#include "undefinemacros.hpp"