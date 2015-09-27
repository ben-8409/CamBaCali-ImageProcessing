/**
 * Creation of masks for calibration tool measurements
 */
#include <string>

// POCO
#include <Poco/Exception.h>
#include <Poco/Path.h>

// Local thirdparty includes
#include "tclap/CmdLine.h"
#include "tclap/ValueArg.h"
// Local original includes
#include "serveraccess.hpp"
#include "utils.hpp"

using namespace std;

// Program properties //TODO: replace with macro
const string version = "0.1.0";
// arguments
string server;
// global variables
std::vector<string> screenIds;
Poco::Path path;

int main(int argc, char const *argv[]) {
  //Utility
  Utils utils;

  //Command line arguments
  try {
    TCLAP::CmdLine cmd("clmasks processes a set of pictures to the physical displays on a picture for later use.", ' ' , version);

    // required
    TCLAP::ValueArg<string> serverArg("a", "server_address", "HTTP Address and port of the control server server. Example: http://kallisto:3000", true, "", "SERVERADDRESS");
    cmd.add(serverArg);
    TCLAP::ValueArg<std::string> pathArg("p", "path", "Path to input images (masks will be saved in a subdirectory). Uses current dir if not set.", false, Poco::Path::current(), "PATH");
    cmd.add(pathArg);
    //Parse!
    cmd.parse(argc, argv);

    //Load arguments
    server = serverArg.getValue();
    path = Poco::Path(pathArg.getValue());
    if(!path.isDirectory()) path.makeDirectory();

  } catch (TCLAP::ArgException &e) {
    std::cout << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return 1;
  }

  // Verify user supplied arguments
  // Check if Path exists and is accessible
  if(!utils.existsAndExecutable(path)) {
    cout << "Working directory dir does not exist or is not accessible." << endl;
    return 1;
  }
  /* code */

  // Get list of screen names
  try {
      //setup server access
      ServerAccess serverAccess(server);
      //get screen list
      screenIds = serverAccess.getScreenIds();
  } catch (Poco::Exception &e) {
      std::cout << "Error while connecting to server: " << e.what() << std::endl;
      return 1;
  }

  return 0;
}
