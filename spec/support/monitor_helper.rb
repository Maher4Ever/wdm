require 'ostruct'
require 'thread'

module WDM
  module SpecSupport

    # Runs the monitor and collects changes for the specified amount of times
    # on the given directory. It stops the monitor afterwards.
    #
    # @yield
    #
    def run_and_collect_multiple_changes(monitor, times, directory, *flags, &block)
      watch_and_run(monitor, times, false, directory, *flags, &block)
    end

    # Helper method for running the monitor one time.
    #
    # @yield
    #
    def run(monitor, directory, *flags, &block)
      result = watch_and_run(monitor, 1, false, directory, *flags, &block)
      result.changes[0].directory = result.directory
      result.changes[0]
    end

    # Helper method for using the run method with the fixture helper.
    #
    # @yield
    #
    def run_with_fixture(monitor, *flags, &block)
      fixture do |f|
        run(monitor, f, *flags, &block)
      end
    end

    # Runs the monitor recursively and collects changes for the specified amount of times
    # on the given directory. It stops the monitor afterwards.
    #
    # @yield
    #
    def run_recursively_and_collect_multiple_changes(monitor, times, directory, *flags, &block)
      watch_and_run(monitor, times, true, directory, *flags, &block)
    end

    # Helper method for running the monitor recursively one time.
    #
    # @yield
    #
    def run_recursively(monitor, directory, *flags, &block)
      result = watch_and_run(monitor, 1, true, directory, *flags, &block)
      result.changes[0].directory = result.directory
      result.changes[0]
    end

    # Helper method for using the run method recursively with the fixture helper.
    #
    # @yield
    #
    def run_recursively_with_fixture(monitor, *flags, &block)
      fixture do |f|
        run_recursively(monitor, f, *flags, &block)
      end
    end

  private

    # Very customizable method to watch directories and then run the monitor
    #
    # @yield
    #
    def watch_and_run(monitor, times, recursively, directory, *flags)
      result = OpenStruct.new(directory: directory, changes: [])
      i = 0
      result.changes[i] = OpenStruct.new(called: false)
      can_return = false
      callback = Proc.new do |change|
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

      if recursively
        monitor.watch_recursively(directory, *flags, &callback)
      else
        monitor.watch(directory, *flags, &callback)
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
  end
end