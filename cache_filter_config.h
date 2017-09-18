#pragma once

#include <string>

#include "envoy/server/filter_config.h"
#include "common/common/logger.h"

namespace Envoy {
namespace Server {
namespace Configuration {

/**
 * Config registration for the rate limit filter. @see NamedHttpFilterConfigFactory.
 */
class UfesOrchFilterConfig : public NamedHttpFilterConfigFactory,  public Logger::Loggable<Logger::Id::config> {
public:
  HttpFilterFactoryCb createFilterFactory(const Json::Object& config, const std::string&,
                                          FactoryContext& context) override;
  std::string name() override { return "ufesorch"; }
};

} // Configuration
} // Server
} // Envoy
