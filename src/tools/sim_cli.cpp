#include "mx/engine/engine.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>



using namespace mx; 


namespace { 

struct TextSink : mx::engine::IEventSink { 
    explicit TextSink(std::ostream& out) : out_(out) {}

    void on_ack_new(const AckNew& ack_new) override { 
        out_ << "event_seq=" << ack_new.header.event_seq << " AckNew order_id=" << ack_new.order_id << "\n"; 
    }

    void on_ack_cancel(const AckCancel& ack_cancel) override {
        out_ << "event_seq=" << ack_cancel.header.event_seq << " AckCancel order_id=" << ack_cancel.order_id << "\n"; 
    }

    void on_reject(const Reject& reject) override { 
        out_ << "event_seq=" << reject.header.event_seq << " Reject reason=" << static_cast<int>(reject.reason) << "\n"; 
    }

private:
    std::ostream& out_;
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
    if (argc != 2 && argc != 4) { 
        std::cerr << "Usage: sim_cli <input_file> [--log <log_file>]\n"; 
        return 1;
    }

    std::string input_path = argv[1];
    std::filesystem::path exe_dir = std::filesystem::path(argv[0]).parent_path(); 
    std::filesystem::path default_log = exe_dir / ".." / "tests/golden/expected/basic.log";

    // std::filesystem::path exe_dir = std::filesystem::path(argv[0]).parent_path();
    // std::filesystem::path default_log = exe_dir / ".." / "tests/golden/expected/basic.log";
    std::string log_path = default_log.lexically_normal().string();
    if (argc == 4) {
        std::string flag = argv[2];
        if (flag != "--log") {
            std::cerr << "Usage: sim_cli <input_file> [--log <log_file>]\n";
            return 1;
        }
        log_path = argv[3];
    }

    std::ifstream infile(input_path); 
    if (!infile.is_open()) { 
        std::cerr << "Error opening file: " << input_path << "\n"; 
        return 1; 
    }

    std::filesystem::path log_dir = std::filesystem::path(log_path).parent_path();
    if (!log_dir.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(log_dir, ec);
        if (ec) {
            std::cerr << "Error creating log directory: " << log_dir << "\n";
            return 1;
        }
    }

    std::ofstream logfile(log_path); 
    if (!logfile.is_open()) { 
        std::cerr << "Error opening log file: " << log_path << "\n"; 
        return 1;
    }

    TextSink sink(logfile); 
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
