using System.Drawing;
using FreeImageAPI;
using System.Runtime.InteropServices;

namespace NormalMap
{
    public class NormalMap
    {
        private FilterData[] sobel3x3Default;
        private FilterData[] sobel5x5Default;
        private FilterData[] sobel7x7Default;
        private FilterData[] sobel9x9Default;
        private float defaultScale3x3;
        private float defaultScale5x5;
        private float defaultScale7x7;
        private float defaultScale9x9;

        private FilterData[] sobel3x3;
        private FilterData[] sobel5x5;
        private FilterData[] sobel7x7;
        private FilterData[] sobel9x9;

        private byte[] textureBytes;
        private byte[] normalMapBytes;
        private byte[] depthMapBytes;

        private uint bpp;
        private uint height;
        private uint width;
        private uint stride;

        private uint redMask;
        private uint greenMask;
        private uint blueMask;

        private float[] edgePixels;

        private float[] vec;

        public enum EdgeFilter : int
        {
            Sobel3X3 = 0,
            Sobel5X5 = 1,
            Sobel7X7 = 2,
            Sobel9X9 = 3
        }

        // Filter that will be used for computing the edge algorithm
        public EdgeFilter Filter { get; set; }
        // Invert the X component
        public bool InvertX { get; set; }
        // Invert the Y component
        public bool InvertY { get; set; }
        // Determines whether or not to wrap border pixels, which will pick pixels from the other side of the image
        public bool WrapBorder { get; set; }
        // Applies bumpiness to the image - the bigger the value, the bumpier the image will look
        public float Strength { get; set; }
        // Include depth information on the Alpha component of the normal map texture
        public bool DepthMapOnAlpha { get; set; }

        public NormalMap()
        {
            textureBytes = null;
            normalMapBytes = null;
            depthMapBytes = null;

            bpp = 0;
            height = 0;
            width = 0;
            stride = 0;

            redMask = 0;
            greenMask = 0;
            blueMask = 0;

            edgePixels = null;
            vec = new float[3];

            // Initialize Default Filters
            InitFilter(out sobel3x3Default, 3, Kernels.sobel3x3GX, Kernels.sobel3x3GY);
            InitFilter(out sobel5x5Default, 5, Kernels.sobel5x5GX, Kernels.sobel5x5GY);
            InitFilter(out sobel7x7Default, 7, Kernels.sobel7x7GX, Kernels.sobel7x7GY);
            InitFilter(out sobel9x9Default, 9, Kernels.sobel9x9GX, Kernels.sobel9x9GY);

            // Initialize Filters that will be used with the chosen scale value
            InitFilter(out sobel3x3, 3, Kernels.sobel3x3GX, Kernels.sobel3x3GY);
            InitFilter(out sobel5x5, 5, Kernels.sobel5x5GX, Kernels.sobel5x5GY);
            InitFilter(out sobel7x7, 7, Kernels.sobel7x7GX, Kernels.sobel7x7GY);
            InitFilter(out sobel9x9, 9, Kernels.sobel9x9GX, Kernels.sobel9x9GY);

            // Set basic scale values
            // These are also different from code to code
            // They are customized to ensure proper derivation on the larger kernels
            defaultScale3x3 = 4.0f;
            defaultScale5x5 = 8.0f;
            defaultScale7x7 = 16.0f;
            defaultScale9x9 = 16.0f;

            SetDefaultFilterValues(EdgeFilter.Sobel3X3);
            SetDefaultFilterValues(EdgeFilter.Sobel5X5);
            SetDefaultFilterValues(EdgeFilter.Sobel7X7);
            SetDefaultFilterValues(EdgeFilter.Sobel9X9);

            // Set default options
            Filter = EdgeFilter.Sobel3X3;
            InvertX = false;
            InvertY = false;
            WrapBorder = false;
            Strength = 1.0f;
            DepthMapOnAlpha = false;
        }

        ~NormalMap()
        {
            textureBytes = null;
            normalMapBytes = null;
            depthMapBytes = null;

            bpp = 0;
            height = 0;
            width = 0;
            stride = 0;

            redMask = 0;
            greenMask = 0;
            blueMask = 0;

            edgePixels = null;
            vec = null;
        }

