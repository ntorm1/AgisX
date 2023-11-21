#include "AgisXSerialize.h"

#include "nlohmann/json.hpp"

//import AgisXNode;



namespace AgisX
{

//==================================================================================================
std::string agis_operator_to_string(AgisOperator op) {
    static const std::unordered_map<AgisOperator, std::string> op_to_string{
        {AgisOperator::INIT, "INIT"},
        {AgisOperator::IDENTITY, "IDENTITY"},
        {AgisOperator::ADD, "ADD"},
        {AgisOperator::SUBTRACT, "SUBTRACT"},
        {AgisOperator::MULTIPLY, "MULTIPLY"},
        {AgisOperator::DIVIDE, "DIVIDE"}
    };

    auto it = op_to_string.find(op);
    return (it != op_to_string.end()) ? it->second : "UNKNOWN";
}


//==================================================================================================
AgisOperator string_to_agis_operator(std::string const& str) {
    static const std::unordered_map<std::string, AgisOperator> string_to_op{
        {"INIT", AgisOperator::INIT},
        {"IDENTITY", AgisOperator::IDENTITY},
        {"ADD", AgisOperator::ADD},
        {"SUBTRACT", AgisOperator::SUBTRACT},
        {"MULTIPLY", AgisOperator::MULTIPLY},
        {"DIVIDE", AgisOperator::DIVIDE}
    };

    auto it = string_to_op.find(str);
    return (it != string_to_op.end()) ? it->second : AgisOperator::INIT; // Default to INIT if not found
}


//==================================================================================================
bool serialize_pair(Json& json, std::string const& key, std::string const& value)
{
	json[key] = value;
	return true;
}


//==================================================================================================
std::optional<std::string> deserialize_string(Json const& json, std::string const& key)
{
    if (json.contains(key))
	{
		return json[key].get<std::string>();
	}
	else
	{
		return std::nullopt;
	}
}

}