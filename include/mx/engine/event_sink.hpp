#pragma once
#include "mx/core/messages.hpp"


namespace mx::engine { 


struct IEventSink { 
    virtual ~IEventSink() = default; 
    virtual void on_ack_new(const AckNew& ack_new) = 0;
    virtual void on_ack_cancel(const AckCancel& ack_cancel) = 0;
    virtual void on_reject(const Reject& reject) = 0;
    // virtual void on_trade(const Trade& trade) = 0;
    // virtual void on_fill(const Fill& fill) = 0;
}; 

/*

Why not templates instead of virtual calls?


Why virtual is fine here (especially early)

event emission rate is much lower than market data rate

virtual call overhead is negligible at this stage

makes composition easy (CompositeSink)


Later optimization options: 

CRTP

function pointers

inline lambdas

compile-time sinks

But do not optimize this now. Correctness + clarity first.

*/

}


