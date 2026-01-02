#pragma once
#include "mx/core/types.hpp"

#include <cstdint> 
#include <type_traits>


namespace mx { 

enum class RejectReason : uint8_t { 
    InvalidOrder = 0, 
    InvalidPrice = 1,
    UnknownOrderId = 2,
    Other = 255,
}; 

struct NewOrder { 
    ClientId client_id{};
    Side side{}; 
    PriceTicks price{}; 
    Qty qty{}; 
}; 

struct CancelOrder {
    OrderId order_id{}; 
}; 

enum class EventType : uint8_t { 
    AckNew, 
    AckCancel, 
    Reject, 
}; 

struct EventHeader {
    EventSeq event_seq{}; 
    EventType event_type{}; 
}; 

struct AckNew {
    EventHeader header{}; 
    OrderId order_id{}; 
}; 

struct AckCancel { 
    EventHeader header{}; 
    OrderId order_id{}; 
}; 

struct Reject { 
    EventHeader header{}; 
    RejectReason reason{}; 
}; 

struct Trade {
    // TODO: implement 
}; 

struct Fill { 
    //TODO: implement 
}; 

static_assert(std::is_trivially_copyable_v<NewOrder>); 
static_assert(std::is_trivially_copyable_v<CancelOrder>);
static_assert(std::is_trivially_copyable_v<AckNew>);
static_assert(std::is_trivially_copyable_v<Reject>);


}