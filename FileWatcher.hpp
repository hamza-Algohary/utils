#include <thread>
#include <fstream>
#include <functional>

#include <macros.hpp>
namespace My{
namespace fs = std::filesystem;
class FileWatcher{
private:
    std::string file;
    std::function<void(bool exists ,bool changed)> pulse = [](bool , bool){};
    //std::mutex mtx;
    bool exit = false;
    int delay = 500;
public:
    inline FileWatcher() = default;
    inline ~FileWatcher(){stop();}
    /*inline FileWatcher(const FileWatcher &watcher){

    }*/
    inline void set_path(std::string file){this->file = file;}
    inline std::string get_path(){return file;}
    inline void set_file_change_function(const std::function<void(bool exists ,bool changed)> lambda){pulse = lambda;}
    inline void start(){
        exit = false;
        std::thread watcher([&](){
            static std::mutex mtx;
            //namespace fs = std::filesystem;
            std::lock_guard<std::mutex> guard(mtx);
            bool exists=0 , changed=0;
            long long int index;
            bool run = true;
            auto last_time = fs::last_write_time(getenv("HOME"));/*fs::last_write_time(file);*/

            while(!exit){
                if(fs::exists(file) != exists){
                    exists = !exists;
                    run = true;
                }else{
                    run = false;
                }
                if(fs::exists(file)){
                    if(last_time != fs::last_write_time(file)){
                        changed = true;
                        last_time = fs::last_write_time(file);
                    }else{
                        changed = false;
                    }
                }else{
                    changed = false;
                }

                if(run || changed){
                    pulse(exists , changed);
                }
                std::this_thread::sleep_for(std::chrono::duration<int , std::milli>(delay));
            }
        });
        watcher.detach();
    }
    inline void stop(){
        exit = true;
    }
    inline void set_delay(int delay){
        this->delay = delay;
    }
};

}
#include <undefinemacros.hpp>