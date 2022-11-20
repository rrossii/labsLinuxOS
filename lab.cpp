#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <pthread.h>
#include <cstring>

using namespace std;

typedef long long ll;

string readTextFromFile(const string &fileName);
void saveResToFile(const string &fileName, const string &text, ll exeTime, size_t numberOfThreads);
void saveResLimitToFile(const string &fileName, const string &text, ll exeTime, size_t
numberOfThreads, size_t limitedThreads);
void findLongestSentence(const string &text, bool limit);
vector<string> divideTextIntoChunksOfSentences(const string &text, size_t parts);
void runCalculatingThreads(vector<string> &chunksOfSentences, int priority, bool limit);
size_t CountWords(const string &sentence);
void RunNThreads(size_t numberOfThreads);
void deleteUnnecessarySymbols(string &text);
string LongestSentenceInVector(const vector<string> &sentences);
int progressUpdateInPercents(size_t nowSizeOfVectorSentences, size_t
finalSizeOfVectorSentences);
void limitThreads(size_t numOfThreads, vector<string> &chunksOfSentences);

vector<string> sentencesFoundByThreads;

mutex progressUpdateMutex;
mutex sentencesFoundByThreadsMutex;
sem_t limitThreadSemaphore;

uint64_t CurrentTimeMillis() {
    uint64_t ms = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    return ms;
}

int main() {
    RunNThreads(1);
    RunNThreads(2);
    RunNThreads(4);
    RunNThreads(8);
    return 0;
}

vector<string> divideTextIntoChunksOfSentences(const string &text, size_t parts) {
    vector<size_t> endOfSentence; // indexes of dots
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '.') {
            endOfSentence.emplace_back(i);
        }
    }

    vector<string> dividedText; // multiSentence

    size_t numberOfSentencesInText = endOfSentence.size();

    size_t startOfSentence = 0;
    int index = 0;
    size_t indexOfEndOfSentence = endOfSentence[index];
    if (endOfSentence.size() <= parts) {
        for (size_t i = 0; i < numberOfSentencesInText; i++) {
            size_t numOfCharsToEmplace = indexOfEndOfSentence - startOfSentence + 1;
            dividedText.emplace_back(text.substr(startOfSentence, numOfCharsToEmplace));

            startOfSentence = indexOfEndOfSentence + 1;
            index++;
            indexOfEndOfSentence = endOfSentence[index];
        }
    } else {
        index = -1; // -1 + numberOfSentencesInOneMultiSentence (на початку речення, коли індекс 0, рахуватиме неправильно)
        size_t numberOfSentencesInOneMultiSentence = numberOfSentencesInText / parts;
        size_t leftoverSentences = numberOfSentencesInText % parts;

        for (size_t i = 0; i < parts; i++) {
            if (index + 1 >= numberOfSentencesInText) {
                break;
            }
            string multiSentence;
            indexOfEndOfSentence = endOfSentence[index + numberOfSentencesInOneMultiSentence];
            size_t numOfCharsToEmplace = indexOfEndOfSentence - startOfSentence + 1;

            multiSentence += text.substr(startOfSentence, numOfCharsToEmplace);
            index += numberOfSentencesInOneMultiSentence;
            startOfSentence = indexOfEndOfSentence;

            dividedText.emplace_back(multiSentence);
        }

        for (size_t i = 0; i < leftoverSentences; i++) {
            if (index + 1 >= numberOfSentencesInText) {
                break;
            }
            string multiSentence;
            indexOfEndOfSentence = endOfSentence[index + 1];
            size_t numOfCharsToEmplace = indexOfEndOfSentence - startOfSentence + 1;

            multiSentence += text.substr(startOfSentence + 1, numOfCharsToEmplace); // was startOfSentence
            index++;
            startOfSentence = indexOfEndOfSentence;

            dividedText[i] += multiSentence;
        }
    }

    assert(dividedText.size() <= parts);
    return dividedText;
}

void deleteUnnecessarySymbols(string &text) {
    string::iterator iter = unique(text.begin(), text.end(), [](auto lhs, auto rhs){
        return lhs == rhs && lhs == ' '; // add later \n also
    });
    text.erase(iter, text.end() );
}

int progressUpdateInPercents(size_t nowSizeOfVectorSentences, size_t finalSizeOfVectorSentences) {
    return (nowSizeOfVectorSentences * 100) / finalSizeOfVectorSentences;
}

void limitThreads(size_t numOfThreads, vector<string> &chunksOfSentences) {
    sem_init(&limitThreadSemaphore, 0, numOfThreads);

    runCalculatingThreads(chunksOfSentences, 0, true);

    sem_destroy(&limitThreadSemaphore);
}

