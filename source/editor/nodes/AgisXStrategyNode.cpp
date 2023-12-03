module;

module AgisXStrategyNodeMod;

import StrategyNode;
import AllocationNode;

namespace AgisX
{

//==================================================================================================
std::expected<UniquePtr<Agis::AST::AllocationNode>, Agis::AgisException> AgisXAllocationNode::to_agis() const noexcept
{
	return std::unexpected(Agis::AgisException("not implemented"));
}

std::expected<UniquePtr<Agis::AST::StrategyNode>, Agis::AgisException> AgisXStrategyNode::to_agis() const noexcept
{
	return std::unexpected(Agis::AgisException("not implemented"));
}

}