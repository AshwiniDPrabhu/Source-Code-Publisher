/////////////////////////////////////////////////////////////////////
// CodePublisherPopUp.xaml.cs - Prototype for OOD Project #3       //
// ver 1.0                                                         //
// Author: Ashwini Prabhu, Syracuse University                     //
//         Spring 2019, adprabhu@syr.edu                           //
/////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package defines one class Code Publisher PopUp Window that provides a 
 * Textbox for displaying file content selected in the main Code Publisher window.
 * 
 * Required Files:
 * ---------------
 * MainWindow.xaml, MainWindow.xaml.cs
 * CodePublisherPopUp.xaml, CodePublisherPopUp.xaml.cs
 * 
 * Maintenance History:
 * --------------------
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
//using System.Windows.Forms;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Threading;
using System.IO;
//using Application = System.Windows.Forms.Application;

namespace CodePublisher
{
    /// <summary>
    /// Interaction logic for CodePublisherPopUp.xaml
    /// </summary>

   
    public partial class CodePublisherPopUp : Window
    {
        MainWindow mWin = null;

        public CodePublisherPopUp()
        {
            InitializeComponent();
        }

        //----< called by MainWindow to give this window her ref >-------
        public void setMainWindow(MainWindow mwin)
        {
            mWin = mwin;
        }

        //----< called by MainWindow to display a selected file >------------
        public void loadFile(string file)
        {
            cp_TextBox.Text = File.ReadAllText(file);
            cp_TextBox.IsReadOnly = true;
        }   
    }
}
