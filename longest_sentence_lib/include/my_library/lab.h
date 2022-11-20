#ifndef LABSLINUXOS_LAB_H
#define LABSLINUXOS_LAB_H

#include <string>
#include <vector>


extern "C" int entry_function();

std::string readTextFromFile(const std::string &fileName);
void saveResToFile(const std::string &fileName, const std::string &text, long long exeTime, size_t numberOfThreads);
void saveResLimitToFile(const std::string &fileName, const std::string &text, long long exeTime, size_t
numberOfThreads, size_t limitedThreads);
void findLongestSentence(const std::string &text, bool limit);
std::vector<std::string> divideTextIntoChunksOfSentences(const std::string &text, size_t parts);
void runCalculatingThreads(std::vector<std::string> &chunksOfSentences, int priority, bool limit);
size_t CountWords(const std::string &sentence);
void RunNThreads(size_t numberOfThreads);
void deleteUnnecessarySymbols(std::string &text);
std::string LongestSentenceInVector(const std::vector<std::string> &sentences);
int progressUpdateInPercents(size_t nowSizeOfVectorSentences, size_t finalSizeOfVectorSentences);
void limitThreads(size_t numOfThreads, std::vector<std::string> &chunksOfSentences);

#endif //LABSLINUXOS_LAB_H
