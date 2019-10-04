#pragma once

#include "ofMain.h"
#include <math.h>
#include "ofxXmlSettings.h"
#include "ofxCv.h"
#include "ofxCvHaarFinder.h"
#include "ofxDatGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void audioIn(ofSoundBuffer & input);
    
        const string VIDEOS_PATH = "videos/";
        const string METADATA_PATH = "metadata/";
        const string PLAYLISTS_PATH = "playlists/";
        const string IMAGES_PATH = "images/";
        const string HAAR_FINDER_PATH = "haar_settings/";
        string imgObject = "apple.png";
    
        const int APP_BORDER = 50;
    
        // Enum to define the different app windows
        enum AppWindow {
            START_WINDOW,
            VIDEO_LIBRARY_WINDOW,
            VIDEO_DETAIL_WINDOW,
            PLAYLIST_LIBRARY_WINDOW,
            USER_WINDOW
        };
        AppWindow currentWindow = START_WINDOW;
        AppWindow previousWindow = currentWindow;
    
        // Struct to group mean and stddev together
        struct meanStdDev {
            float mean;
            float stdDev;
        };
    
        // Struct to group the rhythm and its corresponding frame
        struct rhythm {
            float rhythm;
            int frame;
        };
    
        // Used to sort vector of rhythm (descending)
        struct more_than_key
        {
            inline bool operator() (const rhythm& struct1, const rhythm& struct2)
            {
                return (struct1.rhythm > struct2.rhythm);
            }
        };
    
        // Struct to save a video metadata
        struct metadata {
            vector<string> tags;
            struct rgb {
                int r;
                int g;
                int b;
            } colour;
            int luminance;
            float faces;
            struct edges {
                meanStdDev horizontal;
                meanStdDev vertical;
                meanStdDev degree45;
                meanStdDev degree135;
                meanStdDev noDirection;
            } edges;
            vector<meanStdDev> texture;
            vector<rhythm> rhythm;
            float object;
        };
    
        struct video {
            int x;
            int y;
            int width;
            int height;
            ofVideoPlayer player;
            string name;
            metadata metadata;
        };
    
        struct playlist {
            string name;
            vector<string> videoNames;
            // Conditions, only number of faces for now
            float faces;
        };
        vector<playlist> playlists;
    
        string haarFaceXMLFile = "haarcascade_frontalface_default.xml";
        string haarMouthXMLFile = "Mouth.xml";
        string haarEyesXMLFile = "parojosG.xml";
    
        metadata videoAnalysis(video* video);
        int getNumberOfFaces(ofPixels pix);
        meanStdDev getEdges(ofPixels pix, cv::Mat edgesKernel, int threshold = 200);
        meanStdDev getTexture(ofPixels pix, cv::Mat gaborKernel, int threshold = 200);
        int getRhythm(ofPixels pix, int threshold = 200);
        float getObject(ofPixels pix, string objectFileName);
    
        int drawFaceDetection(ofPixels pix);
    
        void writeMetadata(string videoname, metadata videoMetadata);
        metadata readMetadata(string videoname);
        void writePlaylist(playlist playlist);
        playlist readPlaylist(string playlistName);
    
        ofDirectory dir;
        ofDirectory playlistsDir;
    
        vector<video> videoLibrary;
        ofVideoPlayer defaultVideo;
        // Index of the selected video
        int currentVideoDetailIndex;
    
        map<string, int> videoIndexMap;
    
        // VIDEO LIBRARY dimensions
        // Number of videos per row and column
        int row;
        int column;
        // The library width and height
        int vidLibraryWidth;
        int vidLibraryHeight;
        // The space between the videos
        int vidSpace;
        // The videos width and height
        int vidWidth;
        int vidHeight;
    
        // VIDEO DETAIL dimensions
        // Video Panel
        int videoPanelHeight;
        int videoPanelWidth;
    
        // Face Detection
        ofxCvHaarFinder faceFinder;
        ofxCvHaarFinder mouthFinder;
        ofxCvHaarFinder eyesFinder;
    
        ofxDatGui* gui;
        ofxDatGuiButton* playlistButton;
        ofxDatGuiButton* videoLibraryButton;
        ofxDatGuiButton* createPlaylistButton;
        ofxDatGui* createPlaylistGui;
        bool createPlaylistEnabled;
        ofxDatGuiDropdown* addToPlaylistDropdown;
        ofxDatGuiLabel* chooseUserLabel;
        ofxDatGuiButton* userButton;
        ofxDatGuiButton* operatorButton;
    
        ofTrueTypeFont    verdana30;
        ofTrueTypeFont    verdana15;
    
        void changeAppWindow(AppWindow window);
        void drawStartWindow();
        void drawVideoLibrary();
        void drawVideoDetail();
        void drawPlaylistLibrary();
        void drawUserVideo();
        void drawCamera();
        bool isInsideArea(int x, int y, int areaX, int areaY, int areaWidth, int areaHeight);
    
        void onSliderEvent(ofxDatGuiSliderEvent e);
        void onTextInputEvent(ofxDatGuiTextInputEvent e);
        void onMatrixEvent(ofxDatGuiMatrixEvent e);
        void onButtonEvent(ofxDatGuiButtonEvent e);
        void onDropdownEvent(ofxDatGuiDropdownEvent e);
    
        string getVideoName(video video);
        meanStdDev getMeanStd();
    
        // USER PART
        // Camera
        ofVideoGrabber vidGrabber;
        int camWidth;
        int camHeight;
    
        int camFaces;
    
        // Context audio
        ofSoundStream soundStream;
        ofSoundStreamSettings settings;
        float soundReceived;
        float lockInterval;
        bool audioDetected;
    
        int currentPlaylistIndex;
        int currentPlayingVideoIndex;
        int currentPlayingVideoLibraryIndex;
        int getCurrentPlayingPlaylist();
};