        public void Generate(out Bitmap normalMap, out Bitmap depthMap)
        {
            // Red and Blue components are swapped in little endian systems
            const int r = 2;
            const int g = 1;
            const int b = 0;
            const int a = 3;

            normalMap = null;
            depthMap = null;

            if (bpp != 3 && bpp != 4)
            {
                return;
            }

            // Apply Grayscale
            float normalizedVal = 1.0f / 255.0f;
            float val = 0.0f;
            int index = 0;

            int row = 0;
            int col = 0;
            for (row = 0; row < height; row++)
            {
                for (col = 0; col < width; col++)
                {
                    index = row * (int)stride + col * (int)bpp;
                    val =
                        (((float)textureBytes[index + r]) * 0.3f +
                        ((float)textureBytes[index + g]) * 0.59f +
                        ((float)textureBytes[index + b]) * 0.11f) * normalizedVal;

                    edgePixels[row * width + col] = val;
                    textureBytes[index + a] = 255;
                }
            }

            // Choose the currently selected filter
            FilterData[] filter = null;
            switch(Filter)
            {
                case EdgeFilter.Sobel3X3:
                    filter = sobel3x3;
                    break;
                case EdgeFilter.Sobel5X5:
                    filter = sobel5x5;
                    break;
                case EdgeFilter.Sobel7X7:
                    filter = sobel7x7;
                    break;
                case EdgeFilter.Sobel9X9:
                    filter = sobel9x9;
                    break;
            }

            // Apply edge filter
            float dx = 0.0f;
            float dy = 0.0f;
            byte sum = 0;
            int i = 0;
            for (row = 0; row < height; row++)
            {
                for (col = 0; col < width; col++)
                {
                    if (WrapBorder)
                    {
                        for (i = 0; i < filter.Length; i++)
                        {
                            dx += GetEdgePixelWrap(row + filter[i].row, col + filter[i].col) * filter[i].wX;
                            dy += GetEdgePixelWrap(row + filter[i].row, col + filter[i].col) * filter[i].wY;
                        }
                    }
                    else
                    {
                        for (i = 0; i < filter.Length; i++)
                        {
                            dx += GetEdgePixel(row + filter[i].row, col + filter[i].col) * filter[i].wX;
                            dy += GetEdgePixel(row + filter[i].row, col + filter[i].col) * filter[i].wY;
                        }
                    }

                    // Apply the level of bumpiness on the derivatives
                    // The bigger the value, the bumpier the image will look
                    dx *= Strength;
                    dy *= Strength;

                    // Normalize it and get Z (which is the normal)
                    vec[0] = -dx;
                    vec[1] = -dy;
                    vec[2] = 1.0f;
                    NormalizeVector(ref vec);

                    // Invert values if requested
                    if (InvertX) { vec[0] = -vec[0]; }
                    if (InvertY) { vec[1] = -vec[1]; }

                    // Process the fast version of the Sobel algorithm for the depth map
                    sum = (byte)((dx * dx + dy * dy) * 255.0f);
                    if (sum > 255) { sum = 255; }

                    // Now map from the [-1.0f, 1.0f] range to the [0, 255] range
                    index = row * (int)stride + col * (int)bpp;
                    normalMapBytes[index + r] = (byte)((vec[0] + 1.0f) * 127.5f);
                    normalMapBytes[index + g] = (byte)((vec[1] + 1.0f) * 127.5f);
                    normalMapBytes[index + b] = (byte)((vec[2] + 1.0f) * 127.5f);
                    normalMapBytes[index + a] = 255;

                    // Store edge values on the depth map byte array
                    depthMapBytes[index + r] = sum;
                    depthMapBytes[index + g] = sum;
                    depthMapBytes[index + b] = sum;
                    depthMapBytes[index + a] = 255;

                    dx = 0.0f;
                    dy = 0.0f;
                    sum = 0;
                }
            }

            // Convert the bits from IntPtr to bytes and create the images
            System.IntPtr bits = Marshal.UnsafeAddrOfPinnedArrayElement(normalMapBytes, 0);
            FIBITMAP normalMapScan = FreeImage.ConvertFromRawBits(bits, (int)width, (int)height, (int)stride, bpp * 8, redMask, greenMask, blueMask, false);
            normalMap = FreeImage.GetBitmap(normalMapScan);

            bits = Marshal.UnsafeAddrOfPinnedArrayElement(depthMapBytes, 0);
            FIBITMAP depthMapScan = FreeImage.ConvertFromRawBits(bits, (int)width, (int)height, (int)stride, bpp * 8, redMask, greenMask, blueMask, false);
            depthMap = FreeImage.GetBitmap(depthMapScan);
        }

