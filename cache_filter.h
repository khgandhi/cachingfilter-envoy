#pragma once

#include <chrono>
#include <string>

#include "envoy/http/filter.h"
#include "envoy/upstream/cluster_manager.h"
#include "cache.h"
#include "envoy/buffer/buffer.h"
#include "common/common/logger.h"

#include "common/http/header_map_impl.h"

namespace Envoy {
namespace Http {
namespace Cache {

class UfesOrchFilterConfig {
public:
	 UfesOrchFilterConfig(const std::string orchestrator_cluster_name, 
	 					  int64_t timeout)
	      : orchestrator_cluster_(orchestrator_cluster_name),
	        timeout_(std::chrono::milliseconds(timeout)) {}
private:
	const std::string orchestrator_cluster_;
	const std::chrono::milliseconds timeout_;
};

typedef std::shared_ptr<UfesOrchFilterConfig> UfesOrchFilterConfigSharedPtr;

class UfesOrchFilter : public StreamDecoderFilter, public OrchRequestCallbacks, 
												   public Logger::Loggable<Logger::Id::filter>  {

public:
	UfesOrchFilter(UfesOrchFilterConfigSharedPtr config, OrchClientPtr client);

	// Http::StreamFilterBase
	void onDestroy() override;

	// Http::StreamDecoderFilter
	FilterHeadersStatus decodeHeaders(HeaderMap& headers, bool end_stream) override;
	FilterDataStatus decodeData(Buffer::Instance& data, bool end_stream) override;
	FilterTrailersStatus decodeTrailers(HeaderMap& trailers) override;
	void setDecoderFilterCallbacks(StreamDecoderFilterCallbacks& callbacks) override;

	// Cache::OrchRequestCallbacks
	void complete(OrchestratorResponse& response) override;

private:
	UfesOrchFilterConfigSharedPtr config_;
	OrchClientPtr client_;
	StreamDecoderFilterCallbacks* callbacks_{};
	Upstream::ClusterInfoConstSharedPtr cluster_;
	Http::HeaderMap* downstream_headers_{};

};
  }// namespace Cache
 }// namespace Http
}// namespace Envoy
