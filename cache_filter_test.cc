#include "cache_filter.h"
#include "common/http/header_map_impl.h"
#include "common/http/headers.h"
#include "cache.h"
#include "cache_constants.h"
#include "common/buffer/buffer_impl.h"

#include "test/mocks/http/mocks.h"
#include "test/mocks/upstream/mocks.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::_;
using testing::Invoke;
using testing::NiceMock;

namespace Envoy {
namespace Http {
namespace Cache {

class MockOrchClient: public OrchClient {

public:
	MOCK_METHOD2(orchestrate, void(OrchRequestCallbacks& callbacks, const Http::HeaderMap& headers));

	MOCK_METHOD1(createRequest, std::string(const Http::HeaderMap& header));
};

class UfesOrchFilterTest : public testing::Test {
public:
	  const std::string ufes_orch_json = R"EOF(
	    {
			"type": "decoder",
			"name": "ufesorch",
			"config": {
			  "orchestrator_cluster": "local-orch",
			  "timeout": "10000"
			}
        }
	  )EOF";

	  void SetUpTest(const std::string json) {
	    Json::ObjectSharedPtr config = Json::Factory::loadFromString(json);
	    mock_client_ = new MockOrchClient();
	    config_.reset(new UfesOrchFilterConfig("local-orch", 5000, cm_));
	    filter_.reset(new UfesOrchFilter(config_, OrchClientPtr{mock_client_}));
	    filter_->setDecoderFilterCallbacks(filter_callbacks_);
	  }

	  UfesOrchFilterConfigSharedPtr config_;
	  std::unique_ptr<UfesOrchFilter> filter_;
	  NiceMock<MockStreamDecoderFilterCallbacks> filter_callbacks_;
	  TestHeaderMapImpl request_headers_;
	  MockOrchClient* mock_client_;
	  NiceMock<Upstream::MockClusterManager> cm_;
	  Buffer::OwnedImpl data_;
};

TEST_F(UfesOrchFilterTest, UfesOrchBasicFilter) {
	SetUpTest(ufes_orch_json);
	EXPECT_CALL(filter_callbacks_, encodeHeaders_(_, _))
		.WillOnce(
				Invoke([&](HeaderMap& headers, bool)->void {
					ASSERT_TRUE(headers.get(CacheConstants::get().CacheKeyHeader) != nullptr);
	 })
	);
	EXPECT_CALL(*mock_client_, orchestrate(_, _))
		.WillOnce(
				Invoke ([&](OrchRequestCallbacks& callbacks, const Http::HeaderMap&)-> void {
					OrchestratorResponse response("1234", 200, "true");
					callbacks.complete(response);
		 })
		);

	EXPECT_EQ(FilterHeadersStatus::StopIteration, filter_->decodeHeaders(request_headers_, false));
	EXPECT_EQ(FilterDataStatus::Continue, filter_->decodeData(data_, false));
	EXPECT_EQ(FilterTrailersStatus::Continue, filter_->decodeTrailers(request_headers_));
}

  }
 }
}
