// multimedia_processor.h : Include file for standard system include files,
// or project specific include files.

#pragma once

using namespace std;

// TODO move mesh loading to main function?
void debug();
void preprocess(const string database = "PSBDatabase", const string in = "69.off", const string out = "69_processed.off");
void preprocessAll(const string database = "PSBDatabase");
void extract(const string database = "PSBDatabase", const string in = "69_processed.off");
void extractAll(const string database = "PSBDatabase");
void computeFeatureVectors(const string database = "PSBDatabase");
vector<tuple<int,float>>  queryDatabaseModel(const string database = "PSBDatabase", const string in = "125_processed.off", size_t k = 10);
void computeClosestModels(const string database = "PSBDatabase");