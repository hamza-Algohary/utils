#pragma once
#include <gtkmm.h>
#include "utils.hpp"
#include "Text.hpp"
namespace My{
void enable_alpha_channel_for_window(Gtk::Window &window){
    gtk_widget_set_visual(GTK_WIDGET(window.gobj()) , window.get_screen()->get_rgba_visual()->gobj());
}  
namespace My::Icon{

inline auto make_pixbuf_rounded(Glib::RefPtr<Gdk::Pixbuf> pixbuf){
    auto surface = Cairo::ImageSurface::create(Cairo::Format::FORMAT_ARGB32 , pixbuf->get_width() , pixbuf->get_height());
    auto context = Cairo::Context::create(surface);

    Gdk::Cairo::set_source_pixbuf(context , pixbuf , 0 , 0);
    context->arc(pixbuf->get_width()/2 , pixbuf->get_height()/2 , pixbuf->get_width()/2 , 0 , 2*M_PI);
    context->fill();    
    return surface;
}

inline unsigned char *pixel(Glib::RefPtr<Gdk::Pixbuf> surface , int x , int y){
    int width = surface->get_width();
    int height = surface->get_height();
    int stride = surface->get_rowstride();
    auto p = surface->get_pixels();

    return &p[y*stride + x*4];
}

inline auto make_rgba(double red , double green , double blue , double alpha=1){
    Gdk::RGBA rgba;
    rgba.set_red(red);
    rgba.set_green(green);
    rgba.set_blue(blue);
    rgba.set_alpha(alpha);
    return rgba;
}

inline Glib::RefPtr<Gdk::Pixbuf> fill_icon_with_color(Glib::RefPtr<Gdk::Pixbuf> pixbuf , Gdk::RGBA fill_color , bool override_original_alpha=false){
    for(int x=0 ; x<pixbuf->get_width() ; x++){
        for(int y=0 ; y<pixbuf->get_height() ; y++){
            if( pixel(pixbuf , x , y)[3] != 0x00 ){
                pixel(pixbuf , x , y)[0] = (unsigned char)fill_color.get_red()*255;
                pixel(pixbuf , x , y)[1] = (unsigned char)fill_color.get_green()*255;
                pixel(pixbuf , x , y)[2] = (unsigned char)fill_color.get_blue()*255;
                if(override_original_alpha){
                    pixel(pixbuf , x , y)[3] = (unsigned char)fill_color.get_alpha()*255;
                }
            }
        }
    }

    return pixbuf;
}

inline const bool prefer_built_in_icons = true;
inline const std::string ICONS_DIR = "icons/"; 

inline auto load_builtin_icon(std::string name , int width=100 , int height=100){
    return Gdk::Pixbuf::create_from_file(ICONS_DIR+name , width , height , true);
}

inline auto load_themed_icon(std::string name , int width=100 , int height=100){
    return Gtk::IconTheme::get_default()->load_icon(name , 255 , Gtk::IconLookupFlags::ICON_LOOKUP_FORCE_SVG | Gtk::IconLookupFlags::ICON_LOOKUP_FORCE_SIZE)->scale_simple(width , height , Gdk::INTERP_BILINEAR);
}

inline auto load_icon(std::string builtin_name , std::string themed_name="" , int width=100 , int height=100){
    try{
        if(prefer_built_in_icons && builtin_name != ""){
            try{ return load_builtin_icon(builtin_name , width , height);}
            catch(...) {return load_themed_icon(themed_name , width , height);}
        }else{
            try{ return load_themed_icon(builtin_name , width , height);}
            catch(...) {return load_builtin_icon(themed_name , width , height);}       
        }
    }catch(...){
        return Gdk::Pixbuf::create(Gdk::Colorspace::COLORSPACE_RGB , true , 8 , width , height);
    }
}

inline auto load_icon(std::string name , int width = 100 , int height = 100){
    return load_icon(name , name , width , height);
}

inline auto load_icon_with_color(std::string themed_name , std::string builtin_name="" , int width=100 , int height=100 , Gdk::RGBA color = make_rgba(1,1,1)){
    return fill_icon_with_color(load_icon(themed_name , builtin_name , width , height),color);
}

inline auto load_icon_with_color(std::string name , int width=100 , int height = 100, Gdk::RGBA color = make_rgba(1,1,1)){
    return load_icon_with_color(name , name , width , height , color);
}

}

namespace My{
class IconImage : public Gtk::Image{
public:
    inline IconImage(std::string builtin_name , std::string themed_name="" , int width=100 , int height=100){
        set(My::Icon::load_icon_with_color(builtin_name , themed_name , 30 , 30 , get_style_context()->get_color()));
    }
};

inline void clear_container(Gtk::Container& container){
    for(auto child : container.get_children()){
        container.remove(*child);
    }
}

}
/*Glib::RefPtr<Gtk::Widget> dynamic_ui_from_widgets_info(std::vector<dgtk::WidgetInfo, std::allocator<dgtk::WidgetInfo>> widgets){
    
}*/


class ImageStack : public Gtk::Stack{
    std::vector<Gtk::Image> images;
public:
    //Gtk::Stack stack;
    inline ImageStack() = default;
    inline ImageStack(std::vector<Glib::RefPtr<Gdk::Pixbuf>> icons){
        for(auto icon : icons){
            append(icon);
            //images.push_back(Gtk::Image(icon));
        }
        /*for(auto &image : images){
            stack.add(image);
        }*/
    }
    inline ImageStack(std::vector<std::string> icons){
        for(auto icon : icons){
            append(icon);
        }        
    }
    inline void append(Glib::RefPtr<Gdk::Pixbuf> icon){
        images.push_back(Gtk::Image(icon));
        this->add(*images.rend());
    }
    inline void append(std::string icon_name , std::string themed_name="" , int width = 10 , int height = 10){
        append(My::Icon::load_icon_with_color(icon_name , themed_name , width , height , this->get_style_context()->get_color()));
    }
};

struct LongButton : public Gtk::ModelButton{
    Gtk::Image image;
    inline LongButton(std::string label,std::string icon_name , std::string themed_icon_name=""){
        image.set(My::Icon::load_icon_with_color(icon_name , themed_icon_name , 20 , 20 , My::Icon::make_rgba(1,1,1)));
        this->set_image(image);
        this->set_label(label);
        this->property_always_show_image() = true;
        this->set_alignment(0 , 0.5);
        this->set_size_request(-1 , 32);
        this->set_image_position(Gtk::PositionType::POS_LEFT);
    }
};

struct DetailButton : public Gtk::Button{
    Gtk::Image image;
    inline DetailButton(){
        image.set(My::Icon::load_icon_with_color("view-more-symbolic.svg" , 10 , 10));
        this->set_image(image);
    }
};

//typedef Gtk::HScale Slider;

class Slider : public Gtk::HScale{
    Control<int> control;
public:
    inline Slider(Control<int> control):Slider(){
        set_control(control);
    } 
    inline Slider(){
        get_adjustment()->set_page_increment(1);
        get_adjustment()->set_lower(0);
        get_adjustment()->set_upper(100);        
        this->signal_change_value().connect([this](Gtk::ScrollType type , double value){
            if(value < this->get_adjustment()->get_lower()) {
                value = this->get_adjustment()->get_lower();
            }else if(value > this->get_adjustment()->get_upper()){
                value = this->get_adjustment()->get_upper();
            }
            limit_bounds(value , this->get_adjustment()->get_lower() , this->get_adjustment()->get_upper());
            this->control.setter((int)value);
            return true;
        });
    }
    inline void set_control(Control<int> getter_and_setter){
        control = getter_and_setter;
        control.on_change = [this](){
            this->set_value(this->control.getter());
        };        
    }
};

inline void attach_adjustment_to_stack(Glib::RefPtr<Gtk::Adjustment>& adjustment , Gtk::Stack& stack, bool first_is_zero=false){
    adjustment->signal_value_changed().connect([&adjustment , &stack](){
        int size = stack.get_children().size();    
        int min = adjustment->get_lower();
        int max = adjustment->get_upper();
        int value = adjustment->get_value();
        int section;

        if(max == min){
            section =  0;
        }else{
            section = (double)value / (max - min) * size;
        }

        limit_bounds(section , 0 , (int)stack.get_children().size()-1);
        stack.set_visible_child(*stack.get_children().at(section));
    });
}

struct Tile : public Gtk::Button{
    Gtk::Image icon;
    typedef enum{SIZE_SMALL=1,SIZE_MEDIUM,SIZE_LARGE}TileSize;
    inline static int base_size = 50;
    inline Tile(std::string name , std::string icon_name , std::string command ,TileSize size ,  Gdk::RGBA bgcolor=My::Icon::make_rgba(0x25/255.0 , 0x25/255.0 , 0x26/255.0) , bool left=false){
        int icon_size=0;
        if(size == SIZE_SMALL) icon_size = base_size-15;
        if(size == SIZE_MEDIUM) icon_size = base_size*2-40;
        if(size == SIZE_LARGE) icon_size = base_size*2-10;
        icon.set(My::Icon::load_themed_icon(icon_name , icon_size , icon_size));
        if(size!=SIZE_LARGE){
            set_size_request(base_size*size , base_size*size);
            if(size==SIZE_MEDIUM){
                set_image_position(Gtk::PositionType::POS_TOP);
                set_label(name);  
                get_style_context()->add_class("medium");
            }else{
                get_style_context()->add_class("small");
            } 
        }else{
            set_size_request(base_size*4 , base_size*2);
            set_label(name); 
            get_style_context()->add_class("large");
            if(left){
                set_image_position(Gtk::PositionType::POS_LEFT);
                icon.property_margin_right()=2;
            }else{    
                icon.property_margin_left()=2;
                set_image_position(Gtk::PositionType::POS_RIGHT);
            }
        }
        bgcolor.set_alpha(0.9);
        override_background_color(bgcolor);
        property_always_show_image()=true;
        set_image(icon);

        signal_clicked().connect([this , command](){
            My::execute(command + " &");
        });
    }
};

class Tiles : Gtk::Grid{
    std::vector<Tile> tiles;
public:
    inline void add_tile(std::string label , std::string icon , Tile::TileSize size);
};


/*template <class T>
struct WidgetsPackage{
    T topauto dynamic_stuff(std::initializer_list<>){;
    std::vector<Gtk::Widget> dependencies;
    inline WidgetsPackage() = default;
    inline WidgetsPackage(T &top , std::vector<Gtk::Widget>& widgets):top(top),dependencies(widgets){}
    inline WidgetsPackage(std::vector<Gtk::Widget>& widgets):dependencies(widgets){}
    inline WidgetsPackage(T &top):top(top){}
};

template<class T>
inline auto dynamic_widget(T& widget){
    auto target = std::make_shared<WidgetsPackage<T>>(std::move(widget));
    //*target.get() = std::move(widget);
    for(auto child : target->get_children()){
        target->dependencies.push_back(std::move(*child));
    }
    return target;
} */

} // namespace My
