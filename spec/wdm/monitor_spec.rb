require 'spec_helper'

shared_examples_for :watch_method do
  it 'throws an exception when no block is given' do
    expect {
      watch_method.call('does not matter')
    }.to raise_error(LocalJumpError, 'no block given')
  end

  it 'throws an exception when the passed directory does not exist' do
    expect {
      watch_method.call('nonexistent_directory') {}
    }.to raise_error(WDM::InvalidDirectoryError)
  end

  it 'throws an exception when the passed directory is a file' do
    expect {
      watch_method.call(__FILE__) {}
    }.to raise_error(WDM::InvalidDirectoryError)
  end

  it 'throws an exception when a non-symbol flag is passed' do
    expect {
      watch_method.call('does not matter', "not a symbol") {}
    }.to raise_error(TypeError)
  end

  it 'throws an exception when an unknown flag is passed' do
    expect {
      watch_method.call('does not matter', :not_a_flag) {}
    }.to raise_error(WDM::UnknownFlagError)
  end

  it 'throws an exception when it is called while the monitor is running' do
    expect {
      run_with_fixture(subject) do
        watch_method.call('does not matter') {}
      end
    }.to raise_error(WDM::MonitorRunningError)
  end
end

describe WDM::Monitor do
  describe '#watch' do
    let(:watch_method) { Proc.new { |*args, &block| subject.watch(*args, &block) } }

    it_should_behave_like :watch_method
  end

  describe '#watch_recursively' do
    let(:watch_method) { Proc.new { |*args, &block| subject.watch_recursively(*args, &block) } }

    it_should_behave_like :watch_method
  end

  describe 'run!' do
    it 'runs the block when there are changes' do
      result = run_with_fixture(subject) do
        touch 'file.txt'
      end

      expect(result.called).to be_truthy
    end

    it 'does not run the block when there are no changes' do
      result = run_with_fixture(subject) do
        # nothing!
      end

      expect(result.called).to be_falsey
    end

    it 'Passes an instance of WDM::Change to the callback' do
      result = run_with_fixture(subject) do
        touch 'file.txt'
      end

      expect(result.change).to be_instance_of(WDM::Change)
    end

    context 'with no flags passed to watch' do
      it 'detects added files' do
        result = run_with_fixture(subject) do
          touch 'file.txt'
        end

        expect(result.change.path).to be == "#{result.directory}/file.txt"
        expect(result.change.type).to be == :added
      end

      it 'detects modified files' do
        fixture do |dir|
          touch 'file.txt'

          result = run(subject, dir) do
            touch 'file.txt'
          end

          expect(result.change.path).to be == "#{result.directory}/file.txt"
          expect(result.change.type).to be == :modified
        end
      end

      it 'detects removedd files' do
        fixture do |dir|
          touch 'file.txt'

          result = run(subject, dir) do
            rm 'file.txt'
          end

          expect(result.change.path).to be == "#{result.directory}/file.txt"
          expect(result.change.type).to be == :removed
        end
      end

      it 'detects renamed files' do
        fixture do |dir|
          touch 'file.txt'

          result = run_and_collect_multiple_changes(subject, 2, dir) do
            mv 'file.txt', 'another.txt'
          end

          expect(result.changes[0].change.path).to be == "#{result.directory}/file.txt"
          expect(result.changes[0].change.type).to be == :renamed_old_file

          expect(result.changes[1].change.path).to be == "#{result.directory}/another.txt"
          expect(result.changes[1].change.type).to be == :renamed_new_file
        end
      end
    end

    context 'with the :directories flag passed to watch' do
      it 'detects added directories' do
        result = run_with_fixture(subject, :directories) do
          mkdir 'new_dir'
        end

        expect(result.change.path).to be == "#{result.directory}/new_dir"
        expect(result.change.type).to be == :added
      end

      it 'detects removedd directories' do
        fixture do |dir|
          mkdir 'new_dir'

          result = run(subject, dir, :directories) do
            rmdir 'new_dir'
          end

          expect(result.change.path).to be == "#{result.directory}/new_dir"
          expect(result.change.type).to be == :removed
        end
      end
    end

    context 'when directories are registered with (watch_recursively)' do
      it 'detects changes in subdirectories' do
        fixture do |dir|
          mkdir 'some_dir'

          result = run_recursively(subject, dir) do
            touch 'some_dir/file.txt'
          end

          expect(result.change.path).to be == "#{result.directory}/some_dir/file.txt"
          expect(result.change.type).to be == :added
        end
      end
    end

    it 'reports changes with absolute paths even when passed relative directory to watch' do
      fixture do |dir|
        relative_dir = Pathname.new(dir).relative_path_from(Pathname.new(Dir.pwd)).to_s

        result = run(subject, relative_dir) do
          touch 'file.txt'
        end

        expect(result.change.path).to be == "#{dir}/file.txt"
      end
    end

    it 'reports changes with ruby path separators (/) even when passed the ones for windows (\)' do
      fixture do |dir|

        expected_dir = dir.dup # Ruby will use '/' by default, so we just copy it
        dir.gsub!('/', '\\') # Convert '/' to '\'

        result = run(subject, dir) do
          touch 'file.txt'
        end

        expect(result.change.path).to be == "#{expected_dir}/file.txt"
      end
    end

    it 'converts short path names to long in the changed files' do
      long_file_name  = 'longer_than_12_chars.txt'
      short_file_name = 'LONGER~1.TXT' # For more info, look up "8.3 filenames"

      fixture do |dir|
        touch long_file_name

        result = run(subject, dir) do
          touch short_file_name
        end

        expect(result.change.path).to be == "#{result.directory}/#{long_file_name}"
      end
    end

    it 'uses UTF-8 for the changed paths' do
      result = run_with_fixture(subject) do
        touch 'file.txt'
      end

      expect(result.change.path.encoding.name).to be == "UTF-8"
    end
  end
end
