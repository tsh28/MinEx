#pragma once
#include "mx/core/types.hpp"
#include <functional>
#include <list>
#include <map>

namespace mx::engine { 


struct Order { 
    mx::OrderId id{}; 
    mx::ClientId client_id{}; 
    mx::Side side{}; 
    mx::PriceTicks price{}; 
    mx::Qty qty{}; 
}; 

class OrderBook { 
public:

    // Called once per match (maker=resting, taker=incoming).
    using MatchCb = std::function<void(const Order& maker,
                                     const Order& taker,
                                     mx::Qty match_qty,
                                     mx::PriceTicks trade_price)>;

    // Add an order to the book as resting liquidity (must have qty > 0).
    void add_resting(const Order& order); 

    // Cancel by order id. Returns true if removed
    bool cancel_order(mx::OrderId order_id); 

    // Match incoming vs resting opposite side. Decrements incoming.qty.
    // After this returns:
    //   - incoming.qty == 0 => fully filled
    //   - incoming.qty > 0  => caller may rest remainder via add_resting(incoming)
    void match_incoming(Order& incoming, const MatchCb& on_match);

    // Helpers
    bool has_best_ask() const { return !asks_.empty(); }
    bool has_best_bid() const { return !bids_.empty(); }
    mx::PriceTicks best_ask_price() const { return asks_.begin()->first; }
    mx::PriceTicks best_bid_price() const { return bids_.begin()->first; }

private: 
    struct PriceLevel {
        std::list<Order> fifo_orders; 
    }; 

    struct Handle { 
        mx::Side side; 
        mx::PriceTicks price;
        std::list<Order>::iterator it; 
    }; 

    // For selling: lowest ask price first, which will be matched first against incoming buys
    std::map<mx::PriceTicks, PriceLevel> asks_;
    // For buying: highest bid price first, which will be matched first against incoming sells
    std::map<mx::PriceTicks, PriceLevel, std::greater<mx::PriceTicks>> bids_; 

    std::unordered_map<mx::OrderId, Handle> cancel_lookup_; 

    // internal helpers
    template <typename SideMap>
    void add_to_side(SideMap& side_map, const Order& o);

    template <typename SideMap>
    void erase_if_level_empty(SideMap& side_map, mx::PriceTicks price);
}; 
} //namespace mx::engine
