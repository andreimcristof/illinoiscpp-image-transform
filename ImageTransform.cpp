#include <iostream>
#include <cmath>
#include <cstdlib>

#include "uiuc/PNG.h"
#include "uiuc/HSLAPixel.h"
#include "ImageTransform.h"

/* ******************
(Begin multi-line comment...)

Write your name and email address in the comment space here:

Name:
Email:

(...end multi-line comment.)
******************** */

using uiuc::HSLAPixel;
using uiuc::PNG;

/**
 * Returns an image that has been transformed to grayscale.
 *
 * The saturation of every pixel is set to 0, removing any color.
 *
 * @return The grayscale image.
 */
PNG grayscale(PNG image)
{
  /// This function is already written for you so you can see how to
  /// interact with our PNG class.
  for (unsigned x = 0; x < image.width(); x++)
  {
    for (unsigned y = 0; y < image.height(); y++)
    {
      HSLAPixel &pixel = image.getPixel(x, y);

      // `pixel` is a reference to the memory stored inside of the PNG `image`,
      // which means you're changing the image directly. No need to `set`
      // the pixel since you're directly changing the memory of the image.
      pixel.s = 0;
    }
  }

  return image;
}

/**
 * Returns an image with a spotlight centered at (`centerX`, `centerY`).
 *
 * A spotlight adjusts the luminance of a pixel based on the distance the pixel
 * is away from the center by decreasing the luminance by 0.5% per 1 pixel euclidean
 * distance away from the center.
 *
 * For example, a pixel 3 pixels above and 4 pixels to the right of the center
 * is a total of `sqrt((3 * 3) + (4 * 4)) = sqrt(25) = 5` pixels away and
 * its luminance is decreased by 2.5% (0.975x its original value).  At a
 * distance over 160 pixels away, the luminance will always decreased by 80%.
 *
 * The modified PNG is then returned.
 *
 * @param image A PNG object which holds the image data to be modified.
 * @param centerX The center x coordinate of the crosshair which is to be drawn.
 * @param centerY The center y coordinate of the crosshair which is to be drawn.
 *
 * @return The image with a spotlight.
 */
PNG createSpotlight(PNG image, int centerX, int centerY)
{

  for (unsigned x = 0; x < image.width(); x++)
  {
    for (unsigned y = 0; y < image.height(); y++)
    {
      HSLAPixel &pixel = image.getPixel(x, y);
      double euclideanDistance;
      double computedLuminance;
      double twentyPercentLuminance = (20 * pixel.l) / 100;

// https://en.wikipedia.org/wiki/Euclidean_distance

      euclideanDistance = sqrt( (x-centerX)*(x-centerX) + (y-centerY)*(y-centerY) );
      
      // skip the center pixel itself
      if(euclideanDistance == 0) continue;

      double multiplyFactor = 0.5;
      double luminanceSubtraction = ((multiplyFactor * euclideanDistance) * pixel.l) / 100; 
      computedLuminance = euclideanDistance > 160 ? twentyPercentLuminance : luminanceSubtraction;

      if(euclideanDistance > 160) {
        pixel.l = twentyPercentLuminance;
        continue;
      }

      // if(x == 320 && y == 50) {
      //   std::cout << "euclideanDistance: " << euclideanDistance << std::endl;    
      //   std::cout << "pixel.l: " << pixel.l << std::endl;
      //   std::cout << "computedLuminance: " << computedLuminance << std::endl;
      //   std::cout << "updated pixel.l: " << pixel.l - computedLuminance << std::endl;
      // }

      pixel.l -= computedLuminance;     
    }
  }

  return image;
}

/**
 * Returns a image transformed to Illini colors.
 *
 * The hue of every pixel is set to the a hue value of either orange or
 * blue, based on if the pixel's hue value is closer to orange than blue.
 *
 * @param image A PNG object which holds the image data to be modified.
 *
 * @return The illinify'd image.
 **/
PNG illinify(PNG image)
{

  for (unsigned x = 0; x < image.width(); x++)
  {
    for (unsigned y = 0; y < image.height(); y++)
    {
      HSLAPixel &pixel = image.getPixel(x, y);


      // thoughts:
      // Hue edges are 0 and 360. ILLINI_ORANGE is 216. ILBlue is 11. Smaller tendency wins. After 360 it resets back to 11 (its a circle). So:
      // Midpoint between 11 and 216 is 113.5. Tilt towards 11 means between 11 and midpoint, tilt towards 216 means after midpoint. 
      // But from 216 upwards, back to 11 means reaching edge then back to 11: 360 - 216 + 11 = 155. So we have smth like:  
      // left side of 216, and right side of 216. So thats: midpointLeft is 113.5, midpointRight is 216 + 155/2 = 293.5
      
      // so then the algo is comparing two midpoints: 
      // <11> ---orangeTilt----- 113.5 ---blueTilt----- <216> ---blueTilt---- 293.5 ---orangeTilt--- <11>.

      int ILLINI_ORANGE = 11;
      int ILLINI_BLUE = 216;
      double leftMidpoint = 113.5;
      double rightMidpoint = 293.5;

      // so tilt towards blue is mid-graph, otherwise it must be orange
      bool tiltBlue =  (leftMidpoint <= pixel.h && pixel.h <= rightMidpoint);

      if(x == 10 && y == 4) {
        std::cout << "pixel.h: " << pixel.h << std::endl;
        std:: cout << "tiltBlue: " << tiltBlue << std::endl; 
      }

      pixel.h = tiltBlue ? ILLINI_BLUE : ILLINI_ORANGE;
    }
  }

  return image;
}

/**
 * Returns an image that has been watermarked by another image.
 *
 * The luminance of every pixel of the second image is checked, if that
 * pixel's luminance is 1 (100%), then the pixel at the same location on
 * the first image has its luminance increased by 0.2.
 *
 * @param firstImage  The first of the two PNGs, which is the base image.
 * @param secondImage The second of the two PNGs, which acts as the stencil.
 *
 * @return The watermarked image.
 */
PNG watermark(PNG firstImage, PNG secondImage)
{
  for (unsigned x = 0; x < secondImage.width(); x++)
  {
    for (unsigned y = 0; y < secondImage.height(); y++)
    {
      HSLAPixel &pixelSecondImage = secondImage.getPixel(x, y);

      if (pixelSecondImage.l == 1) {
        HSLAPixel &pixelFirstImage = firstImage.getPixel(x,y);
        pixelFirstImage.l += 0.2;
      }
    }
  }
  return firstImage;
}
