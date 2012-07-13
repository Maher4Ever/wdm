require 'ostruct'
require 'thread'

module WDM
  module SpecSupport

    # Runs the monitor and collects changes for the specified amount of times
    # on the given directory. It stops the monitor afterwards.
    #
    # @yield
    #
    def run_and_collect_multiple_changes(monitor, times, directory)
      result = OpenStruct.new(directory: directory, changes: [])
      i = 0
      result.changes[i] = OpenStruct.new(called: false)
      can_return = false

      monitor.watch(directory) do |change|
        next if can_return

        result.changes[i].called = true;
        result.changes[i].change = change

        i += 1

        if i < times
          result.changes[i] = OpenStruct.new(called: false)
        else
          can_return = true
        end
      end

      thread = Thread.new(monitor) { |m| m.run! }
      sleep(0.2) # give time for the monitor to bootup

      yield # So much boilerplate code to get here :S

      sleep(0.2) # allow the monitor to run the callbacks

      # Nothing can change after the callback if there is only one of them,
      # so never wait for one callback
      if times > 1
        until can_return
          sleep(0.1)
        end
      end

      return result
    ensure
      monitor.stop
      thread.join if thread
    end

    # Helper method for running the monitor one time.
    #
    # @yield
    #
    def run(monitor, directory, &block)
      result = run_and_collect_multiple_changes(monitor, 1, directory, &block)
      result.changes[0].directory = result.directory
      result.changes[0]
    end

    # Helper method for using the run method with the fixture helper.
    #
    # @yield
    #
    def run_with_fixture(monitor, &block)
      fixture do |f|
        run(monitor, f, &block)
      end
    end
  end
end