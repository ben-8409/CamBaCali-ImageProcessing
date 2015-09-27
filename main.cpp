#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <array>
#include <random>
#include <iterator>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Exception.h>

#include "colordetector.hpp"
#include "screenregistration.hpp"
#include "serveraccess.hpp"
#include "jsongenerator.hpp"
#include "tclap/CmdLine.h"
#include "tclap/ValueArg.h"
#include "tclap/SwitchArg.h"

using namespace std;

//Program settings and variables
bool debug = false;
bool loadMasks, saveMasks, verbose;
int lap;
string sample, server, uuid;
Poco::Path path;
Poco::Path maskPath;
const string redscreen = "redscreen";
const string example = "screen_y1x1";
string ext = "ppm";
string screensFile;
vector <string> screenIds;
string serverName;
float maxDist;
//for random number generation
random_device rd;
mt19937 rng(rd());
uniform_int_distribution<int> uni(0,35);
ScreenRegistration *screenReg;

cv::Mat readImage(const string &filename) {
  cv::Mat input = cv::imread(filename);
  cv::resize(input, input, cv::Size(), 0.25, 0.25, CV_INTER_AREA);
  return input;
}

string randomImageId()
{
    int random_integer = uni(rng);
    return screenIds.at(random_integer);
}

string image_filename(const string &basename)
{
    Poco::Path image_path(path);
    image_path.setFileName(basename);
    image_path.setExtension(ext);
    cout << "Load " << image_path.toString() << endl << flush;
    if(!Poco::File(image_path).canRead())
        cerr << "Cannot read " << image_path.toString() << endl << flush;
    return image_path.toString();
}
string mask_filename(const string &basename)
{
    Poco::Path mask_path(maskPath);
    mask_path.setFileName(basename);
    mask_path.setExtension("ppm");
    return mask_path.toString();
}

void previewImage(const string &name, const cv::Mat image)
{
    cv::namedWindow(name);
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(600,400));
    cv::imshow(name, resized);
    cv::waitKey(5000);
}
std::vector<std::string> screensListFromString(const string &screensList) {
    std::vector<std::string> list;
    if(screensList.size()>1) {
        std::stringstream sstr(screensList);
        while(sstr.good()) { //while no error or EOF
            std::string screen;
            std::getline(sstr, screen, ',');
            //TODO mre efficient version required
            screen.erase(std::find_if(screen.rbegin(), screen.rend(), std::bind1st(std::not_equal_to<char>(), ' ')).base(), screen.end());
            screen.erase(screen.begin(), std::find_if(screen.begin(), screen.end(), std::bind1st(std::not_equal_to<char>(), ' ')));
            list.push_back(screen);
        }
    }
    return list;
}

