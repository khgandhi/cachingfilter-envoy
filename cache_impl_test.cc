#include <string>

#include "cache_impl.h"
#include "cache.h"
#include "common/http/message_impl.h"

#include "test/mocks/http/mocks.h"
#include "test/mocks/upstream/mocks.h"
#include "test/test_common/utility.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

using testing::_;
using testing::NiceMock;
using testing::Return;
using testing::ReturnRef;
using testing::Invoke;

namespace Envoy {
namespace Http {
namespace Cache {

static Http::TestHeaderMapImpl genRequestHeadersToOrchestratorClient() {
	 return Http::TestHeaderMapImpl{{"test", "test"}, {":authority", "test.example.com"}, 
	 								{":path", "/test/path"}};
}

class MockOrchRequestCallbacks: public OrchRequestCallbacks {
public:
  MOCK_METHOD1(complete, void(OrchestratorResponse& response));
};


TEST(CacheImplTest, UfesClientBasic) {
	NiceMock<Upstream::MockClusterManager> cm;
	NiceMock<MockOrchRequestCallbacks> requestCallbacks;
	RestOrchClient restOrchClient (cm, "test_orch");
	EXPECT_CALL(cm, httpAsyncClientForCluster("test_orch")).WillOnce(ReturnRef(cm.async_client_));
	EXPECT_CALL(cm.async_client_, send_(_, _, _))
	  .WillOnce(
		  Invoke([&](Http::MessagePtr&, Http::AsyncClient::Callbacks& callbacks,
					 const Optional<std::chrono::milliseconds>&) -> Http::AsyncClient::Request* {
			callbacks.onSuccess(Http::MessagePtr {new Http::ResponseMessageImpl(Http::HeaderMapPtr{new Http::TestHeaderMapImpl{{"cache-key", "1234"}, 
																															   {":status", "200"},
																															   {"go-to-origin", "true"}
																															  }
																								  }
																			    )
		                                         }
		                       );
			return nullptr;
		  }));
	EXPECT_CALL(requestCallbacks, complete(_))
	.WillOnce(
			Invoke ([&](OrchestratorResponse& response)-> void {
				ASSERT_EQ("1234", response.getCacheKey());
	 })
	);
	restOrchClient.orchestrate(requestCallbacks, genRequestHeadersToOrchestratorClient());
}

} // namespace cache
}// namespace http
}// namespace envoy