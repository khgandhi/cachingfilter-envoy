#include "cache_impl.h"

#include <chrono>
#include <string>
#include <unordered_map>

#include "common/http/message_impl.h"
#include "cache_constants.h"
#include "common/common/enum_to_int.h"
#include "common/http/utility.h"
#include "envoy/http/header_map.h"
#include "envoy/http/message.h"
#include "envoy/upstream/cluster_manager.h"
#include "cache.h"
#include "common/common/assert.h"


namespace Envoy {
namespace Http {
namespace Cache {

void RestOrchClient::orchestrate(OrchRequestCallbacks& callbacks, const Http::HeaderMap& headers) {
 	//setting the callback
 	ASSERT(!callbacks_);
  	callbacks_ = &callbacks;
	 
	// create an http request for the external orchestrator layer
	Envoy::Http::MessagePtr request(new Envoy::Http::RequestMessageImpl());
	request->headers().insertMethod().value(Http::Headers::get().MethodValues.Post);
	request->headers().insertPath().value(std::string(CacheConstants::get().OrchestratorClientProperties.OrchestratorUri));
	request->headers().insertHost().value(orch_cluster_name_);
	request->headers().insertContentType().value(std::string("application/json"));
	request->body() = Buffer::InstancePtr(new Buffer::OwnedImpl(createRequest(headers)));

	 // make a call to the underlying orchestrator cluster.
	cm_.httpAsyncClientForCluster(orch_cluster_name_)
	    .send(std::move(request), *this, std::chrono::milliseconds(2000));
  }

 // create a request for the orchestrator layer.
 std::string RestOrchClient::createRequest(const Http::HeaderMap& original_headers_map) {
	 std::string host = std::string (original_headers_map.Host()->value().c_str());
	 std::string uri = std::string (original_headers_map.Path()->value().c_str());
	 OrchestratorRequest request(host, uri);
	 return request.toJsonString();
 }

// call method if the we get a response from orchestrator
void RestOrchClient::onSuccess(Http::MessagePtr&& response) {
	// ger response code
	uint64_t response_code = Http::Utility::getResponseStatus(response->headers());

	// get cache key from the response headers
	std::string cache_key = std::string (response->headers().get(CacheConstants::get().CacheKeyHeader)->value().c_str());

	std::string go_to_origin = std::string (response->headers().get(CacheConstants::get().GoToOrigin)->value().c_str());

	ENVOY_LOG(debug, "cache key '{}' returned", cache_key);
	ENVOY_LOG(debug, "go to origin flag '{}' returned", go_to_origin);

	//invoke the callback with orchestration response
	OrchestratorResponse orch_resp_obj (cache_key, response_code, go_to_origin);
	callbacks_->complete(orch_resp_obj);
	callbacks_ = nullptr;
}

// call method if we get a failure response from orchestrator
void RestOrchClient::onFailure(Http::AsyncClient::FailureReason) {
	//invoke the callback with an error code on the response code.
	OrchestratorResponse orch_resp_obj(nullptr, enumToInt(Http::Code::ServiceUnavailable), nullptr);
	callbacks_->complete(orch_resp_obj);
	callbacks_ = nullptr;
}

void OrchestratorRequest::addHttpHeader(const std::string& key, const std::string& value) {
	http_headers_.emplace(key, value);
}

void OrchestratorRequest::addQueryParam(const std::string& key, const std::string& value) {
	query_parameters_.emplace(key, value);
}

std::string OrchestratorRequest::toJsonString() {
	// start json
	std::string json("{\n");

	// target host ("targetHost":")
	json += "\"targetHost\":\"" + target_host_ + "\",\n";

	//target uri ("targetUri":")
	json += "\"targetUri\":\"" + target_uri_ + "\",\n";

	// http headers
	json += "\"httpHeaders\":{\n";

	for(std::pair<std::string, std::string> element : http_headers_) {
		json += "\"" + element.first + "\":"+ "\""+element.second+"\"\n,";
	}

	// removing the extra trailing comma
	json = json.substr(0, json.size() -1) + "\n}\n";

	// query paramaters
	json += "\"queryParams\":{\n";
	for(std::pair<std::string, std::string> element : query_parameters_) {
		json += "\"" + element.first + "\":"+ "\""+element.second+"\"\n,";
	}
	// removing the extra trailing comma
	json = json.substr(0, json.size() -1) + "\n}\n";

	//end of json
	json += "}";

	return json;

  }
 } // namespace cache
} // namespace http
} // namespace envoy
