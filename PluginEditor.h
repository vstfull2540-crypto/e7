#pragma once

#include "PluginProcessor.h"

class AiVocalEQEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit AiVocalEQEditor (AiVocalEQProcessor&);
    ~AiVocalEQEditor() override { stopTimer(); }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    AiVocalEQProcessor& proc;

    juce::ComboBox style;
    juce::TextButton learn { "LEARN" };
    juce::Slider amount, air, output;
    juce::Label amountL, airL, outputL, status;

    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<CA> styleAtt;
    std::unique_ptr<SA> amountAtt, airAtt, outputAtt;
    std::unique_ptr<BA> learnAtt;

    juce::Rectangle<int> barsArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AiVocalEQEditor)
};
