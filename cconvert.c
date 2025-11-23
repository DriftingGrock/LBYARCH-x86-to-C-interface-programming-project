#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

extern void imgCvtGrayFloatToInt(float *inputImage, unsigned char *outputImage, int size);

float randomFloat()
{
    return (float)rand() / (float)RAND_MAX;
}

int verifyConversion(float *input, unsigned char *output, int size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        int expected = (int)(input[i] * 255.0f + 0.5f); // Round to nearest
        if (expected < 0)
            expected = 0;
        if (expected > 255)
            expected = 255;

        if (output[i] != expected)
        {
            printf("Mismatch at index %d: expected %d, got %d (input: %f)\n",
                   i, expected, output[i], input[i]);
            return 0;
        }
    }
    return 1;
}

void runPerformanceTest(int width, int height, int runs)
{
    int i, run;
    int size = width * height;
    double totalTime = 0.0;

    printf("\n========================================\n");
    printf("Performance Test: %d x %d (%d pixels)\n", width, height, size);
    printf("Running %d iterations...\n", runs);
    printf("========================================\n");

    float *inputImage = (float *)malloc(size * sizeof(float));
    unsigned char *outputImage = (unsigned char *)malloc(size * sizeof(unsigned char));

    if (inputImage == NULL || outputImage == NULL)
    {
        printf("Invalid sizes\n");
        return;
    }

    for (i = 0; i < size; i++)
    {
        inputImage[i] = randomFloat();
    }

    for (run = 0; run < runs; run++)
    {
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);

        QueryPerformanceCounter(&start);
        imgCvtGrayFloatToInt(inputImage, outputImage, size);
        QueryPerformanceCounter(&end);

        double elapsed = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;
        totalTime += elapsed;

        if ((run + 1) % 10 == 0)
        {
            printf("Completed %d/%d runs...\n", run + 1, runs);
        }
    }

    printf("\nVerifying correctness... ");
    if (verifyConversion(inputImage, outputImage, size))
    {
        printf("PASSED!\n");
    }
    else
    {
        printf("FAILED!\n");
    }

    double avgTime = totalTime / runs;
    printf("\nResults:\n");
    printf("  Total time: %.2f microseconds\n", totalTime);
    printf("  Average time per run: %.2f microseconds\n", avgTime);
    printf("  Average time per run: %.6f milliseconds\n", avgTime / 1000.0);
    printf("  Average time per run: %.9f seconds\n", avgTime / 1000000.0);

    free(inputImage);
    free(outputImage);
}

int main()
{
    srand(time(NULL));
    char choice;

    do
    {
        printf("Manual input(1)\n");
        printf("Random Number Generated(2)\n");
        printf("Enter 1 or 2: ");

        int mode;
        scanf("%d", &mode);

        if (mode == 1)
        {
            int height, width, i, j;

            printf("\nEnter height and width: ");
            scanf("%d %d", &height, &width);

            int size = height * width;

            float *inputImage = (float *)malloc(size * sizeof(float));
            unsigned char *outputImage = (unsigned char *)malloc(size * sizeof(unsigned char));

            if (inputImage == NULL || outputImage == NULL)
            {
                printf("invalid mem location\n");
                return 1;
            }

            printf("Enter %d float pixel values:\n", size);
            for (i = 0; i < size; i++)
            {
                scanf("%f", &inputImage[i]);
            }

            imgCvtGrayFloatToInt(inputImage, outputImage, size);

            printf("\nOutput Image:\n");
            for (i = 0; i < height; i++)
            {
                for (j = 0; j < width; j++)
                {
                    printf("%4d", outputImage[i * width + j]);
                    if (j < width - 1)
                    {
                        printf(", ");
                    }
                }
                printf("\n");
            }

            free(inputImage);
            free(outputImage);
        }
        else if (mode == 2)
        {
            int numRuns = 30;

            runPerformanceTest(10, 10, numRuns);
            runPerformanceTest(100, 100, numRuns);
            runPerformanceTest(1000, 1000, numRuns);

            printf("\n========================================\n");
            printf("End test");
            printf("========================================\n");
        }
        else
        {
            printf("Invalid\n");
        }

        printf("\nRun again(y/n): ");
        scanf(" %c", &choice);
        printf("\n");

    } while (choice == 'y' || choice == 'Y');

    printf("End program\n");

    return 0;
}
