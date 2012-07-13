require 'spec_helper'

describe WDM::Monitor do
  it 'runs the block when there are changes' do
    result = run_with_fixture(subject) do
      touch 'file.txt'
    end

    result.called.should be_true
  end

  it 'does not run the block when there are no changes' do
    result = run_with_fixture(subject) do
      # nothing!
    end

    result.called.should be_false
  end

  it 'Passes an instance of WDM::Change to the callback' do
    result = run_with_fixture(subject) do
      touch 'file.txt'
    end

    result.change.should be_instance_of(WDM::Change)
  end

  it 'detects added files' do
    result = run_with_fixture(subject) do
      touch 'file.txt'
    end

    result.change.file.should == "#{result.directory}/file.txt"
    result.change.type.should == :added
  end

  it 'detects modified files' do
    fixture do |dir|
      touch 'file.txt'

      result = run(subject, dir) do
        touch 'file.txt'
      end

      result.change.file.should == "#{result.directory}/file.txt"
      result.change.type.should == :modified
    end
  end

  it 'detects removedd files' do
    fixture do |dir|
      touch 'file.txt'

      result = run(subject, dir) do
        rm 'file.txt'
      end

      result.change.file.should == "#{result.directory}/file.txt"
      result.change.type.should == :removed
    end
  end

  it 'detects renamed files' do
    fixture do |dir|
      touch 'file.txt'

      result = run_and_collect_multiple_changes(subject, 2, dir) do
        mv 'file.txt', 'another.txt'
      end

      result.changes[0].change.file.should == "#{result.directory}/file.txt"
      result.changes[0].change.type.should == :renamed_old_file

      result.changes[1].change.file.should == "#{result.directory}/another.txt"
      result.changes[1].change.type.should == :renamed_new_file
    end
  end

  it 'reports changes with ruby path separators (/) even when passed the ones for windows (\)' do
    fixture do |dir|

      expected_dir = dir.dup # Ruby will use '/' by default, so we just copy it
      dir.gsub!('/', '\\') # Convert '/' to '\'

      result = run(subject, dir) do
        touch 'file.txt'
      end

      result.change.file.should == "#{expected_dir}/file.txt"
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

      result.change.file.should == "#{result.directory}/#{long_file_name}"
    end
  end

  it 'uses UTF-8 for the changed files' do
    result = run_with_fixture(subject) do
      touch 'file.txt'
    end

    result.change.file.encoding.name.should == "UTF-8"
  end
end