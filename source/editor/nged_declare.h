#include <nlohmann/json_fwd.hpp>
#include <string>
#include <uuid.h>

#include "gmath.h"

#include <phmap/phmap.h>

namespace nged
{
    using sint = intptr_t;
    using uint = uintptr_t;
    using Json = nlohmann::json;
    using Vec2 = gmath::Vec2;
    using Mat3 = gmath::Mat3;
    using AABB = gmath::AABB;
    using Color = gmath::sRGBColor;

    template<class K, class V>
    using HashMap = phmap::flat_hash_map<K, V>;

    template<class K>
    using HashSet = phmap::flat_hash_set<K>;

    template<class T>
    using Vector = std::vector<T>;

    using String = std::string;
    using StringView = std::string_view;

    class Graph;
    class GraphItem;
    class Node;
    class Link;
    class Router;
    class ResizableBox;
    class GroupBox;
    class GraphItemFactory;
    class NodeGraphDoc;
    class Canvas;
    class NodeFactory;
    class MessageHub;

    using GraphItemPtr = std::shared_ptr<GraphItem>;
    using NodePtr = std::shared_ptr<Node>;
    using LinkPtr = std::shared_ptr<Link>;
    using RouterPtr = std::shared_ptr<Router>;
    using GraphPtr = std::shared_ptr<Graph>;
    using WeakGraphPtr = std::weak_ptr<Graph>;
    using NodeGraphDocPtr = std::shared_ptr<NodeGraphDoc>;
    using NodeFactoryPtr = std::shared_ptr<NodeFactory>;
    using GraphItemFactoryPtr = std::shared_ptr<GraphItemFactory>;
    using UID = uuids::uuid;
    using msghub = MessageHub;

}