int main(int argc, char** argv)
{
    // getting cmd line parameters
    try {
        TCLAP::CmdLine cmd("Calib-Tool processes pictures recorded with gphoto to detect the tiled displays screens and calculate individual screens mean color.", ' ', "0.4.0");

        //Value arguments
        TCLAP::ValueArg<std::string> serverArg("a", "server_address", "Adress of the Calib-Control server to connect do", false, "", "Server address, Like: http://ip:port");
        TCLAP::ValueArg<std::string> screensFileArg("c", "screens_config", "Filename to read the names of screens from for debugging", false, "", "filename");
        cmd.xorAdd( serverArg, screensFileArg );
        TCLAP::ValueArg<std::string> pathArg("p", "path", "Path to load files from", false, Poco::Path::current(), "Local path");
        cmd.add(pathArg);
        TCLAP::ValueArg<std::string> extArg("e", "ext", "Filename extenstion", false, "ppm", ".ppm or .jpg");
        cmd.add(extArg);
        TCLAP::ValueArg<std::string> sampleArg("i", "input_sample", "filename of the sample to load", false, "", "Local path to sample file");
        cmd.add(sampleArg);
        TCLAP::ValueArg<std::string> uuidArg("u", "uuid", "UUID of the current calibration run", false, "", "UUID, if left empty, server assign one");
        cmd.add(uuidArg);
        TCLAP::ValueArg<int> lapArg("r", "lap", "Lap", false, 1, "If not supplied, default 1 is used");
        cmd.add(lapArg);
        TCLAP::ValueArg<float> distanceArg("b", "distance", "Max color distance", false, 50.0f, "If not supplied, default 50.0f is used");
        cmd.add(distanceArg);
        TCLAP::SwitchArg saveMasksSwitch("s", "save_masks", "Save masks locally for later use, uses input path ./masks");
        TCLAP::SwitchArg loadMasksSwitch("l", "load_masks", "Load masks from cache");
        cmd.xorAdd( saveMasksSwitch, loadMasksSwitch );
        TCLAP::SwitchArg verboseSwitch("v", "verbose", "Be verbose");
        cmd.add(verboseSwitch);
        TCLAP::SwitchArg debugSwitch("d", "debug", "Be verbose");
        cmd.add(debugSwitch);


        //Parse!
        cmd.parse(argc, argv);

        //Get values
        maxDist = distanceArg.getValue();
        loadMasks = loadMasksSwitch.getValue();
        saveMasks = saveMasksSwitch.getValue();
        verbose = verboseSwitch.getValue();
        sample = sampleArg.getValue();
        screensFile = screensFileArg.getValue();
        server = serverArg.getValue();
        ext = extArg.getValue();
        lap = lapArg.getValue();
        path = Poco::Path(pathArg.getValue());
        uuid = uuidArg.getValue();

        debug = debugSwitch.getValue();
    } catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cout << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 1;
    }
    try {
        //get screen ids from file if not supplied
        if(screenIds.size() < 1 && screensFile!="") {
          std::ifstream is(screensFile);
          std::istream_iterator<string> start(is), end;
          std::vector<string> screens(start, end);
          screenIds = screens;
        }
    } catch (ifstream::failure &e) {
        std::cout << "Error while reading config: " << e.what() << std::endl;
        return 1;
    }

    try {
        //get screen ids from sserver only if not supplied
        if(screenIds.size() < 1 && server!="") {
            //setup server access
            ServerAccess serverAccess(server);
            //get screen list
            screenIds = serverAccess.getScreenIds();
        }
    } catch (Poco::Exception &e) {
        std::cout << "Error while connecting to server: " << e.what() << std::endl;
        return 1;
    }

    if(screenIds.size() < 1) {
        cout << "No screen names supplied, no server access or screens name could not be parsed!" << endl;
        return 1;
    }
    if(verbose || debug) {
        cout << "Screens: ";
        for( auto& screen : screenIds)
        {
             cout << screen << ", ";
        }
        cout << endl;
    }

    //Check input and Output dirs
    if(!path.isDirectory()) path.makeDirectory();
    Poco::File pathF(path);
    if(!pathF.exists() || !pathF.canExecute()) {
        cout << "Input dir does not exist or is not accessible" << endl;
        return 1;
    }
    maskPath = Poco::Path(path, Poco::Path("masks/"));
    Poco::File maskPathF(maskPath);
    if(saveMasks) {
        if(maskPathF.exists())
        {
            if(!maskPathF.canWrite()) {
                cerr << "Mask output dir exists and i cannot write in it" << endl;
                return 1;
            }
        } else if(!maskPathF.createDirectory()){
            cerr << "Mask output dir could not be created" << endl;
        } else {
            cout << "Created mask output dir." << endl;
        }
    }
    if(loadMasks) {
        if(!maskPathF.exists() || !maskPathF.canRead()) {
            cerr << "Cannot access mask loading dir " << maskPath.toString() << endl;
            return 1;
        }
    }



    if(loadMasks && saveMasks)
    {
        std::cout << "Choose either saving or loadings screen masks." << std::endl << std::flush;
        return 1;
    }

    if(loadMasks) {
        screenReg = new ScreenRegistration();
        for (auto& id: screenIds)
        {
            //do not resize the mask!
            cv::Mat mask = cv::imread(mask_filename(id));
            cv::cvtColor(mask, mask, CV_BGR2GRAY);
            if(mask.type() != CV_8U)
            {
                cerr << "Error loading mask type " << mask.type();
                return 1;
            }
            screenReg->setMask(id, mask);
        }

    } else {
        //calculate masks now
        cv::Mat image;
        cv::Mat red;
        cout << "Read redscreen" << endl;
        image = readImage(image_filename(redscreen));
        previewImage("input", image);


        //Find red areas
        ColorDetector colordetector;
        cout << "Set target color" << endl;
        colordetector.setTargetColor(255,0,0);
        cout << "Create mask" << endl;
        red = colordetector.binaryMask(image);

        //Erode and Dilate
        cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(10,10));
        cv::Mat mask;
        cv::morphologyEx(red, mask, cv::MORPH_OPEN, structuringElement);

        previewImage("redscreen mask", mask);

        //now add one screen image
        screenReg = new ScreenRegistration(mask, maxDist);

        //add screens loop
        cv::Mat allScreens;
        allScreens.create(mask.rows, mask.cols, CV_8UC3);
        for (auto& id: screenIds)
        {
          cv::Mat input = readImage(image_filename("screen_" + id));
          screenReg->addScreen(id, input);
          cv::Mat cur_mask = screenReg->getMask(id);
          //copy to all screens mask
          input.copyTo(allScreens, cur_mask);
            if(saveMasks) {
                cout << "Save mask for " << id << " to " << mask_filename(id) << std::endl << std::flush;
                cv::imwrite(mask_filename(id), cur_mask);
            }
        }
        previewImage("allscreens", allScreens);
        cv::imwrite("allscreens.ppm", allScreens);

        //notify server of results
        if(server!="" && uuid !=""){
            JsonGenerator jsonGen;
            std::string payload = jsonGen.masksReadyMessage(uuid);
            ServerAccess serverAccess(server);
            serverAccess.masksReady(uuid, payload);
        }
    }

    if(sample!="") {
        //Load one Sample
        cout << "Load sample " << image_filename(sample) << std::endl << std::flush;
        cv::Mat sampleImage = readImage(image_filename(sample));

        //if debug
        if(debug) {
          cv::Mat preview = screenReg->applyMasks(sampleImage);
          cv::imwrite("preview.ppm", preview);
          previewImage("masks applied to sample", preview);
        }

        std::map<std::string, cv::Scalar> colors = screenReg->calcMeanColors(sampleImage);


        cv::Scalar color;
        for (auto& id: screenIds)
        {
            color = colors[id];
            if(verbose) {
                std::cout << "Color for " << id << ": R " << color[2] << " G " << color[1] << " B " << color[0] << std::endl;
            }
        }

        if(debug) {
          cv::Mat previewMean = screenReg->previewMeanColors(sampleImage);
          cv::imwrite("previewMean.ppm", previewMean);
          previewImage("masks applied to sample", previewMean);
        }

        JsonGenerator jsonGen;
        std::string results = jsonGen.generateResults(colors, path.toString(), uuid, lap);
        if(verbose || server!="") {
            std::cout << results << endl;
        }
        if(server!=""){
            ServerAccess serverAccess(server);
            serverAccess.postResults(uuid, results);
        }
    }

    return 0;
 }
