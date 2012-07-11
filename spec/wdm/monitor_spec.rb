require 'spec_helper'

describe WDM::Monitor do
  let(:callback) { Proc.new { @called = true } }

  before { @called = false }

  it 'runs the block when there are changes' do
    watch_fixture_with(subject, callback) do
      touch 'file.txt'
    end

    @called.should be_true
  end

  it 'does not run the block when there are no changes' do
    watch_fixture_with(subject, callback) do
      # nothing!
    end

    @called.should be_false
  end
end