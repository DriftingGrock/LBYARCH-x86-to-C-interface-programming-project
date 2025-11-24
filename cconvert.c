#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

extern void imgCvtGrayFloatToInt(float* inputImage, unsigned char* outputImage, int size);

void imgCvtGrayFloatToInt_C(float* inputImage, unsigned char* outputImage, int size) {
	float cell = 0;
	
    int i = 0;
    for (i = 0; i < size; i++) {
        cell = inputImage[i] * 255.0f + 0.5f; 
        if (cell > 255)
            outputImage[i] = (unsigned char)255;
        else if (cell < 0)
            outputImage[i] = (unsigned char)0;
        else
            outputImage[i] = (unsigned char)(cell);
    }
}

float randomFloat() {
    return (float)rand() / (float)RAND_MAX;
}

int verifyConversion(float* input, unsigned char* output, int size) {
    int i;
    for (i = 0; i < size; i++) {
        int expected = (int)(input[i] * 255.0f + 0.5f); 
        if (expected < 0) expected = 0;
        if (expected > 255) expected = 255;
        
        if (output[i] != expected) {
            printf("Mismatch at index %d: expected %d, got %d (input: %f)\n", 
                   i, expected, output[i], input[i]);
            return 0;
        }
    }
    return 1;
}

void runPerformanceTest(int width, int height, int runs) {
    int i, run;
    int size = width * height;
    double totalTimeASM = 0.0;
    double totalTimeC = 0.0;
    
    printf("\n========================================\n");
    printf("Performance Test: %d x %d (%d pixels)\n", width, height, size);
    printf("Running %d iterations for each version...\n", runs);
    printf("========================================\n");
    
    float* inputImage = (float*)malloc(size * sizeof(float));
    unsigned char* outputImageASM = (unsigned char*)malloc(size * sizeof(unsigned char));
    unsigned char* outputImageC = (unsigned char*)malloc(size * sizeof(unsigned char));
    
    if (inputImage == NULL || outputImageASM == NULL || outputImageC == NULL) {
        printf("Invalid mem allocation\n");
        return;
    }
    
    for (i = 0; i < size; i++) {
        inputImage[i] = randomFloat();
    }
    
    printf("\nAssembly Ver\n");
    for (run = 0; run < runs; run++) {
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        
        QueryPerformanceCounter(&start);
        imgCvtGrayFloatToInt(inputImage, outputImageASM, size);
        QueryPerformanceCounter(&end);
        
        double elapsed = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;
        totalTimeASM += elapsed;
        
        if ((run + 1) % 10 == 0) {
            printf("  Completed %d/%d runs...\n", run + 1, runs);
        }
    }
    
    // ===== Test C Version =====
    printf("\nC Version\n");
    for (run = 0; run < runs; run++) {
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        
        QueryPerformanceCounter(&start);
        imgCvtGrayFloatToInt_C(inputImage, outputImageC, size);
        QueryPerformanceCounter(&end);
        
        double elapsed = (double)(end.QuadPart - start.QuadPart) * 1000000.0 / frequency.QuadPart;
        totalTimeC += elapsed;
        
        if ((run + 1) % 10 == 0) {
            printf("  Completed %d/%d runs...\n", run + 1, runs);
        }
    }
    
    printf("\n[Correctness Check]\n");
    printf("Verifying Assembly output... ");
    if (verifyConversion(inputImage, outputImageASM, size)) {
        printf("PASSED!\n");
    } else {
        printf("FAILED!\n");
    }
    
    printf("Verifying C output... ");
    if (verifyConversion(inputImage, outputImageC, size)) {
        printf("PASSED!\n");
    } else {
        printf("FAILED!\n");
    }
    
    double avgTimeASM = totalTimeASM / runs;
    double avgTimeC = totalTimeC / runs;
    
    printf("\n[Performance Results]\n");
    printf("Assembly (x86-64):\n");
    printf("  Total time: %.2f microseconds\n", totalTimeASM);
    printf("  Average time: %.2f microseconds\n", avgTimeASM);
    printf("  Average time: %.6f milliseconds\n", avgTimeASM / 1000.0);
    
    printf("\nC Version:\n");
    printf("  Total time: %.2f microseconds\n", totalTimeC);
    printf("  Average time: %.2f microseconds\n", avgTimeC);
    printf("  Average time: %.6f milliseconds\n", avgTimeC / 1000.0);
    
    printf("\nComparison:\n");
    if (avgTimeASM < avgTimeC) {
        printf("  Assembly is %.2fx FASTER than C\n", avgTimeC / avgTimeASM);
    } else {
        printf("  C is %.2fx FASTER than Assembly\n", avgTimeASM / avgTimeC);
    }
    
    free(inputImage);
    free(outputImageASM);
    free(outputImageC);
}

int main() {
    srand(time(NULL)); 
    char choice;
    
    do {
        printf("\nSelect mode:\n");
        printf("1. Manual input\n");
        printf("2. Performance test\n");
        printf("Enter 1 or 2: ");
        
        int mode;
        scanf("%d", &mode);
        
        if (mode == 1) {
            int height, width, i, j;
            
            printf("\nEnter height and width: ");
            scanf("%d %d", &height, &width);
            
            int size = height * width;
            
            float* inputImage = (float*)malloc(size * sizeof(float));
            unsigned char* outputImage = (unsigned char*)malloc(size * sizeof(unsigned char));
            
            if (inputImage == NULL || outputImage == NULL) {
                printf("invalid meme allocation\n");
                return 1;
            }
            
            printf("Enter %d float pixel values:\n", size);
            for (i = 0; i < size; i++) {
                scanf("%f", &inputImage[i]);
            }
            
            imgCvtGrayFloatToInt(inputImage, outputImage, size);
            
            printf("\nOutput Image (Assembly x86-64):\n");
            for (i = 0; i < height; i++) {
                for (j = 0; j < width; j++) {
                    printf("%4d", outputImage[i * width + j]);
                    if (j < width - 1) {
                        printf(", ");
                    }
                }
                printf("\n");
            }
            
            free(inputImage);
            free(outputImage);
            
        } else if (mode == 2) {
            int numRuns = 30;
            
            runPerformanceTest(10, 10, numRuns);
            runPerformanceTest(100, 100, numRuns);
            runPerformanceTest(1000, 1000, numRuns);
            
            printf("performance test end\n");
            
        } else {
            printf("Invalid choice\n");
        }
        
        printf("\nRun again? (y/n): ");
        scanf(" %c", &choice);
        printf("\n");
        
    } while (choice == 'y' || choice == 'Y');
    
    printf("Program done\n");
    
    return 0;
}
