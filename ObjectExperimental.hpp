#include <string.h>
#include <typeinfo>



class NoWrapObject{

};


class Object{
public:
    void* data = nullptr;
    size_t data_size = 0;
    std::string type;
    
    Object(){
       type = "none";
    }


    /**
     * This constructor will do the following:
     * 1. Call standard/overloaded copy constructor something(cont something& init)
     * 2. store the data in the variable data
     * 3. store type of the data
     * 
     * @note this class can appear a bit buggy when used
     * with some std::classes, but userdefine schould work mostly
     * 
     * additionaly some memory leaks might apear since deconstructor call is difficult
     * but syntax is great :)
    */
   template<class value_type,typename = typename std::enable_if<!std::is_base_of<NoWrapObject,value_type>::value >::type>
   Object(const value_type& _data) {
      type = type(value_type);
      data = malloc(sizeof(value_type)); 
      data_size = sizeof(value_type);
      new (data) value_type(_data); //this will increase reference count by 1 from smart pointer in Array
   }
   
    

   /**
    * Called when cast to something
    * will throw runtime_error if not the same type as initialized
   */
    operator Object(){
        Object out;
        out.data_size = data_size;
        out.type = type;
        out.data = malloc(data_size);
        memcpy(out.data,data,data_size);
        return out;
    }

    
    template<class value_type>
    operator Array<value_type>(){
        auto requested_type = type(Array<value_type>);
        if(type != requested_type){
            throw std::runtime_error("Don't want convert " + type + " to " + requested_type);
        }
        Array<value_type> tmp = *((Array<value_type>*)data);
        ((Array<value_type>*)data)->arrayRef.reset(); //decrease reference count by 1
        return tmp;
    }

    template<class value_type>
    operator value_type(){
        auto requested_type = type(value_type);
        if(type != requested_type){
            throw std::runtime_error("Don't want convert " + type + " to " + requested_type);
        }
        Array<value_type> tmp = *((value_type*)data);
        delete ((value_type*)data);
        return tmp;
    }
   ~Object(){
      if(data != nullptr){
        free(data);
      }
   }
};

class TupleSubscript;
class Tuple{
public:
    Object* data;
    size_t len;
    
    template<class... Args>
    Tuple(Args const&... args);

    TupleSubscript operator[](int idx);

    ~Tuple(){
        //operator delete[](data);
        delete[] data;
    }
};


class TupleSubscript : NoWrapObject{
public:
    Tuple* tuple;
    int idx;
    TupleSubscript(Tuple* _tuple,int _idx): tuple(_tuple), idx(_idx) {}

    template<class value_type>
    Tuple& operator=(value_type value){
        tuple->data[idx] = Object(value);
        return *tuple;
    }

    /*operator Object(){
        return (tuple->data[idx]);
    }*/
    /*operator Object(){
        return (tuple->data[idx]);
    }*/

    template<class value_type> 
    operator value_type(){
        return (tuple->data[idx]).operator value_type();
    }

    template<class value_type> 
    value_type astype(){
        return (tuple->data[idx]).operator value_type();
    }

};


/**
 * @param anything..
 * 
*/
template<class... Args>
Tuple::Tuple(Args const&... args){
    std::initializer_list<Object> _list = {args...};
    Object* list = (Object*)_list.begin();
    
    len = _list.size();
    data = new Object[len];
    //std::cout << "len "<<len << "\n"; 
    for(int k = 0; k < len ; k++){
        data[k].data_size = list[k].data_size;
        data[k].type = list[k].type;
        data[k].data = malloc(list[k].data_size);
        memcpy(data[k].data,list[k].data,list[k].data_size);
    }
}
TupleSubscript Tuple::operator[](int idx){
    return TupleSubscript(this,idx);
} 
