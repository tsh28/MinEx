#pragma once
#include "mx/core/messages.hpp"
#include "mx/engine/event_sink.hpp"


namespace mx::engine { 

class MatchingEngine { 
public: 
    explicit MatchingEngine(IEventSink& sink) : sink_(sink) {}

    void on_command(const mx::NewOrder& cmd); 
    void on_command(const mx::CancelOrder& cmd); 


private: 
    IEventSink& sink_;
    mx::OrderId next_order_id_{1}; 
    mx::EventSeq next_event_seq_{1}; 

    mx::EventSeq next_seq() { 
        return next_event_seq_++; 
    }
    void reject(mx::RejectReason reason); 

}; 

}