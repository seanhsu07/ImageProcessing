# ImageProcessing
Image Processing Using C with pthreads
This program uses pthreads to process image and adapt the method of adaptive threashold to convert image to black and white. The binarize function creates threads with information and different starting and ending index for the threads to process data of t a bmp file at different locations. The worker function utilizes loops to detect edges and loop through the entire data segment of the bmp file and check the pixels aroud (specified by given radius) to determine the threshold. 
# Revision 15;
