require 'tmpdir'

module WDM
  module SpecHelpers
    include FileUtils

    # Prepares the temporary fixture directory and
    # cleans it afterwards.
    #
    # @yield [path] an empty fixture directory
    # @yieldparam [String] path the path to the fixture directory
    #
    def fixture
      pwd  = FileUtils.pwd
      path = File.expand_path(File.join(pwd, "spec/.fixtures/#{rand(99999)}"))

      FileUtils.mkdir_p(path)
      FileUtils.cd(path)

      yield(path)

    ensure
      FileUtils.cd pwd
      FileUtils.rm_rf(path) if File.exists?(path)
    end

    # Prepares the monitor for the test with a fixture directory and the given 
    # callback, then it yields back to the user. It also gives time for 
    # the callbacks to be called. After the tests run, it stops the monitor.
    #
    # @yield
    # 
    def watch_fixture_with(monitor, callback)
      fixture do |f|
        monitor.watch(f, &callback)

        thread = Thread.new(monitor) { |m| m.run! } 
        sleep(0.1) # give time for the monitor to bootup

        yield

        sleep(0.1) # allow the monitor to run the callbacks
        subject.stop
        thread.join
      end
    end
  end
end