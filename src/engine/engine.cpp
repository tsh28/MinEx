#include "mx/engine/engine.hpp"

namespace mx::engine { 

void MatchingEngine::reject(mx::RejectReason reason) { 
    Reject rej; 
    rej.header.event_seq = next_seq();
    rej.header.event_type = mx::EventType::Reject;
    rej.reason = reason;
    sink_.on_reject(rej); 
}

void MatchingEngine::on_command(const mx::NewOrder& cmd) { 
    // Make sure price and qty are valid
    if (cmd.price <= 0) {
        reject(mx::RejectReason::InvalidPrice);
        return; 
    }
    else if (cmd.qty <= 0) {
        reject(mx::RejectReason::InvalidOrder);
        return; 
    }

    // No matching yet, just ack, accept and assigne order_id
    mx::AckNew ack; 
    ack.header.event_seq = next_seq(); 
    ack.header.event_type = mx::EventType::AckNew; 
    ack.order_id = next_order_id_++;
    sink_.on_ack_new(ack); 
}

void MatchingEngine::on_command(const mx::CancelOrder& cmd) { 
    mx::AckCancel ack; 
    ack.header.event_seq = next_seq();
    ack.header.event_type = mx::EventType::AckCancel;
    ack.order_id = cmd.order_id;
    sink_.on_ack_cancel(ack); 
}



} //namespace mx::engine