        public bool SaveTexture(string fileName)
        {
            FIBITMAP texture = FreeImage.ConvertFromRawBits(textureBytes, (int)width, (int)height, (int)stride, bpp * 8, redMask, greenMask, blueMask, false);
            bool result = FreeImage.SaveEx(texture, fileName);
            FreeImage.UnloadEx(ref texture);
            return result;
        }

        public bool SaveNormalMap(string fileName)
        {
            // Insert the depth map on the alpha component, if selected
            if (DepthMapOnAlpha)
            {
                int row = 0;
                int col = 0;
                int index = 0;
                for (row = 0; row < height; row++)
                {
                    for (col = 0; col < width; col++)
                    {
                        index = row * (int)stride + col * (int)bpp;
                        normalMapBytes[index + 3] = depthMapBytes[index];
                    }
                }
            }

            FIBITMAP normalMap = FreeImage.ConvertFromRawBits(normalMapBytes, (int)width, (int)height, (int)stride, bpp * 8, redMask, greenMask, blueMask, false);
            bool result = FreeImage.SaveEx(normalMap, fileName);
            FreeImage.UnloadEx(ref normalMap);
            return result;
        }

        public bool SaveDepthMap(string fileName)
        {
            FIBITMAP depthMap = FreeImage.ConvertFromRawBits(depthMapBytes, (int)width, (int)height, (int)stride, bpp * 8, redMask, greenMask, blueMask, false);
            bool result = FreeImage.SaveEx(depthMap, fileName);
            FreeImage.UnloadEx(ref depthMap);
            return result;
        }

        float GetEdgePixel(int row, int col)
        {
            int pos = row * (int)width + col;
            if (pos < 0 || pos >= edgePixels.Length)
            {
                return 0.0f;
            }

            return edgePixels[pos];
        }

        float GetEdgePixelWrap(int row, int col)
        {
            int pos = row * (int)width + col;
            // Wrap the position if it's out of bounds
            if (pos < 0)
            {
                pos += edgePixels.Length;
            }
            else if (pos >= edgePixels.Length)
            {
                pos -= edgePixels.Length;
            }

            return edgePixels[pos];
        }


        float GetPixel(int row, int col, float[] pixels)
        {
            int pos = row * (int)width + col;
            if (pos < 0 || pos >= pixels.Length)
            {
                return 0.0f;
            }

            return pixels[pos];
        }

        float GetPixelWrap(int row, int col, float[] pixels)
        {
            int pos = row * (int)width + col;
            // Wrap the position if it's out of bounds
            if (pos < 0)
            {
                pos += pixels.Length;
            }
            else if (pos >= pixels.Length)
            {
                pos -= pixels.Length;
            }

            return pixels[pos];
        }

        public void SetTexture(FIBITMAP texture)
        {
            // Get all the necessary information from the image
            width = FreeImage.GetWidth(texture);
            height = FreeImage.GetHeight(texture);
            bpp = FreeImage.GetBPP(texture) / 8;
            stride = FreeImage.GetPitch(texture);

            redMask = FreeImage.GetRedMask(texture);
            greenMask = FreeImage.GetGreenMask(texture);
            blueMask = FreeImage.GetBlueMask(texture);

            textureBytes = new byte[height * stride];
            Marshal.Copy(FreeImage.GetBits(texture), textureBytes, 0, textureBytes.Length);

            normalMapBytes = new byte[textureBytes.Length];
            depthMapBytes = new byte[textureBytes.Length];

            edgePixels = new float[height * width];
        }

