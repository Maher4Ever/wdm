require 'pathname'
require 'wdm'

Dir["#{File.dirname(__FILE__)}/support/**/*.rb"].each { |f| require f }

RSpec.configure do |config|
  config.run_all_when_everything_filtered = true
  config.color = true
  config.filter_run :focus

  config.before(:all) { `rake compile` }

  config.include WDM::SpecSupport
end

# Be verbose about errors in threads
Thread.abort_on_exception = true