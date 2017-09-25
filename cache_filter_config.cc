#include "cache_filter_config.h"

#include <chrono>
#include <string>

#include "cache_filter.h"
#include "cache_impl.h"
#include "envoy/registry/registry.h"

namespace Envoy {
namespace Server {
namespace Configuration {

HttpFilterFactoryCb UfesOrchFilterConfig::createFilterFactory(const Json::Object& config,
                                                           const std::string&,
                                                           FactoryContext& context) {
  std::string orchestrator_cluster = config.getString("orchestrator_cluster");
  int64_t timeout = config.getInteger("timeout");

  Http::Cache::UfesOrchFilterConfigSharedPtr filter_config(new Http::Cache::UfesOrchFilterConfig(orchestrator_cluster, timeout));
  Http::Cache::OrchClientPtr orch_client(new Http::Cache::RestOrchClient(context.clusterManager(), orchestrator_cluster));

  return [filter_config,orch_client](Http::FilterChainFactoryCallbacks& callbacks) -> void {
    callbacks.addStreamDecoderFilter(Http::StreamDecoderFilterSharedPtr{new Http::Cache::UfesOrchFilter(filter_config, orch_client)});
  };
}

/**
 * Static registration for the ufes orch filter. @see RegisterFactory.
 */
static Registry::RegisterFactory<UfesOrchFilterConfig, NamedHttpFilterConfigFactory> register_;

} // Configuration
} // Server
} // Envoy
