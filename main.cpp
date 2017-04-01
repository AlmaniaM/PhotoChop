#include <iostream>
#include <QApplication>
#include <QFrame>

#include "imageiolib.h"
#include "main.h"

using namespace std;


//-------------------------------------------------------------
//Custom Filters:

Image makeAqua();
Image makeGradient();
Image redShift(const Image& source, int shiftAmount);
Image blur(const Image& source);
Image rotateRight(const Image& source);


//-------------------------------------------------------------
//Helpers and Main:
/** @brief clamp Forces a value into range 0-255
 * @param value double value to clamp
 * @return 0 if value < 0, 255 if value > 255, value otherwise
 */
byte clamp(double value) {
    value = min(value, 255.0);
    value = max(value, 0.0);
    return static_cast<byte>(value);
}




int main(int argc, char *argv[])
{
    //Setup work for the GUI that will draw the images
    QApplication myApp(argc, argv);
    QFrame mainWindow;
    QHBoxLayout mainLayout;
    mainWindow.setLayout(&mainLayout);



    //Create an image struct from a file - don't have to worry about details
    Image crabOriginal = readImage("crab.bmp");
    //Show the image in the GUI - definately don't worry about details
    displayImage(crabOriginal, "Original", mainLayout);

    //Use redShift function to make a new image, then draw it to GUI
    Image redCrab = redShift(crabOriginal, 100);
    displayImage(redCrab, "Red", mainLayout);


    Image blurCrab = blur(crabOriginal);
    displayImage(blurCrab, "Blurred", mainLayout);

    Image gradient = makeGradient();
    displayImage(gradient, "Gradient", mainLayout);


    Image aqua = makeAqua();
    displayImage(aqua, "Aqua", mainLayout);

    Image rotatedCrab = rotateRight(crabOriginal);
    displayImage(rotatedCrab, "Rotated", mainLayout);

    

    //Show the GUI... exec will keep program running until GUI window is closed
    mainWindow.show();
    return myApp.exec();
}



//-------------------------------------------------------------
//Filter Implementation

/**
 * @brief makeAqua Make an image with pure aqua color
 * @return the created image
 */
Image makeAqua() {
    Image newImage = {0}; //all black

    for(int i = 0; i < IMG_HEIGHT; i++) {            //for each row
        for(int j = 0; j < IMG_WIDTH; j++) {         //for each column
            newImage.data[i][j][GREEN] = 128;
            newImage.data[i][j][BLUE] = 255;
            //no red needed...
        }
    }
    return newImage;
}


/**
 * @brief makeGradient Make an image with black to white vertical gradient
 * @return the created image
 */
Image makeGradient() {
    Image newImage = {0}; //all black

    for(int i = 0; i < IMG_HEIGHT; i++) {            //for each row
        for(int j = 0; j < IMG_WIDTH; j++) {         //for each column
            for(int k = 0; k < NUM_CHANNELS; k++) {  //for each color channel
                //set each color to the row index * 2... will make 0-254
                newImage.data[i][j][k] = i * 2;
            }
        }
    }
    return newImage;
}


/**
 * @brief redShift copy an image but add the given amount to the red value of each pixel
 * @param source iamge to copy
 * @param shiftAmount value to add to red channel of each pixel
 * @return the modified copy
 */
Image redShift(const Image& source, int shiftAmount) {
    Image newImage = {0}; //all black

    for(int i = 0; i < IMG_HEIGHT; i++) {            //for each row
        for(int j = 0; j < IMG_WIDTH; j++) {         //for each column
            //clamp function makes sure we don't go past 255 and wrap back around to 0
            newImage.data[i][j][RED] = clamp(source.data[i][j][RED] + shiftAmount);
            //blue and green are not modified
            newImage.data[i][j][BLUE] = source.data[i][j][BLUE];
            newImage.data[i][j][GREEN] = source.data[i][j][GREEN];
        }
    }

    return newImage;
}




/**
 * @brief blur makes a blurred copy of an image. Leaves a 1-pixel black border around the
 *          new image
 * @param source image to copy
 * @return the modified copy
 */
Image blur(const Image& source) {
    Image newImage = {0}; //all black

    //skip first and last row and column... avoids stepping off edges when do neighbors
    // alternative would be special logic to handle those cases
    for(int i = 1; i < IMG_HEIGHT - 1; i++) {            //for each row but first and last
        for(int j = 1; j < IMG_WIDTH - 1; j++) {         //for each column but first and last
            for(int k = 0; k < NUM_CHANNELS; k++) {      //for each color channel
                //get value of this and 4 neighbors
                byte currentPixelValue = source.data[i][j][k];
                byte abovePixelValue = source.data[i - 1][j][k];
                byte belowPixelValue = source.data[i + 1][j][k];
                byte leftPixelValue = source.data[i][j - 1][k];
                byte rightPixelValue = source.data[i][j + 1][k];

                byte avgValue = (currentPixelValue + abovePixelValue + belowPixelValue +
                                 leftPixelValue + rightPixelValue) / 5;


                newImage.data[i][j][k] = avgValue;
            }
        }
    }

    return newImage;
}


/**
 * @brief rotateRight makes a copy of an image that is rotated 90 degrees clockwise
 * @param source Image to be rotated. This MUST be a square image.
 * @return the rotated copy
 */
Image rotateRight(const Image& source) {
    Image newImage = {0}; //all black

    for(int i = 0; i < IMG_HEIGHT; i++) {               //for each row
        for(int j = 0; j < IMG_WIDTH; j++) {            //for each column
            for(int k = 0; k < NUM_CHANNELS; k++) {     //for each color channel
                //calculate where i, j should rotate to
                int newRow = j;                         //new row = old column
                int newCol = (IMG_WIDTH - 1) - i;       //new column = last column - old row

                newImage.data[newRow][newCol][k] = source.data[i][j][k];
            }
        }
    }

    return newImage;
}
