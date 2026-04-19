#include "mx/engine/engine.hpp"

#include <gtest/gtest.h>
#include <vector>

// Minimal sink that collects emitted events for assertions
struct TestSink : mx::engine::IEventSink {
    std::vector<mx::AckNew> acks;
    std::vector<mx::AckCancel> cancels;
    std::vector<mx::Reject> rejects;

    void on_ack_new(const mx::AckNew& ack_new) override { acks.push_back(ack_new); }
    void on_ack_cancel(const mx::AckCancel& ack_cancel) override { cancels.push_back(ack_cancel); }
    void on_reject(const mx::Reject& reject) override { rejects.push_back(reject); }
};

TEST(MatchingEngineTest, EmitsAcksWithSeqAndIds) {
    TestSink sink;
    mx::engine::MatchingEngine engine(sink);

    mx::NewOrder first{};
    first.client_id = 1;
    first.side = mx::Side::Buy;
    first.price = 10000;
    first.qty = 10;

    mx::NewOrder second{};
    second.client_id = 2;
    second.side = mx::Side::Sell;
    second.price = 10001;
    second.qty = 5;

    engine.on_command(first);
    engine.on_command(second);

    ASSERT_TRUE(sink.rejects.empty());
    ASSERT_EQ(sink.acks.size(), 2u);
    EXPECT_EQ(sink.acks[0].order_id, 1u);
    EXPECT_EQ(sink.acks[0].header.event_seq, 1u);
    EXPECT_EQ(sink.acks[1].order_id, 2u);
    EXPECT_EQ(sink.acks[1].header.event_seq, 2u);
}
