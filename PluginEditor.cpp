#include "PluginEditor.h"

AiVocalEQEditor::AiVocalEQEditor (AiVocalEQProcessor& p)
    : AudioProcessorEditor (&p), proc (p)
{
    setSize (520, 380);

    style.addItemList ({ "Pop", "Rap", "Rock", "Speech" }, 1);
    addAndMakeVisible (style);
    styleAtt = std::make_unique<CA> (proc.apvts, "style", style);

    learn.setClickingTogglesState (true);
    learn.setColour (juce::TextButton::buttonOnColourId, juce::Colour (0xffd9363e));
    addAndMakeVisible (learn);
    learnAtt = std::make_unique<BA> (proc.apvts, "learn", learn);

    auto setupKnob = [this] (juce::Slider& s, juce::Label& l, const juce::String& name)
    {
        s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 18);
        addAndMakeVisible (s);
        l.setText (name, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.attachToComponent (&s, false);
        addAndMakeVisible (l);
    };
    setupKnob (amount, amountL, "AI AMOUNT");
    setupKnob (air, airL, "AIR");
    setupKnob (output, outputL, "OUTPUT");
    amountAtt = std::make_unique<SA> (proc.apvts, "amount", amount);
    airAtt    = std::make_unique<SA> (proc.apvts, "air", air);
    outputAtt = std::make_unique<SA> (proc.apvts, "output", output);

    status.setJustificationType (juce::Justification::centredLeft);
    status.setFont (juce::FontOptions (13.f));
    addAndMakeVisible (status);

    startTimerHz (20);
}

void AiVocalEQEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff15171c));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (24.f, juce::Font::bold));
    g.drawText ("EQ AI MIX VOCALS FULL", 20, 10, getWidth() - 40, 30, juce::Justification::centredLeft);

    g.setColour (juce::Colour (0xffe0b050));
    g.setFont (juce::FontOptions (15.f));
    g.drawText (juce::String::fromUTF8 ("อาร์ม หนองม่วง มือเปิ่มสตูดิโอ"),
                20, 40, getWidth() - 40, 22, juce::Justification::centredLeft);

    // แสดงเกนที่ AI ปรับในแต่ละย่าน
    g.setColour (juce::Colour (0xff22252c));
    g.fillRoundedRectangle (barsArea.toFloat(), 6.f);

    const int n = AiVocalEQProcessor::numBands;
    const float colW = (float) barsArea.getWidth() / n;
    const float midY = (float) barsArea.getCentreY() - 6.f;
    const float scale = ((float) barsArea.getHeight() - 30.f) * 0.5f / 8.f; // ±8 dB เต็มกรอบ

    g.setColour (juce::Colours::grey.withAlpha (0.4f));
    g.drawHorizontalLine ((int) midY, (float) barsArea.getX() + 6.f, (float) barsArea.getRight() - 6.f);

    for (int b = 0; b < n; ++b)
    {
        const float gain = proc.bandGainDb[(size_t) b].load();
        const float h = gain * scale;
        const float x = (float) barsArea.getX() + colW * b + colW * 0.25f;
        g.setColour (gain >= 0.f ? juce::Colour (0xff4cc38a) : juce::Colour (0xffe0714c));
        g.fillRect (x, h >= 0.f ? midY - h : midY, colW * 0.5f, std::abs (h));

        g.setColour (juce::Colours::lightgrey);
        g.setFont (juce::FontOptions (12.f));
        g.drawText (juce::String (AiVocalEQProcessor::bandNames[b]) + (b == AiVocalEQProcessor::numBands - 1 ? "" : " Hz"),
                    (int) (barsArea.getX() + colW * b), barsArea.getBottom() - 22, (int) colW, 14,
                    juce::Justification::centred);
        g.drawText (juce::String (gain, 1) + " dB",
                    (int) (barsArea.getX() + colW * b), barsArea.getY() + 4, (int) colW, 14,
                    juce::Justification::centred);
    }
}

void AiVocalEQEditor::resized()
{
    style.setBounds (20, 74, 180, 28);
    learn.setBounds (212, 74, 100, 28);
    status.setBounds (324, 74, getWidth() - 344, 28);

    barsArea = { 20, 114, getWidth() - 40, 120 };

    const int knobW = 110, gap = (getWidth() - 3 * knobW) / 4;
    int x = gap;
    for (auto* s : { &amount, &air, &output })
    {
        s->setBounds (x, 270, knobW, 100);
        x += knobW + gap;
    }
}

void AiVocalEQEditor::timerCallback()
{
    const bool learning = learn.getToggleState();
    const int frames = proc.learnFrames.load();

    if (learning)
        status.setText (juce::String::fromUTF8 ("กำลังเรียนรู้... ร้อง/พูดต่อเนื่อง 10-20 วินาที"), juce::dontSendNotification);
    else if (frames >= 8)
        status.setText (juce::String::fromUTF8 ("พร้อมใช้งาน (เรียนรู้แล้ว)"), juce::dontSendNotification);
    else
        status.setText (juce::String::fromUTF8 ("กด LEARN แล้วเล่นเสียงร้อง"), juce::dontSendNotification);

    repaint (barsArea);
}
