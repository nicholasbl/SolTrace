#include "simulation_data.hpp"

SimulationData::SimulationData()
{
    return;
}

SimulationData::~SimulationData()
{
    return;
}

element_id SimulationData::add_element(element_ptr el)
{
    auto key = this->next_element_id;

    // Make sure this element_id is not in use
    assert(this->my_elements.find(key) == this->my_elements.end());
    assert(this->composite_elements.find(key) == this->composite_elements.end());

    if (!el->is_composite() && !el->is_single())
    {
        // We should never get here--it is expected that all elements are
        // either SingleElements or CompositeElements
        key = ELEMENT_ERROR;
    }
    else if (el->get_id() != ELEMENT_ID_UNASSIGNED)
    {
        key = ELEMENT_ALREADY_REGISTERED;
    }
    else
    {
        el->set_id(key);
        ++this->next_element_id;
        if (el->is_single())
        {
            single_element_ptr sptr =
                std::dynamic_pointer_cast<SingleElement>(el);
            assert(sptr != nullptr);
            this->add_single_element(key, sptr);
        }
        else
        {
            composite_element_ptr cptr =
                std::dynamic_pointer_cast<CompositeElement>(el);
            assert(cptr != nullptr);
            this->add_composite_element(key, cptr);
        }
    }

    return key;
}

void SimulationData::add_single_element(element_id key,
                                        single_element_ptr el)
{
    SingleElementMap::value_type to_insert(key, el);
    auto result = this->my_elements.insert(to_insert);
    assert(result.second);
    return;
}

void SimulationData::add_composite_element(element_id key,
                                           composite_element_ptr el)
{
    CompositeElementMap::value_type to_insert(key, el);
    auto result = this->composite_elements.insert(to_insert);
    assert(result.second);
    auto iter = el->get_iterator();
    while (!el->is_at_end(iter))
    {
        this->add_element(iter->second);
        ++iter;
    }
    return;
}

bool SimulationData::replace_element(element_id id, element_ptr el)
{
    bool retval = false;

    if (this->my_elements.find(id) != this->my_elements.end())
    {
        assert(this->composite_elements.find(id) ==
               this->composite_elements.end());

        retval = true;
        el->set_id(id);

        if (el->is_single())
        {
            // Original element and replacement are both SingleElements
            auto iter = this->my_elements.find(id);
            this->remove_single_element(iter);

            single_element_ptr ptr =
                std::dynamic_pointer_cast<SingleElement>(el);
            assert(ptr != nullptr);
            this->add_single_element(id, ptr);
        }
        else
        {
            // Original element is SingleElement and replacement
            // is CompositeElement
            assert(el->is_composite());

            auto iter = this->my_elements.find(id);
            this->remove_single_element(iter);

            composite_element_ptr ptr =
                std::dynamic_pointer_cast<CompositeElement>(el);
            assert(ptr != nullptr);
            this->add_composite_element(id, ptr);
        }
    }
    else if (this->composite_elements.find(id) != this->composite_elements.end())
    {
        assert(this->my_elements.find(id) == this->my_elements.end());

        retval = true;
        el->set_id(id);

        if (el->is_composite())
        {
            // Original element and replacement are CompositeElements
            auto iter = this->composite_elements.find(id);
            assert(iter != this->composite_elements.end());
            this->remove_composite_element(iter);

            composite_element_ptr rep =
                std::dynamic_pointer_cast<CompositeElement>(el);
            assert(rep != nullptr);
            this->add_composite_element(id, rep);
        }
        else
        {
            // Original element is CompositeElement and replacement
            // is SingleElement
            assert(el->is_single());

            auto iter = this->composite_elements.find(id);
            this->remove_composite_element(iter);

            single_element_ptr ptr =
                std::dynamic_pointer_cast<SingleElement>(el);
            assert(ptr != nullptr);
            this->add_single_element(id, ptr);
        }
    }
    else
    {
        // Intentional no-op
    }
    return retval;
}

element_ptr SimulationData::get_element(element_id id)
{
    // return this->my_elements.get_item(id);
    element_ptr retval = nullptr;
    auto iter1 = this->my_elements.find(id);
    auto iter2 = this->composite_elements.find(id);
    if (iter1 != this->my_elements.end())
    {
        retval = iter1->second;
    }
    else if (iter2 != this->composite_elements.end())
    {
        retval = iter2->second;
    }
    else
    {
        // Intentional no-op
    }
    return retval;
}

uint_fast64_t SimulationData::remove_element(element_id id)
{
    uint_fast64_t retval = 0;
    auto iter1 = this->my_elements.find(id);
    auto iter2 = this->composite_elements.find(id);
    if (iter1 != this->my_elements.end())
    {
        retval = remove_single_element(iter1);
    }
    else if (iter2 != this->composite_elements.end())
    {
        retval = remove_composite_element(iter2);
    }
    else
    {
        // Intentional no-op
    }
    return retval;
}

uint_fast64_t SimulationData::remove_single_element(
    SingleElementMap::iterator iter)
{
    iter->second->set_id(ELEMENT_ID_UNASSIGNED);
    this->my_elements.erase(iter);
    return 1;
}

uint_fast64_t SimulationData::remove_composite_element(
    CompositeElementMap::iterator iter)
{
    composite_element_ptr cptr = iter->second;
    iter->second->set_id(ELEMENT_ID_UNASSIGNED);
    this->composite_elements.erase(iter);
    return this->remove_subelements(cptr);
}

uint_fast64_t SimulationData::remove_subelements(composite_element_ptr el)
{
    uint_fast64_t retval = 0;
    element_ptr ptr = nullptr;
    for (auto iter = el->get_iterator(); !el->is_at_end(iter); ++iter)
    {
        ptr = iter->second;
        retval += this->remove_element(ptr->get_id());
    }
    return retval;
}

int SimulationData::update_simulation_positions()
{
    int sts = 0;
    return sts;
}

int SimulationData::update_simulation_positions(const Time &t)
{
    this->my_parameters.sim_dt.my_time = t;
    return this->update_simulation_positions();
}

int SimulationData::update_simulation_positions(const Date &d)
{
    this->my_parameters.sim_dt.my_date = d;
    return this->update_simulation_positions();
}

int SimulationData::update_simulation_positions(const DateTime &dt)
{
    this->my_parameters.sim_dt.set(dt);
    return this->update_simulation_positions();
}

int SimulationData::import_from_file(const char *file_name)
{
    int sts = 0;
    // TODO: Implement this
    return sts;
}

int SimulationData::import_from_file(const std::string file_name)
{
    return this->import_from_file(file_name.c_str());
}
