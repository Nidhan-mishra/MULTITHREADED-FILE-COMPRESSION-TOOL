#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Function to compress a chunk of the file using RLE
void compressChunk(ifstream& inFile, ofstream& outFile, size_t start, size_t end, atomic<bool>& done) {
    inFile.seekg(start);
    char currentChar, prevChar;
    int count = 1;

    // Start reading the file chunk
    inFile.get(prevChar);

    for (size_t i = start + 1; i < end; ++i) {
        inFile.get(currentChar);
        if (currentChar == prevChar) {
            count++;
        }
        else {
            outFile << prevChar << count;
            prevChar = currentChar;
            count = 1;
        }
    }

    // Write the last character and its count for this chunk
    outFile << prevChar << count;

    done.store(true);
}


// Function to compress the file using RLE without multi-threading
void compressFileWithoutThreads(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::in);
   

    ofstream outFile(outputFile, ios::out);
   

    char currentChar, prevChar;
    int count = 1;

    inFile.get(prevChar);

    while (inFile.get(currentChar)) {
        if (currentChar == prevChar) {
            count++;
        }
        else {
            outFile << prevChar << count;
            count = 1;
        }
        prevChar = currentChar;
    }

    outFile << prevChar << count;

    cout << "File compression completed successfully (without multi-threading)!" << endl;
}

// Function to compress the file using RLE with multi-threading
void compressFileWithThreads(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile, ios::in);
   

    ofstream outFile(outputFile, ios::out);
   

    // Get the file size
    inFile.seekg(0, ios::end);
    size_t fileSize = inFile.tellg();
    inFile.seekg(0, ios::beg);

    // Split the work into chunks for multiple threads
    size_t numThreads = 4; // You can change this based on the number of threads you want
    size_t chunkSize = fileSize / numThreads;

    vector<thread> threads;
    vector<atomic<bool>> done(numThreads);

    for (size_t i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = (i == numThreads - 1) ? fileSize : start + chunkSize;

        threads.push_back(thread(compressChunk, ref(inFile), ref(outFile), start, end, ref(done[i])));
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "File compression completed successfully (with multi-threading)!" << endl;
}





// Function to measure the performance
template <typename Func>
long long measureExecutionTime(Func func, const string& inputFile, const string& outputFile) {
    auto start = high_resolution_clock::now();
    func(inputFile, outputFile);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    return duration.count(); 
}

int main() {
    int choice, threadingChoice;
    string inputFile, outputFile;

  
    cout << "Simple File Compression System\n";
    cout << "1. Compress a file\n";
    /*cout << "2. Decompress a file\n";*/
    cout << "Enter your select (type:1): ";
    cin >> choice;

   
    cout << "Enter the input file name: ";
    cin >> inputFile;
    cout << "Enter the output file name: ";
    cin >> outputFile;

    if (choice == 1) {
        // Ask if the user wants to use threads for compression
        cout << "Do you want to use multi-threading for compression? (1 for Yes, 0 for No): ";
        cin >> threadingChoice;

        if (threadingChoice == 1) {
            // Measure and display the performance with multi-threading
            long long timeWithThreads = measureExecutionTime(compressFileWithThreads, inputFile, outputFile);
            cout << "Compression time with threads: " << timeWithThreads << " milliseconds\n";
        }
        else {
            // Measure and display the performance without multi-threading
            long long timeWithoutThreads = measureExecutionTime(compressFileWithoutThreads, inputFile, outputFile);
            cout << "Compression time without threads: " << timeWithoutThreads << " milliseconds\n";
        }
    }
    
    else {
        cout << "Invalid choice!" << endl;
    }

    return 0;
}
