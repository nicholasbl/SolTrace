#ifndef SOLTRACE_STAGE_ELEMENT_H
#define SOLTRACE_STAGE_ELEMENT_H

#include <memory>
#include "composite_element.hpp"
#include "element.hpp"

class StageElement: public CompositeElement
{
public:
    StageElement(int_fast64_t stage);
    ~StageElement();
    virtual bool is_stage() const { return true; }
    virtual element_id add_element(element_ptr el);
private:
};

using stage_ptr = std::shared_ptr<StageElement>;
template <typename... Args>
inline auto make_stage(Args &&...args)
{
    return make_element<StageElement>(std::forward<Args>(args)...);
}

#endif
