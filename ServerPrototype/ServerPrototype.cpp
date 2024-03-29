/////////////////////////////////////////////////////////////////////////
// ServerPrototype.cpp - Console App that processes incoming messages  //
// ver 2.0															   //
// Author: Ashwini Prabhu, Syracuse University                         //
//         Spring 2019, adprabhu@syr.edu							   //
/////////////////////////////////////////////////////////////////////////

#include "ServerPrototype.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <chrono>
#include <cString>

namespace MsgPassComm = MsgPassingCommunication;

using namespace Repository;
using namespace FileSystem;
using Msg = MsgPassingCommunication::Message;

Files Server::getFiles(const Repository::SearchPath& path)
{
  return Directory::getFiles(path);
}

std::vector<std::string> Server::strToStdVectrStr(std::string str) {
	std::vector<std::string> tempStr = Utilities::split(str, ' ');
	return tempStr;
}

std::vector<std::string> Server::publishFilesRun(Msg msg)
{
	std::string wDirectory = msg.value("wDirectory")+"\\";
	std::string Pattern = msg.value("Patterns");
	std::string Regex = msg.value("Regex");
	std::string hasOption = msg.value("hasOption");
	std::vector<std::string> pattern = strToStdVectrStr(Pattern);
	std::vector<std::string> regex = strToStdVectrStr(Regex);
	bool HasOption = true;
	if (hasOption.compare("True") == 0)
		HasOption = true;
	else
		HasOption = false;

  objectFactory factory;
  IExecutive *obj = factory.createExecutive();  
  obj->setCmdArgs(wDirectory, pattern, regex, HasOption);
  std::vector<std::string> convertedFiles = obj->getConvertedFileList();
  return convertedFiles;
}

std::function<Msg(Msg)> publishFiles = [](Msg msg)
{
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("publishFiles");

  std::string args = msg.value("args");
  std::vector<std::string> convertedFiles = Server::publishFilesRun(msg);
  size_t count = 0;
  std::cout << "Files converted through server \n";
  
  if (args != "")
  {
	  reply.attribute("args",args);
	  for (auto file : convertedFiles) {
		  std::string countStr = Utilities::Converter<size_t>::toString(++count);
		  reply.attribute("convFile" + countStr, file);
	  }
  }
  else
  {
    std::cout << "\n  Null Argument passed.";
  }
  
  return reply;
};

std::function<Msg(Msg)> downloadFile = [](Msg msg)
{
	Msg reply;
	reply.to(msg.from());
	reply.from(msg.to());
	
	std::string args = msg.value("downloadFile");
	std::string fileToDownload = msg.value("name");
	std::cout << "FilePath to download: " << fileToDownload;

	reply.command("file");

	if (fileToDownload != "")
	{
		reply.attribute("file", fileToDownload);
		reply.file(fileToDownload);
	}
	else
	{
		std::cout << "\n  Null file name passed.";
	}

	return reply;
};

Dirs Server::getDirs(const Repository::SearchPath& path)
{
  return Directory::getDirectories(path);
}

template<typename T>
void show(const T& t, const std::string& msg)
{
  std::cout << "\n  " << msg.c_str();
  for (auto item : t)
  {
    std::cout << "\n    " << item.c_str();
  }
}

std::function<Msg(Msg)> echo = [](Msg msg) {
  Msg reply = msg;
  reply.to(msg.from());
  reply.from(msg.to());
  return reply;
};

std::function<Msg(Msg)> getFiles = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getFiles");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files files = Server::getFiles(searchPath);
    size_t count = 0;
    for (auto item : files)
    {
      std::string countStr = Utilities::Converter<size_t>::toString(++count);
      reply.attribute("file" + countStr, item);
    }
  }
  else
  {
    std::cout << "\n  getFiles message did not define a path attribute";
  }
  return reply;
};

std::function<Msg(Msg)> getDirs = [](Msg msg) {
  Msg reply;
  reply.to(msg.from());
  reply.from(msg.to());
  reply.command("getDirs");
  std::string path = msg.value("path");
  if (path != "")
  {
    std::string searchPath = storageRoot;
    if (path != ".")
      searchPath = searchPath + "\\" + path;
    Files dirs = Server::getDirs(searchPath);
    size_t count = 0;
    for (auto item : dirs)
    {
      if (item != ".." && item != ".")
      {
        std::string countStr = Utilities::Converter<size_t>::toString(++count);
        reply.attribute("dir" + countStr, item);
      }
    }
  }
  else
  {
    std::cout << "\n  getDirs message did not define a path attribute";
  }
  return reply;
};

int main(int argc, char *argv[])
{
  std::cout << "\n  Testing Server Prototype";
  std::cout << "\n ==========================\n";
  if (argc > 1) {
	  std::cout << "Command line argument : -- serverEndPoint Port :" << argv[1] << std::endl;
	  serverEndPoint.port = Utilities::Converter<size_t>::toValue(argv[1]);
  }
  Server server(serverEndPoint, "ServerPrototype");
  server.start();
  std::cout << "\n  testing getFiles and getDirs methods";
  std::cout << "\n --------------------------------------";
  Files files = server.getFiles();
  show(files, "Files:");
  Dirs dirs = server.getDirs();
  show(dirs, "Dirs:");
  std::cout << "\n  testing message processing";
  std::cout << "\n ----------------------------";
  server.addMsgProc("echo", echo);
  server.addMsgProc("getFiles", getFiles);
  server.addMsgProc("getDirs", getDirs);
  server.addMsgProc("serverQuit", echo);
  server.addMsgProc("publishFiles",publishFiles);
  server.addMsgProc("downloadFile", downloadFile);
  server.processMessages();  
  Msg msg(serverEndPoint, serverEndPoint); 
  msg.name("msgToSelf");
  msg.command("echo");
  msg.attribute("verbose", "show me");
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  msg.command("getFiles");
  msg.remove("verbose");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  msg.command("getDirs");
  msg.attributes()["path"] = storageRoot;
  server.postMessage(msg);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
 
  std::cout << "\n  press enter to exit \n";
  std::cin.get();
  msg.command("serverQuit");
  server.postMessage(msg);
  server.stop();
  return 0;
}

