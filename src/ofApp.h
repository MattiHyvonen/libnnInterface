#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#include <memory>

#include "libnnet.h"

std::vector<float> listToNumbers(std::string list);

class ofApp : public ofBaseApp{

    const char* SEND_IP = "127.0.0.1";
    
    const int SEND_PORT = 12345;
    const int RECEIVE_PORT = 12346;
    const int MAX_PACKET_SIZE = 100000;
    
    unsigned int N_outputs = 1; //number of outputs
    unsigned int N_hiddenLayers = 1; //number of hidden layers
    unsigned int N_inputs = 2; //number of inputs
    unsigned int hiddenLayerSize = 4; //number of neurons in hidden layers
    
    std::vector<LayerLinkIndexes> links;
    
    NNet NN;

    bool learn = false;
    
    ofxUDPManager udpSender;
    ofxUDPManager udpReceiver;

    std::vector<std::shared_ptr<float> > inputs;
    std::vector<std::shared_ptr<float> > outputs;
    std::vector<float> desiredOutputs;
    
    ofColor drawColor = ofColor(80, 25, 0);
    
public:
    
    void setInputValues(std::vector<float> values);
    void setOutputValues(std::vector<float> values);
    
    void receive();
    void send();
    
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
};