        void NormalizeVector(ref float[] v)
        {
            float oneOverLength = 1.0f / (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            v[0] *= oneOverLength;
            v[1] *= oneOverLength;
            v[2] *= oneOverLength;
        }

        private void InitFilter(out FilterData[] filterDataArray, int size, float[] filterX, float[] filterY)
        {
            filterDataArray = new FilterData[size * size];

            int initialVal = -size / 2;
            int startRow = initialVal;
            int startCol = initialVal;

            int index = 0;
            for (int row = 0; row < size; row++)
            {
                for (int col = 0; col < size; col++)
                {
                    index = row * size + col;
                    filterDataArray[index] = new FilterData(
                        startRow,
                        startCol++,
                        filterX[index],
                        filterY[index]);
                }

                startCol = initialVal;
                startRow++;
            }
        }

        public void SetDefaultFilterValues(EdgeFilter filter)
        {
            switch (filter)
            {
                case EdgeFilter.Sobel3X3:
                    UpdateFilter(filter, defaultScale3x3);
                    break;
                case EdgeFilter.Sobel5X5:
                    UpdateFilter(filter, defaultScale5x5);
                    break;
                case EdgeFilter.Sobel7X7:
                    UpdateFilter(filter, defaultScale7x7);
                    break;
                case EdgeFilter.Sobel9X9:
                    UpdateFilter(filter, defaultScale9x9);
                    break;
            }
        }

        public void UpdateFilter(EdgeFilter filter, float scale)
        {
            float oneOverScale = 1.0f / scale;
            FilterData[] src = null;
            FilterData[] dst = null;
            switch (filter)
            {
                case EdgeFilter.Sobel3X3:
                    src = sobel3x3Default;
                    dst = sobel3x3;
                    break;
                case EdgeFilter.Sobel5X5:
                    src = sobel5x5Default;
                    dst = sobel5x5;
                    break;
                case EdgeFilter.Sobel7X7:
                    src = sobel7x7Default;
                    dst = sobel7x7;
                    break;
                case EdgeFilter.Sobel9X9:
                    src = sobel9x9Default;
                    dst = sobel9x9;
                    break;
            }

            for (int i = 0; i < dst.Length; i++)
            {
                dst[i].wX = src[i].wX * oneOverScale;
                dst[i].wY = src[i].wY * oneOverScale;
            }
        }

        public float GetDefaultScale(EdgeFilter filter)
        {
            float scale = 1.0f;
            switch (filter)
            {
                case EdgeFilter.Sobel3X3:
                    scale = defaultScale3x3;
                    break;
                case EdgeFilter.Sobel5X5:
                    scale = defaultScale5x5;
                    break;
                case EdgeFilter.Sobel7X7:
                    scale = defaultScale7x7;
                    break;
                case EdgeFilter.Sobel9X9:
                    scale = defaultScale9x9;
                    break;
            }

            return scale;
        }
    }

    class FilterData
    {
        public int row;
        public int col;
        public float wX;
        public float wY;

        public FilterData()
        {
            row = 0;
            col = 0;
            wX = 0.0f;
            wY = 0.0f;
        }

        public FilterData(int row, int col, float wX, float wY)
        {
            this.row = row;
            this.col = col;
            this.wX = wX;
            this.wY = wY;
        }

        ~FilterData()
        {
            row = 0;
            col = 0;
            wX = 0.0f;
            wY = 0.0f;
        }
    }

    // Some of these Kernels are customized; in other words, there's no correct kernel
    // Several different implementations use different values on kernels of the same size
    // Only the 3x3 kernel is a Sobel fixed kernel, the others will vary from code to code
    public class Kernels
    {
        public static float[] sobel3x3GX = new float[]
        {
            -1.0f, 0.0f, 1.0f,
            -2.0f, 0.0f, 2.0f,
            -1.0f, 0.0f, 1.0f,
        };

        public static float[] sobel3x3GY = new float[]
        {
            -1.0f, -2.0f, -1.0f,
             0.0f,  0.0f,  0.0f,
             1.0f,  2.0f,  1.0f,
        };

        public static float[] sobel5x5GX = new float[]
        {
            -2.0f, -1.0f, 0.0f, 1.0f, 2.0f,
            -3.0f, -2.0f, 0.0f, 2.0f, 3.0f,
            -4.0f, -3.0f, 0.0f, 3.0f, 4.0f,
            -3.0f, -2.0f, 0.0f, 2.0f, 3.0f,
            -2.0f, -1.0f, 0.0f, 1.0f, 2.0f,
        };

        public static float[] sobel5x5GY = new float[]
        {
            -2.0f, -3.0f, -4.0f, -3.0f, -2.0f,
            -1.0f, -2.0f, -3.0f, -2.0f, -1.0f,
             0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
             1.0f,  2.0f,  3.0f,  2.0f,  1.0f,
             2.0f,  3.0f,  4.0f,  3.0f,  2.0f
        };