void RunNThreads(size_t numberOfThreads) {
    string inputFile = "/home/rosska/CLionProjects/labsLinuxOS/text1.txt";
//    string inputFile = "/home/rosska/CLionProjects/labsLinuxOS/text2.txt";
    string text = readTextFromFile(inputFile);

    deleteUnnecessarySymbols(text);
    vector<string> chunksOfSentences = divideTextIntoChunksOfSentences(text, numberOfThreads);

    auto startTime = CurrentTimeMillis();

    runCalculatingThreads(chunksOfSentences, -20, false);

    auto endTime = CurrentTimeMillis();

    auto longest = LongestSentenceInVector(sentencesFoundByThreads);
    cout << longest << '\n';

    string outputFile = "/home/rosska/CLionProjects/labsLinuxOS/output.txt";
    saveResToFile(outputFile, longest, endTime - startTime, numberOfThreads);
    sentencesFoundByThreads.clear();

// ===========================================

    startTime = CurrentTimeMillis();

    size_t limitedNumOfThread = 2;
    limitThreads(limitedNumOfThread, chunksOfSentences);

    endTime = CurrentTimeMillis();

    longest = LongestSentenceInVector(sentencesFoundByThreads);
    saveResLimitToFile(outputFile, longest, endTime - startTime, numberOfThreads,limitedNumOfThread);

    sentencesFoundByThreads.clear();
}

string LongestSentenceInVector(const vector<string> &sentences) {
    size_t maxWords = 0;
    string result;

    for (auto &sentence : sentences) {
        size_t currentCountWords = CountWords(sentence);
        if (currentCountWords > maxWords) {
            result = sentence;
            maxWords = currentCountWords;
        }
    }

    return result;
}

string FindLongestSentenceInText(const string &multiSentence) {
    vector<string> sentences;

    size_t startIndex = 0;
    for (size_t i = 0; i < multiSentence.size(); i++) {
        if (multiSentence[i] == '.') {
            size_t numOfCharsToEmplace = i - startIndex + 1;
            string sentence = multiSentence.substr(startIndex, numOfCharsToEmplace);
            sentences.emplace_back(sentence);

            startIndex = i+1;
        }
    }

    return LongestSentenceInVector(sentences);
}

size_t CountWords(const string &sentence) {
    size_t countWords = 0;

    for (int i = 1; i < sentence.size(); i++) {
        if (sentence[i] == ' ' || (sentence[i] == '.' && sentence[i-1] != ' ')) {
            countWords++;
        }
    }
    return countWords;
}

void findLongestSentence(const string &text, bool limit) {
    if (limit) {
        sem_wait(&limitThreadSemaphore);
    }

    string longest = FindLongestSentenceInText(text);

    if (limit) {
        sem_post(&limitThreadSemaphore);
    }

    sentencesFoundByThreadsMutex.lock();
    sentencesFoundByThreads.push_back(longest);
    sentencesFoundByThreadsMutex.unlock();
}

void runCalculatingThreads(vector<string> &chunksOfSentences, int priority, bool limit) {
    size_t numberOfChunks = chunksOfSentences.size();

    vector<thread> longestSentence;
    for (size_t i = 0; i < numberOfChunks; i++) {
        longestSentence.emplace_back(findLongestSentence, chunksOfSentences[i], limit);

        pthread_setschedprio(longestSentence[i].native_handle(), priority);
    }

    for (size_t i = 0; i < numberOfChunks; i++) {
        longestSentence[i].detach();
    }

    //sem_post(&limitThreadSemaphore);
    bool threadsWorkFinished;
    do {
        progressUpdateMutex.lock();
        cout << "progress: " << progressUpdateInPercents(sentencesFoundByThreads.size(),
                                                         chunksOfSentences.size()) << "%\n";

        auto comparator = [](const string &current, const string &next) {
            //return current.size() > next.size();
            return CountWords(current) < CountWords(next);
        };

        if (!sentencesFoundByThreads.empty()) {
            string currentLongestSentence = *max_element(sentencesFoundByThreads.begin(),
                                                         sentencesFoundByThreads.end(),
                                                         comparator);
            cout << "The longest sentence by now: " << currentLongestSentence << "\n\n";
        }

        threadsWorkFinished = sentencesFoundByThreads.size() >=
                              chunksOfSentences.size();

        progressUpdateMutex.unlock();
        this_thread::sleep_for(std::chrono::milliseconds(1000));
    } while (!threadsWorkFinished);

    cout << '\n';
}

string readTextFromFile(const string &fileName) {
    ostringstream ss;
    fstream input;
    input.open(fileName);

    if (!input.is_open()) {
        cout << "Cannot open the file! - " << fileName << "!\n";
        exit(EXIT_FAILURE); // ?
    }

    ss << input.rdbuf();
    input.close();
    return ss.str();
}

void saveResToFile(const string &fileName, const string &text, ll exeTime, size_t numberOfThreads) {
    fstream output;
    output.open(fileName, std::ios_base::app);

    output << "Longest sentence size = [" << CountWords(text) << "]\nSentence = [" << text << "]\n";
    output << "Executing time for [" <<  numberOfThreads << "] threads is [" << exeTime << "]ms\n\n";

    output.close();
}

void saveResLimitToFile(const string &fileName, const string &text, ll exeTime, size_t numberOfThreads, size_t limitedThreads) {
    fstream output;

    output.open(fileName, std::ios_base::app);
    output << "Longest sentence size = [" << CountWords(text) << "]\nSentence = [" << text << "]\n";
    output << "Executing time for [" << numberOfThreads << "] threads where " << " was executing [" << limitedThreads << "] threads is [" << exeTime << "]ms\n\n";

    output.close();
}
