#include "ofApp.h"

#include <sstream>

std::vector<float> listToNumbers(std::string list) {
    stringstream ss;
    ss.str(list);
    std::vector<float> numbers;
    for(float f = 0; ss >> f; numbers.push_back(f));
    return numbers;
}


void printNumbers(std::vector<float> list) {
    for(unsigned int i=0; i<list.size(); i++) {
        std::cout << list[i] << " ";
    } 
}

net::net(unsigned int n_ins, unsigned int n_outs) {
    for(unsigned int i=0; i<n_ins; i++)
        inputs.push_back(std::make_shared<float>(0.5) );
    
    for(unsigned int i=0; i<n_outs; i++)
        outputs.push_back(std::make_shared<float>(0) );
}


void net::forward() {
    unsigned int N = inputs.size();
    if(outputs.size() < N) N = outputs.size();
    
    for(unsigned int i=0; i<N; i++)
        *(outputs[i]) = *(inputs[i]);
}


std::vector<std::vector<int> > net::getLinks() {
    std::vector<std::vector<int > > result(outputs.size() );
    std::vector<int> kaikkiInputit(inputs.size() );
    for(unsigned int i=0; i<kaikkiInputit.size(); i++) 
        kaikkiInputit[i]=i;
    for(unsigned int i=0; i < result.size(); i++)
        result[i] = kaikkiInputit;
    return result;
}


std::vector<std::vector<float> > net::getWeights() {
    std::vector<std::vector<float> > result(outputs.size() );
    std::vector<float> kaikkiPainot(inputs.size() );
        
    for(int i=0; i < result.size(); i++) {
        for(int j=0; j<kaikkiPainot.size(); j++) 
            kaikkiPainot[j]= ((float)j/kaikkiPainot.size());
        
        result[i] = kaikkiPainot;
    }
    return result;
}


void ofApp::setInputValues(std::vector<float> values) {
    int N = inputs.size();
    if(values.size() < inputs.size() ) N = values.size();
    for(unsigned int i=0; i<N; i++)
        *inputs[i] = values[i];
}

void ofApp::setOutputValues(std::vector<float> values) {
    int N = outputs.size();
    if(values.size() < outputs.size() ) N = values.size();
    for(unsigned int i=0; i<N; i++)
        *outputs[i] = values[i];
}

void ofApp::receive() {
    //hakee paketin ja muuttaa inputs -pointtereiden arvoja

    //paketti voi olla:
    //1) inputs <f> <f> <f> <f> ...
    //2) learning <1|0>
    //3) desiredOutputs <f> <f> <f> <f> ...
    
    //vastaanotetaan paketti. Jos tyhjä, palataan
    char udpData[MAX_PACKET_SIZE];
    std::string packet_str;
    for(; udpReceiver.Receive(udpData, MAX_PACKET_SIZE); packet_str = udpData) {
        std::cout << "Tuli paketti: \"" << packet_str << "\"\n";
        if(packet_str == "") continue;

        //luetaan eka sana ja verrataan käskyihin
        stringstream ss;
        std::string word;
        ss.str(packet_str);
        ss >> word;
        
        if(word.empty() ) {
            std::cout << "Huono paketti: " << packet_str << "\n";
            continue;
        }

        if(word == "inputs") {
            std::vector<float> numbers;
            for(float f = 0; ss >> f; numbers.push_back(f));
            std::cout << "Asetetaan inputit: ";
            printNumbers(numbers);
            std::cout << "\n";
            if(numbers.empty() ) continue;
            setInputValues(numbers);
        }

        else if (word == "desiredOutputs") {
            std::vector<float> numbers;
            for(float f = 0; ss >> f; numbers.push_back(f));
            std::cout << "Asetetaan desiredOutputit: ";
            printNumbers(numbers);
            std::cout << "\n";
            if(numbers.empty() ) continue;
            desiredOutputs = numbers;
            learn = true;
        }

        else {
            std::cout << "Huono käsky: " << word << "\n";
            continue;
        }
    }
}


void ofApp::send() {
    //lähetetään luvut asciina
    stringstream ss;
    for(unsigned int i=0; i<outputs.size(); i++)  {
        ss << *outputs[i];
        if(i < outputs.size()-1)
            ss << " ";
        else
            ss << ";\n";
    }
    //std::cout << "Lähetetään: " << ss.str().c_str() << "\n";
    udpSender.Send(ss.str().c_str(), ss.str().length() );
}

//--------------------------------------------------------------
void ofApp::setup(){

    /* luodaan neural net */

    int outs = 1; //number of outputs
    int hids = 1; //number of hidden layers
    int ins = 2; //number of inputs
    
    NN = NNet(outs, hids);
    
    inputs.resize(ins);
    for(int i=0; i<ins; i++)
        inputs[i] = std::make_shared<float>(0);
    
    outputs = NN.getOutputSignals();
    
    NN.setLearningcurve(0.98, 0.1, 0.5);

    NN.linkInput(inputs);
    NN.linkHidden(1, 3); //args: layer_i, number of neurons to create
    
    NN.linkOutput();
    
    //NN.getStats(); //kertoo juttuja
    
    /* luodaan UDP-yhteydet */

    udpSender.Create();
    udpSender.Connect(SEND_IP, SEND_PORT);
    udpSender.SetNonBlocking(true);
    
    udpReceiver.Create();
    udpReceiver.Bind(RECEIVE_PORT);
    udpReceiver.SetNonBlocking(true);
    
    /* piirron asetuksia */
    
    ofSetBackgroundColor(ofColor::white);
    ofSetColor(100, 40, 0);
    
    ofSetFrameRate(20);
}

//--------------------------------------------------------------
void ofApp::update(){    

    receive();
    
    if(learn) {
        NN.back(desiredOutputs);
        learn = false;
    }
    
    NN.forward();
    
    send();
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    float margin = 60;
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

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

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}