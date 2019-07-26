/////////////////////////////////////////////////////////////////////
// MainWindow.xaml.cs - Prototype for OOD Project #4              //
// ver 2.0                                                         //
// Author: Ashwini Prabhu, Syracuse University                     //
//         Spring 2019, adprabhu@syr.edu                           //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines one class MainWindow that provides 2 tabs:
 * Navigate - To navigate to the required file using regex patterns and convert it to HTML
 * Display -  Displays the file list which are converted to HTML format and displays in webpage
 * or pop-up window based on selected choice
 * 
 * Required Files:
 * ---------------
 * MainWindow.xaml, MainWindow.xaml.cs
 * CodePublisherPopUp.xaml, CodePublisherPopUp.xaml.cs
 * 
 * Maintenance History:
 * --------------------
 * ver 2.0 : 30th April 2019
 * - second release
 * ver 1.0 : 12 April 2019
 * - first release
 * 
 */
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
//using System.Windows.Shapes;
using System.Threading;
using MsgPassingCommunication;
using System.IO;
using System.Collections;

namespace CodePublisher
{
  public partial class MainWindow : Window
  {
    private string wDirectory;
    private string Patterns;
    private string Regex;
    private bool hasOption = true;
    private List<string> convFiles = new List<string>();
        
    public CodePublisherPopUp cpPopUpWindow;
    string workingDir = "../../../ClientDirectory";
    public MainWindow()
    {
      InitializeComponent();
    }

    private Stack<string> pathStack_ = new Stack<string>();
    private Translater translater;
    private CsEndPoint endPoint_;
    private CsEndPoint serverEndPoint;
    private string clientPortValue_ = "8082";
    private string serverPortValue_ = "8080";
    private Thread rcvThrd = null;
    private Dictionary<string, Action<CsMessage>> dispatcher_ 
      = new Dictionary<string, Action<CsMessage>>();

    //----< process incoming messages on child thread >----------------

    private void processMessages()
    {
      ThreadStart thrdProc = () => {
        while (true)
        {
          CsMessage msg = translater.getMessage();
          string msgId = msg.value("command");
          if (dispatcher_.ContainsKey(msgId))
            dispatcher_[msgId].Invoke(msg);
        }
      };
      rcvThrd = new Thread(thrdProc);
      rcvThrd.IsBackground = true;
      rcvThrd.Start();
    }
    //----< function dispatched by child thread to main thread >-------

    private void clearDirs()
    {
      DirList.Items.Clear();
    }
    //----< function dispatched by child thread to main thread >-------

    private void addDir(string dir)
    {
      DirList.Items.Add(dir);
    }
    //----< function dispatched by child thread to main thread >-------

    private void insertParent()
    {
      DirList.Items.Insert(0, "..");
    }
    //----< function dispatched by child thread to main thread >-------

    private void clearFiles()
    {
      FileList.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >-------
    private void clearConvertedFiles()
    {
      lstConverted.Items.Clear();
    }

    //----< function dispatched by child thread to main thread >-------

    private void addFile(string file)
    {
      FileList.Items.Add(file);
    }

    //----< function dispatched by child thread to main thread >-------
    private void addConFile(string file)
    {
      lstConverted.Items.Add(file);
    }

    //----< add client processing for message with key >---------------

    private void addClientProc(string key, Action<CsMessage> clientProc)
    {
      dispatcher_[key] = clientProc;
    }
    //----< load getDirs processing into dispatcher dictionary >-------

    private void DispatcherLoadGetDirs()
    {
      Action<CsMessage> getDirs = (CsMessage rcvMsg) =>
      {
        Action clrDirs = () =>
        {
          clearDirs();
        };
        Dispatcher.Invoke(clrDirs, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("dir"))
          {
            Action<string> doDir = (string dir) =>
            {
              addDir(dir);
            };
            Dispatcher.Invoke(doDir, new Object[] { enumer.Current.Value });
          }
        }
        Action insertUp = () =>
        {
          insertParent();
        };
        Dispatcher.Invoke(insertUp, new Object[] { });
      };
      addClientProc("getDirs", getDirs);
    }
    //----< load getFiles processing into dispatcher dictionary >------

