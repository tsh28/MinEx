#include "mx/engine/engine.hpp"

#include <fstream>
#include <iostream>
#include <sstream>



using namespace mx; 


namespace { 

struct TextSink : mx::engine::IEventSink { 
    void on_ack_new(const AckNew& ack_new) override { 
        std::cout << "AckNew: event_seq=" << ack_new.header.event_seq << " order_id=" << ack_new.order_id << "\n"; 
    }

    void on_ack_cancel(const AckCancel& ack_cancel) override {
        std::cout << "AckCancel: event_seq=" << ack_cancel.header.event_seq << " order_id=" << ack_cancel.order_id << "\n"; 
    }

    void on_reject(const Reject& reject) override { 
        std::cout << "Reject: event_seq=" << reject.header.event_seq << " reason=" << static_cast<int>(reject.reason) << "\n"; 
    }
}; 


Side parse_side (const std::string& s) { 
    return (s == "B") ? Side::Buy : Side::Sell;     
}

bool split_kv(const std::string& tok, std::string& k, std::string& v) { 
    auto pos = tok.find('=');
    if (pos == std::string::npos) return false; 
    k = tok.substr(0, pos); 
    v = tok.substr(pos + 1);
    return true; 
}

} //namespace


int main(int argc, char* argv[]) { 
    if (argc != 2) { 
        std::cerr << "Usage: sim_cli <input_file>\n"; 
        return 1;
    }

    std::ifstream infile(argv[1]); 
    if (!infile.is_open()) { 
        std::cerr << "Error opening file: " << argv[1] << "\n"; 
        return 1; 
    }

    TextSink sink; 
    mx::engine::MatchingEngine engine(sink); 
    std::string line; 
    while (std::getline(infile, line)) { 
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line); 
        std::string cmd; 
        iss >> cmd; 
        if (cmd == "NEW") { 
            NewOrder new_order{}; 
            std::string tok; 
            while (iss >> tok) {
                std::string k,v; 
                if (!split_kv(tok,k,v)) { 
                    continue; 
                }
                // "NEW cid=1 side=B px=1000 qty=10"
                if (k == "cid") new_order.client_id = static_cast<ClientId>(std::stoul(v)); 
                else if (k == "side") new_order.side = parse_side(v); 
                else if (k == "px") new_order.price = static_cast<PriceTicks>(std::stoi(v)); 
                else if (k == "qty") new_order.qty = static_cast<Qty>(std::stoi(v));

            }
            engine.on_command(new_order); 
        } 
        else if (cmd == "CANCEL") { 
            CancelOrder cancel_order{}; 
            std::string tok; 
            while (iss >> tok) { 
                std::string k,v; 
                if (!split_kv(tok,k,v)) { 
                    continue; 
                }
                if (k == "oid") { 
                    cancel_order.order_id = static_cast<OrderId>(std::stoull(v)); 
                }
            }
            engine.on_command(cancel_order); 
        }

    }
    return 0; 
}
