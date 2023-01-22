#include "MapFile.hpp"
#include "macros.hpp"

namespace My{

class GroupedMapFile{
    std::map<std::string , std::map<std::string , std::string>> groups;
    FileWatcher watcher;  
    std::vector<std::pair<std::string,std::string>> changed_groups;
public:
    inline GroupedMapFile() = default;
    inline GroupedMapFile(const GroupedMapFile &other) = default;
    std::function<void(std::vector<std::pair<std::string,std::string>>)> on_change = [](std::vector<std::pair<std::string,std::string>>){}; 
    inline void watch_file(fs::path file , std::function<void(std::vector<std::pair<std::string,std::string>>)> on_change = [](std::vector<std::pair<std::string,std::string>>){}){
        watcher.stop();
        this->on_change = on_change;
        watcher.set_path(file);
        watcher.set_file_change_function([this](bool exists , bool changed){
            if(exists){
                this->on_change(load_string(My::to_string(this->watcher.get_path())));
            }
        });
        watcher.start();
    }
    inline const std::vector<std::pair<std::string,std::string>>& load_string(std::string str){
        My::Text text = str;
        changed_groups.clear();
        std::string current_group="";
        while(text.str() != ""){
            auto line = text.get_until({'\n'});
            My::Text group = line.copy();
            group.skip_chars(" \t");
            if(group.str()[0]=='['){
                group.get_until({'['});
                current_group = group.get_until({']'}).trim_from_end().str();
                //std::cout << current_group << std::endl;
            }else{
                std::string key = line.get_until({'='}).str() , value = line.str();
                if(groups[current_group][key]!=value || value == ""){
                    changed_groups.push_back({current_group,key});
                    groups[current_group][key] = value;
                }
            }
        }
        return changed_groups;
    }
    inline void reset(){
        groups.clear();
    }
    inline auto& get_groups(){
        return groups;
    }
    inline int get_int(std::string group , std::string key , std::optional<int> fallback = {}){
        if(!fallback.has_value()) return std::stoi(groups[group][key]);
        else try_return_in_order(std::stoi(groups[group][key]) , fallback.value());
    }
    inline double get_double(std::string group ,std::string key , std::optional<double> fallback = {}){
        if(!fallback.has_value()) return std::stod(groups[group][key]);
        else try_return_in_order(std::stod(groups[group][key]) , fallback.value());
    }
    inline bool get_bool(std::string group ,std::string key , std::pair<std::vector<std::string> , std::vector<std::string>> words = {{"true"},{"false"}} , std::optional<bool> fallback = {}){
        if(!fallback.has_value()) return My::string_to_bool(groups[group][key] , words);
        else try_return_in_order(My::string_to_bool(groups[group][key] , words) , fallback.value());
    }
    inline std::string get_string(std::string group ,std::string key , std::string if_empty = ""){
        return (groups[group][key]!=""?groups[group][key]:if_empty);
    }    
    inline std::vector<My::Text> get_vector(std::string group ,std::string key , char separator=';'){
        My::Text text = groups[group][key];
        return text.split(separator);
    }
};

}

#include "undefinemacros.hpp"