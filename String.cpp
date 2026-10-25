/*
String requirements
1. the size should be 24 bytes
2. must implement Short String Optimization (In stack for small strings else in heap)
3. Assume little endian least significant byte store first then most significant byte
*/
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace viral {

    class String{
    private:
        //vector like structure
        struct HeapData{
            //least significant bit of size_tag is 0
            std::size_t size_tag; //8 bytes
            std::size_t capacity; //8 bytes
            char* data; //8 bytes 
        };

        //sso array like structure on stack
        struct StackData{
            //least significant bit of size_tag is 1
            std::uint8_t size_tag; //1 byte
            char data[23]; //23 bytes
        };

        static_assert(sizeof(HeapData)==24);
        static_assert(sizeof(StackData)==24);

        union Storage{
            HeapData heap;
            StackData stack;
        }; //string owns this thus will be 24 bytes

        Storage storage_;

        bool is_heap() const{
            const unsigned char first_byte = *reinterpret_cast<const unsigned char*>(&storage_);
            return (first_byte & 1)==0;
        }

    public:
        String(){
            //creates empty short string
            storage_.stack.size_tag = 1; //adding the tag, though size is 0;
            storage_.stack.data[0] = '\0';
        }

        String(const char* str, std::size_t str_size){
            if(str_size<=23){
                //short string optimization and store in stack
                //copy bytes to stack.data
                std::memcpy(storage_.stack.data, str, str_size);
                //modify the size to account for tag
                storage_.stack.size_tag = static_cast<std::uint8_t>(str_size<<1 | 1);
            }
            else{
                //store in heap

                //allocate memory in heap 
                storage_.heap.data = new char[str_size];
                std::memcpy(storage_.heap.data, str, str_size);
                //modify the size to account for tag
                storage_.heap.size_tag = str_size<<1;
                storage_.heap.capacity = str_size;
            }
        }

        String(const char* str): 
            String(str, std::strlen(str))
        {}

        ~String(){
            //destroy heap
            if(is_heap()){
                delete[] storage_.heap.data;
            }
        }
        std::size_t size() const{
            if(is_heap()){
                return storage_.heap.size_tag>>1;
            }
            else{
                return storage_.stack.size_tag>>1;
            }
        }

        std::size_t capacity() const{
            if(is_heap()){
                return storage_.heap.capacity;
            }
            return 23; //sso max capacity
        }

        String(const String& other){
            if(other.is_heap()){
                storage_.heap.capacity = other.capacity();
                storage_.heap.data = new char[storage_.heap.capacity];
                storage_.heap.size_tag = (other.size())<<1;
                
                std::memcpy(storage_.heap.data, other.storage_.heap.data, other.size());
            }
            else{
                storage_.stack.size_tag = (other.size() << 1)|1;
                std::memcpy(storage_.stack.data, other.storage_.stack.data, other.size());
            }
        }
    };
}