#include "correlation.h"

/************************ TODO 2 **************************/
/*
 *	INPUT:
 *		origImg:		the original image,
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that
 *						origImg[3*(row*imgWidth+column)+0],
 *						origImg[3*(row*imgWidth+column)+1],
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *
 *		scale, offset:  after correlating the kernel with the origImg,
 *						each pixel should be divided by scale and then added by offset
 *
 *		selection:      a byte array of the same size as the image,
 *						indicating where in the original image should be filtered, e.g.,
 *						selection[k] == 1 ==> pixel k should be filtered
 *                      selection[k] == 0 ==> pixel k should NOT be filtered
 *                      a special case is selection is a NULL pointer, which means all the pixels should be filtered.
 *
 *  OUTPUT:
 *		rsltImg:		the filtered image of the same size as original image.
 *						it is a valid pointer ( allocated already ).
 */

void image_filter(double* rsltImg, const unsigned char* origImg, const unsigned char* selection,
                  int imgWidth, int imgHeight,
                  const double* kernel, int knlWidth, int knlHeight,
                  double scale, double offset)
{
    // Note: copying origImg to rsltImg is NOT the solution, it does nothing!
    int x, y;
    for(y=0;y<imgHeight;y++){
        for (x=0;x<imgWidth;x++){
            pixel_filter(&(rsltImg[3*(y*imgWidth+x)+0]),x,y,origImg,imgWidth,imgHeight,kernel,knlWidth,knlHeight,scale,offset);
        }
    }

//printf("TODO: %s:%d\n", __FILE__, __LINE__); 

}

/************************ END OF TODO 2 **************************/


/************************ TODO 3 **************************/
/*
 *	INPUT:
 *      x:				a column index,
 *      y:				a row index,
 *		origImg:		the original image,
 *		imgWidth:		the width of the image
 *		imgHeight:		the height of the image
 *						the image is arranged such that
 *						origImg[3*(row*imgWidth+column)+0],
 *						origImg[3*(row*imgWidth+column)+1],
 *						origImg[3*(row*imgWidth+column)+2]
 *						are R, G, B values for pixel at (column, row).
 *
 *      kernel:			the 2D filter kernel,
 *		knlWidth:		the width of the kernel
 *		knlHeight:		the height of the kernel
 *
 *		scale, offset:  after correlating the kernel with the origImg,
 *						the result pixel should be divided by scale and then added by offset
 *
 *  OUTPUT:
 *		rsltPixel[0], rsltPixel[1], rsltPixel[2]:
 *						the filtered pixel R, G, B values at row y , column x;
 */

void pixel_filter(double rsltPixel[3], int x, int y, const unsigned char* origImg, int imgWidth, int imgHeight,
                  const double* kernel, int knlWidth, int knlHeight,
                  double scale, double offset)
{
//	printf("TODO: %s:%d\n", __FILE__, __LINE__); 
	int u, v; 
	int row, col;
	rsltPixel[0]=0;
	rsltPixel[1]=0;
	rsltPixel[2]=0;

	for(u=-knlHeight/2;u<=knlHeight/2;u++)
			for(v=-knlWidth/2;v<=knlWidth/2;v++)
			{
				row=u+y;
				col=v+x;
				if(row>=0&&col>=0&&row<imgHeight&&col<imgWidth)
				{
				rsltPixel[0]+=origImg[3*(row*imgWidth+col)+0]*kernel[(u+knlWidth/2)*knlWidth+v+knlHeight/2];
				rsltPixel[1]+=origImg[3*(row*imgWidth+col)+1]*kernel[(u+knlWidth/2)*knlWidth+v+knlHeight/2];
				rsltPixel[2]+=origImg[3*(row*imgWidth+col)+2]*kernel[(u+knlWidth/2)*knlWidth+v+knlHeight/2];
				}
			}
		rsltPixel[0]=rsltPixel[0]/scale+offset;
		rsltPixel[1]=rsltPixel[1]/scale+offset;
		rsltPixel[2]=rsltPixel[2]/scale+offset;
}

/************************ END OF TODO 3 **************************/

