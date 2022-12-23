#include "Utils.hpp"

std::vector<BaseObjectState*> getPrioritsedTargets(const std::set<BaseObjectState*>& objs, int side, b2Vec2 center) {
    std::vector<BaseObjectState*> filteredObjs;
    for (auto obj : objs) {
        if (obj->side() != side && obj->hostility() > Hostility::NON_HOSTILE && !obj->invisible()) {
            filteredObjs.push_back(obj);
        }
    }

    std::sort(filteredObjs.begin(), filteredObjs.end(), [center](BaseObjectState* left, BaseObjectState* right) {
        // Lexicographical sort of (most hostility, least distance)
        if (left->hostility() == right->hostility()) {
            return (left->body()->GetPosition() - center).LengthSquared() < (right->body()->GetPosition() - center).LengthSquared();
        }
        return left->hostility() > right->hostility();
    });
    return filteredObjs;
}
