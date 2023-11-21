#pragma once
#include "AgisAST.h"
#include <optional>
#include <nlohmann/json_fwd.hpp>

using Json = nlohmann::json;

using namespace Agis::AST;

namespace AgisX
{
	std::string agis_operator_to_string(AgisOperator op);
	AgisOperator string_to_agis_operator(std::string const& str);

	bool serialize_pair(Json& json, std::string const& key, std::string const& value);
	std::optional<std::string> deserialize_string(Json const& json, std::string const& key);
}