        public static float[] sobel7x7GX = new float[]
        {
            -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f,
            -4.0f, -3.0f, -2.0f, 0.0f, 2.0f, 3.0f, 4.0f,
            -5.0f, -4.0f, -3.0f, 0.0f, 3.0f, 4.0f, 5.0f,
            -6.0f, -5.0f, -4.0f, 0.0f, 4.0f, 5.0f, 6.0f,
            -5.0f, -4.0f, -3.0f, 0.0f, 3.0f, 4.0f, 5.0f,
            -4.0f, -3.0f, -2.0f, 0.0f, 2.0f, 3.0f, 4.0f,
            -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f
        };

        public static float[] sobel7x7GY = new float[]
        {
            -3.0f, -4.0f, -5.0f, -6.0f, -5.0f, -4.0f, -3.0f,
            -2.0f, -3.0f, -4.0f, -5.0f, -4.0f, -3.0f, -2.0f,
            -1.0f, -2.0f, -3.0f, -4.0f, -3.0f, -2.0f, -1.0f,
             0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
             1.0f,  2.0f,  3.0f,  4.0f,  3.0f,  2.0f,  1.0f,
             2.0f,  3.0f,  4.0f,  5.0f,  4.0f,  3.0f,  2.0f,
             3.0f,  4.0f,  5.0f,  6.0f,  5.0f,  4.0f,  3.0f,
        };

        public static float[] sobel9x9GX = new float[]
        {
            -4.0f, -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f,
            -5.0f, -4.0f, -3.0f, -2.0f, 0.0f, 2.0f, 3.0f, 4.0f, 5.0f,
            -6.0f, -5.0f, -4.0f, -3.0f, 0.0f, 3.0f, 4.0f, 5.0f, 6.0f,
            -7.0f, -6.0f, -5.0f, -4.0f, 0.0f, 4.0f, 5.0f, 6.0f, 7.0f,
            -8.0f, -7.0f, -6.0f, -5.0f, 0.0f, 5.0f, 6.0f, 7.0f, 8.0f,
            -7.0f, -6.0f, -5.0f, -4.0f, 0.0f, 4.0f, 5.0f, 6.0f, 7.0f,
            -6.0f, -5.0f, -4.0f, -3.0f, 0.0f, 3.0f, 4.0f, 5.0f, 6.0f,
            -5.0f, -4.0f, -3.0f, -2.0f, 0.0f, 2.0f, 3.0f, 4.0f, 5.0f,
            -4.0f, -3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f, 4.0f
        };

        public static float[] sobel9x9GY = new float[]
        {
            -4.0f, -5.0f, -6.0f, -7.0f, -8.0f, -7.0f, -6.0f, -5.0f, -4.0f,
            -3.0f, -4.0f, -5.0f, -6.0f, -7.0f, -6.0f, -5.0f, -4.0f, -3.0f,
            -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -5.0f, -4.0f, -3.0f, -2.0f,
            -1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -4.0f, -3.0f, -2.0f, -1.0f,
             0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,
             1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  4.0f,  3.0f,  2.0f,  1.0f,
             2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  5.0f,  4.0f,  3.0f,  2.0f,
             3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  6.0f,  5.0f,  4.0f,  3.0f,
             4.0f,  5.0f,  6.0f,  7.0f,  8.0f,  7.0f,  6.0f,  5.0f,  4.0f,
        };

        public static float[] gaussian3x3 = new float[]
        {
            0.0585498f,   0.0965324f,    0.0585498f,
            0.0965324f,   0.1591550f,    0.0965324f,
            0.0585498f,   0.0965324f,    0.0585498f
        };

        public static float[] gaussian5x5 = new float[]
        {
            0.003765f,    0.015019f,    0.023792f,    0.015019f,    0.003765f,
            0.015019f,    0.059912f,    0.094907f,    0.059912f,    0.015019f,
            0.023792f,    0.094907f,    0.150342f,    0.094907f,    0.023792f,
            0.015019f,    0.059912f,    0.094907f,    0.059912f,    0.015019f,
            0.003765f,    0.015019f,    0.023792f,    0.015019f,    0.003765f
        };
    }
}
