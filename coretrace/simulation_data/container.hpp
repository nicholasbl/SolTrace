#ifndef SOLTRACE_ELEMENT_CONTAINER_H
#define SOLTRACE_ELEMENT_CONTAINER_H

#include <cstdint>
#include <map>
#include <memory>

// #include "element.hpp"
// #include "ray_source.hpp"

// typedef uint64_t element_id;

template <typename K, typename V>
class Container
{
public:

    typedef typename std::shared_ptr<V> value_pointer;
    typedef typename std::map<K, value_pointer>::iterator iterator;
    typedef typename std::map<K, value_pointer>::const_iterator const_iterator;

    Container():next_id(0){}
    ~Container(){}

    K add_item(value_pointer item)
    {
        auto key = this->next_id;
        typename std::map<K, value_pointer>::value_type to_insert(key, item);
        auto result = this->container.insert(to_insert);
        if (result.second == true)
        {
            ++next_id;
        }
        else
        {
            // Insertion failed. The only obvious reason for this is that
            // the key is a duplicate. However, given the key is assigned
            // by the container itself and is a 64-bit integer, where the
            // next id is just incrementing the integer, this should never 
            // happen. But just in case...
            key = -1;
        }
        return key;
    }
    bool remove_item(K id)
    {
        return this->container.erase(id);
    }
    value_pointer get_item(K id)
    {
        // return this->container[id];
        auto item = this->container.find(id);
        // value_pointer ptr = (item == this->container.end()
        //                          ? std::nullptr_t
        //                          : item->second);
        // return ptr;
        return (item == this->container.end() ? nullptr : item->second);
    }
    bool replace_item(K id, value_pointer item)
    {
        bool retval = false;
        // TODO: What to do here if the item is not in the container?
        // For now we fail if the key does not exist as this would otherwise
        // be a silent failure. That seems dangerous.
        auto pos = this->container.find(id);
        if (pos != this->container.end())
        {
            this->container[id] = item;
        }
        return retval;
    }

    uint64_t get_number_of_items() const
    {
        return this->container.size();
    }
    uint64_t get_total_number_of_items() const
    {
        // TODO: Implement this if needed. For elements, a value can 
        // be a CompositeElement. In this case, it has its own 
        // collection of elements which need to be counted.
        return 0;
    }

    iterator get_iterator() { return container.begin(); }
    const_iterator get_const_iterator() { return container.cbegin(); }
    bool is_at_end(iterator iter) const { return iter == container.end(); } 
    bool is_at_end(const_iterator citer) const { return citer == container.cend(); }

private:
    std::map<K, value_pointer> container;
    mutable K next_id;
};

#endif
