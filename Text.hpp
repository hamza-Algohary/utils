#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <set>
#include <algorithm>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <optional>

namespace  My{
    inline bool exist_in(std::string input , std::string keyword , int index){
        if(input.length()-index < keyword.length()){
            return false;
        } 
        for(int i=index  ,j=0; j<keyword.length() ; i++ , j++){
            if(input[i] != keyword[j]){
                return false;
            }
        }
        return true;
    }
    inline std::optional<int> search_first(std::string input , std::string keyword , int from=0 , int to=-1){
        if(to==-1){
            to=input.length();
        }
        for(int i=from ; i<to ; i++){
            bool found = exist_in(input , keyword , i);
            if(found){
                return i;
            } 
        }
        return {};
    }
    inline std::string get_until_string(std::string str , std::string keyword , bool include_keyword = false){
        //auto index = boost::algorithm::find_first(str , keyword);
        //auto iter = std::find_first_of(str.begin() , str.end() , keyword.begin() , keyword.end());
        //int index = std::distance(str.begin() , iter);
        auto end = search_first(str , keyword);
        return str.substr(0 , end.value_or(str.length()-1));
        /*if(end.has_value()){
            std::cout << end.emplace() << std::endl;
            return str.substr(0 , end.emplace());
        }    
        return str;  */
    }
    inline std::string get_until_char(std::string str , char delimiter){
        std::stringstream ss;
        ss << str;

        std::string result;
        std::getline(ss , result , delimiter);        
        return result;
    }
    inline std::string crop_from_first(std::string str , int chars){
        std::stringstream result;
        for(int i=chars;i<str.size();i++){
            result << str.at(i);
        }
        return result.str();
    }
    inline std::string get_smallest_string(std::string s1 , std::string s2){
        if(s1.size() < s2.size())
            return s1;
        else if(s2.size() < s1.size())
            return s2;
        else 
            return "";
    }
    inline std::string get_smallest_string(const std::vector<std::string> &texts){
        if (texts.size() < 1)
            return "";
        
        if (texts.size() < 2)
            return texts.at(0);
        
        std::string result = get_smallest_string(texts.at(0) , texts.at(1));
        for(int i=2;i<texts.size();i++){
            result = get_smallest_string(result , texts.at(i));
        }

        return result;
    }
    inline std::string reverse_string(std::string str){
        std::stringstream ss;
        for(int i=str.size()-1;i>=0;i--){
            ss << str[i];
        }
        return ss.str();
    }
    inline std::string to_string(char c){
        std::stringstream ss;
        ss << c;
        return ss.str();
    }
    inline std::string to_string(std::string file){
        std::ifstream fin(file);
        std::stringstream ss;
        ss << fin.rdbuf();
        return ss.str();
    }
    inline std::string remove_char(std::string str, char to_be_skipped){
        std::string result="";
        for(auto c:str){
            if(c!=to_be_skipped)
                result.append(to_string(c));
        }
        return result;
    }
    inline std::string remove_chars(std::string str , std::vector<char> chars){
        std::string result="";
        for (auto c : chars){
            result = remove_char(result , c);
        }   
        return result;
    }
    /*inline std::optional<std::string> get_end_of_bracket(std::string str , int bracket_index){
        
    }*/
class Text{
public:
    std::string original_source;
    std::string source;
    bool operator==(const My::Text &other){
        return this->str()==other.str();
    }
    inline Text(const std::string &str){
        source = str;
        original_source = source;
    }
    inline Text(const Text& text){
        original_source = text.original_source;
        source = text.source;
    }
    inline Text(const char* cstr){
        source = cstr;
    }
    inline Text(const char& c){
        std::stringstream ss;
        ss << c;
        source = ss.str();
    }
    inline Text(const auto &stream){
        std::stringstream ss;
        ss << stream.rdbuf();
        source = ss.str();
        original_source = ss.str();
    }
    inline Text()=default;
    inline Text original() const {
        return Text (original_source);
    }
    inline std::string str() const {
        return source;
    }
    inline void reset(){
        source = original_source;
    }
    inline Text get_until(std::vector<char> delimiters , bool remove_delimiter=true){
        //std::vector<std::string> results;
        std::set<std::string> results;
        for(auto delim : delimiters){
            results.insert(get_until_char(source , delim));
        }
        /*for(auto r : results){
            std::cout << r << std::endl;
        }*/
        //std::cout << "|_|_|_|_|_|_|_|_|_|_|\n";
        std::string result = *results.begin();//get_smallest_string(results);
        source = crop_from_first(source , result.size()+remove_delimiter);
        return Text(result);
    }
    inline Text skip_until(std::vector<char> delimiters , bool remove_delimiter=true){
        this->get_until(delimiters , remove_delimiter);
        return this->copy();
    }

    inline int skip_chars(std::string chars){
        int i=0;
        for(;i<source.size();i++){
            if(std::find(chars.begin() , chars.end() , source.at(i)) == chars.end())
                break;
        }
        source = crop_from_first(source , i);
        return i;
    }
    inline Text extract_word(){
        skip_chars({'\n','\t',' '});
        return get_until({'\n','\t',' '});
    }
    inline Text extract_optional_token(char begin , char end , bool include_brackets=true){
        skip_chars({'\n','\t',' '});
        std::cout << "Source: " << source << std::endl;
        if(source[0]!=begin){
            std::cout << "Token not found\n";
            return "";
        }
        if(!include_brackets){
            skip_chars({begin});
        }
        return get_until({end}).str() + (include_brackets?to_string(end):"");
    }
    inline Text& operator << (auto var){
        std::stringstream ss;
        ss << var;
        source.append(ss.str());
        return *this;
    }
    inline Text& operator >> (auto &var){
        std::stringstream ss;
        ss << extract_word().str();
        ss >> var;
        return *this;
    }
    inline Text copy(size_t from=0 , size_t to=SIZE_MAX) const {
        if(from < 0 || to < from || source == "")
            return "";
        if(to > source.size())
            to = source.size()-1;
        if(from == to){
            //source = crop_from_first(source , 1);
            return source.at(from);
        }

        Text result;
        for (int i=from ; i<=to ; i++){
            result << source.at(i);
        }
        return result;
        //source = crop_from_first(source , result.str().size());
    }
    inline std::vector<Text> split(char c) const {
        Text text(source);
        std::vector<Text> texts;
        while(text.str()!=""){
            texts.push_back(text.get_until({c}));
        }
        return texts;
    }
    inline Text reverse(){
        Text result = copy();
        result.source = reverse_string(result.source);
        return result;
    }
    inline Text replace_all(Text to_be_replaced , Text replacer) const {
        Text result = copy();
        boost::replace_all(result.source , to_be_replaced.str() , replacer.str());
        return result;
    }
    inline Text trim_from_end(std::string chars="\n\t "){
        Text result(reverse_string(source));
        result.skip_chars(chars);
        result = result.reverse();
        return result;
    }
};   
/*inline const std::ostream& operator << (const std::ostream& out , const My::Text& text){
    out << text.str();
    return out;
}*/
inline std::vector<std::string> split(std::string str , char c){
    My::Text text(str);
    std::vector<std::string> tokens;
    for(auto token : My::Text(str).split(c)){
        tokens.push_back(token.str());
    }
    return tokens;
}
}