    private void DispatcherLoadGetFiles()
    {
      Action<CsMessage> getFiles = (CsMessage rcvMsg) =>
      {
        Action clrFiles = () =>
        {
          clearFiles();
        };
        Dispatcher.Invoke(clrFiles, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
          if (key.Contains("file"))
          {
            Action<string> doFile = (string file) =>
            {
              addFile(file);
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("getFiles", getFiles);
    }

    //----< load publishFiles processing into dispatcher dictionary >------
    private void DispatcherPublishFiles()
    {
      Action<CsMessage> publishFiles = (CsMessage rcvMsg) =>
      {
          Console.WriteLine("Received converted file list in client: ");
          Action clrConvertedFiles = () =>
        {
          clearConvertedFiles();
        };
        Dispatcher.Invoke(clrConvertedFiles, new Object[] { });
        var enumer = rcvMsg.attributes.GetEnumerator();
        while (enumer.MoveNext())
        {
          string key = enumer.Current.Key;
         if (key.Contains("convFile"))
          {
            Console.WriteLine("convFile");
            Action<string> doFile = (string file) =>
            {
              Console.WriteLine(file);
              addConFile(file);
            };
            Dispatcher.Invoke(doFile, new Object[] { enumer.Current.Value });
          }
        }
      };
      addClientProc("publishFiles", publishFiles);
    }

    //----< load all dispatcher processing >---------------------------
    private void loadDispatcher()
    {
        DispatcherLoadGetDirs();
        DispatcherLoadGetFiles();
        DispatcherPublishFiles();
        Patterns = "*.h *.cpp";
        Regex = "[U](.*)";
        txtPatterns.Text = Patterns;
        txtRegex.Text = Regex;
    }
    
    //----< start Comm, fill window display with dirs, files and other information >------
    private void Window_Loaded(object sender, RoutedEventArgs e)
    {
        if (!System.IO.Directory.Exists(workingDir))
            System.IO.Directory.CreateDirectory(workingDir);
            
            endPoint_ = new CsEndPoint();
            endPoint_.machineAddress = "localhost";
            string[] args = Environment.GetCommandLineArgs();
            int len = args.Count();
            
            if (len > 1 && args[1] != "")
            {
                clientPortValue_ = args[1];
            }
            
            if (len > 2 && args[2] != "")
            {
                serverPortValue_ = args[2];
            }
            Console.WriteLine("\nClient Port: " + clientPortValue_);
            endPoint_.port = Int32.Parse(clientPortValue_);

            serverEndPoint = new CsEndPoint();
            serverEndPoint.machineAddress = "localhost";

            Console.WriteLine("\nServer Port: " + serverPortValue_);
            serverEndPoint.port = Int32.Parse(serverPortValue_);

            translater = new Translater();
            translater.listen(endPoint_, workingDir, workingDir);

            PathTextBlock.Text = "Project4_OOD_ADP";
            pathStack_.Push("../../Project4_OOD_ADP");
            CsMessage msg = new CsMessage();
            msg.add("to", CsEndPoint.toString(serverEndPoint));
            msg.add("from", CsEndPoint.toString(endPoint_));
            msg.add("command", "getDirs");
            msg.add("path", pathStack_.Peek());
            translater.postMessage(msg);
            msg.remove("command");
            msg.add("command", "getFiles");
            translater.postMessage(msg);

            processMessages();
            loadDispatcher();           
            Thread testThread = null;
            testThread = new Thread(atu);
            testThread.Start();
    }

        //----< To set command line arguments >------
        public void setCmdArgs()
    {
            wDirectory = Path.GetFullPath("../../../../Project4_OOD_ADP");
            Patterns = "*.h *.cpp";
            Regex = "[U](.*)";
            CsMessage argsMsg = new CsMessage();
            argsMsg.add("args", "convFile");
            argsMsg.add("to", CsEndPoint.toString(serverEndPoint));
            argsMsg.add("from", CsEndPoint.toString(endPoint_));
            argsMsg.add("command", "publishFiles");
            argsMsg.add("wDirectory", wDirectory);
            argsMsg.add("Patterns", Patterns);
            argsMsg.add("Regex", Regex);
            argsMsg.add("hasOption", hasOption.ToString());
            translater.postMessage(argsMsg);
    }

    //----< strip off name of first part of path >---------------------
    private string removeFirstDir(string path)
    {
      string modifiedPath = path;
      int pos = path.IndexOf("/");
      modifiedPath = path.Substring(pos + 1, path.Length - pos - 1);
      return modifiedPath;
    }

    //----< respond to mouse double-click on dir name >----------------
    private void DirList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
      string selectedDir = (string)DirList.SelectedItem;
      if (selectedDir == null)
        return;
      string path;
      if(selectedDir == "..")
      {
        if (pathStack_.Count > 1)
          pathStack_.Pop();
        else
          return;
      }
      else
      {
        path = pathStack_.Peek() + "/" + selectedDir;
        pathStack_.Push(path);
      }
      PathTextBlock.Text = removeFirstDir(pathStack_.Peek());
      
      CsEndPoint serverEndPoint = new CsEndPoint();
      serverEndPoint.machineAddress = "localhost";
      serverEndPoint.port = Int32.Parse(serverPortValue_);
      CsMessage msg = new CsMessage();
      msg.add("to", CsEndPoint.toString(serverEndPoint));
      msg.add("from", CsEndPoint.toString(endPoint_));
      msg.add("command", "getDirs");
      msg.add("path", pathStack_.Peek());
      translater.postMessage(msg);
      
      msg.remove("command");
      msg.add("command", "getFiles");
      translater.postMessage(msg);
    }

    //----< To exit on clicking file Menu Exit >-------------------------
    private void click_Exit(object sender, RoutedEventArgs e)
    {
        Application.Current.Shutdown();
    }

    //----< Displays file in pop-up or browser >------------
    private async void convertedFiles_MouseDoubleClick(object sender, MouseButtonEventArgs e)
    {
            if (dispBrowser.IsChecked == true && dispPopUpWindow.IsChecked == false)
            {
                if (lstConverted.SelectedIndex == -1) return;
                string selectedFile = lstConverted.SelectedItem.ToString();
                downloadFile(selectedFile);
                string file = workingDir + "\\" + Path.GetFileName(selectedFile) + ".html";
                var dispConvertedFile = System.Diagnostics.Process.Start(file);
                txtProgress.Text = "Waiting to close the file";
                while (!dispConvertedFile.HasExited)
                    await Task.Delay(500);
                txtProgress.Text = "File closed";
            }
            else if (dispBrowser.IsChecked == false && dispPopUpWindow.IsChecked == true)
            {
                if (lstConverted.SelectedIndex == -1) return;
                string selectedFile = lstConverted.SelectedItem.ToString();              
                cpPopUpWindow = new CodePublisherPopUp();
                cpPopUpWindow.setMainWindow(this);
                cpPopUpWindow.Show();
                downloadFile(selectedFile);
                string file = workingDir + "\\" + Path.GetFileName(selectedFile) + ".html";
                await Task.Delay(500);
                cpPopUpWindow.loadFile(file);
            }
    }
       
    //----< to download file in the client directory >------------
    private void downloadFile(string filePath)
    {
        Console.WriteLine("\nfilepath: " + filePath);
        string fileName = Path.GetFileName(filePath) + ".html";
        CsMessage dlFileMsg = new CsMessage();
        dlFileMsg.add("args", "downloadFile");
        dlFileMsg.add("to", CsEndPoint.toString(serverEndPoint));
        dlFileMsg.add("from", CsEndPoint.toString(endPoint_));
        dlFileMsg.add("command", "downloadFile");
        dlFileMsg.add("name", fileName);
        translater.postMessage(dlFileMsg);
    }

    //----< Checks if Sub-Directory checkbox is checked or not >------------
    private void CbRecurse_Click(object sender, RoutedEventArgs e)
    {
        if (cbRecurse.IsChecked.Value)
        {
            hasOption = true;
        }
        else
        {
            hasOption = false;
        }
    }
        
        //----< sets command line arguments and fetches converted file list on clicking convert button >----------------------
        private void btnConvert_Click(object sender, RoutedEventArgs e)
        {
            wDirectory = Path.GetFullPath("../../../../Project4_OOD_ADP");
            Patterns = txtPatterns.Text;
            Regex = txtRegex.Text;
            CsMessage cMsg = new CsMessage();
            cMsg.add("args", "convFile");
            cMsg.add("to", CsEndPoint.toString(serverEndPoint));
            cMsg.add("from", CsEndPoint.toString(endPoint_));
            cMsg.add("command", "publishFiles");
            cMsg.add("wDirectory", wDirectory);
            cMsg.add("Patterns", Patterns);
            cMsg.add("Regex", Regex);
            cMsg.add("hasOption", hasOption.ToString());
            translater.postMessage(cMsg);
        }

        //-----< demonstrate requirement 3>----------------------------------------------
        private void displayReq3()
        {
            Console.WriteLine("\n");
            Console.WriteLine("\n----------Demonstrating Requirement #3 ----------");
            Console.WriteLine("\n Assembled the parts from Project 1, 2 and 3 in Client-server configuration");
            Console.WriteLine("\n Client does not wait for a reply from the server for a request before sending additional requests. \n");
        }

        // -----< demonstrate requirement 4>----------------------------------------------
        private void displayReq4()
        {
            Console.WriteLine("\n");
            Console.WriteLine("\n----------Demonstrating Requirement #4 ----------");
            Console.WriteLine("\n-------------------------------------------------------\n");
            Console.WriteLine("\n Provided Code Publisher GUI with Navigation and Display views as in Project-3.");
        }

        // -----< demonstrate requirement 5>----------------------------------------------
        private void displayReq5()
        {
            Console.WriteLine("\n");
            Console.WriteLine("----------Demonstrating Requirement #5 ----------");
            Console.WriteLine("\n Provided appropriate message design for the application \n");
        }

        // -----< demonstrate requirement 6>----------------------------------------------
        private void displayReq6()
        {
            Console.WriteLine("\n");
            Console.WriteLine("\n----------Demonstrating Requirement #6 ----------");
            Console.WriteLine("\n-----------------------------------------------------------");
            Console.WriteLine("\n Supports source code conversion in server and transfers converted file\n");
            Console.WriteLine("\n back to client on a separate request.\n");
        }

        // -----< demonstrate requirement 7 >-------------------------------------
        private void displayReq7()
        {
            Console.WriteLine("\n");
            Console.WriteLine("\n---------------Demonstrating Requirement #7 ---------------------");
            Console.WriteLine("\n---------------------------------------------------------------");
            Console.WriteLine("\n Demonstrates operations for 2 or more concurrent clients\n ");
        }

        // -----< demonstrate requirement 8 >-------------------------------------
        private void displayReq8()
        {
            Console.WriteLine("\n");
            Console.WriteLine("\nDemonstrating Requirement #8 - Automated Test Unit");
            Console.WriteLine("\n---------------------------------------------------------------");
            Console.WriteLine("\n Automated Test units exists for all requirements. \n ");
        }

        //----< Automated Test unit for Code Publisher GUI>-------------------------
        public void atu()
        {
            Thread.Sleep(2000);
            Dispatcher.Invoke(() => { atu_1(); });
            Thread.Sleep(2000);
            Dispatcher.Invoke(() => { atu_2(); });
            Thread.Sleep(2000);
            Dispatcher.Invoke(() => { atu_3(); });
            Thread.Sleep(2000);
        }

        //----< Automated Test unit for displaying requirements>-------------------------
        public void atu_1()
        {
            displayReq3();
            displayReq4();
            displayReq5();
            displayReq6();
            displayReq7();
            displayReq8();
        }

        //----< Automated Test unit to set Command Line arguments and get converted files>-------------------------
        public void atu_2()
        {
            setCmdArgs();
        }

        //----< Automated Test unit to display converted file by downloading to client directory>-------------------------
        public void atu_3()
        {
            MyTabControl.SelectedIndex = 1;
            string selectedFile = "./CppCommWithFileXfer/Utilities/Utilities.cpp";
            downloadFile(selectedFile);
            string file = workingDir + "\\" + Path.GetFileName(selectedFile) + ".html";
            var dispConvertedFile = System.Diagnostics.Process.Start(file);
        }
    }
}
