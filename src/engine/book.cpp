#include "mx/engine/book.hpp"


namespace mx::engine { 

void OrderBook::add_resting(const Order& order) {
    if (order.side == mx::Side::Buy) { 
        auto& level = bids_[order.price]; 
        level.fifo_orders.push_back(order); 
        auto it = std::prev(level.fifo_orders.end()); 
        cancel_lookup_[order.id] = Handle{mx::Side::Buy, order.price, it};
    }
    else { 
        auto& level = asks_[order.price]; 
        level.fifo_orders.push_back(order); 
        auto it = std::prev(level.fifo_orders.end()); 
        cancel_lookup_[order.id] = Handle{mx::Side::Sell, order.price, it};
    }
}

bool OrderBook::cancel_order(mx::OrderId order_id) {
    auto it = cancel_lookup_.find(order_id); 
    if (it == cancel_lookup_.end()) return false; 

    // Found, check side 
    Handle& handle = it->second; 
    if (handle.side == mx::Side::Buy) { 
        auto price_level_it = bids_.find(handle.price); 
        if (price_level_it != bids_.end()) { 
            auto& price_level = price_level_it->second;
            price_level.fifo_orders.erase(handle.it); 
            if (price_level.fifo_orders.empty()) { 
                bids_.erase(price_level_it); 
            }
        }
    }
    else { 
        auto price_level_it = asks_.find(handle.price); 
        if (price_level_it != asks_.end()) { 
            auto& price_level = price_level_it->second;
            price_level.fifo_orders.erase(handle.it); 
            if (price_level.fifo_orders.empty()) { 
                asks_.erase(price_level_it); 
            }
        }
    }

    return true; 

}

void OrderBook::match_incoming(Order& incoming, const MatchCb& on_match) { 
    if (incoming.side == mx::Side::Buy) {
        // Match against asks
        while (incoming.qty > 0 ) {
            auto price_level = asks_.begin(); 
            if (price_level == asks_.end() || price_level->first > incoming.price ) {
                break; 
            }
            auto& orders_list = price_level->second.fifo_orders; 
            while (incoming.qty > 0 && !orders_list.empty()) { 
                auto& match_order = orders_list.front();
                mx::Qty match_qty = std::min(incoming.qty, match_order.qty);  
                on_match(match_order, incoming, match_qty, price_level->first); 
                incoming.qty -= match_qty; 
                match_order.qty -= match_qty; 
                if (match_order.qty == 0) {
                    // Fully filled, removed from book and cancel_lookup
                    cancel_lookup_.erase(match_order.id); 
                    orders_list.pop_front();
                    if (orders_list.empty()) {
                        asks_.erase(price_level);
                    }
                }
            }
        }
    }
    else {
        // Match against bids
        while (incoming.qty > 0 ) {
            auto price_level = bids_.begin(); 
            if (price_level == bids_.end() || price_level->first < incoming.price ) {
                break; 
            }
            auto& orders_list = price_level->second.fifo_orders; 
            while (incoming.qty > 0 && !orders_list.empty()) { 
                auto& match_order = orders_list.front();
                mx::Qty match_qty = std::min(incoming.qty, match_order.qty);  
                on_match(match_order, incoming, match_qty, price_level->first); 
                incoming.qty -= match_qty; 
                match_order.qty -= match_qty; 
                if (match_order.qty == 0) {
                    // Fully filled, removed from book and cancel_lookup
                    cancel_lookup_.erase(match_order.id); 
                    orders_list.pop_front();
                    if (orders_list.empty()) {
                        asks_.erase(price_level);
                    }
                }
            }
        }
    }
}


} //namespace mx::engine