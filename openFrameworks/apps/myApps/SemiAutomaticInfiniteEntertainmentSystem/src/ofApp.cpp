#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    //    "there is no way to set vertical sync with OpenGL (it’s a bug with OpenGL on mojave)"
    //    https://forum.openframeworks.cc/t/of-and-macbook-pro-2018/31362/3
    // Set frame rate to 60 fps
    ofSetFrameRate(60);

    // Set ofLog Level
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofToggleFullscreen();
    
    // Setup background colour (black)
    ofBackground(0,0,0);
    
    // Setup Video Library
    dir.listDir(VIDEOS_PATH);
    dir.allowExt("mp4");
    dir.allowExt("mov");
    
    defaultVideo.load("default.mp4");
    defaultVideo.setLoopState(OF_LOOP_NORMAL);
    
    //allocate the vector to have as many ofVideoPlayers as files
    if( dir.size() ){
        videoLibrary.assign(dir.size(), video());
    }
    
    // 4 x 3 videos
    row = 4;
    column = 3;
    // Defining the library width and height
    vidLibraryWidth = ofGetWidth() - (2 * APP_BORDER);
    vidLibraryHeight = ofGetHeight() - (2 * APP_BORDER);
    // Defining the space between the videos
    vidSpace = 20;
    // Defining the videos width and height
    vidWidth = (vidLibraryWidth - (row + 1) * vidSpace) / row;
    vidHeight = (vidLibraryHeight - (column + 1) * vidSpace) / column;
    for(int i = 0; i < (int)dir.size(); i++){
        videoLibrary[i].player.load(dir.getPath(i));
        videoLibrary[i].player.setLoopState(OF_LOOP_NORMAL);
        videoLibrary[i].player.setVolume(0);
        
        // Defining video positions and sizes
        videoLibrary[i].x = APP_BORDER + vidSpace + (vidSpace + vidWidth) * (i % row);
        videoLibrary[i].y = APP_BORDER + vidSpace + (vidSpace + vidHeight) * (i / (column + 1));
        videoLibrary[i].width = vidWidth;
        videoLibrary[i].height = vidHeight;
        
        // Saving video's name in metadata
        videoLibrary[i].name = getVideoName(videoLibrary[i]);
        videoLibrary[i].metadata = readMetadata(videoLibrary[i].name);
        
        videoIndexMap[videoLibrary[i].name] = i;
    }
    
    for(int i = 0; i < videoLibrary.size(); i++){
        ofLogVerbose("[DEBUG] (key, value)  > ") << "("<<videoLibrary[i].name<<", "<<videoIndexMap[videoLibrary[i].name]<<")";
    }
    
    /*-------------------------------------------------------------------------------*/
    /*---------------------------- FACE DETECTION -----------------------------------*/
    /*-------------------------------------------------------------------------------*/
    
    faceFinder.setup(HAAR_FINDER_PATH+haarFaceXMLFile);
    faceFinder.setScaleHaar(1.5);
    
    mouthFinder.setup(HAAR_FINDER_PATH+haarMouthXMLFile);
    mouthFinder.setScaleHaar(1.5);
    
    eyesFinder.setup(HAAR_FINDER_PATH+haarEyesXMLFile);
    eyesFinder.setScaleHaar(1.5);
    
    /*-------------------------------------------------------------------------------*/
    /*------------------------------------ GUI --------------------------------------*/
    /*-------------------------------------------------------------------------------*/
    
    verdana30.load("verdana.ttf", 30, true, true);
    verdana30.setLineHeight(34.0f);
    verdana30.setLetterSpacing(1.035);
    
    verdana15.load("verdana.ttf", 15, true, true);
    verdana30.setLineHeight(34.0f);
    verdana30.setLetterSpacing(1.035);
    
    // Instantiate and position the gui
    gui = new ofxDatGui(300,300);
    
    // Aux vars to help setup gui
    ofxDatGuiFolder* folder;
    ofxDatGuiSlider* slider;
    ofxDatGuiLabel* label;
    ofxDatGuiTextInput* input;
    ofxDatGuiDropdown* dropdown;
    ofxDatGuiButton* button;
    ofxDatGuiDropdown* myDropdown;
    ofxDatGuiMatrix* matrix;
    
    userButton = new ofxDatGuiButton("User");
    userButton->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    userButton->setStripeVisible(false);
    userButton->setBackgroundColor(ofColor(238, 57, 135));
    userButton->setLabelColor(ofColor::white);
    userButton->setPosition(ofGetWidth()/2 - userButton->getWidth(), ofGetHeight()/2);
    userButton->onButtonEvent(this, &ofApp::onButtonEvent);
    
    operatorButton = new ofxDatGuiButton("Operator");
    operatorButton->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    operatorButton->setStripeVisible(false);
    operatorButton->setBackgroundColor(ofColor(238, 57, 135));
    operatorButton->setLabelColor(ofColor::white);
    operatorButton->setPosition(ofGetWidth()/2, ofGetHeight()/2);
    operatorButton->onButtonEvent(this, &ofApp::onButtonEvent);
    
    chooseUserLabel = new ofxDatGuiLabel("Choose User");
    chooseUserLabel->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    chooseUserLabel->setStripeVisible(false);
    chooseUserLabel->setWidth(userButton->getWidth()+operatorButton->getWidth());
    chooseUserLabel->setPosition(userButton->getX(), ofGetHeight()/2 - chooseUserLabel->getHeight());
    
    gui->setAutoDraw(false);

    // Tags
    folder = gui->addFolder("Tags");
    folder->setStripeColor(ofColor(238, 57, 135));
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    
    input = folder->addTextInput("Add Tag", "<tag>");
    input->setStripeVisible(false);
    
    // Colour
    folder = gui->addFolder("Colour");
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    slider = folder->addSlider("Red", 0, 255);
    slider->setStripeColor(ofColor::red);
    
    slider = folder->addSlider("Green", 0, 255);
    slider->setStripeColor(ofColor::green);
    
    slider = folder->addSlider("Blue", 0, 255);
    slider->setStripeColor(ofColor::blue);
    
    slider = folder->addSlider("Luminance", 0, 255);
    slider->setStripeColor(ofColor::white);
    
    // Faces
    folder = gui->addFolder("Faces");
    folder->setStripeColor(ofColor(238, 57, 135));
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    
    input = folder->addTextInput("Faces", "-1");
    input->setStripeVisible(false);
    
    // Edges
    label = gui->addLabel("Edge Distribution");
    label->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    label->setStripeColor(ofColor(238, 57, 135));
    
    folder = gui->addFolder("Horizontal Edges", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));

    input = folder->addTextInput("Mean", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Std. Dev.", "-1");
    input->setStripeVisible(false);
    
    folder = gui->addFolder("Vertical Edges", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    input = folder->addTextInput("Mean", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Std. Dev.", "-1");
    input->setStripeVisible(false);
    
    folder = gui->addFolder("45º Degree Edges", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    input = folder->addTextInput("Mean", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Std. Dev.", "-1");
    input->setStripeVisible(false);
    
    folder = gui->addFolder("135º Degree Edges", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    input = folder->addTextInput("Mean", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Std. Dev.", "-1");
    input->setStripeVisible(false);
    
    folder = gui->addFolder("Non Directional Edges", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    input = folder->addTextInput("Mean", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Std. Dev.", "-1");
    input->setStripeVisible(false);
    
    // Texture
    folder = gui->addFolder("Texture", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    int numButtons = 24;
    matrix = folder->addMatrix("Texture", numButtons, true);
    matrix->setStripeVisible(false);
    matrix->setRadioMode(true);
    
    input = folder->addTextInput("Mean", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Std. Dev.", "-1");
    input->setStripeVisible(false);
    
    // Rhythms
    folder = gui->addFolder("Rhythm", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    numButtons = 10;
    matrix = folder->addMatrix("Rhythm", numButtons, true);
    matrix->setStripeVisible(false);
    matrix->setRadioMode(true);
    
    input = folder->addTextInput("Frame", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("Rhythm", "-1");
    input->setStripeVisible(false);
    
    // Object
    folder = gui->addFolder("Object", ofColor::white);
    folder->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    folder->setStripeColor(ofColor(238, 57, 135));
    
    input = folder->addTextInput("Object", "-1");
    input->setStripeVisible(false);
    input = folder->addTextInput("File Path", "<image_file>");
    input->setStripeVisible(false);
    
    button = gui->addButton("Retrieve Metadata");
    button->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    button->setStripeColor(ofColor(238, 57, 135));
    
    // once the gui has been assembled, register callbacks to listen for component specific events //
    gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    gui->onMatrixEvent(this, &ofApp::onMatrixEvent);
    gui->onButtonEvent(this, &ofApp::onButtonEvent);
    
    playlistButton = new ofxDatGuiButton("Playlist Library");
    playlistButton->setStripeVisible(false);
    playlistButton->setBackgroundColor(ofColor(238, 57, 135));
    playlistButton->setLabelColor(ofColor::white);
    playlistButton->setPosition(ofGetWidth()-APP_BORDER-playlistButton->getWidth(), APP_BORDER-playlistButton->getHeight());
    playlistButton->onButtonEvent(this, &ofApp::onButtonEvent);
    // Start disabled since it's not on start window
    playlistButton->setEnabled(false);
    
    videoLibraryButton = new ofxDatGuiButton("Video Library");
    videoLibraryButton->setStripeVisible(false);
    videoLibraryButton->setBackgroundColor(ofColor(238, 57, 135));
    videoLibraryButton->setLabelColor(ofColor::white);
    videoLibraryButton->setPosition(ofGetWidth()-APP_BORDER-videoLibraryButton->getWidth(), APP_BORDER-videoLibraryButton->getHeight());
    videoLibraryButton->onButtonEvent(this, &ofApp::onButtonEvent);
    // Start disabled since it's not on start window
    videoLibraryButton->setEnabled(false);
    
    createPlaylistButton = new ofxDatGuiButton("Create Playlist");
    createPlaylistButton->setStripeVisible(false);
    createPlaylistButton->setBackgroundColor(ofColor(238, 57, 135));
    createPlaylistButton->setLabelColor(ofColor::white);
    createPlaylistButton->setPosition(videoLibraryButton->getX(), videoLibraryButton->getY()+videoLibraryButton->getHeight());
    createPlaylistButton->onButtonEvent(this, &ofApp::onButtonEvent);
    // Start disabled since it's not on start window
    createPlaylistButton->setEnabled(false);
    
    createPlaylistGui = new ofxDatGui(200,200);
    createPlaylistGui->setAutoDraw(false);
    label = createPlaylistGui->addLabel("Create Playlist");
    label->setLabelAlignment(ofxDatGuiAlignment::CENTER);
    label->setStripeVisible(false);
    
    createPlaylistGui->addBreak();
    
    input = createPlaylistGui->addTextInput("Playlist Name", "name");
    input->setStripeVisible(false);
    createPlaylistGui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    // Start disabled since it's not on vid lib window
    createPlaylistEnabled = false;
    createPlaylistGui->setEnabled(createPlaylistEnabled);
    
    vector<string> vidOptions;
    for(int i = 0; i < videoLibrary.size(); i++){
        vidOptions.push_back(videoLibrary[i].name);
    }
    dropdown = createPlaylistGui->addDropdown("Add Video", vidOptions);
    dropdown->setStripeColor(ofColor(238, 57, 135));
    
    createPlaylistGui->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    input = createPlaylistGui->addTextInput("Faces", "0");
    input->setStripeVisible(false);
    createPlaylistGui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    
    createPlaylistGui->onTextInputEvent(this, &ofApp::onTextInputEvent);
    
    button = createPlaylistGui->addButton("Confirm");
    button->setStripeVisible(false);
    createPlaylistGui->onButtonEvent(this, &ofApp::onButtonEvent);
    
    /*-------------------------------------------------------------------------------*/
    /*---------------------------------- CAMERA -------------------------------------*/
    /*-------------------------------------------------------------------------------*/
    
    //  ofAvFoundationGrabber:  supported dimensions are: 1280x720
    camWidth = 1280;
    camHeight = 720;
    
    vidGrabber.setVerbose(true);
    vidGrabber.setup(camWidth,camHeight);
    
    /*-------------------------------------------------------------------------------*/
    /*------------------------------- PLAYLISTS -------------------------------------*/
    /*-------------------------------------------------------------------------------*/
    
    // Setup Video Library
    playlistsDir.listDir(PLAYLISTS_PATH);
    playlistsDir.allowExt("xml");
    
    //allocate the vector to have as many playlists as files
    if( playlistsDir.size() ){
        playlists.assign(playlistsDir.size(), playlist());
    }
    
    for(int i = 0; i < (int)playlistsDir.size(); i++){
        string playlistName = playlistsDir.getName(i);
        int finish = playlistName.find_last_of(".");
        playlistName = playlistName.substr(0, finish);
        
        playlists[i] = readPlaylist(playlistName);
    }
    
    for(int i = 0; i < playlists.size(); i++){
        ofLogVerbose("Playlist > ") << i <<","<< playlists[i].name;
        for(int j = 0; j < playlists[i].videoNames.size(); j++){
          ofLogVerbose("Video > ") << j <<","<< playlists[i].videoNames[j];
        }
    }
    
    // Construct component to update with playlists read
    vector<string> playlistOptions;
    for(int i = 0; i < playlists.size(); i++){
        playlistOptions.push_back(playlists[i].name);
    }
    playlistOptions.push_back("None");
    
    addToPlaylistDropdown = new ofxDatGuiDropdown("Add To Playlist", playlistOptions);
    addToPlaylistDropdown->setStripeColor(ofColor(238, 57, 135));
    
    addToPlaylistDropdown->onDropdownEvent(this, &ofApp::onDropdownEvent);
    
    currentPlaylistIndex = 0;
    currentPlayingVideoIndex = 0;
    currentPlayingVideoLibraryIndex = 0;
    
    int bufferSize = 256;
    
    vector<ofSoundDevice> list = soundStream.getDeviceList();
    ofSoundDevice mic;
    for(int i = 0; i < list.size(); i++){
        if(list[i].name == "Apple Inc.: MacBook Air Microphone"){
            mic = list[i];
        }
    }
    settings.setApi(ofSoundDevice::Api::OSX_CORE);
    settings.setInDevice(mic);
    settings.setInListener(this);
    settings.sampleRate = 44100;
    settings.numOutputChannels = 0;
    settings.numInputChannels = 1;
    settings.bufferSize = bufferSize;
    soundStream.setup(settings);
    
    soundReceived = 0;
    lockInterval = 0;
    audioDetected = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    chooseUserLabel->update();
    userButton->update();
    operatorButton->update();
    gui->update();
    playlistButton->update();
    videoLibraryButton->update();
    createPlaylistButton->update();
    createPlaylistGui->update();
    addToPlaylistDropdown->update();
    defaultVideo.update();
    for(int i = 0; i < (int)dir.size(); i++){
        videoLibrary[i].player.update();
    }
    vidGrabber.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    switch(currentWindow){
        case START_WINDOW:
            drawStartWindow();
            break;
        case VIDEO_LIBRARY_WINDOW:
            drawVideoLibrary();
            break;
        case VIDEO_DETAIL_WINDOW:
            drawVideoDetail();
            break;
        case PLAYLIST_LIBRARY_WINDOW:
            drawPlaylistLibrary();
            break;
        case USER_WINDOW:
            drawUserVideo();
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::audioIn(ofSoundBuffer & input){
    if(input.getRMSAmplitude() >= 0.1){
        soundReceived = input.getRMSAmplitude();
        ofLog() << "Audio detected";
        ofLogVerbose("RMS > ") << input.getRMSAmplitude();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    ofLogVerbose("[DEBUG] Key Pressed > ") << ofToString(key)+" pressed.";
    if(ofGetKeyPressed(OF_KEY_COMMAND)){
        switch (key) {
            case 'f':
                ofToggleFullscreen();
                break;
            case 's':
            {
                if(currentWindow == VIDEO_DETAIL_WINDOW){
                    metadata videoMetadata = videoAnalysis(&videoLibrary[currentVideoDetailIndex]);
                    writeMetadata(videoLibrary[currentVideoDetailIndex].name, videoMetadata);
                    readMetadata(videoLibrary[currentVideoDetailIndex].name);
                }
                break;
            }
            case 'l':
                changeAppWindow(VIDEO_LIBRARY_WINDOW);
                break;
            case 'u':
                changeAppWindow(USER_WINDOW);
                break;
            default:
                ofLogVerbose("[DEBUG] Key Pressed > ") << key;
                break;
        }
    }
    if(ofGetKeyPressed(OF_KEY_SHIFT)){
        if(key == 'H') changeAppWindow(START_WINDOW);
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    bool insideVideo = false;
    bool insideCreatePlaylist = false;
    ofRectangle box;
    switch (currentWindow) {
        case VIDEO_LIBRARY_WINDOW:
            // Increase video thumbnail size and play a snippet in Library Window
            
            // Video's sizes must be defined here so that when the mouse cursor
            // hovers over the videos their size can increase correctly.
            // If the size was defined in drawLibrary() then it would constantly
            // override the increased size.
            for(int i = 0; i < (int)dir.size(); i++){
                insideVideo = isInsideArea(x,y,videoLibrary[i].x,videoLibrary[i].y,videoLibrary[i].width,videoLibrary[i].height);
                if(insideVideo){
                    // Increase video size by 5
                    videoLibrary[i].x = (APP_BORDER + vidSpace + (vidSpace + vidWidth) * (i % row)) - 5;
                    videoLibrary[i].y = (APP_BORDER + vidSpace + (vidSpace + vidHeight) * (i / (column + 1))) - 5;
                    videoLibrary[i].width = vidWidth + 10;
                    videoLibrary[i].height = vidHeight + 10;
                    
                    if(videoLibrary[i].player.isPaused()){
                        videoLibrary[i].player.setSpeed(0.5);
                        videoLibrary[i].player.setPosition(0.4);
                        videoLibrary[i].player.play();
                    }
                }else{
                    videoLibrary[i].x = APP_BORDER + vidSpace + (vidSpace + vidWidth) * (i % row);
                    videoLibrary[i].y = APP_BORDER + vidSpace + (vidSpace + vidHeight) * (i / (column + 1));
                    videoLibrary[i].width = vidWidth;
                    videoLibrary[i].height = vidHeight;
                    
                    if(!videoLibrary[i].player.isPaused()){
                        videoLibrary[i].player.setSpeed(1.0);
                        videoLibrary[i].player.setPosition(0.5);
                        videoLibrary[i].player.setPaused(true);
                    }
                }
            }
            break;
        case VIDEO_DETAIL_WINDOW:
            break;
        case PLAYLIST_LIBRARY_WINDOW:
        {
            // Only check if the gui is enabled/being drawn
            if(createPlaylistEnabled){
                insideCreatePlaylist = isInsideArea(x,y,
                                                    createPlaylistGui->getPosition().x,
                                                    createPlaylistGui->getPosition().y,
                                                    createPlaylistGui->getWidth(),
                                                    createPlaylistGui->getHeight());
            }
            // Play a snippet in Library Window
            // Playlistindex to translate Video Lib coodinates to Playlist coordinates
            int playlistVideoIndex = 0;
            for(int k = 0; k < playlists.size(); k++) {
                for(int j = 0; j < playlists[k].videoNames.size(); j++){
                    for(int i = 0; i < (int)dir.size(); i++){
                        if(playlists[k].videoNames[j] == videoLibrary[i].name){
                            // Change videos positions using the library positions to match their playlists
                            insideVideo = isInsideArea(x,y,
                                                       videoLibrary[column*k+j].x,
                                                       videoLibrary[column*k+j].y,
                                                       videoLibrary[column*k+j].width,
                                                       videoLibrary[column*k+j].height);
                            if(insideVideo && !insideCreatePlaylist){
                                if(videoLibrary[i].player.isPaused()){
                                    videoLibrary[i].player.setSpeed(0.5);
                                    videoLibrary[i].player.setPosition(0.4);
                                    videoLibrary[i].player.play();
                                }
                            }else{
                                if(!videoLibrary[i].player.isPaused()){
                                    videoLibrary[i].player.setSpeed(1.0);
                                    videoLibrary[i].player.setPosition(0.5);
                                    videoLibrary[i].player.setPaused(true);
                                }
                            }
                            playlistVideoIndex++;
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    bool insideVideo = false;
    bool insideMetadataPanel = false;
    bool insideAddToPlaylist = false;
    bool insideCreatePlaylist = false;
    ofPoint guiPos;
    switch(currentWindow){
        case VIDEO_LIBRARY_WINDOW:
        {
            // Check in which video is user pressing and show its metadata
            for(int i = 0; i < (int)dir.size(); i++){
                insideVideo = isInsideArea(x, y, videoLibrary[i].x, videoLibrary[i].y, videoLibrary[i].width, videoLibrary[i].height);
                if(insideVideo){
                    ofLogVerbose("[DEBUG] Current video index > ") << i;
                    currentVideoDetailIndex = i;
                    changeAppWindow(VIDEO_DETAIL_WINDOW);
                }
            }
            ofLogVerbose("[DEBUG] > ") << " mousePressed()";
            break;
        }
        case VIDEO_DETAIL_WINDOW:
            videoLibrary[currentVideoDetailIndex] = videoLibrary[currentVideoDetailIndex];
            insideVideo = isInsideArea(x, y,
                                       videoLibrary[currentVideoDetailIndex].x, videoLibrary[currentVideoDetailIndex].y,
                                       videoLibrary[currentVideoDetailIndex].width, videoLibrary[currentVideoDetailIndex].height);
            guiPos = gui->getPosition();
            
            insideMetadataPanel = isInsideArea(x, y, guiPos.x, guiPos.y, gui->getWidth(), gui->getHeight());
            insideAddToPlaylist = isInsideArea(x, y, addToPlaylistDropdown->getX(), addToPlaylistDropdown->getY(),
                                               addToPlaylistDropdown->getWidth(), addToPlaylistDropdown->getHeight());
            if(insideVideo){
                videoLibrary[currentVideoDetailIndex].player.setPaused(!videoLibrary[currentVideoDetailIndex].player.isPaused());
            } else if(insideMetadataPanel){
                // Metadata buttons
            } else if(insideAddToPlaylist){
                // insideAddToPlaylist dropdown
            } else {
                changeAppWindow(previousWindow);
            }
            break;
        case PLAYLIST_LIBRARY_WINDOW:
        {
            // Only check if the gui is enabled/being drawn
            if(createPlaylistEnabled){
                insideCreatePlaylist = isInsideArea(x,y,
                                                    createPlaylistGui->getPosition().x,
                                                    createPlaylistGui->getPosition().y,
                                                    createPlaylistGui->getWidth(),
                                                    createPlaylistGui->getHeight());
            }
            // Playlistindex to translate Video Lib coodinates to Playlist coordinates
            int playlistVideoIndex = 0;
            for(int k = 0; k < playlists.size(); k++){
                for(int j = 0; j < playlists[k].videoNames.size(); j++){
                    for(int i = 0; i < (int)dir.size(); i++){
                        if(playlists[k].videoNames[j] == videoLibrary[i].name){
                            // Change videos positions using the library positions to match their playlists
                            insideVideo = isInsideArea(x,y,
                                                       videoLibrary[column*k+j].x,
                                                       videoLibrary[column*k+j].y,
                                                       videoLibrary[column*k+j].width,
                                                       videoLibrary[column*k+j].height);
                            
                            if(insideVideo && !insideCreatePlaylist){
                                currentVideoDetailIndex = i;
                                changeAppWindow(VIDEO_DETAIL_WINDOW);
                                goto exitLoop;
                            }
                            playlistVideoIndex++;
                        }
                    }
                }
            }
            exitLoop:
            break;
        }
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    switch (currentWindow) {
        case VIDEO_LIBRARY_WINDOW:
        {
            // Video's sizes must be defined here so that when the mouse cursor
            // hovers over the videos their size can increase correctly.
            // If the size was defined in drawLibrary() then it would constantly
            // override the increased size.
            // Something is wrong here I actually want 3 rows and 4 columns...
            // 4 x 3 videos
            row = 4;
            column = 3;
            // Defining the library width and height
            vidLibraryWidth = ofGetWidth() - (2 * APP_BORDER);
            vidLibraryHeight = ofGetHeight() - (2 * APP_BORDER);
            // Defining the space between the videos
            vidSpace = 20;
            // Defining the videos width and height
            vidWidth = (vidLibraryWidth - (row + 1) * vidSpace) / row;
            vidHeight = (vidLibraryHeight - (column + 1) * vidSpace) / column;
            for(int i = 0; i < (int)dir.size(); i++){
                // Defining video positions and sizes
                videoLibrary[i].x = APP_BORDER + vidSpace + (vidSpace + vidWidth) * (i % row);
                videoLibrary[i].y = APP_BORDER + vidSpace + (vidSpace + vidHeight) * (i / (column + 1));
                videoLibrary[i].width = vidWidth;
                videoLibrary[i].height = vidHeight;
            }
            break;
        }
        case VIDEO_DETAIL_WINDOW:
        {
            break;
        }
        case PLAYLIST_LIBRARY_WINDOW:
        {
            // Video's sizes must be defined here so that when the mouse cursor
            // hovers over the videos their size can increase correctly.
            // If the size was defined in drawLibrary() then it would constantly
            // override the increased size.
            // 4 x 3 videos
            row = 3;
            column = 4;
            // Defining the library width and height
            vidLibraryWidth = ofGetWidth() - (2 * APP_BORDER);
            vidLibraryHeight = ofGetHeight() - (2 * APP_BORDER);
            // Defining the space between the videos
            vidSpace = 20;
            // Get Playlist Title bounding box
            ofRectangle box = verdana15.getStringBoundingBox("Playlist Placeholder", 0,  0);

            // Defining the videos width and height
            vidWidth = (vidLibraryWidth - (column + 1) * vidSpace) / column;
            vidHeight = (vidLibraryHeight - (row + 1) * vidSpace - row*box.height) / row;
            for(int i = 0; i < (int)dir.size(); i++){
                // Defining video positions and sizes
                videoLibrary[i].x = APP_BORDER + vidSpace + (vidSpace + vidWidth) * (i % column);
                videoLibrary[i].y = APP_BORDER + vidSpace + box.height + (vidSpace + box.height + vidHeight) * (i / (row + 1));
                videoLibrary[i].width = vidWidth;
                videoLibrary[i].height = vidHeight;
            }
            break;
        }
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
ofApp::metadata ofApp::videoAnalysis(video* video){
    ofLogVerbose("[DEBUG] Video Analysis > ") << "Analysing video's metadata.";
    float startTime = ofGetElapsedTimef();
    
    int saveFrame = video->player.getCurrentFrame();
    video->player.setPaused(true);
    video->player.firstFrame();
    
    ofPixels pix = video->player.getPixels();
    int w = pix.getWidth();
    int h = pix.getHeight();
    int nChannels = pix.getNumChannels();

    metadata videoMetadata = *new metadata();
    // Init texture
    videoMetadata.texture.assign(24, meanStdDev());
    
    // Get textures for the first frame only
    int index = 0;
    meanStdDev textureMeanStdDev;
    cv::Size kernelSize(100, 100);
    for (int i = 0; i < 6; i++) {
        for (int j = 1; j < 5; j++) {
            textureMeanStdDev = getTexture(pix, cv::getGaborKernel(kernelSize, 0.5 * j * 5, i * 30, j * 5, 0.5, 0));
            videoMetadata.texture[index].mean = textureMeanStdDev.mean;
            videoMetadata.texture[index].stdDev = textureMeanStdDev.stdDev;
            index++;
        }
    }
    
    // Variables for each frame
    float frameAvgLuminance = 0;
    float frameAvgRed = 0;
    float frameAvgGreen = 0;
    float frameAvgBlue = 0;
    float avgFrameHorizontalEdges = 0;
    float avgFrameVerticalEdges = 0;
    float avgFrameDegree45Edges = 0;
    float avgFrameDegree135Edges = 0;
    float avgFrameNoDirectionEdges = 0;
    float stdDevHorizontalEdges = 0;
    float stdDevVerticalEdges = 0;
    float stdDevDegree45Edges = 0;
    float stdDevDegree135Edges = 0;
    float stdDevNoDirectionEdges = 0;
    meanStdDev meanStdDevHorizEdges = {0,0};
    meanStdDev meanStdDevVertiEdges = {0,0};
    meanStdDev meanStdDev45DegreeEdges = {0,0};
    meanStdDev meanStdDev135DegreeEdges = {0,0};
    meanStdDev meanStdDevNoDiretEdges = {0,0};
    
    // Variables for the entire movie
    float luminance = 0;
    int r = 0;
    int g = 0;
    int b = 0;
    int horizontalEdges = 0;
    int verticalEdges = 0;
    int degree45Edges = 0;
    int degree135Edges = 0;
    int noDirectionEdges = 0;
    ofPixels previousPixels;
    
    int totalFaces = 0;
    
    float objectRepeat = 0;
    
    int frame = 0;
    do {
        if (frame % 10 == 0) {
            ofLogVerbose("[DEBUG] Current video frame: ") << frame;
        }
        
        pix = video->player.getPixels();
        
        float luminanceSum = 0;
        int redSum = 0;
        int greenSum = 0;
        int blueSum = 0;
        int numFaces = 0;
        
        // Iterating frame
        for(int i = 0; i < w * h * nChannels; i+= nChannels) {
            redSum += (int)pix[i];
            greenSum += (int)pix[i+1];
            blueSum += (int)pix[i+2];
        }
        luminanceSum = 0.2125*redSum+0.7154*greenSum+0.0721*blueSum;
        
        // Frame Average Luminance
        frameAvgLuminance = luminanceSum / (w * h);
        luminance += frameAvgLuminance;
        
        // Frame Average R G B
        frameAvgRed = redSum / (w * h);
        r += frameAvgRed;
        frameAvgGreen = greenSum / (w * h);
        g += frameAvgGreen;
        frameAvgBlue = blueSum / (w * h);
        b += frameAvgBlue;
        
        // Get number of faces in the frame
        totalFaces += getNumberOfFaces(pix);
        
        // Edge detection
        meanStdDev aux = getEdges(pix, (cv::Mat_<char>(2, 2) << 1, 1, -1, -1));
        meanStdDevHorizEdges.mean += aux.mean;
        meanStdDevHorizEdges.stdDev += aux.stdDev;
        
        aux = getEdges(pix, (cv::Mat_<char>(2, 2) << 1, -1, 1, -1));
        meanStdDevVertiEdges.mean += aux.mean;
        meanStdDevVertiEdges.stdDev += aux.stdDev;
        
        aux = getEdges(pix, (cv::Mat_<char>(2, 2) << sqrt(2), 0, 0, -sqrt(2)));
        meanStdDev45DegreeEdges.mean += aux.mean;
        meanStdDev45DegreeEdges.stdDev += aux.stdDev;
        
        aux = getEdges(pix, (cv::Mat_<char>(2, 2) << 0, sqrt(2), -sqrt(2), 0));
        meanStdDev135DegreeEdges.mean += aux.mean;
        meanStdDev135DegreeEdges.stdDev += aux.stdDev;
        
        aux = getEdges(pix, (cv::Mat_<char>(2, 2) << 2, -2, -2, 2));
        meanStdDevNoDiretEdges.mean += aux.mean;
        meanStdDevNoDiretEdges.stdDev += aux.stdDev;
        
        float rhythm = getRhythm(pix);
        if(frame > 0){
            float previousRhythm = videoMetadata.rhythm[frame-1].rhythm;
            videoMetadata.rhythm.push_back({fabs(rhythm - previousRhythm), frame});
        }else{
            videoMetadata.rhythm.push_back({rhythm, frame});
        }
        
        if (frame % 10 == 0) {
            objectRepeat += getObject(pix, imgObject);
        }
        
        video->player.nextFrame();
        video->player.update();
        frame++;
    } while(frame < video->player.getTotalNumFrames());
    
    // Video averages
    videoMetadata.luminance = luminance / video->player.getTotalNumFrames();
    
    videoMetadata.colour.r = r / video->player.getTotalNumFrames();
    videoMetadata.colour.g = g / video->player.getTotalNumFrames();
    videoMetadata.colour.b = b / video->player.getTotalNumFrames();
    
    videoMetadata.faces = (totalFaces * 1.0) / video->player.getTotalNumFrames();
    
    videoMetadata.edges.horizontal.mean = meanStdDevHorizEdges.mean / video->player.getTotalNumFrames();
    videoMetadata.edges.horizontal.stdDev = meanStdDevHorizEdges.stdDev / video->player.getTotalNumFrames();
    
    videoMetadata.edges.vertical.mean = meanStdDevVertiEdges.mean / video->player.getTotalNumFrames();
    videoMetadata.edges.vertical.stdDev = meanStdDevVertiEdges.stdDev / video->player.getTotalNumFrames();
    
    videoMetadata.edges.degree45.mean = meanStdDev45DegreeEdges.mean / video->player.getTotalNumFrames();
    videoMetadata.edges.degree45.stdDev = meanStdDev45DegreeEdges.stdDev / video->player.getTotalNumFrames();
    
    videoMetadata.edges.degree135.mean = meanStdDev135DegreeEdges.mean / video->player.getTotalNumFrames();
    videoMetadata.edges.degree135.stdDev = meanStdDev135DegreeEdges.stdDev / video->player.getTotalNumFrames();
    
    videoMetadata.edges.noDirection.mean = meanStdDevNoDiretEdges.mean / video->player.getTotalNumFrames();
    videoMetadata.edges.noDirection.stdDev = meanStdDevNoDiretEdges.stdDev / video->player.getTotalNumFrames();
    
    // Textures done in the beginning
    
    // Get top 10
    std::sort(videoMetadata.rhythm.begin(), videoMetadata.rhythm.end(), more_than_key());
    vector<rhythm> top10;
    top10.assign(10, rhythm());
    ofLogVerbose("top 10");
    for(int i = 0; i < 10; i++){
        top10[i] = videoMetadata.rhythm[i];
    }
    videoMetadata.rhythm = top10;
    
    for(int i = 0; i < videoMetadata.rhythm.size(); i++){
        ofLogVerbose("rhythm > ") << videoMetadata.rhythm[i].rhythm;
        ofLogVerbose("frame > ") << videoMetadata.rhythm[i].frame;
    }
    
    videoMetadata.object = (objectRepeat / video->player.getTotalNumFrames()) * 100;
    ofLogVerbose("object > ") << videoMetadata.object;
    
    string metadataDebug = "\n .colour.r = "+ofToString(videoMetadata.colour.r)
    +"\n .colour.g = "+ofToString(videoMetadata.colour.g)
    +"\n .colour.b = "+ofToString(videoMetadata.colour.b)
    +"\n .luminance = "+ofToString(videoMetadata.luminance)
    +"\n .faces = "+ofToString(videoMetadata.faces)
    +"\n .edges.horizontal.mean = "+ofToString(videoMetadata.edges.horizontal.mean)
    +"\n .edges.horizontal.stdDev = "+ofToString(videoMetadata.edges.horizontal.stdDev)
    +"\n .edges.vertical.mean = "+ofToString(videoMetadata.edges.vertical.mean)
    +"\n .edges.vertical.stdDev = "+ofToString(videoMetadata.edges.vertical.stdDev)
    +"\n .edges.degree45.mean = "+ofToString(videoMetadata.edges.degree45.mean)
    +"\n .edges.degree45.stdDev = "+ofToString(videoMetadata.edges.degree45.stdDev)
    +"\n .edges.degree135.mean = "+ofToString(videoMetadata.edges.degree135.mean)
    +"\n .edges.degree135.stdDev = "+ofToString(videoMetadata.edges.degree135.stdDev)
    +"\n .edges.noDirection.mean = "+ofToString(videoMetadata.edges.noDirection.mean);
    +"\n .edges.noDirection.stdDev = "+ofToString(videoMetadata.edges.noDirection.stdDev);
    
    for(int i = 0; i < videoMetadata.texture.size(); i++){
        metadataDebug+=+"\n ["+ofToString(i+1)+"].texture.mean = "+ofToString(videoMetadata.texture[i].mean)
        +"\n ["+ofToString(i+1)+"].texture.stdDev = "+ofToString(videoMetadata.texture[i].stdDev);
    }
    
    ofLogVerbose("[DEBUG] Video Metadata > ") << metadataDebug;
    
    video->player.setFrame(saveFrame);
    video->player.play();
    
    ofLogVerbose("[DEBUG] Total time > ") << ofGetElapsedTimef() - startTime << " seconds.";
    
    return videoMetadata;
}

//--------------------------------------------------------------
int ofApp::getNumberOfFaces(ofPixels pix){
    faceFinder.findHaarObjects(pix);
    mouthFinder.findHaarObjects(pix);
    eyesFinder.findHaarObjects(pix);
    
    int numFaces = 0;
    
    ofRectangle face;
    for(unsigned int i = 0; i < faceFinder.blobs.size(); i++) {
        face = faceFinder.blobs[i].boundingRect;
        
        bool eyesInsideFace = false;
        ofRectangle eyes;
        // Check if the face has eyes
        for(unsigned int i = 0; i < eyesFinder.blobs.size() && !eyesInsideFace; i++) {
            eyes = eyesFinder.blobs[i].boundingRect;
            
            // Check if there are eyes inside the face
            eyesInsideFace = (face.x+face.width > eyes.x+eyes.width)
            && (face.x < eyes.x)
            && (face.y+face.height > eyes.y+eyes.height)
            && (face.y < eyes.y);
        }
        
        bool mouthInsideFace = false;
        ofRectangle mouth;
        // Check if the face has a mouth
        for(unsigned int i = 0; i < mouthFinder.blobs.size() && !mouthInsideFace; i++) {
            mouth = mouthFinder.blobs[i].boundingRect;
            
            // Check if there are eyes inside the face
            mouthInsideFace = (face.x+face.width > mouth.x+mouth.width)
            && (face.x < mouth.x)
            && (face.y+face.height > mouth.y+mouth.height)
            && (face.y < mouth.y);
        }
        
        // Check if the if it's a real face with either eyes or a mouth
        if(eyesInsideFace || mouthInsideFace){
            // Found a real face
            bool faceInsideOldFace = false;
            bool oldFaceInsideFace = false;
            // Check if there's a face inside another face
            if(i >= 1){
                // We will save this face so that in the next iterations we
                // can search for faces inside this one and avoid counting
                // faces more than once.
                ofRectangle oldFace = faceFinder.blobs[i - 1].boundingRect;
                faceInsideOldFace = (oldFace.x+oldFace.width > face.x+face.width)
                && (oldFace.x < face.x)
                && (oldFace.y+oldFace.height > face.y+face.height)
                && (oldFace.y < face.y);
                
                oldFaceInsideFace = (face.x+face.width > oldFace.x+oldFace.width)
                && (face.x < oldFace.x)
                && (face.y+face.height > oldFace.y+oldFace.height)
                && (face.y < oldFace.y);
            }
            
            // Avoid counting repeated faces
            if(!faceInsideOldFace && !oldFaceInsideFace){
                ofLog() << "REAL FACE";
                numFaces++;
            }
        }
    }
    
    return numFaces;
}

//--------------------------------------------------------------
int ofApp::drawFaceDetection(ofPixels pix){
    faceFinder.findHaarObjects(pix);
    mouthFinder.findHaarObjects(pix);
    eyesFinder.findHaarObjects(pix);

    // Removes fill from rectangles
    ofNoFill();

    // Count number of faces in a frame
    int numFaces = 0;

    ofRectangle face;
    for(unsigned int i = 0; i < faceFinder.blobs.size(); i++) {
        face = faceFinder.blobs[i].boundingRect;

        bool eyesInsideFace = false;
        ofRectangle eyes;
        // Check if the face has eyes
        for(unsigned int i = 0; i < eyesFinder.blobs.size() && !eyesInsideFace; i++) {
            eyes = eyesFinder.blobs[i].boundingRect;

            // Check if there are eyes inside the face
            eyesInsideFace = (face.x+face.width > eyes.x+eyes.width)
                               && (face.x < eyes.x)
                               && (face.y+face.height > eyes.y+eyes.height)
                               && (face.y < eyes.y);
        }

        bool mouthInsideFace = false;
        ofRectangle mouth;
        // Check if the face has a mouth
        for(unsigned int i = 0; i < mouthFinder.blobs.size() && !mouthInsideFace; i++) {
            mouth = mouthFinder.blobs[i].boundingRect;

            // Check if there are eyes inside the face
            mouthInsideFace = (face.x+face.width > mouth.x+mouth.width)
                                && (face.x < mouth.x)
                                && (face.y+face.height > mouth.y+mouth.height)
                                && (face.y < mouth.y);
        }

        // Draw face if it has mouth and eyes
        if(eyesInsideFace || mouthInsideFace){
            bool faceInsideOldFace = false;
            bool oldFaceInsideFace = false;
            // Check if there's a face inside another face
            if(i >= 1){
                // We will save this face so that in the next iterations we
                // can search for faces inside this one and avoid counting
                // faces more than once.
                ofRectangle oldFace = faceFinder.blobs[i - 1].boundingRect;
                faceInsideOldFace = (oldFace.x+oldFace.width > face.x+face.width)
                                 && (oldFace.x < face.x)
                                 && (oldFace.y+oldFace.height > face.y+face.height)
                                 && (oldFace.y < face.y);

                oldFaceInsideFace = (face.x+face.width > oldFace.x+oldFace.width)
                                 && (face.x < oldFace.x)
                                 && (face.y+face.height > oldFace.y+oldFace.height)
                                 && (face.y < oldFace.y);
            }
            // Avoid repeated faces
            if(!faceInsideOldFace && !oldFaceInsideFace){
                numFaces++;

                ofLog() << "REAL FACE";
                // Draw green rectangles framing faces found
                ofSetHexColor(0x00FF00);
                ofDrawRectangle(face.x, face.y, face.width, face.height);
                // Draw blue rectangles framing eyes found
                ofSetHexColor(0x0000FF);
                ofDrawRectangle(eyes.x, eyes.y, eyes.width, eyes.height);
                // Draw red rectangles framing mouths found
                ofSetHexColor(0xFF0000);
                ofDrawRectangle(mouth.x, mouth.y, mouth.width, mouth.height);
                
                // Reset draw colour to white
                ofSetHexColor(0xFFFFFF);
            }
        }

    }
    ofLogVerbose("[DEBUG] Number of found faces >") << numFaces;
    return numFaces;
}

//--------------------------------------------------------------
ofApp::meanStdDev ofApp::getEdges(ofPixels pix, cv::Mat edgesKernel, int threshold){
    // Default threshold is 200
    pix.setImageType(OF_IMAGE_GRAYSCALE);
    
    int w = pix.getWidth();
    int h = pix.getHeight();
    int nChannels = pix.getNumChannels(); // Grayscale = 1
    
    // Edges filter
    cv::Mat edgesCV = ofxCv::toCv(pix);
    
    cv::filter2D(edgesCV, edgesCV, -1, edgesKernel);
    
    int notZero = 0;
    for(int i = 0; i < w * h; i+= nChannels) {
        // Consider only pixels above threshold
        if(pix[i] >= threshold) {
            notZero++;
        }
    }
    
    // Average to be comparable with other videos
    float avgNotZeros = (notZero * 1.0) / (w * h);
    
    float squaredDiff = 0;
    for(int i = 0; i < w * h; i+= nChannels) {
        squaredDiff += pow(pix[i] - avgNotZeros, 2);
    }

    float stdDevNotZeros = sqrtf(squaredDiff/(w * h * nChannels));
    
    return meanStdDev{avgNotZeros, stdDevNotZeros};
}

//--------------------------------------------------------------
ofApp::meanStdDev ofApp::getTexture(ofPixels pix, cv::Mat gaborKernel, int threshold){
    // Default threshold is 200
    pix.setImageType(OF_IMAGE_GRAYSCALE);
    
    int w = pix.getWidth();
    int h = pix.getHeight();
    int nChannels = pix.getNumChannels(); // Grayscale = 1
    
    // Gabor filter
    cv::Mat gaborCV = ofxCv::toCv(pix);
    
    cv::filter2D(gaborCV, gaborCV, -1, gaborKernel);
    
    int notZero = 0;
    for(int i = 0; i < w * h; i+= nChannels) {
        // Consider only pixels above threshold
        if(pix[i] >= threshold) {
            notZero++;
        }
    }

    // Average to be comparable with other videos
    float avgNotZeros = (notZero * 1.0) / (w * h);
    
    float squaredDiff = 0;
    for(int i = 0; i < w * h; i+= nChannels) {
        squaredDiff += pow(pix[i] - avgNotZeros, 2);
    }
    
    float stdDevNotZeros = sqrtf(squaredDiff/(w * h * nChannels));
    
    return meanStdDev{avgNotZeros, stdDevNotZeros};
}

//--------------------------------------------------------------
int ofApp::getRhythm(ofPixels pix, int threshold){
    // Default threshold is 200
    pix.setImageType(OF_IMAGE_GRAYSCALE);
    
    int w = pix.getWidth();
    int h = pix.getHeight();
    int nChannels = pix.getNumChannels(); // Grayscale = 1
    
    // Edge filter
    cv::Mat rhythmCV = ofxCv::toCv(pix);
    
    cv::Laplacian(rhythmCV, rhythmCV, -1);
    
    int notZero = 0;
    for(int i = 0; i < w * h; i+= nChannels) {
        // Consider only pixels above threshold
        if(pix[i] >= threshold) {
            notZero++;
        }
    }
    
    return notZero;
}

//--------------------------------------------------------------
float ofApp::getObject(ofPixels pix, string objectFileName){
    const double kDistanceCoef = 4.0;
    const int kMaxMatchingSize = 50;
    
    pix.setImageType(OF_IMAGE_GRAYSCALE);
    ofImage ofObject;
    ofObject.load(IMAGES_PATH+objectFileName);
    ofObject.setImageType(OF_IMAGE_GRAYSCALE);
    
    cv::Mat object = ofxCv::toCv(ofObject);
    cv::Mat vidFrame = ofxCv::toCv(pix);
    
    vector<cv::KeyPoint> kpts1;
    vector<cv::KeyPoint> kpts2;
    
    cv::Mat desc1;
    cv::Mat desc2;
    
    vector<cv::DMatch> matches;
    
    // detecting keypoints
    // computing descriptors
    cv::Ptr<cv::ORB> orb = cv::ORB::create();
    orb->detectAndCompute(object, cv::Mat(), kpts1, desc1);
    orb->detectAndCompute(vidFrame, cv::Mat(), kpts2, desc2);
    
    // matching descriptors
    matches.clear();
    cv::BFMatcher desc_matcher(cv::NORM_L2, true);
    if(desc2.dims <= 0) {
        return 0;
    }
    
    desc_matcher.match(desc1, desc2, matches, cv::Mat());

    std::sort(matches.begin(), matches.end());
    while (matches.front().distance * kDistanceCoef < matches.back().distance) {
        matches.pop_back();
    }
    while (matches.size() > kMaxMatchingSize) {
        matches.pop_back();
    }
    
    vector<char> match_mask(matches.size(), 1);
    if (static_cast<int>(match_mask.size()) < 3) {
        return 0;
    }
    
    vector<cv::Point2f> pts1;
    vector<cv::Point2f> pts2;
    for (int i = 0; i < static_cast<int>(matches.size()); ++i) {
        pts1.push_back(kpts1[matches[i].queryIdx].pt);
        pts2.push_back(kpts2[matches[i].trainIdx].pt);
    }
    findHomography(pts1, pts2, cv::RANSAC, 4, match_mask);
    
    cv::Mat H12;
    cv::RNG rng = cv::theRNG();
    H12.create(3, 3, CV_32FC1);
    H12.at<float>(0,0) = rng.uniform( 0.8f, 1.2f);
    H12.at<float>(0,1) = rng.uniform(-0.1f, 0.1f);
    H12.at<float>(0,2) = rng.uniform(-0.1f, 0.1f)*object.cols;
    H12.at<float>(1,0) = rng.uniform(-0.1f, 0.1f);
    H12.at<float>(1,1) = rng.uniform( 0.8f, 1.2f);
    H12.at<float>(1,2) = rng.uniform(-0.1f, 0.1f)*object.rows;
    H12.at<float>(2,0) = rng.uniform( -1e-4f, 1e-4f);
    H12.at<float>(2,1) = rng.uniform( -1e-4f, 1e-4f);
    H12.at<float>(2,2) = rng.uniform( 0.8f, 1.2f);
    
    warpPerspective(object, vidFrame, H12, object.size());
    
    float repeatability;
    int correspCount;
    evaluateFeatureDetector(object, vidFrame, H12, &kpts1, &kpts2, repeatability, correspCount);
//    ofLogVerbose("Repeatability > ") << repeatability;
//    ofLogVerbose("CorrespCount > ") << correspCount;
//    ofLogVerbose("Keypoints 1 > ") << kpts1.size();
//    ofLogVerbose("Keypoints 2 > ") << kpts2.size();
    
    return repeatability;
}
//--------------------------------------------------------------
void ofApp::writeMetadata(string videoname, metadata videoMetadata){
    ofxXmlSettings  XML;
    int extpos = videoname.find_last_of(".");
    string xmlname = videoname.substr(0,extpos)+".xml";
    // File exists and has a valid XML format
    bool xmlExists = XML.loadFile(METADATA_PATH+xmlname);

    // If metadata exists it reads it else it creates the XML file and writes the video metadata
    if(xmlExists){
        ofLog() << "Metadata XML file already exists. Overwriting it.";
    } else {
        ofLogError() << "Metadata XML file doesn't exist > "+METADATA_PATH+xmlname;
        ofLog() << "Creating and writing to the Metadata XML file: "+xmlname;

        XML.addTag("METADATA");

        // XML::METADATA
        XML.pushTag("METADATA");

        XML.addTag("TAGS");

        XML.addTag("LUMINANCE");
        XML.addTag("COLOUR");
        XML.addTag("FACES");
        
        XML.addTag("EDGES");
        
        // XML::METADATA::EDGES
        XML.pushTag("EDGES");
        
        XML.addTag("HORIZONTAL");
        XML.addTag("VERTICAL");
        XML.addTag("FORTYFIVEDEGREES");
        XML.addTag("HUNDREDTHIRTYFIVEDEGREES");
        XML.addTag("NODIRECTION");
        
        // XML::METADATA
        XML.popTag();
        
        XML.addTag("TEXTURES");
        // XML::METADATA::TEXTURES
        XML.pushTag("TEXTURES");
        for(int i=0; i < videoMetadata.texture.size(); i++){
            XML.addTag("TEXTURE");
            
            // XML::METADATA::TEXTURES::TEXTURE
            XML.pushTag("TEXTURE", i);
            XML.addTag("MEAN");
            XML.addTag("STDDEV");
            // XML::METADATA::TEXTURES
            XML.popTag();
        }
        
        // XML::METADATA
        XML.popTag();
        
        XML.addTag("RHYTHMS");
        // XML::METADATA::RHYTHMS
        XML.pushTag("RHYTHMS");
        for(int i=0; i < videoMetadata.rhythm.size(); i++){
            XML.addTag("RHYTHM");
            
            // XML::METADATA::RHYTHMS::RHYTHM
            XML.pushTag("RHYTHM", i);
            XML.addTag("VALUE");
            // XML::METADATA::RHYTHMS
            XML.popTag();
        }
        // XML::METADATA
        XML.popTag();
        
        XML.addTag("OBJECT");
        
        // XML
        XML.popTag();
    }
    
    // XML::METADATA
    XML.pushTag("METADATA");
    
    // XML::METADATA::TAGS
    XML.pushTag("TAGS");
    XML.clear(); // Update tags
    for(int i=0; i < videoMetadata.tags.size(); i++){
        XML.setValue("TAG", videoMetadata.tags[i], i);
    }
    // XML::METADATA
    XML.popTag();
    
    XML.setValue("LUMINANCE", videoMetadata.luminance, 0);
    XML.setAttribute("COLOUR", "RED", videoMetadata.colour.r, 0);
    XML.setAttribute("COLOUR", "GREEN", videoMetadata.colour.g, 0);
    XML.setAttribute("COLOUR", "BLUE", videoMetadata.colour.b, 0);
    XML.setValue("FACES", videoMetadata.faces, 0);
    
    // XML::METADATA::EDGES
    XML.pushTag("EDGES");
    XML.setAttribute("HORIZONTAL", "MEAN", videoMetadata.edges.horizontal.mean, 0);
    XML.setAttribute("HORIZONTAL", "STDDEV", videoMetadata.edges.horizontal.stdDev, 0);
    XML.setAttribute("VERTICAL", "MEAN", videoMetadata.edges.vertical.mean, 0);
    XML.setAttribute("VERTICAL", "STDDEV", videoMetadata.edges.vertical.stdDev, 0);
    XML.setAttribute("FORTYFIVEDEGREES", "MEAN", videoMetadata.edges.degree45.mean, 0);
    XML.setAttribute("FORTYFIVEDEGREES", "STDDEV", videoMetadata.edges.degree45.stdDev, 0);
    XML.setAttribute("HUNDREDTHIRTYFIVEDEGREES", "MEAN", videoMetadata.edges.degree135.mean, 0);
    XML.setAttribute("HUNDREDTHIRTYFIVEDEGREES", "STDDEV", videoMetadata.edges.degree135.stdDev, 0);
    XML.setAttribute("NODIRECTION", "MEAN", videoMetadata.edges.noDirection.mean, 0);
    XML.setAttribute("NODIRECTION", "STDDEV", videoMetadata.edges.noDirection.stdDev, 0);

    // XML::METADATA
    XML.popTag();
    
    // XML::METADATA::TEXTURES
    XML.pushTag("TEXTURES");
    for(int i=0; i < videoMetadata.texture.size(); i++){
        // XML::METADATA::TEXTURES::TEXTURE
        XML.pushTag("TEXTURE", i);
        XML.setValue("MEAN", videoMetadata.texture[i].mean, 0);
        XML.setValue("STDDEV", videoMetadata.texture[i].stdDev, 0);
        // XML::METADATA::TEXTURES
        XML.popTag();
    }

    // XML::METADATA
    XML.popTag();
    
    // XML::METADATA::RHYTHMS
    XML.pushTag("RHYTHMS");
    for(int i=0; i < videoMetadata.rhythm.size(); i++){
        // XML::METADATA::RHYTHMS::RHYTHM
        XML.setAttribute("RHYTHM", "FRAME", videoMetadata.rhythm[i].frame, i);
        XML.pushTag("RHYTHM", i);
        XML.setValue("VALUE", videoMetadata.rhythm[i].rhythm, 0);
        // XML::METADATA::RHYTHMS
        XML.popTag();
    }
    
    // XML::METADATA
    XML.popTag();
    
    XML.setValue("OBJECT", videoMetadata.object, 0);
    
    // XML
    XML.popTag();

    ofLogVerbose("[DEBUG] METADATA_PATH+xmlname > ") << METADATA_PATH+xmlname;
    if(XML.saveFile(METADATA_PATH+xmlname)){
        ofLogVerbose("[DEBUG] WRITE METADATA SUCCESS");
    }
    
    //DEBUG
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Luminance > "+ofToString(videoMetadata.luminance);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Red > "+ofToString(videoMetadata.colour.r);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Green > "+ofToString(videoMetadata.colour.g);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Blue > "+ofToString(videoMetadata.colour.b);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Faces > "+ofToString(videoMetadata.faces);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Horizontal Edges mean > "+ofToString(videoMetadata.edges.horizontal.mean);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Horizontal Edges std dev > "+ofToString(videoMetadata.edges.horizontal.stdDev);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Vertical Edges mean > "+ofToString(videoMetadata.edges.vertical.mean);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Vertical Edges std dev > "+ofToString(videoMetadata.edges.vertical.stdDev);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "45º Degree Edges mean > "+ofToString(videoMetadata.edges.degree45.mean);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "45º Degree Edges std dev > "+ofToString(videoMetadata.edges.degree45.stdDev);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "135º Degree Edges mean > "+ofToString(videoMetadata.edges.degree135.mean);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "135º Degree Edges std dev > "+ofToString(videoMetadata.edges.degree135.stdDev);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "No Direction Edges mean > "+ofToString(videoMetadata.edges.noDirection.mean);
    ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "No Direction Edges std dev > "+ofToString(videoMetadata.edges.noDirection.stdDev);
}
    
//--------------------------------------------------------------
ofApp::metadata ofApp::readMetadata(string videoname){
    ofxXmlSettings  XML;
    int extpos = videoname.find_last_of(".");
    string xmlname = videoname.substr(0,extpos)+".xml";
    // File exists and has a valid XML format
    bool xmlExists = XML.loadFile(METADATA_PATH+xmlname);
    

    metadata xmlMetadata = *new metadata;
    // Init texture
    xmlMetadata.texture.assign(24, meanStdDev());
    // Init rhythm
    xmlMetadata.rhythm.assign(10, rhythm());
    
    if(xmlExists){
        ofLog() << "XML file loaded successfully";
        
        // XML::METADATA
        XML.pushTag("METADATA");
        
        // XML::METADATA::TAGS
        XML.pushTag("TAGS");
        for(int i = 0; i < XML.getNumTags("TAG"); i++){
            xmlMetadata.tags.push_back(XML.getValue("TAG", "error", i));
        }
        
        // XML::METADATA
        XML.popTag();
        
        xmlMetadata.luminance = XML.getValue("LUMINANCE", -1);
        
        xmlMetadata.colour.r = XML.getAttribute("COLOUR", "RED", -1);
        xmlMetadata.colour.g = XML.getAttribute("COLOUR", "GREEN", -1);
        xmlMetadata.colour.b = XML.getAttribute("COLOUR", "BLUE", -1);
        
        xmlMetadata.faces = XML.getValue("FACES", -1.0);
        
        // XML::METADATA::EDGES
        XML.pushTag("EDGES");
        xmlMetadata.edges.horizontal.mean = XML.getAttribute("HORIZONTAL", "MEAN", -1.0);
        xmlMetadata.edges.horizontal.stdDev = XML.getAttribute("HORIZONTAL", "STDDEV", -1.0);
        xmlMetadata.edges.vertical.mean = XML.getAttribute("VERTICAL", "MEAN", -1.0);
        xmlMetadata.edges.vertical.stdDev = XML.getAttribute("VERTICAL", "STDDEV", -1.0);
        xmlMetadata.edges.degree45.mean = XML.getAttribute("FORTYFIVEDEGREES", "MEAN", -1.0);
        xmlMetadata.edges.degree45.stdDev = XML.getAttribute("FORTYFIVEDEGREES", "STDDEV", -1.0);
        xmlMetadata.edges.degree135.mean = XML.getAttribute("HUNDREDTHIRTYFIVEDEGREES", "MEAN", -1.0);
        xmlMetadata.edges.degree135.stdDev = XML.getAttribute("HUNDREDTHIRTYFIVEDEGREES", "STDDEV", -1.0);
        xmlMetadata.edges.noDirection.mean = XML.getAttribute("NODIRECTION", "MEAN", -1.0);
        xmlMetadata.edges.noDirection.stdDev = XML.getAttribute("NODIRECTION", "STDDEV", -1.0);
        
        // XML::METADATA
        XML.popTag();
        
        // XML::METADATA::TEXTURES
        XML.pushTag("TEXTURES");
        for(int i=0; i < xmlMetadata.texture.size(); i++){
            // XML::METADATA::TEXTURES::TEXTURE
            XML.pushTag("TEXTURE", i);
            xmlMetadata.texture[i].mean = XML.getValue("MEAN", -1.0);
            xmlMetadata.texture[i].stdDev = XML.getValue("STDDEV", -1.0);
            // XML::METADATA::TEXTURES
            XML.popTag();
        }
        
        // XML::METADATA
        XML.popTag();
        
        // XML::METADATA::RHYTHMS
        XML.pushTag("RHYTHMS");
        for(int i = 0; i < XML.getNumTags("RHYTHM"); i++){
            xmlMetadata.rhythm[i].frame = XML.getAttribute("RHYTHM", "FRAME", -1, i);
            // XML::METADATA::RHYTHMS::RHYTHM
            XML.pushTag("RHYTHM", i);
            xmlMetadata.rhythm[i].rhythm = XML.getValue("VALUE", -1.0);
            // XML::METADATA::RHYTHMS
            XML.popTag();
        }
        // XML::METADATA
        XML.popTag();
        
        xmlMetadata.object = XML.getValue("OBJECT", -1.0);
        
        // DEBUG
        for(int i = 0; i < xmlMetadata.tags.size(); i++){
            ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Tag > "+ofToString(xmlMetadata.tags[i]);
        }
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Luminance > "+ofToString(xmlMetadata.luminance);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Red > "+ofToString(xmlMetadata.colour.r);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Green > "+ofToString(xmlMetadata.colour.g);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Blue > "+ofToString(xmlMetadata.colour.b);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Faces > "+ofToString(xmlMetadata.faces);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Horizontal Edges mean > "+ofToString(xmlMetadata.edges.horizontal.mean);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Horizontal Edges std dev > "+ofToString(xmlMetadata.edges.horizontal.stdDev);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Vertical Edges mean > "+ofToString(xmlMetadata.edges.vertical.mean);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "Vertical Edges std dev > "+ofToString(xmlMetadata.edges.vertical.stdDev);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "45º Degree Edges mean > "+ofToString(xmlMetadata.edges.degree45.mean);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "45º Degree Edges std dev > "+ofToString(xmlMetadata.edges.degree45.stdDev);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "135º Degree Edges mean > "+ofToString(xmlMetadata.edges.degree135.mean);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "135º Degree Edges std dev > "+ofToString(xmlMetadata.edges.degree135.stdDev);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "No Direction Edges mean > "+ofToString(xmlMetadata.edges.noDirection.mean);
        ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") << "No Direction Edges std dev > "+ofToString(xmlMetadata.edges.noDirection.stdDev);
        
        for(int i = 0; i < xmlMetadata.texture.size(); i++){
            ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") <<
            "Texture ["+ofToString(i+1)+"] mean > "+ofToString(xmlMetadata.texture[i].mean);
            ofLogVerbose("[DEBUG] ["+xmlname+"] VIDEO METADATA") <<
            "Texture ["+ofToString(i+1)+"] std dev > "+ofToString(xmlMetadata.texture[i].stdDev);
        }
        
        // XML
        XML.popTag();
    } else {
        ofLogError("XML file ["+xmlname+"] does not exist.");
    }
    
    return xmlMetadata;
}

//--------------------------------------------------------------
void ofApp::writePlaylist(playlist playlist){
    ofxXmlSettings XML2;
    int extpos = playlist.name.find_last_of(".");
    string xmlname = playlist.name.substr(0,extpos)+".xml";
    // File exists and has a valid XML format
    bool xmlExists = XML2.loadFile(PLAYLISTS_PATH+xmlname);
    
    // If playlist exists it reads it else it creates the XML file and writes the playlist
    if(xmlExists){
        ofLog() << "Playlist XML file already exists. Overwriting it.";
    } else {
        ofLogError() << "Playlist XML file doesn't exist > "+PLAYLISTS_PATH+xmlname;
        ofLog() << "Creating and writing to the Playlist XML file: "+xmlname;
        
        XML2.addTag("PLAYLIST");
        // XML::PLAYLIST
        XML2.pushTag("PLAYLIST");
        
        XML2.addTag("VIDEOS");
        // XML::PLAYLIST::VIDEOS
        XML2.pushTag("VIDEOS");

        // XML::PLAYLIST::VIDEOS::VIDEO_NAME
        for(int i = 0; i < playlist.videoNames.size(); i++){
            XML2.addTag("VIDEONAME");
        }
        
        // XML::PLAYLIST
        XML2.popTag();
        
        XML2.addTag("CONDITIONS");
        // XML::PLAYLIST::CONDITIONS
        XML2.pushTag("CONDITIONS");
        
        XML2.addTag("FACES");
        // XML::PLAYLIST
        XML2.popTag();
        // XML
        XML2.popTag();
    }
    XML2.saveFile(PLAYLISTS_PATH+xmlname);
    // XML::PLAYLIST
    XML2.pushTag("PLAYLIST");

    // XML::PLAYLIST::VIDEOS
    XML2.pushTag("VIDEOS");
    // Remove all tags and re-write them to remove old videos
    XML2.clear();
    for(int i = 0; i < playlist.videoNames.size(); i++){
        XML2.setValue("VIDEONAME", playlist.videoNames[i], i);
    }
    
    // XML::PLAYLIST
    XML2.popTag();
    
    // XML::PLAYLIST::CONDITIONS
    XML2.pushTag("CONDITIONS");
    
    XML2.setValue("FACES", playlist.faces);
    // XML::PLAYLIST
    XML2.popTag();
    
    // XML
    XML2.popTag();
    
    XML2.saveFile(PLAYLISTS_PATH+xmlname);
}

//--------------------------------------------------------------
ofApp::playlist ofApp::readPlaylist(string playlistName){
    ofxXmlSettings XML2;
    int extpos = playlistName.find_last_of(".");
    string xmlname = playlistName.substr(0,extpos)+".xml";
    // File exists and has a valid XML format
    bool xmlExists = XML2.loadFile(PLAYLISTS_PATH+xmlname);
    
    playlist xmlPlaylist;
    xmlPlaylist.name = playlistName;
    
    if(xmlExists){
        ofLog() << "XML file loaded successfully";
        
        // XML::PLAYLIST
        XML2.pushTag("PLAYLIST");
        // XML::PLAYLIST::VIDEOS
        XML2.pushTag("VIDEOS");
        
        for(int i=0; i < XML2.getNumTags("VIDEONAME"); i++){
            xmlPlaylist.videoNames.push_back(XML2.getValue("VIDEONAME", "", i));
        }
        // XML::PLAYLIST
        XML2.popTag();
        
        // XML::PLAYLIST::CONDITIONS
        XML2.pushTag("CONDITIONS");
        
        xmlPlaylist.faces = XML2.getValue("FACES", -1.0);
        
        // XML::PLAYLIST
        XML2.popTag();
        
        // XML
        XML2.popTag();
    } else {
        ofLogError("XML file ["+xmlname+"] does not exist.");
    }
    
    return xmlPlaylist;
}

//--------------------------------------------------------------
void ofApp::changeAppWindow(AppWindow window){
    previousWindow = currentWindow;
    currentWindow = window;
    if(!defaultVideo.isPaused()) defaultVideo.setPaused(true);
    switch (currentWindow) {
        case START_WINDOW:
            userButton->setEnabled(true);
            operatorButton->setEnabled(true);
            for(int i = 0; i < videoLibrary.size(); i++){
                videoLibrary[i].player.setPaused(true);
            }
            break;
        case USER_WINDOW:
        {
            currentPlaylistIndex = 0;
            currentVideoDetailIndex = 0;
            for(int i = 0; i < videoLibrary.size(); i++){
                videoLibrary[i].player.setVolume(1);
                videoLibrary[i].player.setPaused(true);
            }
            // TODO FIX INDEX

            if(playlistsDir.size() > 0){
                int index = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
                videoLibrary[index].player.play();
            }else{
                defaultVideo.play();
            }
            break;
        }
        case VIDEO_LIBRARY_WINDOW:
        {
            // 4 x 3 videos
            row = 4;
            column = 3;
            // Defining the library width and height
            vidLibraryWidth = ofGetWidth() - (2 * APP_BORDER);
            vidLibraryHeight = ofGetHeight() - (2 * APP_BORDER);
            // Defining the space between the videos
            vidSpace = 20;
            // Defining the videos width and height
            vidWidth = (vidLibraryWidth - (row + 1) * vidSpace) / row;
            vidHeight = (vidLibraryHeight - (column + 1) * vidSpace) / column;
            
            // Update video size
            videoLibrary[currentVideoDetailIndex].x = (APP_BORDER + vidSpace + (vidSpace + vidWidth)
                                                       * (currentVideoDetailIndex % row)) - 5;
            videoLibrary[currentVideoDetailIndex].y = (APP_BORDER + vidSpace + (vidSpace + vidHeight)
                                                       * (currentVideoDetailIndex / (column + 1))) - 5;
            videoLibrary[currentVideoDetailIndex].width = vidWidth + 10;
            videoLibrary[currentVideoDetailIndex].height = vidHeight + 10;
            
            // Prepare video thumbnail/micon
            videoLibrary[currentVideoDetailIndex].player.setPosition(0.4);
            videoLibrary[currentVideoDetailIndex].player.setSpeed(0.5);
            videoLibrary[currentVideoDetailIndex].player.setVolume(0);
            videoLibrary[currentVideoDetailIndex].player.setPaused(true);
            
            for(int i = 0; i < videoLibrary.size(); i++){
                videoLibrary[i].player.setVolume(0);
            }
            
            videoLibraryButton->setEnabled(false);
            playlistButton->setEnabled(true);
            
            break;
        }
        case VIDEO_DETAIL_WINDOW:
        {
            /*-----------------------------------------------------------------------*/
            /*----------------------------- VIDEO PANEL -----------------------------*/
            /*-----------------------------------------------------------------------*/
            videoPanelHeight = ofGetHeight() / 2;
            videoPanelWidth = ofGetWidth() / 2;
            
            // Draw video under the title on the 1st vertical half
            // Keep video's original aspect ratio
            int vidOriginalHeight = videoLibrary[currentVideoDetailIndex].player.getHeight();
            int vidOriginalWidth = videoLibrary[currentVideoDetailIndex].player.getWidth();
            
            // Video height depends on the width
            vidWidth = videoPanelWidth - 2*APP_BORDER;
            vidHeight = (vidWidth * vidOriginalHeight) / vidOriginalWidth;
            
            int vidX = (ofGetWidth()*.75 - vidWidth) / 2;
            
            ofRectangle stringBoundingBox = verdana30.getStringBoundingBox("Watching: ", APP_BORDER, APP_BORDER);
            
            videoLibrary[currentVideoDetailIndex].x = vidX;
            int titleSpace = 10;
            videoLibrary[currentVideoDetailIndex].y = stringBoundingBox.y+stringBoundingBox.height+titleSpace;
            videoLibrary[currentVideoDetailIndex].width = vidWidth;
            videoLibrary[currentVideoDetailIndex].height = vidHeight;
            
            /*-----------------------------------------------------------------------*/
            /*------------------------ VIDEO METADATA PANEL -------------------------*/
            /*-----------------------------------------------------------------------*/
            
            metadata videoMetadata = readMetadata(videoLibrary[currentVideoDetailIndex].name);
            
            float scaleFactor = 0.75;
            ofPushMatrix();
                // Change axis to the center of 4th 1/4 of the screen
                ofTranslate((ofGetWidth() * 0.75) + (ofGetWidth() * 0.25)/2, 0);
                ofScale(scaleFactor);
            
                string text = "Video Metadata";
                float stringWidth = verdana30.stringWidth(text);
                float stringHeight = verdana30.stringHeight(text);
            
                verdana30.drawString(text, -stringWidth/2, APP_BORDER/scaleFactor);
            ofPopMatrix();
            
            // Space between the title and the gui
            titleSpace = 20;
            gui->setPosition((ofGetWidth() * 0.75) + APP_BORDER/2, APP_BORDER+stringHeight+titleSpace);
            // 1/4 of the screen - border
            gui->setWidth((ofGetWidth() * 0.25) - APP_BORDER);
            
            ofxDatGuiFolder* folder;
            ofxDatGuiLabel* label;
            ofxDatGuiButton* button;
            ofxDatGuiSlider* slider;
            ofxDatGuiTextInput* input;
            // Tags
            folder = gui->getFolder("Tags");
            for(int i = 0; i < videoMetadata.tags.size(); i++){
                input = gui->getTextInput("Tag "+ofToString(i+1)+" >", "Tags");
                if(input->getLabel() == "X"){ // Doesn't exist yet
                    input = folder->addTextInput("Tag "+ofToString(i+1)+" >", videoMetadata.tags[i]);
                    input->setLabelAlignment(ofxDatGuiAlignment::RIGHT);
                    input->setStripeVisible(false);
                }
                input->onTextInputEvent(this, &ofApp::onTextInputEvent);
            }
            

            // Colour
            slider = gui->getSlider("RED", "Colour");
            slider->setValue(videoMetadata.colour.r);
            slider = gui->getSlider("GREEN", "Colour");
            slider->setValue(videoMetadata.colour.g);
            slider = gui->getSlider("BLUE", "Colour");
            slider->setValue(videoMetadata.colour.b);
            // Luminance
            slider = gui->getSlider("LUMINANCE", "Colour");
            slider->setValue(videoMetadata.luminance);
            
            // Faces
            input = gui->getTextInput("Faces");
            ofLogVerbose("GUI FACES > ") << videoMetadata.faces;
            input->setText(ofToString(videoMetadata.faces));
            
            // Edges
            input = gui->getTextInput("Mean", "Horizontal Edges");
            input->setText(ofToString(videoMetadata.edges.horizontal.mean));
            input = gui->getTextInput("Std. Dev.", "Horizontal Edges");
            input->setText(ofToString(videoMetadata.edges.horizontal.stdDev));
            
            input = gui->getTextInput("Mean", "Vertical Edges");
            input->setText(ofToString(videoMetadata.edges.vertical.mean));
            input = gui->getTextInput("Std. Dev.", "Vertical Edges");
            input->setText(ofToString(videoMetadata.edges.vertical.stdDev));
            
            input = gui->getTextInput("Mean", "45º Degree Edges");
            input->setText(ofToString(videoMetadata.edges.degree45.mean));
            input = gui->getTextInput("Std. Dev.", "45º Degree Edges");
            input->setText(ofToString(videoMetadata.edges.degree45.stdDev));
            
            input = gui->getTextInput("Mean", "135º Degree Edges");
            input->setText(ofToString(videoMetadata.edges.degree135.mean));
            input = gui->getTextInput("Std. Dev.", "135º Degree Edges");
            input->setText(ofToString(videoMetadata.edges.degree135.stdDev));
            
            input = gui->getTextInput("Mean", "Non Directional Edges");
            input->setText(ofToString(videoMetadata.edges.noDirection.mean));
            input = gui->getTextInput("Std. Dev.", "Non Directional Edges");
            input->setText(ofToString(videoMetadata.edges.noDirection.stdDev));
            
            // Texture
          
            // Rhythm
            
            // Object
            input = gui->getTextInput("Object", "Object");
            input->setText(ofToString(videoMetadata.object));
            input = gui->getTextInput("File Path", "Object");
            input->setText(imgObject);
            
            // Prepare to draw video
            videoLibrary[currentVideoDetailIndex].player.firstFrame();
            videoLibrary[currentVideoDetailIndex].player.setSpeed(1.0);
            videoLibrary[currentVideoDetailIndex].player.setVolume(1.0);
            videoLibrary[currentVideoDetailIndex].player.play();
            
            break;
        }
        case PLAYLIST_LIBRARY_WINDOW:
        {
            // Video's sizes must be defined here so that when the mouse cursor
            // hovers over the videos their size can increase correctly.
            // If the size was defined in drawLibrary() then it would constantly
            // override the increased size.
            // 4 x 3 videos
            row = 3;
            column = 4;
            // Defining the library width and height
            vidLibraryWidth = ofGetWidth() - (2 * APP_BORDER);
            vidLibraryHeight = ofGetHeight() - (2 * APP_BORDER);
            // Defining the space between the videos
            vidSpace = 20;
            // Get Playlist Title bounding box
            ofRectangle box = verdana15.getStringBoundingBox("Playlist Placeholder", 0,  0);
            
            // Defining the videos width and height
            vidWidth = (vidLibraryWidth - (column + 1) * vidSpace) / column;
            vidHeight = (vidLibraryHeight - (row + 1) * vidSpace - row*box.height) / row;
            for(int i = 0; i < (int)dir.size(); i++){
                // Defining video positions and sizes
                videoLibrary[i].x = APP_BORDER + vidSpace + (vidSpace + vidWidth) * (i % column);
                videoLibrary[i].y = APP_BORDER + vidSpace + box.height + (vidSpace + box.height + vidHeight) * (i / (row + 1));
                videoLibrary[i].width = vidWidth;
                videoLibrary[i].height = vidHeight;
            }
            
            for(int i = 0; i < videoLibrary.size(); i++){
                videoLibrary[i].player.setVolume(0);
            }
            
            playlistButton->setEnabled(false);
            videoLibraryButton->setEnabled(true);
            ofLogVerbose("[DEBUG] playlists.size() > ") << playlists.size();
            if(playlists.size() < row){
                createPlaylistButton->setEnabled(true);
            }
            break;
        }
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::drawStartWindow(){
    ofFill();
    ofRectangle box = verdana30.getStringBoundingBox("(Semi) Automatic Infinite Entertainment System", 0, 0);
    
    ofSetColor(238, 57, 135);
    ofDrawRectangle(ofGetWidth()/2 - box.width/2 - APP_BORDER,
                    ofGetHeight()*.25 - APP_BORDER,
                    box.width + 2*APP_BORDER,
                    2*APP_BORDER);
    
    ofSetColor(255); // White, gray scale
    verdana30.drawString("(Semi) Automatic Infinite Entertainment System", ofGetWidth()/2-box.width/2, ofGetHeight()*.25);
    
    chooseUserLabel->setWidth(userButton->getWidth()+operatorButton->getWidth());
    userButton->draw();
    userButton->setPosition(ofGetWidth()/2 - userButton->getWidth(), ofGetHeight()/2);
    operatorButton->draw();
    operatorButton->setPosition(ofGetWidth()/2, ofGetHeight()/2);
    chooseUserLabel->setWidth(userButton->getWidth()+operatorButton->getWidth());
    chooseUserLabel->setPosition(userButton->getX(), ofGetHeight()/2 - chooseUserLabel->getHeight());
    chooseUserLabel->draw();
}
//--------------------------------------------------------------
void ofApp::drawVideoLibrary(){
    ofSetColor(ofColor::white);
    verdana30.drawString("Video Library", APP_BORDER, APP_BORDER);
    
    ofRectangle box = verdana30.getStringBoundingBox("Video Library", APP_BORDER, APP_BORDER);

    playlistButton->setPosition(ofGetWidth()-APP_BORDER-playlistButton->getWidth(), APP_BORDER-playlistButton->getHeight());
    playlistButton->draw();
    
    for(int i = 0; i < (int)dir.size(); i++){
        videoLibrary[i].player.draw(videoLibrary[i].x, videoLibrary[i].y, videoLibrary[i].width, videoLibrary[i].height);
        if(videoLibrary[i].player.isPaused() || (videoLibrary[i].player.getPosition() >= 0.6)){
            videoLibrary[i].player.setPosition(0.5);
        }
    }
}

//--------------------------------------------------------------
void ofApp::drawVideoDetail(){
    // Screen will be divided into 2 main parts.
    // 1 for the video and another for its metadata
    
    ofNoFill();
    ofSetColor(255); // White, gray scale
    ofDrawLine(3*(ofGetWidth()/4),APP_BORDER,3*(ofGetWidth()/4),ofGetHeight()-APP_BORDER); // 3/4 screen line
    
    /*-----------------------------------------------------------------------*/
    /*----------------------------- VIDEO PANEL -----------------------------*/
    /*-----------------------------------------------------------------------*/
    
    videoPanelHeight = ofGetHeight();
    videoPanelWidth = ofGetWidth() * 0.75;
    
    // Draw video under the title on the 1st vertical half
    // Keep video's original aspect ratio
    int vidOriginalHeight = videoLibrary[currentVideoDetailIndex].player.getHeight();
    int vidOriginalWidth = videoLibrary[currentVideoDetailIndex].player.getWidth();
    
    // Video height depends on the width
    vidWidth = videoPanelWidth - 2*APP_BORDER;
    vidHeight = (vidWidth * vidOriginalHeight) / vidOriginalWidth;
    
    ofRectangle stringBoundingBox = verdana30.getStringBoundingBox("Watching: ", APP_BORDER, APP_BORDER);
    
    videoLibrary[currentVideoDetailIndex].x = APP_BORDER;
    int titleSpace = 10;
    videoLibrary[currentVideoDetailIndex].y = (ofGetHeight() / 2) - (vidHeight / 2);
    videoLibrary[currentVideoDetailIndex].width = vidWidth;
    videoLibrary[currentVideoDetailIndex].height = vidHeight;
    
    // VIDEO PANEL
    int center = 0;
    int metadataWidth = 0;
    float stringWidth = 0;
    float stringHeight = 0;
    
    stringWidth = verdana30.stringWidth(videoLibrary[currentVideoDetailIndex].name);
    int centerTitle = (videoLibrary[currentVideoDetailIndex].y - APP_BORDER) / 2;
    verdana30.drawString(videoLibrary[currentVideoDetailIndex].name, APP_BORDER, APP_BORDER + centerTitle);
    
    // Video frame
    ofDrawRectangle(
                    videoLibrary[currentVideoDetailIndex].x,
                    videoLibrary[currentVideoDetailIndex].y,
                    videoLibrary[currentVideoDetailIndex].width,
                    videoLibrary[currentVideoDetailIndex].height);
    
    videoLibrary[currentVideoDetailIndex]
    .player
    .draw(videoLibrary[currentVideoDetailIndex].x, videoLibrary[currentVideoDetailIndex].y,
          videoLibrary[currentVideoDetailIndex].width, videoLibrary[currentVideoDetailIndex].height);
    
    addToPlaylistDropdown->setPosition(videoLibrary[currentVideoDetailIndex].x
                                       +videoLibrary[currentVideoDetailIndex].width
                                       -addToPlaylistDropdown->getWidth(),
                                       videoLibrary[currentVideoDetailIndex].y
                                       -addToPlaylistDropdown->getHeight());
    addToPlaylistDropdown->draw();
    
    /*-----------------------------------------------------------------------*/
    /*------------------------ VIDEO METADATA PANEL -------------------------*/
    /*-----------------------------------------------------------------------*/
    
    float scaleFactor = 0.75;
    ofPushMatrix();
        // Change axis to the center of 4th 1/4 of the screen
        ofTranslate((ofGetWidth() * 0.75) + (ofGetWidth() * 0.25)/2, 0);
        ofScale(scaleFactor);
    
        string text = "Video Metadata";
        stringWidth = verdana30.stringWidth(text);
        stringHeight = verdana30.stringHeight(text);
    
        verdana30.drawString(text, -stringWidth/2, APP_BORDER/scaleFactor);
    ofPopMatrix();
    
    gui->draw();
}

//--------------------------------------------------------------
void ofApp::drawPlaylistLibrary(){
    ofSetColor(ofColor::white);
    verdana30.drawString("Playlist Library", APP_BORDER, APP_BORDER);
    
    ofRectangle box = verdana30.getStringBoundingBox("Playlist Library", APP_BORDER, APP_BORDER);

    int titleSpace = APP_BORDER/2;
    int currentRow = 0;
    int playlistVideoIndex = 0;
    for(int k = 0; k < playlists.size(); k++){
        for(int j = 0; j < playlists[k].videoNames.size(); j++){
            for(int i = 0; i < (int)dir.size(); i++){
                if(playlists[k].videoNames[j] == videoLibrary[i].name){
                    videoLibrary[i].player.draw(
                                                videoLibrary[column*k + j].x,
                                                videoLibrary[column*k + j].y,
                                                videoLibrary[column*k + j].width,
                                                videoLibrary[column*k + j].height);
                    if(j == 0){ //First video
                        verdana15.drawString(playlists[k].name, videoLibrary[column*k].x, videoLibrary[column*k].y - 5);
                    }
                    if(videoLibrary[i].player.isPaused() || (videoLibrary[i].player.getPosition() >= 0.6)){
                        videoLibrary[i].player.setPosition(0.5);
                    }
                    playlistVideoIndex++;
                }
            }
        }
    }
    
    videoLibraryButton->setPosition(ofGetWidth()-APP_BORDER-videoLibraryButton->getWidth(), APP_BORDER-videoLibraryButton->getHeight());
    createPlaylistButton->setPosition(videoLibraryButton->getX(), videoLibraryButton->getY()+videoLibraryButton->getHeight());
    createPlaylistGui->setPosition(createPlaylistButton->getX(), createPlaylistButton->getY()+createPlaylistButton->getHeight());
    videoLibraryButton->draw();
    if(playlists.size() < row){
        createPlaylistButton->draw();
    }else{
        createPlaylistButton->setEnabled(false);
    }
    if(createPlaylistEnabled){
        createPlaylistGui->draw();
    }
}

//--------------------------------------------------------------
void ofApp::drawUserVideo(){
    if(playlistsDir.size() > 0){
        // check if drawVid 1st or later
        int oldPlaylistIndex = currentPlaylistIndex;
        int oldPlaylistVideoIndex = currentPlayingVideoIndex;
        
        // Audio Detection
        
        // Skip video if audio detected
        // Only check for audio input once per second
        if(soundReceived >= 0.1 && (ofGetSystemTimeMillis() - lockInterval) >= 1000){
            // Audio was detected so turn on flag to change video later
            audioDetected = true;
            
            lockInterval = ofGetSystemTimeMillis();
            soundReceived = 0;
        }
        
        float startTime = ofGetElapsedTimef();
        if(startTime > 3) { // Every 3 seconds
            ofPixels pix = vidGrabber.getPixels();
            pix.setImageType(OF_IMAGE_GRAYSCALE);
            camFaces = getNumberOfFaces(pix);

            for(int i = 0; i < playlists.size(); i++){
                if(camFaces <= playlists[i].faces){
                    currentPlaylistIndex = i;
                }
            }
            // Update video index to avoid accessing unexistent videos
            // Ex: playlist 1 has 3 videos and playlist 0 has 2 videos
            // If we change from 1 to 0, we're gonna try to access video 3 when
            // it doesn't exist. If it's the same size nothing changes
            currentPlayingVideoIndex %= playlists[currentPlaylistIndex].videoNames.size();

            ofResetElapsedTimeCounter();
        }

        if(oldPlaylistIndex != currentPlaylistIndex){
            if(oldPlaylistVideoIndex != currentPlayingVideoIndex){
                // Changed both Playlist and Video
                //Pause current vid
                int oldIndex = videoIndexMap[playlists[oldPlaylistIndex].videoNames[oldPlaylistVideoIndex]];
                videoLibrary[oldIndex].player.setPaused(true);
                // Play new video
                int index = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
                videoLibrary[index].player.play();
            } else {
                // Changed only Playlist
                //Pause current vid
                int oldIndex = videoIndexMap[playlists[oldPlaylistIndex].videoNames[currentPlayingVideoIndex]];
                videoLibrary[oldIndex].player.setPaused(true);
                // Play new video
                int index = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
                videoLibrary[index].player.play();
            }
        }
        
        int index = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
        if(videoLibrary[index].player.getPosition() >= 0.99 || audioDetected){
            // ONLY THE VIDEO IS BEING CHANGED
            //Pause current vid
            int oldIndex = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
            videoLibrary[oldIndex].player.setPaused(true);

            // Change to new video
            currentPlayingVideoIndex++;
            currentPlayingVideoIndex %= playlists[currentPlaylistIndex].videoNames.size();

            ofLogVerbose("playlist index > ") << currentPlaylistIndex;
            ofLogVerbose("video index > ") << currentPlayingVideoIndex;
            ofLogVerbose("video name > ") << playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex];
            // Play new video
            int index = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
            videoLibrary[index].player.play();
            // Turn off flag
            if(audioDetected) audioDetected = !audioDetected;
        }
        
        index = videoIndexMap[playlists[currentPlaylistIndex].videoNames[currentPlayingVideoIndex]];
        videoLibrary[index].player.draw(APP_BORDER, APP_BORDER, ofGetWidth()-2*APP_BORDER, ofGetHeight()-2*APP_BORDER);
        
        //Safety check for videos playing in the background
        for(int i = 0; i < videoLibrary.size(); i++){
            if(i != index){
                videoLibrary[i].player.setPaused(true);
            }
        }
    }else{
        defaultVideo.draw(APP_BORDER, APP_BORDER, ofGetWidth()-2*APP_BORDER, ofGetHeight()-2*APP_BORDER);
    }
}

//--------------------------------------------------------------
void ofApp::drawCamera(){
    vidGrabber.draw(0, 0, camWidth, camHeight);
    float startTime = ofGetElapsedTimef();
    if(startTime > 1) { // Every 1 seconds
        ofPixels pix = vidGrabber.getPixels();
        pix.setImageType(OF_IMAGE_GRAYSCALE);
        camFaces = drawFaceDetection(pix);
        
        ofResetElapsedTimeCounter();
    }
}

//--------------------------------------------------------------
bool ofApp::isInsideArea(int x, int y, int areaX, int areaY, int areaWidth, int areaHeight){
    return areaX <= x && x <= areaX+areaWidth
    && areaY <= y && y <= areaY+areaHeight;
}

//--------------------------------------------------------------
void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
    cout << "onSliderEvent: " << e.target->getLabel() << " " << e.target->getValue() << endl;
    if(e.target->getLabel() == gui->getTextInput("Red", "Colour")->getLabel()){
        videoLibrary[currentVideoDetailIndex].metadata.colour.r = e.target->getValue();
    }else if(e.target->getLabel() == gui->getTextInput("Green", "Colour")->getLabel()){
        videoLibrary[currentVideoDetailIndex].metadata.colour.g = e.target->getValue();
    }else if(e.target->getLabel() == gui->getTextInput("Blue", "Colour")->getLabel()){
        videoLibrary[currentVideoDetailIndex].metadata.colour.b = e.target->getValue();
    }else if(e.target->getLabel() == gui->getTextInput("Luminance", "Colour")->getLabel()){
        videoLibrary[currentVideoDetailIndex].metadata.luminance = e.target->getValue();
    }
    
//    writeMetadata(videoLibrary[currentVideoDetailIndex].name, videoLibrary[currentVideoDetailIndex].metadata);
}

//--------------------------------------------------------------
void ofApp::onTextInputEvent(ofxDatGuiTextInputEvent e)
{
    cout << "onTextInputEvent: " << e.target->getLabel() << " " << e.target->getText() << endl;
    for(int i = 0; i < videoLibrary[currentVideoDetailIndex].metadata.tags.size(); i++){
        if(e.target->getLabel() == "Tag "+ofToString(i+1)+" >"){
            if(e.target->getText() == ""){
                e.target->setVisible(false);
                ofLogVerbose("erase tag > ") << videoLibrary[currentVideoDetailIndex].metadata.tags[i];
                videoLibrary[currentVideoDetailIndex].metadata.tags.erase(videoLibrary[currentVideoDetailIndex].metadata.tags.begin()+i);
            }
        }
    }
    ofLogVerbose(" X > ") << e.target->getX();
    ofLogVerbose(" Y > ") << e.target->getY();
    ofLogVerbose(" index > ") << e.target->getIndex();
    ofLogVerbose(" label > ") << e.target->getLabel();
    ofLogVerbose(" name > ") << e.target->getName();
    if(e.target->getLabel() == gui->getTextInput("Add Tag", "Tags")->getLabel()){
        ofxDatGuiFolder* folder = gui->getFolder("Tags");
        
        videoLibrary[currentVideoDetailIndex].metadata.tags.push_back(e.target->getText());
        ofxDatGuiTextInput* input = folder->addTextInput("Tag "
                                     +ofToString(videoLibrary[currentVideoDetailIndex].metadata.tags.size())
                                     +" >", videoLibrary[currentVideoDetailIndex].metadata.tags.back());
        input->setLabelAlignment(ofxDatGuiAlignment::RIGHT);
        input->setStripeVisible(false);
        input->onTextInputEvent(this, &ofApp::onTextInputEvent);
        
        int frame = videoLibrary[currentVideoDetailIndex].player.getCurrentFrame();
        changeAppWindow(VIDEO_LIBRARY_WINDOW);
        writeMetadata(videoLibrary[currentVideoDetailIndex].name, videoLibrary[currentVideoDetailIndex].metadata);
        videoLibrary[currentVideoDetailIndex].player.setFrame(frame);
        changeAppWindow(VIDEO_DETAIL_WINDOW);
    }else if(e.target->getLabel() == gui->getTextInput("Faces")->getLabel()){
        videoLibrary[currentVideoDetailIndex].metadata.faces = stof(e.target->getText());
    }else if(e.target->getY() == gui->getTextInput("Mean", "Horizontal Edges")->getY()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.horizontal.mean = stof(e.target->getText());
    }else if(e.target->getY() == gui->getTextInput("Std. Dev.", "Horizontal Edges")->getY()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.horizontal.stdDev = stof(e.target->getText());
    }else if(e.target->getY() == gui->getTextInput("Mean", "Vertical Edges")->getY()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.vertical.mean = stof(e.target->getText());
    }else if(e.target->getY() == gui->getTextInput("Std. Dev.", "Vertical Edges")->getY()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.vertical.stdDev = stof(e.target->getText());
    }else if(e.target->getIndex() == gui->getTextInput("Mean", "45º Degree Edges")->getIndex()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.degree45.mean = stof(e.target->getText());
    }else if(e.target->getIndex() == gui->getTextInput("Std. Dev.", "45º Degree Edges")->getIndex()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.degree45.stdDev = stof(e.target->getText());
    }else if(e.target->getIndex() == gui->getTextInput("Mean", "135º Degree Edges")->getIndex()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.degree135.mean = stof(e.target->getText());
    }else if(e.target->getIndex() == gui->getTextInput("Std. Dev.", "135º Degree Edges")->getIndex()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.degree135.stdDev = stof(e.target->getText());
    }else if(e.target->getIndex() == gui->getTextInput("Mean", "Non Directional Edges")->getIndex()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.noDirection.mean = stof(e.target->getText());
    }else if(e.target->getIndex() == gui->getTextInput("Std. Dev.", "Non Directional Edges")->getIndex()){
        videoLibrary[currentVideoDetailIndex].metadata.edges.noDirection.stdDev = stof(e.target->getText());
        // FIX BELOW
//    }
//    else if(e.target->getLabel() == gui->getTextInput("Mean", "Texture")->getLabel()){
//        ofxDatGuiMatrix* matrix = gui->getMatrix("Texture");
//        ofLogVerbose(" X > ") << matrix->getX();
//        ofLogVerbose(" Y > ") << matrix->getY();
//        ofLogVerbose(" index > ") << matrix->getIndex();
//        ofLogVerbose(" label > ") << matrix->getLabel();
//        ofLogVerbose(" name > ") << matrix->getName();
//        videoLibrary[currentVideoDetailIndex].metadata.texture[matrix->getSelected().front()].mean = stof(e.target->getText());
//    }else if(e.target->getLabel() == gui->getTextInput("Std. Dev.", "Texture")->getLabel()){
//        ofxDatGuiMatrix* matrix = gui->getMatrix("Texture");
//
//        videoLibrary[currentVideoDetailIndex].metadata.texture[matrix->getSelected().front()].stdDev = stof(e.target->getText());
    }else if(e.target->getY() == gui->getTextInput("Mean", "Rhythm")->getY()){
        ofxDatGuiMatrix* matrix = gui->getMatrix("Rhythm");
        videoLibrary[currentVideoDetailIndex].metadata.rhythm[matrix->getSelected().front()].frame = stof(e.target->getText());
    }else if(e.target->getY() == gui->getTextInput("Std. Dev.", "Rhythm")->getY()){
        ofxDatGuiMatrix* matrix = gui->getMatrix("Rhythm");
        videoLibrary[currentVideoDetailIndex].metadata.rhythm[matrix->getSelected().front()].rhythm = stof(e.target->getText());
    }else if(e.target->getLabel() == gui->getTextInput("Object", "Object")->getLabel()){
        videoLibrary[currentVideoDetailIndex].metadata.object = stof(e.target->getText());
    }else if(e.target->getLabel() == gui->getTextInput("File Path", "Object")->getLabel()){
        imgObject = e.target->getText();
    }

//    writeMetadata(videoLibrary[currentVideoDetailIndex].name, videoLibrary[currentVideoDetailIndex].metadata);
}

//--------------------------------------------------------------
void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e)
{
    cout << "the button at index " << e.child << " changed state to: " << e.enabled << endl;
    
    ofxDatGuiTextInput* input;
    metadata videoMetadata = videoLibrary[currentVideoDetailIndex].metadata;
    if(e.enabled){
        if(e.target->getLabel() == "Texture"){
            input = gui->getTextInput("Mean", "Texture");
            input->setText(ofToString(videoMetadata.texture[e.child].mean));
            
            input = gui->getTextInput("Std. Dev.", "Texture");
            input->setText(ofToString(videoMetadata.texture[e.child].stdDev));
        } else if(e.target->getLabel() == "Rhythm"){
            input = gui->getTextInput("Frame", "Rhythm");
            input->setText(ofToString(videoMetadata.rhythm[e.child].frame));
            
            input = gui->getTextInput("Rhythm", "Rhythm");
            input->setText(ofToString(videoMetadata.rhythm[e.child].rhythm));
        }
    }
}

//--------------------------------------------------------------
void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
    cout << e.target->getLabel() << " was clicked!"  << endl;
    if(e.target->getLabel() == userButton->getLabel()){
        changeAppWindow(USER_WINDOW);
        userButton->setEnabled(false);
        operatorButton->setEnabled(false);
    }else if(e.target->getLabel() == operatorButton->getLabel()){
        changeAppWindow(VIDEO_LIBRARY_WINDOW);
        userButton->setEnabled(false);
        operatorButton->setEnabled(false);
    }
    else if(e.target->getLabel() == playlistButton->getLabel()){
        changeAppWindow(PLAYLIST_LIBRARY_WINDOW);
    }else if(e.target->getLabel() == videoLibraryButton->getLabel()){
        changeAppWindow(VIDEO_LIBRARY_WINDOW);
    }else if(e.target->getLabel() == createPlaylistButton->getLabel()){
        createPlaylistGui->setEnabled(createPlaylistEnabled = !createPlaylistEnabled);
        ofLogVerbose("[DEBUG] createPlaylistEnabled > ") << createPlaylistEnabled;
    }else if(e.target->getLabel() == createPlaylistGui->getButton("Confirm")->getLabel()
             && currentWindow == PLAYLIST_LIBRARY_WINDOW){
        playlist playlist;
        playlist.name = createPlaylistGui->getTextInput("Playlist Name")->getText();
        
        playlist.videoNames.push_back(createPlaylistGui->getDropdown("Add Video")->getSelected()->getLabel());
        playlist.faces = stof(createPlaylistGui->getTextInput("Faces")->getText());
        
        for(int i = 0; i < playlist.videoNames.size(); i++){
            ofLogVerbose("[DEBUG] > ") << playlist.videoNames[i];
        }
        playlists.push_back(playlist);
        ofLogVerbose("[DEBUG] Last playlist > ") << playlists.back().name;
        createPlaylistGui->setEnabled(createPlaylistEnabled = !createPlaylistEnabled);
        
        // Reconstruct component to update with new playlists
        vector<string> playlistOptions;
        for(int i = 0; i < playlists.size(); i++){
            playlistOptions.push_back(playlists[i].name);
        }
        playlistOptions.push_back("None");
        
        addToPlaylistDropdown = new ofxDatGuiDropdown("Add To Playlist", playlistOptions);
        addToPlaylistDropdown->setStripeColor(ofColor(238, 57, 135));
        
        addToPlaylistDropdown->onDropdownEvent(this, &ofApp::onDropdownEvent);
        
        for(int i = 0; i < playlists.size(); i++){
            writePlaylist(playlists[i]);
        }
    }else if(e.target->getLabel() == gui->getButton("Retrieve Metadata")->getLabel()){
        metadata videoMetadata = videoAnalysis(&videoLibrary[currentVideoDetailIndex]);
        writeMetadata(videoLibrary[currentVideoDetailIndex].name, videoMetadata);
    }
}

//--------------------------------------------------------------
void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
    if(e.target->getName() == addToPlaylistDropdown->getName()){
        for(int i = 0; i < playlists.size(); i++){
            for(int j = 0; j < playlists[i].videoNames.size(); j++){
                // Search for video to remove or move if it's already assigned to a playlist
                if(playlists[i].videoNames[j] == videoLibrary[currentVideoDetailIndex].name){
                        // Remove from playlist
                        playlists[i].videoNames.erase(playlists[i].videoNames.begin() + j);
                        // Erase playlist if it has no videos
                        if(playlists[i].videoNames.size() == 0){
                            ofLogVerbose("[DEBUG] Delete file > ") << PLAYLISTS_PATH+playlists[i].name+".xml";
                            ofFile::removeFile(PLAYLISTS_PATH+playlists[i].name+".xml");
                            playlists.erase(playlists.begin() + i);
                            
                            // Reconstruct component to update with new playlists
                            vector<string> playlistOptions;
                            for(int i = 0; i < playlists.size(); i++){
                                playlistOptions.push_back(playlists[i].name);
                            }
                            playlistOptions.push_back("None");
                            
                            addToPlaylistDropdown = new ofxDatGuiDropdown("Add To Playlist", playlistOptions);
                            addToPlaylistDropdown->setStripeColor(ofColor(238, 57, 135));
                            
                            addToPlaylistDropdown->onDropdownEvent(this, &ofApp::onDropdownEvent);
                        }
                }
            }
            // Add to playlist and playlist has space
            if(playlists[i].name == e.target->getLabel() && playlists[i].videoNames.size() < row){
                playlists[i].videoNames.push_back(videoLibrary[currentVideoDetailIndex].name);
            }
        }
        for(int i = 0; i < playlists.size(); i++){
            writePlaylist(playlists[i]);
        }
    }
    for(int i = 0; i < playlists.size(); i++){
        ofLogVerbose("[DEBUG] Playlist > ") << playlists[i].name;
        for(int j = 0; j < playlists[i].videoNames.size(); j++){
            ofLogVerbose("[DEBUG]   Videos > ") << playlists[i].videoNames[j];
        }
    }
}

//--------------------------------------------------------------
string ofApp::getVideoName(video video)
{
    string videoPath = video.player.getMoviePath();
    int start = videoPath.find_first_of("/") + 1;
    string videoName = videoPath.substr(start);
    int finish = videoName.find_last_of(".");
    videoName = videoName.substr(0, finish);
    return videoName;
}
