#include "cache_filter.h"

#include <string>

#include "cache.h"
#include "cache_constants.h"
#include "common/http/header_map_impl.h"
#include "envoy/http/header_map.h"
#include "envoy/http/filter.h"
#include "envoy/http/codes.h"
#include "common/common/enum_to_int.h"

namespace Envoy {
namespace Http {
namespace Cache {

UfesOrchFilter::UfesOrchFilter(UfesOrchFilterConfigSharedPtr config, 
                               OrchClientPtr client):config_(config), client_(std::move(client)) {}

FilterHeadersStatus UfesOrchFilter::decodeHeaders(Envoy::Http::HeaderMap& headers, bool) {
  downstream_headers_ = &headers;
  client_->orchestrate(*this, headers);
  return FilterHeadersStatus::StopIteration;
}

FilterDataStatus UfesOrchFilter::decodeData(Buffer::Instance&, bool) {
  return FilterDataStatus::Continue;
}

FilterTrailersStatus UfesOrchFilter::decodeTrailers(HeaderMap&) {
  return FilterTrailersStatus::Continue;
}

void UfesOrchFilter::setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) {
  callbacks_ = &callbacks;
}

void UfesOrchFilter::onDestroy() {

}

void UfesOrchFilter::complete(OrchestratorResponse& response) {
	if (response.getResponseCode() != enumToInt(Http::Code::OK)) {
	ENVOY_LOG(debug, "Orchestrator request error with response code {}.", response.getResponseCode());
   }else {
	   ENVOY_LOG(debug, "Orchestrator request process successfully.");
   	if (!response.getCacheKey().empty()) {
   		downstream_headers_->addReferenceKey(CacheConstants::get().CacheKeyHeader, response.getCacheKey());
   		downstream_headers_->addReferenceKey(CacheConstants::get().GoToOrigin, response.getGoToOrigin());
   	}
   }
	// continue decoding.
   callbacks_->continueDecoding();
}

} //namespace envoy
}// namespace http
}// namespace cache
