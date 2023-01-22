#include "utils.hpp"
#include "FileWatcher.hpp"

#include <macros.hpp>
namespace My{
class MapFile{
    std::map<std::string , std::string> keys;
    std::map<std::string , std::string> default_keys;
    FileWatcher watcher;  
    std::vector<std::string> changed_keys;
public:     
    std::function<void(std::vector<std::string>)> on_change = [](std::vector<std::string>){}; 
    inline const std::vector<std::string> &load_string(std::string str){
        My::Text text = str;
        changed_keys.clear();
        while(text.str() != ""){
            auto line = text.get_until({'\n'});
            std::string key = line.get_until({'='}).str() , value = line.str();
            if(keys[key]!=value){
                changed_keys.push_back(key);
                keys[key] = value;
            }
        }
        return changed_keys;
    }
    inline void reset(){
        keys.clear();
    }
    inline void watch_file(fs::path file , std::function<void(std::vector<std::string>)> on_change = [](std::vector<std::string>){}){
        watcher.stop();
        this->on_change = on_change;
        watcher.set_path(file);
        //this->on_change(load_string(My::to_string(this->watcher.get_path())));
        watcher.set_file_change_function([this](bool exists , bool changed){
            if(exists){
                this->on_change(load_string(My::to_string(this->watcher.get_path())));
            }
        });
        watcher.start();
    }
    inline auto& get_keys(){
        return keys;
    }
    inline int get_int(std::string key , std::optional<int> fallback = {}){
        if(!fallback.has_value()) return std::stoi(keys[key]);
        else try_return_in_order(std::stoi(keys[key]) , fallback.value());
    }
    inline double get_double(std::string key , std::optional<double> fallback = {}){
        if(!fallback.has_value()) return std::stod(keys[key]);
        else try_return_in_order(std::stod(keys[key]) , fallback.value());
    }
    inline bool get_bool(std::string key , std::optional<bool> fallback = {}){
        if(!fallback.has_value()) return My::string_to_bool(keys[key]);
        else try_return_in_order(My::string_to_bool(keys[key]) , fallback.value());
    }
    inline std::string get_string(std::string key){
        return keys[key];
    }
    inline std::vector<My::Text> get_vector(std::string key , char separator=';'){
        My::Text text = keys[key];
        return text.split(separator);
    }   
};
}

#include <undefinemacros.hpp>