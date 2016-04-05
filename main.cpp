#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string.h>
#include <vector>

using namespace std;
using namespace cv;

string imageDirectoryPath = "/home/marcel/Bureau/Stage PRISME/Programme/Mosaicing/images/reference/halos.bmp";

string imageDirectoryPathGauche = "/home/marcel/Bureau/Stage PRISME/Programme/Mosaicing/images/meme_taille_recouvrement/halos_gauche.bmp";
string imageDirectoryPathDroite = "/home/marcel/Bureau/Stage PRISME/Programme/Mosaicing/images/meme_taille_recouvrement/halos_droit.bmp";

double comparaison(Mat image1, Mat image2, int nbCol, int deltaX_1, int deltaX_2);

double comparaison(Mat image1, Mat image2, int nbCol, int deltaX_1, int deltaX_2){

    double difference = 0;

    for(int col=0; col<=nbCol; col++)
    {
        for(int row=0; row<image1.rows; row++)
        {
            difference += abs(image1.at<uchar>(row, deltaX_1+col) - image2.at<uchar>(row, deltaX_2+col));
        }
    }

    return difference/(nbCol*image1.rows*255);
}

int main()
{

    vector<double> correlation;
    //vector<long int> nbPixel;

    Mat image_gauche;
    Mat image_droite;
    Mat image_reconstituee;
    Mat image_test_complete;

    image_test_complete = imread( imageDirectoryPath, 1 );
    image_gauche = imread( imageDirectoryPathGauche, 1 );
    image_droite = imread( imageDirectoryPathDroite, 1 );

    if ( !image_test_complete.data )
    {
        printf("No image data \n");
        return -1;
    }

    cout << "taille image originale : " << image_test_complete.cols << " " << image_test_complete.rows << endl;
    cout << endl;
    cout << "taille image de gauche : " << image_gauche.cols << " " << image_gauche.rows << endl;
    cout << "taille image de droite : " << image_droite.cols << " " << image_droite.rows << endl;
    cout << endl;

    // Convertir les images RGB en images niveau de gris.

    cv::cvtColor(image_test_complete, image_test_complete, cv::COLOR_RGB2GRAY);
    cv::cvtColor(image_gauche, image_gauche, cv::COLOR_RGB2GRAY);
    cv::cvtColor(image_droite, image_droite, cv::COLOR_RGB2GRAY);


    // Calculer le nombre de tests a effectuer
    int nbTest = image_gauche.cols + image_droite.cols; // OK

    // L'afficher àl'écran
    cout << "Nombre de tests à effectuer : " << nbTest << endl;

    // Test des différences
    for(int test=1; test<nbTest; test++){

        if (test<image_gauche.cols) // De 0 à taille image
        {
            int nbCols = test;
            int deltaX_1 = image_droite.cols-test;
            int deltaX_2 = 0;

            correlation.push_back(comparaison(image_gauche, image_droite, nbCols, deltaX_1, deltaX_2));
            cout << comparaison(image_gauche, image_droite, nbCols, deltaX_1, deltaX_2) << endl;
        }
        else if (test>(nbTest-image_gauche.cols)) // Problème identifié
        {
            int nbCols = nbTest-test;
            int deltaX_1 = 0;
            int deltaX_2 = test - image_gauche.cols;

            correlation.push_back(comparaison(image_gauche, image_droite, nbCols, deltaX_1, deltaX_2));
            cout << comparaison(image_gauche, image_droite, nbCols, deltaX_1, deltaX_2) << endl;
        }
        else // Sinon on ne fait que décaler les images à comparer
        {
            int nbCols = image_gauche.cols;
            int deltaX_1 = test - image_gauche.cols;
            int deltaX_2 = 0;

            correlation.push_back(comparaison(image_gauche, image_droite, nbCols, deltaX_1, deltaX_2));
            cout << comparaison(image_gauche, image_droite, nbCols, deltaX_1, deltaX_2) << endl;
        }

    }

    // On trouve le x pour la plus petite valeur
    int x = 0;
    double minCorrelation = 1;

    for(unsigned int i=0; i<correlation.size(); i++){

        if (correlation[i]<minCorrelation){
            minCorrelation = correlation[i];
             x = i+1;
        }
    }

    cout << "déplacement : " << x << endl;

    // Reconstruction de l'image
    image_reconstituee = Mat(image_gauche.rows, nbTest-x, CV_8U, Scalar(255));

    // Copie des données de l'image de gauche
    for(int col=0; col<image_gauche.cols-x; col++)
    {
        for(int row=0; row<image_gauche.rows; row++)
        {
            image_reconstituee.at<uchar>(row,col) = image_gauche.at<uchar>(row,col);
        }
    }

    // Copie des données de l'image de droite
    for(int col=0; col<image_droite.cols; col++)
    {
        for(int row=0; row<image_gauche.rows; row++)
        {
            image_reconstituee.at<uchar>(row,col+(image_gauche.cols-x)) = image_droite.at<uchar>(row,col);
        }
    }

    // Afficher l'image de test complète
    namedWindow("Image originale", WINDOW_AUTOSIZE );
    imshow("Image originale", image_test_complete);

    namedWindow("Image reconstituée", WINDOW_AUTOSIZE );
    imshow("Image reconstituée", image_reconstituee);

    cout << "Taille image reconstituée : " << image_reconstituee.rows << " " << image_reconstituee.cols << endl;

    waitKey(0);
    return 0;
}
