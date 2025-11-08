#include "PlayerGUI.h"


void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);

    if (loop && playerAudio.getPosition() >= playerAudio.getLength())
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(90, 90, 90)); 
}



void PlayerGUI::showmetadata(const juce::File& file)
{
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    if (auto* reader = formatManager.createReaderFor(file))
    {
        juce::String title = reader->metadataValues["title"];
        juce::String artist = reader->metadataValues["artist"];
        double duration = (reader->lengthInSamples / reader->sampleRate);

        juce::String data;

        if (title.isNotEmpty() || artist.isNotEmpty())
            data = "Title: " + (title.isNotEmpty() ? title : "Unknown") +
            "  ||  Artist: " + (artist.isNotEmpty() ? artist : "Unknown") +
            "  ||  Duration: " + juce::String(duration, 2) + " sec";
        else
            data = "File Name: " + file.getFileNameWithoutExtension() +
            "  ||  Duration: " + juce::String(duration, 2) + " sec";

        metadata.setText(data, juce::dontSendNotification);
        delete reader;
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)slider->getValue());
    }
    else if (slider == &speedslider)
    {
        playerAudio.setspeed((float)slider->getValue());
    }
    else if (slider == &positionSlider)
    {
        double pos = slider->getValue() * playerAudio.getLength();
        playerAudio.setPosition(pos);
    }
}


void PlayerGUI::timerCallback()
{
    double pos = playerAudio.getPosition();
    double length = playerAudio.getLength();
    if (loopAB && pointB > pointA && pos >= pointB)
    {
        playerAudio.setPosition(pointA);
        playerAudio.start();
    }

    if (length > 0)
    {
        progress = pos / length;
        positionSlider.setValue(progress, juce::dontSendNotification);
    }
    else
        progress = 0.0;

    auto formatTime = [](double seconds)
        {
            int mins = (int)(seconds / 60);
            int secs = (int)(fmod(seconds, 60));
            return juce::String::formatted("%02d:%02d", mins, secs);
        };

    timeLabel.setText(formatTime(pos) + " || " + formatTime(length),
        juce::dontSendNotification);
}



PlayerGUI::PlayerGUI()
{
    // Add buttons
    for (auto* btn : { &loadButton, &restartButton , &stopButton , &startButton , &pauseButton , &endButton , &muteButton , &loopButton, &setAButton, &setBButton, &loopABButton })
    {
        
        btn->addListener(this);
        addAndMakeVisible(btn);

        btn->setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(200, 200, 200));
        btn->setColour(juce::TextButton::buttonOnColourId, juce::Colour::fromRGB(120, 120, 120));
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        btn->setColour(juce::TextButton::textColourOnId, juce::Colours::black);

        if (btn == &muteButton || btn == &loopButton || btn == &loopABButton)
            btn->setClickingTogglesState(true);
        else
            btn->setClickingTogglesState(false);

        btn->setAlpha(1.0f);       
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    metadata.setText("No File Loaded", juce::dontSendNotification);
    metadata.setJustificationType(juce::Justification::centred);
    metadata.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(metadata);

    // Speed slider
    speedslider.setRange(0.5, 2.0, 0.01);  
    speedslider.setValue(1.0);             
    speedslider.addListener(this);         
    addAndMakeVisible(speedslider); 

    positionSlider.setRange(0.0, 1.0);
    positionSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    addAndMakeVisible(progressBar);
    addAndMakeVisible(timeLabel);

    timeLabel.setText("00:00 || 00:00", juce::dontSendNotification);
    timeLabel.setJustificationType(juce::Justification::centred);


    addAndMakeVisible(interface);
    interface.onChange = [this]() {
        int index = interface.getSelectedItemIndex();
        if (index >= 0 && index < playlist.size()) {
            auto file = playlist[index];
            playerAudio.loadFile(file);
            playerAudio.start();
            showmetadata(file);
        }
     };
    startTimerHz(10);
}

void PlayerGUI::resized()
{
    int y = 20;
    loadButton.setBounds(20, y, 80, 40);
    restartButton.setBounds(120, y, 80, 40);
    stopButton.setBounds(220, y, 80, 40);
    startButton.setBounds(320, y, 80, 40);
    pauseButton.setBounds(420, y, 80, 40);
    endButton.setBounds(520, y, 80, 40);
    muteButton.setBounds(620, y, 80, 40);
    loopButton.setBounds(720, y, 100, 40);
    setAButton.setBounds(840, y, 80, 40);
    setBButton.setBounds(940, y, 80, 40);
    loopABButton.setBounds(1040, y, 100, 40);

    metadata.setBounds(20, 250, getWidth() - 40, 40);
    volumeSlider.setBounds(20, 200, getWidth() - 40, 40);
    interface.setBounds(20, 300, getWidth() - 40, 40);
    speedslider.setBounds(20, 220, getWidth() - 40, 40);
    positionSlider.setBounds(10, 100, getWidth() - 20, 20);
    positionLabel.setBounds(10, 125, getWidth() - 20, 20);
    progressBar.setBounds(20, 150, getWidth() - 40, 20);
    timeLabel.setBounds(20, 175, getWidth() - 40, 20);

}

PlayerGUI::~PlayerGUI()
{
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...",
            juce::File{},
            "*.wav;*.mp3");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults();
                playlist.clear();
                interface.clear();
                for (auto& file : files) {
                    if (file.existsAsFile())
                    {
                        playlist.add(file);
                        interface.addItem(file.getFileNameWithoutExtension(), playlist.size());

                        showmetadata(file);
                    }
                }
                if (playlist.size() > 0)
                {
                    playerAudio.loadFile(playlist[0]);
                    playerAudio.start();
                }
                
            });
    }

    else if (button == &restartButton)
    {
        playerAudio.setPosition(0.0);
        playerAudio.start();
    }

    else if (button == &stopButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }


    else if (button == &pauseButton)
    {
        playerAudio.stop();
    }
   
    else if (button == &startButton)
    {
        playerAudio.setPosition(playerAudio.getPosition());
        playerAudio.start();
    }

    else if (button == &endButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(playerAudio.getLength());
    }

    else if (button == &muteButton)
    {
        mute = !mute;
        if (mute) 
        {
            volume = float(volumeSlider.getValue());
            playerAudio.setGain(0.0f);
            muteButton.setButtonText("UnMute");
        }
        else 
        {
            volumeSlider.setValue(volume);
            playerAudio.setGain(volume);
            muteButton.setButtonText("Mute");
        }
    }

    else if (button == &loopButton) {
        loop = !loop;
        loopButton.setButtonText(loop ? "Loop: ON" : "Loop: OFF");
    }

    else if (button == &setAButton)
    {
        pointA = playerAudio.getPosition();
        setAButton.setButtonText("A: " + juce::String(pointA, 2) + "s");
    }

    else if (button == &setBButton)
    {
        pointB = playerAudio.getPosition();
        setBButton.setButtonText("B: " + juce::String(pointB, 2) + "s");
    }

    else if (button == &loopABButton)
    {
        loopAB = !loopAB;
        loopABButton.setButtonText(loopAB ? "A-B: ON" : "A-B: OFF");
    }

}


