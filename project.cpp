#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

// Function to manually convert a character to uppercase
char toUpperCase(char c) 
{
    if (c >= 'a' && c <= 'z') 
    {
        return c - 'a' + 'A';
    }
    return c;
}

// Function to check if a character is alphanumeric
bool isAlnum(char c) 
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

// Function to normalize and preprocess the words in the text
vector<pair<string, double>> preprocessWords(const string& filename) 
{
    ifstream file(filename);
    if (!file) 
    {
        cerr << "Error opening file: " << filename << endl;
        return {};
    }

    set<string> stopWords = {"A", "AND", "AN", "OF", "IN", "THE"};
    map<string, int> wordCount;
    int totalWords = 0;
    string word;

    while (file >> word) 
    {
        string cleanedWord;
        for (char c : word) 
        {
            if (isAlnum(c)) 
            {
                cleanedWord += toUpperCase(c);
            }    
        }
        if (cleanedWord.empty() || stopWords.count(cleanedWord))
        {
            continue;
        }    
        wordCount[cleanedWord]++;
        totalWords++;
    }

    // Sort words by frequency and take the top 100
    vector<pair<string, double>> normalizedFrequencies;
    for (const auto& wc : wordCount) 
    {
        normalizedFrequencies.emplace_back(wc.first, (double)wc.second / totalWords);
    }
    sort(normalizedFrequencies.begin(), normalizedFrequencies.end(), [](const auto& a, const auto& b) { return b.second > a.second; });
    if (normalizedFrequencies.size() > 100) 
    {
        normalizedFrequencies.resize(100);
    }    

    return normalizedFrequencies;
}

// Function to calculate similarity index between two files
double calculateSimilarity(const vector<pair<string, double>>& file1Freqs,
                           const vector<pair<string, double>>& file2Freqs) 
{
    map<string, double> file1Map, file2Map;
    for (const auto& wf : file1Freqs) file1Map[wf.first] = wf.second;
    for (const auto& wf : file2Freqs) file2Map[wf.first] = wf.second;
    
    double similarityIndex = 0.0;
    for (const auto& wf : file1Map) 
    {
        if (file2Map.count(wf.first)) 
        {
            similarityIndex += wf.second + file2Map[wf.first];
        }
    }
    return similarityIndex;
}

// Main function
int main() 
{
    const int numFiles = 64;
    const string basePath = "textbook_";  // Assuming files are named as textbook_0.txt, textbook_1.txt, etc.
    vector<vector<double>> similarityMatrix(numFiles, vector<double>(numFiles, 0.0));
    vector<vector<pair<string, double>>> normalizedWordCounts(numFiles);

    // Step 1: Preprocess each file
    for (int i = 0; i < numFiles; ++i) 
    {
        string filename = basePath + to_string(i) + ".txt";
        normalizedWordCounts[i] = preprocessWords(filename);
    }

    // Step 2: Populate the similarity matrix
    for (int i = 0; i < numFiles; ++i) 
    {
        for (int j = i + 1; j < numFiles; ++j) 
        {
            double similarity = calculateSimilarity(normalizedWordCounts[i], normalizedWordCounts[j]);
            similarityMatrix[i][j] = similarityMatrix[j][i] = similarity;
        }
    }

    // Step 3: Identify top 10 similar pairs (excluding self-similarity)
    vector<tuple<double, int, int>> similarities;
    for (int i = 0; i < numFiles; ++i) 
    {
        for (int j = i + 1; j < numFiles; ++j) 
        {
            similarities.emplace_back(similarityMatrix[i][j], i, j);
        }
    }

    // Sort pairs by similarity index in descending order
    sort(similarities.begin(), similarities.end(), greater<tuple<double, int, int>>());

    // Output top 10 pairs
    cout << "Top 10 similar pairs of text books:" << endl;
    for (int k = 0; k < 10 && k < similarities.size(); ++k) 
    {
        auto [similarity, i, j] = similarities[k];
        cout << "Pair (" << i << ", " << j << ") with similarity index: " << similarity << endl;
    }

    return 0;
}