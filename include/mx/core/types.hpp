#pragma once
#include <cstdint>


namespace mx {

using ClientId = uint32_t;
using OrderId = uint64_t; 
using ExecId = uint64_t; 
using EventSeq = uint64_t; 

using PriceTicks = int32_t; 
using Qty = int32_t; 
enum class Side : uint8_t { Buy = 0, Sell = 1}; 

}; // namespace mx

