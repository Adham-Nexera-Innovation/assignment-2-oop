#pragma once						// PlayerGUI.h
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void showmetadata(const juce::File& file);
    void timerCallback() override;



private:
    PlayerAudio playerAudio;
  

    // GUI elements
    juce::TextButton loadButton{ "Load" };
    juce::TextButton restartButton{ "|<" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton startButton{ "Start" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton endButton{ ">|" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };
    
    juce::Label metadata;

    juce::Array<juce::File> playlist;
    juce::ComboBox interface;

    juce::Slider speedslider;

    juce::Slider volumeSlider;

    juce::Slider positionSlider;
    juce::Label positionLabel;

   
    juce::ProgressBar progressBar{ progress };
    juce::Label timeLabel;

    juce::TextButton setAButton{ "Set A" }, setBButton{ "Set B" }, loopABButton{ "Loop A-B" };

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool mute = false;
    float volume = 0.5f;
    bool loop = false;
    double progress = 0.0;
    bool loopAB = false;
    double pointA = 0.0, pointB = 0.0;

    // Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};