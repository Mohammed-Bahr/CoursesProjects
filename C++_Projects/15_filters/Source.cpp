/**
 * @file      : CS112_A3_Part2B_S7_20230014_20230148_20230318.cpp
 * @brief     : This program is like baby photoshop and contains 15 different filters.

 * @author    : Ahmed Hassan  20230014 (filter:1, 4, 7, 10 , 14 )
 *              Zaid Ahmed    20230148 (filter:2, 5, 8, 11 ,13 )
 *              Mohamed Ahmed 20230318 (filter: 3, 6, 9, 12 , 15)
 * @link      : https://github.com/Batot-boop/15-Filters

 * @copyright : FCAI Cairo University.
 * @date      : 25/3/2024
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include "Image_Class.h"
using namespace std;

void mainMenu();
void grayscaleConversion(); // Filter 1
void Black_White(); // Filter 2
void Invert_image();// Filter 3
void mergeImages(); void resizeMergeImage(int width, int height, Image& image); // Filter 4
void flipoimage(); // Filter 5
void Rotate();// Filter 6
void darkenandLightenImage(); // Filter 7
void Crop_image(); // Filter 8
void Frame();// filter 9
void detectImageEdges(); int sobelGx(Image& image, int x, int y, int channel); int sobelGy(Image& image, int x, int y, int channel); // Filter 10
void Resizing(); // filter 11
void boxblur();//filter 12
void Natural_Sun(); // filter 13
void oilPainting(); // filter 14
void Old_TV();// filter 15

int main()
{
    while (true)
    {
        int numOfChoice;
        while (true)
        {
            mainMenu();
            while (!(cin >> numOfChoice))
            {
                cin.clear();
                cin.ignore(123, '\n');
                cout << "Error: Enter a valid choice: ";
            }

            if (numOfChoice == 1 || numOfChoice == 2 || numOfChoice == 3 || numOfChoice == 4 || numOfChoice == 5 || numOfChoice == 6 || numOfChoice == 7 || numOfChoice == 8 || numOfChoice == 9 || numOfChoice == 10 || numOfChoice == 11 || numOfChoice == 12 || numOfChoice == 13 || numOfChoice == 14 || numOfChoice == 15 || numOfChoice == 16)
                break;
            else
                cout << "\nPlease enter one of the numbers you see in the list.\n" << "\n";
        }
        switch (numOfChoice)
        {
        case 1:
            grayscaleConversion();
            break;
        case 2:
            Black_White();
            break;
        case 3:
            Invert_image();
            break;
        case 4:
            mergeImages();
            break;
        case 5:
            flipoimage();
            break;
        case 6:
            Rotate();
            break;
        case 7:
            darkenandLightenImage();
            break;
        case 8:
            Crop_image();
            break;
        case 9:
            Frame();
            break;
        case 10:
            detectImageEdges();
            break;
        case 11:
            Resizing();
            break;
        case 12:
            boxblur();
            break;
        case 13:
            Natural_Sun();
            break;
        case 14:
            oilPainting();
            break;
        case 15:
            Old_TV();
            break;
        default:
            cout << "\nExiting PhotoShop\n" << "\n";
            return 0;
        }


    }
}

void mainMenu()
{
    cout << "\n";
    cout << "1-  Grayscale Conversion             2-  Black and White" << "\n";
    cout << "3-  Invert Image                     4-  Merge Images" << "\n";
    cout << "5-  Flip Image                       6-  Rotate Image" << "\n";
    cout << "7-  Darken and Lighten Image         8-  Crop Images" << "\n";
    cout << "9-  Adding a Frame to the Picture    10- Detect Image Edges" << "\n";
    cout << "11- Resizing Images                  12- Blur Images" << "\n";
    cout << "13- Natural Sunlight.                14- Oil Painting" << "\n";
    cout << "15- Old Den Den Mushi (Televisions)  16- Exit" << "\n";
    cout << "\n[Warning: When entering the name and extension, you must use the three files on the GitHub page, which are ours.]" << "\n";
    cout << "Enter number your want choice: ";

}

// <<<<<<< Filter 1 >>>>>> //
void grayscaleConversion()
{
    while (true)
    {
        try
        {
            string imageName;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);
            for (int i = 0; i < image.width; ++i)
            {
                for (int j = 0; j < image.height; ++j)
                {
                    unsigned avg = 0;
                    for (int k = 0; k < 3; ++k)
                    {
                        avg += image(i, j, k);
                    }
                    avg /= 3;
                    image(i, j, 0) = avg;
                    image(i, j, 1) = avg;
                    image(i, j, 2) = avg;
                }
            }

            string newNameImage;
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            if (image.saveImage(newNameImage))
            {
                image.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }

}

// <<<<<<< Filter 2 >>>>>> //
void Black_White()
{
    while (true)
    {
        try
        {
            string imageName;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);
            for (int i = 0; i < image.width; i++) {
                for (int j = 0; j < image.height; j++) {

                    unsigned int avg = 0;
                    for (int k = 0; k < image.channels; k++) {
                        avg += image(i, j, k);
                    }
                    avg = avg / 3;

                    for (int k = 0; k < image.channels; k++) {
                        if (avg > 150)
                            image(i, j, k) = 255;
                        else
                            image(i, j, k) = 0;
                    }
                }
            }

            string newNameImage;
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            if (image.saveImage(newNameImage))
            {
                image.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }

}

// <<<<<<< Filter 3 >>>>>> //
void Invert_image()
{
    while (true)
    {
        try
        {
            // Load the image
            string imageName;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);

            // Invert each pixel's color
            for (int l = 0; l < image.width; ++l)
            {
                for (int k = 0; k < image.height; ++k)
                {
                    for (int j = 0; j < image.channels; ++j)
                    {
                        image(l, k, j) = 255 - image(l, k, j);
                    }
                }
            }

            // Save the edited image
            string newNameImage;
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            if (image.saveImage(newNameImage))
            {
                image.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}


// <<<<<<< Filter 4 >>>>>> //
void mergeImages()
{
    while (true)
    {
        try
        {
            string imageName1, imageName2;
            cout << "Enter a Image1 Name: ";
            cin >> imageName1;
            Image image1(imageName1);
            cout << "Enter a Image2 Name: ";
            cin >> imageName2;
            Image image2("../Pictures/" + imageName2);
            if (image1.width < image2.width && image1.height < image2.height)
            {

                Image imageEdit(image1.width, image1.height);
                resizeMergeImage(image1.width, image1.height, image1);
                resizeMergeImage(image1.width, image1.height, image2);
                for (int i = 0; i < imageEdit.width; ++i)
                {
                    for (int j = 0; j < imageEdit.height; ++j)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            imageEdit(i, j, k) = (image1(i, j, k) + image2(i, j, k)) / 2;
                        }
                    }
                }
                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (imageEdit.saveImage(newNameImage))
                {
                    imageEdit.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }

            }
            else if (image1.width < image2.width && image1.height > image2.height)
            {
                Image imageEdit(image1.width, image2.height);
                resizeMergeImage(image1.width, image2.height, image1);
                resizeMergeImage(image1.width, image2.height, image2);
                for (int i = 0; i < imageEdit.width; ++i)
                {
                    for (int j = 0; j < imageEdit.height; ++j)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            imageEdit(i, j, k) = (image1(i, j, k) + image2(i, j, k)) / 2;
                        }
                    }
                }
                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (imageEdit.saveImage(newNameImage))
                {
                    imageEdit.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
            else if (image1.width > image2.width && image1.height < image2.height)
            {
                Image imageEdit(image2.width, image1.height);
                resizeMergeImage(image2.width, image1.height, image1);
                resizeMergeImage(image2.width, image1.height, image2);
                for (int i = 0; i < image1.width; ++i)
                {
                    for (int j = 0; j < image2.height; ++j)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            imageEdit(i, j, k) = (image1(i, j, k) + image2(i, j, k)) / 2;
                        }
                    }
                }
                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (imageEdit.saveImage(newNameImage))
                {
                    imageEdit.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
            else if (image1.width > image2.width && image1.height > image2.height)
            {
                Image imageEdit(image2.width, image2.height);
                resizeMergeImage(image2.width, image2.height, image1);
                resizeMergeImage(image2.width, image2.height, image2);

                for (int i = 0; i < imageEdit.width; ++i)
                {
                    for (int j = 0; j < imageEdit.height; ++j)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            imageEdit(i, j, k) = (image1(i, j, k) + image2(i, j, k)) / 2;
                        }
                    }
                }
                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (imageEdit.saveImage(newNameImage))
                {
                    imageEdit.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }

            else
            {
                Image imageEdit(image1.width, image1.height);
                resizeMergeImage(image1.width, image1.height, image1);
                resizeMergeImage(image1.width, image1.height, image2);
                for (int i = 0; i < imageEdit.width; ++i)
                {
                    for (int j = 0; j < imageEdit.height; ++j)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            imageEdit(i, j, k) = (image1(i, j, k) + image2(i, j, k)) / 2;
                        }
                    }
                }
                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (imageEdit.saveImage(newNameImage))
                {
                    imageEdit.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
}
//>>>>>>>>>>>>>>> assocated with MegarImage <<<<<<<<<<<<<<<<<//
void resizeMergeImage(int width, int height, Image& image)
{
    Image imageEdit(width, height);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                // Make the new image equal the resized orginal image
                imageEdit(i, j, k) = image(i * image.width / width, j * image.height / height, k);
            }
        }
    }
    if (imageEdit.saveImage("resizeImage.jpg") == true)
    {
        image.loadNewImage("resizeImage.jpg");
        remove("resizeImage.jpg");
    }


}
//>>>>>>>>>>>>>>> assocated with MegarImage <<<<<<<<<<<<<<<<<//


// <<<<<<< Filter 5 >>>>>> //
void flipoimage()
{
    while (true)
    {
        try
        {
            string imageName;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);
            char y;

            cout << "Flip the image a- Vertically | b- Horizontally : " << endl;
            cin >> y;

            if (y == 'b') {
                for (int j = 0; j < image.height; j++) {
                    for (int i = 0; i < (image.width / 2); i++) {
                        int x = 0;
                        for (int k = 0; k < 3; k++) {
                            x = image(i, j, k);
                            image(i, j, k) = image((image.width - i - 1), j, k);
                            image((image.width - i - 1), j, k) = x;
                        }
                    }
                }
                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (image.saveImage(newNameImage))
                {
                    image.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
            else if (y == 'a') {
                for (int i = 0; i < image.width; i++) {
                    for (int j = 0; j < (image.height / 2); j++) {
                        unsigned int x = 0;
                        for (int k = 0; k < 3; k++) {
                            x = image(i, j, k);
                            image(i, j, k) = image(i, (image.height - j - 1), k);
                            image(i, (image.height - j - 1), k) = x;
                        }
                    }
                }

                string newNameImage;
                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (image.saveImage(newNameImage))
                {
                    image.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

// <<<<<<< Filter 6 >>>>>> //
void Rotate()
{
    while (true)
    {
        try
        {
            string imageName, newNameImage;
            cout << "Enter image name : ";
            cin >> imageName;
            Image image(imageName);

            int ag;
            cout << "choose rotation angle (90 , 180 , 270): ";
            cin >> ag;

            if (ag == 90)
            {
                Image image2(image.height, image.width);
                for (int y = 0; y < image.width; ++y) {
                    for (int x = 0; x < image.height; ++x) {
                        for (int c = 0; c < image.channels; ++c) {
                            image2(x, y, c) = image(y, image.height - x - 1, c);
                        }
                    }
                }

                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (image2.saveImage(newNameImage))
                {
                    image2.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
            else if (ag == 180)
            {
                Image image2("../Pictures/" + imageName);
                for (int l = 0; l < image.width; ++l) {
                    for (int k = 0; k < image.height; ++k) {
                        for (int j = 0; j < image.channels; ++j) {
                            image2(image.width - l - 1, image.height - k - 1, j) = image(l, k, j);
                        }
                    }
                }

                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (image2.saveImage(newNameImage))
                {
                    image2.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
            else if (ag == 270)
            {
                Image image2(image.height, image.width);
                for (int y = 0; y < image.width; ++y) {
                    for (int x = 0; x < image.height; ++x) {
                        for (int c = 0; c < image.channels; ++c) {
                            image2(x, y, c) = image(image.width - y - 1, x, c);
                        }
                    }
                }

                cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
                cin >> newNameImage;

                if (image2.saveImage(newNameImage))
                {
                    image2.saveImage(newNameImage);
                    cout << "\nSuccessfuly Editing.\n" << endl;
                    break;
                }
                else
                {
                    cout << "\nFalid Editing.\n" << endl;
                    break;
                }
            }
            else
            {
                cout << "Error: Invalid choice " << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

// <<<<<<< Filter 7 >>>>>> //
void darkenandLightenImage()
{
    while (true)
    {
        try
        {
            int num;
            cout << "Choice [Darken >>> 1] [Lighten >>> 2]: ";
            while (true)
            {
                while (!(cin >> num))
                {
                    cin.clear();
                    cin.ignore(123, '\n');
                    cout << "Error: Enter a valid choice: ";
                }

                if (num == 1 || num == 2)
                    break;
                else
                    cout << "\nPlease enter one of the numbers from(1/2): ";
            }

            string imageName, newNameImage;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);

            if (num == 1)
            {
                for (int i = 0; i < image.width; ++i)
                {
                    for (int j = 0; j < image.height; ++j)
                    {
                        for (int k = 0; k < 3; ++k)
                        {
                            image(i, j, k) = image(i, j, k) / 2;
                        }
                    }
                }
            }

            else
            {
                for (int i = 0; i < image.width; i++)
                {
                    for (int j = 0; j < image.height; j++)
                    {
                        for (int k = 0; k < 3; k++)
                        {
                            int pixelValue = image.getPixel(i, j, k);
                            pixelValue *= 2;
                            pixelValue = min(255, max(0, pixelValue));
                            image.setPixel(i, j, k, pixelValue);
                        }
                    }
                }
            }

            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            if (image.saveImage(newNameImage))
            {
                image.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

// <<<<<<< Filter 8 >>>>>> //
void Crop_image()
{
    while (true)
    {
        try {
            string imageName, newNameImage;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);
            unsigned int x, y, w, h;

            while (x > image.width || y > image.height)
            {
                // Prompt the user to enter the cropping parameters
                cout << "Enter the starting point (center point is in the upper left corner): " << endl;
                cout << "Enter x: " << endl;
                cin >> x;
                cout << "Enter y: " << endl;
                cin >> y;
            }

            //Check if the dimentions is in range
            while (w > (image.width - x) || h > (image.height - y))
            {
                cout << "Enter the dimention of the new photo: " << endl;
                cout << "Enter the width: " << endl;
                cin >> w;
                cout << "Enter the hight: " << endl;
                cin >> h;
            }

            // Create a new image with the specified dimensions
            Image image2(w, h);

            // Copy pixels from the original image to the cropped image
            for (int i = x; i < x + w; i++)
            {
                for (int j = y; j < y + h; j++)
                {

                    for (int k = 0; k < image.channels; k++)
                    {
                        image2((i - x), (j - y), k) = image(i, j, k);
                    }

                }
            }

            //Make the user select a new name for the image
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            //check if the image saved successfuly
            if (image2.saveImage(newNameImage))
            {
                image2.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e) {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

// <<<<<<< Filter 9 >>>>>> //
void Frame()
{
    while (true)
    {
        try
        {
            string filename, filename2;
            cout << "please enter your file name: ";
            cin >> filename;
            Image image(filename);
            Image image2(image.width, image.height);

            int frame;
            cout << "you want a simple or fancy frame (>_<) : \n if you want simple enter => 1 \n if you want fancy enter => 2 \n enter your choice here => ";
            cin >> frame;

            int num;
            cout << "what is your frame width ? => ";
            cin >> num;


            for (int x = 0; x < image.width; ++x) {
                for (int y = 0; y < image.height; ++y) {
                    for (int z = 0; z < image.channels; ++z) {
                        // for simple one
                        if (frame == 1) {
                            for (int v = 0; v < 2 * num; v++) {
                                image(v, y, z) = 255;
                                image(x, v, z) = 255;
                                image(x, image.height - (v + 1), z) = 255;
                                image(image.width - (v + 1), y, z) = 255;
                            }
                            for (int b = num + 1; b < 3 * num; ++b) {
                                image(b, y, 2) = 255;
                                image(x, b, 2) = 255;
                                image(x, image.height - (b + 1), 2) = 255;
                                image(image.width - (b + 1), y, 2) = 255;
                            }
                        }
                        //for fancy one
                        else if (frame == 2) {
                            for (int v = 0; v < 2 * num; v++) {
                                image(v, y, z) = 255;
                                image(x, v, z) = 255;
                                image(x, image.height - (v + 1), z) = 255;
                                image(image.width - (v + 1), y, z) = 255;
                            }
                            for (int b = num + 1; b < 3 * num; ++b) {
                                image(b, y, 2) = 255;
                                image(x, b, 2) = 255;
                                image(x, image.height - (b + 1), 2) = 255;
                                image(image.width - (b + 1), y, 2) = 255;
                            }

                            for (int n = (2 * num) + 1; n < 5 * num; ++n) {
                                if ((image.width / 1 > x && x > image.width / 2) ||
                                    (image.width / 3 > x && x > image.width / 4) ||
                                    (image.width / 5 > x && x > image.width / 6) ||


                                    (image.height / 1 > y && y > image.height / 2) ||
                                    (image.height / 3 > y && y > image.height / 4) ||
                                    (image.height / 5 > y && y > image.height / 6))
                                {
                                    image(n, y, 1) = 255;
                                    image(x, n, 1) = 255;
                                    image(x, image.height - (n + 1), 1) = 255;
                                    image(image.width - (n + 1), y, 1) = 255;
                                }
                            }
                        }
                        else {

                            cout << "ERROR : invalid choice ";

                        }
                        image2(x, y, z) = image(x, y, z);
                    }
                }
            }

            //Make the user select a new name for the image
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> filename2;

            //check if the image saved successfuly
            if (image2.saveImage(filename2))
            {
                image2.saveImage(filename2);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
}

// <<<<<<< Filter 10 >>>>>> //
void detectImageEdges()
{
    while (true)
    {
        try
        {
            string imageName;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);
            Image edgeImage(image.width, image.height);
            for (int i = 1; i < image.width - 1; i++) {
                for (int j = 1; j < image.height - 1; j++) {
                    for (int k = 0; k < image.channels; k++) {
                        int gx = sobelGx(image, i, j, k);
                        int gy = sobelGy(image, i, j, k);
                        int edge = sqrt(gx * gx + gy * gy);
                        edgeImage.setPixel(i, j, k, 255 - edge);
                    }
                }
            }

            string newNameImage;
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            if (edgeImage.saveImage(newNameImage))
            {
                edgeImage.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}
//>>>>>>>>>>>>>>> assocated with detectImageEdges <<<<<<<<<<<<<<<<<//
int sobelGx(Image& image, int x, int y, int channel)
{
    int gx = -image.getPixel(x - 1, y - 1, channel) + image.getPixel(x + 1, y - 1, channel) - 2 * image.getPixel(x - 1, y, channel) + 2 * image.getPixel(x + 1, y, channel) - image.getPixel(x - 1, y + 1, channel) + image.getPixel(x + 1, y + 1, channel);
    return gx;
}

int sobelGy(Image& image, int x, int y, int channel)
{
    int gy = -image.getPixel(x - 1, y - 1, channel) - 2 * image.getPixel(x, y - 1, channel) - image.getPixel(x + 1, y - 1, channel) + image.getPixel(x - 1, y + 1, channel) + 2 * image.getPixel(x, y + 1, channel) + image.getPixel(x + 1, y + 1, channel);
    return gy;
}
//>>>>>>>>>>>>>>> assocated with detectImageEdges <<<<<<<<<<<<<<<<<//


// <<<<<<< Filter 11 >>>>>> //
void Resizing()
{
    while (true)
    {
        try {

            // Take the orginal image name
            string imageName;
            cout << "Enter the name of image : ";
            cin >> imageName;
            Image image(imageName);

            // Take the dimentions of the wanted photo
            int w, h;
            cout << "Enter the new dimentions (width , height) : ";
            cin >> w >> h;
            Image image2(w, h);


            for (int i = 0; i < w; i++)
            {
                for (int j = 0; j < h; j++)
                {
                    for (int k = 0; k < 3; k++)
                    {
                        // Make the new image equal the resized orginal image
                        image2(i, j, k) = image(i * image.width / w, j * image.height / h, k);
                    }
                }
            }

            //Make the user select a new name for the image
            string newNameImage;
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            //check if the image saved successfuly
            if (image2.saveImage(newNameImage))
            {
                image2.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

// <<<<<<< Filter 12 >>>>>> //
void boxblur()
{
    while (true)
    {
        try
        {
            string filename, newNameImage;
            cout << " enter your file name : ";
            cin >> filename;
            Image image(filename);
            cout << "enter radius value: ";
            int value;
            cin >> value;

            Image blurred_image(image.width, image.height);

            for (int x = 0; x < image.width; ++x) {
                for (int y = 0; y < image.height; ++y) {
                    for (int z = 0; z < image.channels; ++z) {
                        int sum = 0, count = 0;

                        for (int i = x - value; i <= x + value; ++i) {
                            for (int j = y - value; j <= y + value; ++j) {
                                if (i >= 0 && i < image.width && j >= 0 && j < image.height) {
                                    sum += image(i, j, z);
                                    count++;
                                }
                            }
                        }


                        blurred_image(x, y, z) = sum / count;
                    }
                }
            }
            //Make the user select a new name for the image
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            //check if the image saved successfuly
            if (blurred_image.saveImage(newNameImage))
            {
                blurred_image.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            };
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
}

// <<<<<<< Filter 13 >>>>>> //
void Natural_Sun()
{
    while (true)
    {
        try
        {
            string imageName;
            cout << "Enter image name : ";
            cin >> imageName;
            Image image(imageName);

            for (int i = 0; i < image.width; i++) {
                for (int j = 0; j < image.height; j++) {
                    image(i, j, 2) = image(i, j, 2) / 1.4;
                }
            }
            string newNameImage;
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            if (image.saveImage(newNameImage))
            {
                image.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << '\n';
        }
    }
}

// <<<<<<< Filter 14 >>>>>> //
void oilPainting()
{
    while (true)
    {
        try
        {
            // Take the orginal image name
            string imageName, newNameImage;
            cout << "Enter a Image Name: ";
            cin >> imageName;
            Image image(imageName);
            Image resultImage(image.width, image.height);
            int radius = 5;
            int intensityLevels = 10;

            int intensityCount[10] = { 0 };
            int averageR[10] = { 0 };
            int averageG[10] = { 0 };
            int averageB[10] = { 0 };
            for (int i = 0; i < image.width; i++)
            {
                for (int j = 0; j < image.height; j++)
                {
                    for (int k = 0; k < intensityLevels; k++)
                    {
                        intensityCount[k] = 0;
                        averageR[k] = 0;
                        averageG[k] = 0;
                        averageB[k] = 0;
                    }
                    for (int dx = -radius; dx <= radius; dx++)
                    {
                        for (int dy = -radius; dy <= radius; dy++)
                        {
                            if (i + dx >= 0 && i + dx < image.width && j + dy >= 0 && j + dy < image.height)
                            {
                                int r = image.getPixel(i + dx, j + dy, 0);
                                int g = image.getPixel(i + dx, j + dy, 1);
                                int b = image.getPixel(i + dx, j + dy, 2);
                                int intensity = (r + g + b) / 3 * intensityLevels / 256;
                                intensityCount[intensity]++;
                                averageR[intensity] += r;
                                averageG[intensity] += g;
                                averageB[intensity] += b;
                            }
                        }
                    }
                    int maxIntensity = 0;
                    for (int k = 1; k < intensityLevels; k++)
                    {
                        if (intensityCount[k] > intensityCount[maxIntensity])
                        {
                            maxIntensity = k;
                        }
                    }
                    resultImage.setPixel(i, j, 0, averageR[maxIntensity] / intensityCount[maxIntensity]);
                    resultImage.setPixel(i, j, 1, averageG[maxIntensity] / intensityCount[maxIntensity]);
                    resultImage.setPixel(i, j, 2, averageB[maxIntensity] / intensityCount[maxIntensity]);
                }
            }


            //Make the user select a new name for the image
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> newNameImage;

            //check if the image saved successfuly
            if (resultImage.saveImage(newNameImage))
            {
                resultImage.saveImage(newNameImage);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
}

// <<<<<<< Filter 15 >>>>>> //
void Old_TV()
{
    while (true)
    {
        try
        {
            string filename, filename2;
            cout << "Please enter your file name: ";
            cin >> filename;


            Image image(filename);
            Image image2(image.width, image.height);

            srand(time(0));

            for (int x = 0; x < image.width; ++x) {
                for (int y = 0; y < image.height; ++y) {
                    for (int z = 0; z < image.channels; ++z) {

                        //int randm = (rand() % 10) + 1;
                        int rand1 = (rand() % 250) + 245;
                        int rand2 = (rand() % 5) + 1;
                        if (y % 2 == 0) {
                            image(x, y, z) -= rand2;
                        }
                        else if (y % 2 == 1) {
                            image(x, y, z) += rand1;
                        }
                        image2(x, y, z) = image(x, y, z);
                    }
                }
            }

            //Make the user select a new name for the image
            cout << "Enter the name of the new image with the extension from (.jpg, .jpeg, .bmp, .png, .tga) you want: ";
            cin >> filename2;

            //check if the image saved successfuly
            if (image2.saveImage(filename2))
            {
                image2.saveImage(filename2);
                cout << "\nSuccessfuly Editing.\n" << endl;
                break;
            }
            else
            {
                cout << "\nFalid Editing.\n" << endl;
                break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error: " << e.what() << endl;
        }
    }
}