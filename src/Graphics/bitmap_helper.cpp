#include "LittleEngine/Graphics/bitmap_helper.h"
#include <vector>
#include <cstring> // for memcpy


namespace LittleEngine::Graphics
{

	void FlipBitmapVertically(unsigned char* bitmap, int width, int height, int pixelSize)
	{
        int stride = width * pixelSize; // pixelSize byte per pixel (GL_RED or GL_RGBA...)
        std::vector<unsigned char> tempRow(stride);

        for (int y = 0; y < height / 2; ++y)
        {
            unsigned char* rowTop = bitmap + y * stride;
            unsigned char* rowBottom = bitmap + (height - 1 - y) * stride;

            // swap rows
            memcpy(tempRow.data(), rowTop, stride);
            memcpy(rowTop, rowBottom, stride);
            memcpy(rowBottom, tempRow.data(), stride);
        }
	}
}