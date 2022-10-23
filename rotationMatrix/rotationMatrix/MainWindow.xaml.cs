using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Xml.Schema;

namespace rotationMatrix
{
    public class RotationMatrix
    {
        double[,] rotationMatrix = new double[3, 3];
        double[,] angleMatrix = new double[3, 3];
        double angle;
        bool _isPopulated = false;

        public RotationMatrix(double _angle)
        { 

            angle = _angle; 

        }

        public RotationMatrix(){}

        public RotationMatrix(RotationMatrix rot1) 
        {
            this.rotationMatrix = rot1.rotationMatrix;
            this.angleMatrix = rot1.rotationMatrix;
            this.angle = rot1.angle;
            this._isPopulated = true;
        }


        public static RotationMatrix operator *(RotationMatrix rot1, RotationMatrix rot2)
        {
            RotationMatrix outRot = new RotationMatrix();
            for (int row = 0; row < 3; row++)
            {
                for (int col = 0; col < 3; col++)
                {
                    outRot.rotationMatrix[row, col] = 0;
                    for (int i = 0; i < 3; i++)
                    {
                        outRot.rotationMatrix[row, col] += (rot1.rotationMatrix[row, i] * rot2.rotationMatrix[i, col]);
                    }
                }
            }
            return outRot;
        }


        // Returns the Dot Product (|a||b|cos(x)) 
        double dotProduct(double angle)
        {
            // Cos(angle) returns
            return Math.Cos(Math.PI * angle / 180.0);
        }

        public string getRotationMatrix()
        {
            string msg = "";
            for (int i = 0; i < rotationMatrix.GetLength(0); i++)
                msg += String.Format("{0}   {1}   {2}\n", Math.Round(rotationMatrix[i, 0], 2), Math.Round(rotationMatrix[i, 1], 2), Math.Round(rotationMatrix[i, 2], 2));
            return msg;
        }

        public void setAngle(double angle)
        {
            this.angle = angle;
        }

        public bool isPopulated()
        {
            return _isPopulated;
        }

        void defineAngleMatrix(string rotationPlane)
        {
            // Define the angles based on rotation plane
            _isPopulated = true;
            switch (rotationPlane)
            {

                case "X":
                    angleMatrix[0, 0] = 0;
                    angleMatrix[0, 1] = 90;
                    angleMatrix[0, 2] = 90;
                    angleMatrix[1, 0] = 90;
                    angleMatrix[1, 1] = angle;
                    angleMatrix[1, 2] = 90 + angle;
                    angleMatrix[2, 0] = 90;
                    angleMatrix[2, 1] = 270 + angle;
                    angleMatrix[2, 2] = angle;
                    break;
                case "Y":
                    angleMatrix[0, 0] = angle;
                    angleMatrix[0, 1] = 90;
                    angleMatrix[0, 2] = 90+angle;
                    angleMatrix[1, 0] = 90;
                    angleMatrix[1, 1] = 0;
                    angleMatrix[1, 2] = 90;
                    angleMatrix[2, 0] = 270+angle;
                    angleMatrix[2, 1] = 90;
                    angleMatrix[2, 2] = angle;
                    break;
                case "Z":
                    angleMatrix[0, 0] = angle;
                    angleMatrix[0, 1] = 90 + angle;
                    angleMatrix[0, 2] = 90;
                    angleMatrix[1, 0] = 270 + angle;
                    angleMatrix[1, 1] = angle;
                    angleMatrix[1, 2] = 90;
                    angleMatrix[2, 0] = 90;
                    angleMatrix[2, 1] = 90;
                    angleMatrix[2, 2] = 0;
                    break;
                default:
                    Console.WriteLine("Error");
                    _isPopulated = false;
                    break;

            }
        }

        public void populateRotationMatrix(string rotationPlane)
        {
            defineAngleMatrix(rotationPlane);
            // Loop through the matrix and populate dot products
            for (int col = 0; col < rotationMatrix.GetLength(0); col++)
            {
                for (int row = 0; row < rotationMatrix.GetLength(1); row++)
                {
                    rotationMatrix[col, row] = dotProduct(angleMatrix[col, row]);
                }
            }
        }

    }


    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        double getRotationValues(TextBox tbx)
        {
            char[] num = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
            string convertedNum = "";
            foreach (char str in tbx.Text)
            {
                if (num.Contains(str))
                {
                    convertedNum += str;
                }
            }
            if (convertedNum.Length > 0)
            {
                return Convert.ToDouble(convertedNum);
            }
            return -1;
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // Define empty rotation matrix class
            RotationMatrix currMatrix = new RotationMatrix();
            string prevRotationPlane = "";
            txtblkOutput.Text = "Output\n";


            // Get values for X, Y and Z coordinates
            var rotation_values = new Dictionary<string, double>()
            {
                {"X", getRotationValues(txtXDeg)},
                {"Y", getRotationValues(txtYDeg)},
                {"Z", getRotationValues(txtZDeg)},
            };

            // Get values for Rotation Order
            var rotation_order = new Dictionary<int, string>() 
            { 
                {1, Convert.ToString(cmbxChoiceOne.Text)},
                {2, Convert.ToString(cmbxChoiceTwo.Text)},
                {3, Convert.ToString(cmbxChoiceThree.Text)},
            };

            // Loop through each initialization and create a dot product for each
            foreach (var rotation in rotation_order)
            {
                if (rotation.Value.Length <=0)
                {
                    continue;
                }
                
                double angle = rotation_values[rotation.Value];
                RotationMatrix rotMat = new RotationMatrix(angle);
                rotMat.populateRotationMatrix(rotation.Value);
                txtblkOutput.Text += "\n" + rotation.Value + " Output\n" + rotMat.getRotationMatrix();
                prevRotationPlane += (rotation.Value);

                if (!currMatrix.isPopulated())
                {
                    currMatrix = rotMat;
                }
                else
                {
                    currMatrix = currMatrix * rotMat;
                    txtblkOutput.Text += "\n" + prevRotationPlane + " Output\n" + currMatrix.getRotationMatrix();
                }
                prevRotationPlane += "*";
            }

        }

        private void ComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            Console.WriteLine("Selected");
        }

        private void txtXDeg_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
    }


}
